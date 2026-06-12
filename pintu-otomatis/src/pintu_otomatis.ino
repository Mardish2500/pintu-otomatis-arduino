/**
 * ============================================================
 * SISTEM PINTU OTOMATIS BERBASIS SENSOR ULTRASONIK HC-SR04
 * Automatic Door System Using HC-SR04 Ultrasonic Sensor
 * ============================================================
 *
 * @file        pintu_otomatis.ino
 * @author      Mardiansyah
 * @version     1.0.0
 * @date        2025
 * @license     MIT License
 *
 * @description
 *   Sistem pintu otomatis yang menggunakan sensor ultrasonik HC-SR04
 *   untuk mendeteksi kehadiran objek/manusia. Pintu dikendalikan oleh
 *   servo motor melalui mekanisme Finite State Machine (FSM) dengan
 *   4 state: CLOSED, OPENING, OPEN, dan CLOSING.
 *
 *   Sistem menggunakan pendekatan non-blocking timer berbasis millis()
 *   untuk memastikan responsivitas penuh tanpa menghentikan eksekusi
 *   program utama.
 *
 * @hardware
 *   - Mikrokontroler : Arduino Uno / Nano / Mega
 *   - Sensor Jarak   : HC-SR04 (TRIG: Pin 11, ECHO: Pin 10)
 *   - Aktuator       : Servo Motor SG90/MG996R (Signal: Pin 3)
 *
 * @principle
 *   - Deteksi jarak menggunakan gelombang ultrasonik (40 kHz)
 *   - Jarak dihitung: d = (t_echo * v_suara) / 2
 *     dimana v_suara ≈ 0.034 cm/µs pada suhu ruang (±20°C)
 *   - Gerakan servo bertahap untuk menghindari jerk (sentakan mekanik)
 *
 * @repository  https://github.com/mardiansyah/pintu-otomatis
 * ============================================================
 */

#include <Servo.h>

// ============================================================
// KONFIGURASI PIN
// ============================================================

#define TRIG_PIN    11      ///< Pin output trigger sensor HC-SR04
#define ECHO_PIN    10      ///< Pin input echo sensor HC-SR04
#define SERVO_PIN    3      ///< Pin sinyal PWM servo motor

// ============================================================
// KONFIGURASI SUDUT SERVO
// ============================================================

#define OPEN_ANGLE   90     ///< Sudut servo saat pintu terbuka penuh (derajat)
#define CLOSE_ANGLE   0     ///< Sudut servo saat pintu tertutup penuh (derajat)

// ============================================================
// KONFIGURASI THRESHOLD & TIMING
// ============================================================

#define DIST_THRESHOLD  15  ///< Jarak deteksi objek maksimum (cm)
#define OPEN_DELAY    5000  ///< Waktu tunggu sebelum pintu menutup kembali (ms)

// ============================================================
// KONFIGURASI GERAK SERVO
// ============================================================

#define OPEN_STEP    3      ///< Increment sudut per langkah saat membuka (derajat)
#define CLOSE_STEP   1      ///< Decrement sudut per langkah saat menutup (derajat)
#define SERVO_DELAY  10     ///< Interval antar langkah servo (ms)

// ============================================================
// DEKLARASI OBJEK
// ============================================================

Servo doorServo;            ///< Objek servo motor untuk menggerakkan pintu

// ============================================================
// DEFINISI STATE MACHINE
// ============================================================

/**
 * @enum DoorState
 * @brief Mendefinisikan state-state yang mungkin pada sistem pintu otomatis.
 *
 * Transisi antar state dikelola di dalam fungsi loop() berdasarkan
 * nilai jarak sensor dan kondisi timer aktif.
 */
enum DoorState {
  CLOSED,   ///< Pintu dalam kondisi tertutup penuh, menunggu deteksi objek
  OPENING,  ///< Pintu sedang dalam proses membuka (servo bergerak ke OPEN_ANGLE)
  OPEN,     ///< Pintu dalam kondisi terbuka penuh, menunggu objek pergi
  CLOSING   ///< Pintu sedang dalam proses menutup (servo bergerak ke CLOSE_ANGLE)
};

// ============================================================
// VARIABEL GLOBAL STATE
// ============================================================

DoorState doorState = CLOSED;   ///< State awal sistem saat boot

unsigned long stateTimer = 0;   ///< Timer untuk tracking durasi state OPEN
unsigned long servoTimer = 0;   ///< Timer non-blocking untuk gerakan servo

int servoAngle = CLOSE_ANGLE;   ///< Sudut servo saat ini (posisi aktual)

// ============================================================
// FUNGSI SETUP
// ============================================================

/**
 * @brief Inisialisasi sistem saat Arduino pertama kali dinyalakan.
 *
 * Mengkonfigurasi pin I/O, menginisialisasi servo motor ke posisi
 * tertutup, dan memulai komunikasi serial untuk debugging.
 */
void setup() {
  // Konfigurasi pin sensor ultrasonik
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  // Inisialisasi servo ke posisi tertutup
  doorServo.attach(SERVO_PIN);
  doorServo.write(CLOSE_ANGLE);

  // Inisialisasi komunikasi serial untuk debugging
  Serial.begin(9600);
  Serial.println("============================================");
  Serial.println("  Sistem Pintu Otomatis - by Mardiansyah  ");
  Serial.println("============================================");
  Serial.println("Sistem diinisialisasi. State: CLOSED");
  Serial.println("--------------------------------------------");
}

// ============================================================
// FUNGSI PENGUKURAN JARAK
// ============================================================

/**
 * @brief Mengukur jarak objek menggunakan sensor ultrasonik HC-SR04.
 *
 * Memancarkan pulsa ultrasonik 10 µs melalui pin TRIG, kemudian
 * mengukur lebar pulsa echo yang diterima. Jarak dihitung menggunakan
 * rumus: d = (durasi × kecepatan_suara) / 2, dimana kecepatan suara
 * ≈ 0.034 cm/µs pada suhu kamar.
 *
 * @note Timeout 30.000 µs setara dengan jangkauan ~510 cm.
 *       Nilai di atas jangkauan efektif HC-SR04 (400 cm) akan
 *       mengembalikan -1 untuk menandakan tidak ada objek.
 *
 * @return Jarak dalam centimeter (cm), atau -1 jika timeout / tidak ada objek.
 */
long getDistance() {
  // Pastikan pin TRIG dalam kondisi LOW sebelum pengukuran
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);

  // Kirim pulsa HIGH selama 10 µs untuk memicu pengukuran
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  // Ukur lebar pulsa echo dengan timeout 30.000 µs
  long duration = pulseIn(ECHO_PIN, HIGH, 30000);

  // Kembalikan -1 jika tidak ada echo (timeout)
  if (duration == 0) return -1;

  // Hitung dan kembalikan jarak dalam cm
  // Rumus: d = (t × v) / 2 = (durasi × 0.034) / 2
  return duration * 0.034 / 2;
}

// ============================================================
// FUNGSI LOOP UTAMA
// ============================================================

/**
 * @brief Loop utama program — menjalankan Finite State Machine (FSM).
 *
 * Setiap iterasi loop melakukan:
 *   1. Pengukuran jarak sensor ultrasonik
 *   2. Evaluasi transisi state berdasarkan jarak dan timer
 *   3. Eksekusi aksi servo sesuai state aktif
 *   4. Output status ke Serial Monitor
 *
 * Pendekatan non-blocking dengan millis() memastikan sistem
 * tetap responsif terhadap perubahan kondisi sensor kapan pun.
 */
void loop() {
  long distance = getDistance();         // Baca jarak dari sensor
  unsigned long now = millis();          // Timestamp saat ini

  // Tampilkan pembacaan jarak ke Serial Monitor
  Serial.print("Jarak: ");
  Serial.print(distance);
  Serial.println(" cm");

  // --------------------------------------------------------
  // EVALUASI STATE MACHINE
  // --------------------------------------------------------
  switch (doorState) {

    // -------------------------------------------------------
    // STATE: CLOSED — Pintu tertutup, menunggu deteksi objek
    // -------------------------------------------------------
    case CLOSED:
      if (distance > 0 && distance <= DIST_THRESHOLD) {
        doorState = OPENING;
        Serial.println("[STATE] CLOSED → OPENING");
      }
      break;

    // -------------------------------------------------------
    // STATE: OPENING — Servo bergerak dari 0° menuju OPEN_ANGLE
    // -------------------------------------------------------
    case OPENING:
      if (now - servoTimer >= SERVO_DELAY) {
        servoTimer = now;

        if (servoAngle < OPEN_ANGLE) {
          // Gerakkan servo satu langkah ke arah terbuka
          servoAngle += OPEN_STEP;

          // Clamp agar tidak melebihi sudut maksimum
          if (servoAngle > OPEN_ANGLE)
            servoAngle = OPEN_ANGLE;

          doorServo.write(servoAngle);

        } else {
          // Servo telah mencapai posisi terbuka penuh
          stateTimer = now;
          doorState = OPEN;
          Serial.println("[STATE] OPENING → OPEN");
        }
      }
      break;

    // -------------------------------------------------------
    // STATE: OPEN — Pintu terbuka, memantau keberadaan objek
    // -------------------------------------------------------
    case OPEN:
      // Jika objek masih terdeteksi, reset countdown timer
      if (distance > 0 && distance <= DIST_THRESHOLD) {
        stateTimer = now;
      }

      // Jika tidak ada objek selama OPEN_DELAY ms, mulai menutup
      if (now - stateTimer >= OPEN_DELAY) {
        doorState = CLOSING;
        Serial.println("[STATE] OPEN → CLOSING");
      }
      break;

    // -------------------------------------------------------
    // STATE: CLOSING — Servo bergerak dari OPEN_ANGLE menuju 0°
    // -------------------------------------------------------
    case CLOSING:
      // SAFETY CHECK: batalkan penutupan jika objek terdeteksi kembali
      if (distance > 0 && distance <= DIST_THRESHOLD) {
        doorState = OPENING;
        Serial.println("[SAFETY] Objek terdeteksi — CLOSING → OPENING");
        break;
      }

      if (now - servoTimer >= SERVO_DELAY) {
        servoTimer = now;

        if (servoAngle > CLOSE_ANGLE) {
          // Gerakkan servo satu langkah ke arah tertutup
          servoAngle -= CLOSE_STEP;

          // Clamp agar tidak kurang dari sudut minimum
          if (servoAngle < CLOSE_ANGLE)
            servoAngle = CLOSE_ANGLE;

          doorServo.write(servoAngle);

        } else {
          // Servo telah mencapai posisi tertutup penuh
          doorState = CLOSED;
          Serial.println("[STATE] CLOSING → CLOSED");
        }
      }
      break;
  }

  // Delay minimal untuk stabilitas pembacaan sensor
  delay(30);
}
