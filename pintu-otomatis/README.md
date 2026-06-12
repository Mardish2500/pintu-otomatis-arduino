# 🚪 Sistem Pintu Otomatis Berbasis Sensor Ultrasonik HC-SR04

<div align="center">

![Arduino](https://img.shields.io/badge/Platform-Arduino-00979D?style=for-the-badge&logo=arduino&logoColor=white)
![C++](https://img.shields.io/badge/Language-C%2B%2B-00599C?style=for-the-badge&logo=c%2B%2B&logoColor=white)
![License](https://img.shields.io/badge/License-MIT-green?style=for-the-badge)
![Status](https://img.shields.io/badge/Status-Active-brightgreen?style=for-the-badge)

**Sistem kontrol pintu otomatis menggunakan mikrokontroler Arduino dengan sensor jarak ultrasonik HC-SR04 dan aktuator servo motor.**

*Proyek Sistem Tertanam (Embedded System) — Teknik Informatika / Teknik Elektro*

</div>

---

## 📋 Daftar Isi

- [Deskripsi Proyek](#-deskripsi-proyek)
- [Fitur Utama](#-fitur-utama)
- [Arsitektur Sistem](#-arsitektur-sistem)
- [Komponen Hardware](#-komponen-hardware)
- [Skema Wiring](#-skema-wiring)
- [Instalasi & Penggunaan](#-instalasi--penggunaan)
- [State Machine](#-state-machine)
- [Konfigurasi Parameter](#-konfigurasi-parameter)
- [Output Serial Monitor](#-output-serial-monitor)
- [Troubleshooting](#-troubleshooting)
- [Pengembangan Lanjutan](#-pengembangan-lanjutan)
- [Referensi](#-referensi)
- [Lisensi](#-lisensi)
- [Author](#-author)

---

## 📌 Deskripsi Proyek

Proyek ini merupakan implementasi **sistem pintu otomatis** (*automatic door system*) yang memanfaatkan teknologi sensor ultrasonik untuk mendeteksi kehadiran objek atau manusia di depan pintu. Sistem dirancang menggunakan pendekatan **Finite State Machine (FSM)** yang memungkinkan manajemen state yang terstruktur dan responsif terhadap perubahan kondisi secara *real-time*.

Sistem bekerja dengan prinsip berikut:
1. Sensor ultrasonik HC-SR04 secara kontinu memancarkan gelombang ultrasonik dan mengukur waktu pantulannya.
2. Apabila jarak objek terdeteksi **≤ 15 cm**, sistem secara otomatis membuka pintu dengan menggerakkan servo motor secara bertahap (*smooth motion*).
3. Pintu akan tetap terbuka selama objek masih terdeteksi dalam jangkauan sensor.
4. Setelah objek meninggalkan area deteksi selama **5 detik**, pintu menutup kembali secara otomatis.
5. Terdapat mekanisme **keselamatan aktif** (*safety mechanism*): apabila objek terdeteksi kembali saat pintu sedang menutup, proses penutupan dihentikan dan pintu membuka ulang.

> **Relevansi Akademis:** Proyek ini mengimplementasikan konsep-konsep inti dalam bidang sistem tertanam, meliputi: pemrograman berbasis event, manajemen state dengan FSM, antarmuka sensor-aktuator, dan penggunaan timer non-blocking dengan `millis()`.

---

## ✨ Fitur Utama

| Fitur | Deskripsi |
|-------|-----------|
| **Deteksi Otomatis** | Mendeteksi objek/manusia dalam radius ≤ 15 cm menggunakan sensor HC-SR04 |
| **Gerakan Halus (Smooth Motion)** | Servo bergerak secara bertahap (*step-by-step*) untuk menghindari gerakan kasar |
| **Non-Blocking Timer** | Menggunakan `millis()` sebagai pengganti `delay()` untuk responsivitas sistem |
| **Finite State Machine** | Manajemen 4 state: `CLOSED`, `OPENING`, `OPEN`, `CLOSING` |
| **Safety Mechanism** | Pintu membuka ulang otomatis jika objek terdeteksi saat proses menutup |
| **Serial Monitoring** | Output debug via Serial Monitor untuk pemantauan sistem secara langsung |
| **Konfigurasi Modular** | Semua parameter kritis didefinisikan sebagai konstanta (`#define`) yang mudah diubah |

---

## 🏗️ Arsitektur Sistem

```
┌─────────────────────────────────────────────────────────┐
│                   SISTEM PINTU OTOMATIS                  │
│                                                         │
│   ┌──────────────┐     ┌──────────────┐                │
│   │  Sensor      │     │  Mikro-      │                │
│   │  HC-SR04     │────▶│  kontroler   │                │
│   │  (Input)     │     │  Arduino     │                │
│   └──────────────┘     │  Uno/Nano    │                │
│                        │              │────▶ Servo      │
│   ┌──────────────┐     │  FSM Engine  │      Motor     │
│   │  Serial      │◀────│  (Proses)    │     (Output)   │
│   │  Monitor     │     └──────────────┘                │
│   │  (Debug)     │                                     │
│   └──────────────┘                                     │
└─────────────────────────────────────────────────────────┘
```

**Alur Data:**
```
HC-SR04 → [Sinyal Echo] → Arduino (getDistance()) → [Jarak cm]
→ FSM (doorState) → [Sudut Servo] → Servo Motor → [Posisi Pintu]
```

---

## 🔧 Komponen Hardware

| No. | Komponen | Spesifikasi | Jumlah |
|-----|----------|-------------|--------|
| 1 | Mikrokontroler | Arduino Uno / Nano / Mega | 1 |
| 2 | Sensor Ultrasonik | HC-SR04 (2 cm – 400 cm) | 1 |
| 3 | Servo Motor | SG90 / MG996R (0° – 180°) | 1 |
| 4 | Kabel Jumper | Male-to-Male / Male-to-Female | Secukupnya |
| 5 | Breadboard | 400/830 titik | 1 |
| 6 | Sumber Daya | USB 5V / Adaptor 5V-2A | 1 |

> **Catatan:** Untuk servo motor dengan torsi lebih besar (MG996R), disarankan menggunakan sumber daya eksternal 5V terpisah agar tidak membebani pin 5V Arduino.

---

## 🔌 Skema Wiring

### HC-SR04 → Arduino

| Pin HC-SR04 | Pin Arduino |
|-------------|-------------|
| VCC         | 5V          |
| GND         | GND         |
| TRIG        | Pin 11      |
| ECHO        | Pin 10      |

### Servo Motor → Arduino

| Pin Servo       | Pin Arduino |
|-----------------|-------------|
| Merah (VCC)     | 5V          |
| Coklat/Hitam (GND) | GND      |
| Oranye/Kuning (Signal) | Pin 3 |

```
Arduino Uno
    ┌──────────────┐
    │          11  │──── TRIG (HC-SR04)
    │          10  │──── ECHO (HC-SR04)
    │           3  │──── Signal (Servo)
    │          5V  │──── VCC (HC-SR04 & Servo)
    │         GND  │──── GND (Semua komponen)
    └──────────────┘
```

---

## 🚀 Instalasi & Penggunaan

### Prasyarat

- **Arduino IDE** versi 1.8.x atau 2.x ([Download](https://www.arduino.cc/en/software))
- Library `Servo.h` (sudah tersedia secara default di Arduino IDE)

### Langkah Instalasi

1. **Clone repositori ini:**
   ```bash
   git clone https://github.com/mardiansyah/pintu-otomatis.git
   cd pintu-otomatis
   ```

2. **Buka file sketch:**
   ```
   Buka Arduino IDE → File → Open → pilih file: src/pintu_otomatis.ino
   ```

3. **Hubungkan hardware** sesuai skema wiring di atas.

4. **Pilih board dan port:**
   ```
   Tools → Board → Arduino Uno (atau sesuai board yang digunakan)
   Tools → Port → COMx (Windows) / /dev/ttyUSBx (Linux/Mac)
   ```

5. **Upload sketch:**
   ```
   Klik tombol Upload (→) atau tekan Ctrl+U
   ```

6. **Pantau output:**
   ```
   Tools → Serial Monitor → Set baud rate: 9600
   ```

---

## 🔄 State Machine

Sistem menggunakan **Finite State Machine (FSM)** dengan 4 state utama:

```
                ┌─────────────────────────────────────────┐
                │                                         │
                ▼                                         │
         ┌────────────┐   objek terdeteksi          ┌────────────┐
         │            │   (jarak ≤ 15 cm)            │            │
         │   CLOSED   │ ─────────────────────────▶  │  OPENING  │
         │            │                              │            │
         └────────────┘                              └────────────┘
                ▲                                         │
                │                                   servo = 90°
                │                                         │
                │                                         ▼
         ┌────────────┐   tidak ada objek 5 dtk    ┌────────────┐
         │            │   ◀──────────────────────── │            │
         │  CLOSING   │                              │    OPEN    │
         │            │                              │            │
         └────────────┘                              └────────────┘
                │                ▲
                │                │
                └────────────────┘
          objek terdeteksi saat menutup
              → kembali ke OPENING
```

### Deskripsi Transisi State

| State | Kondisi Masuk | Kondisi Keluar | Aksi |
|-------|---------------|----------------|------|
| `CLOSED` | Default / setelah `CLOSING` | Jarak ≤ 15 cm | Tidak ada |
| `OPENING` | Objek terdeteksi | Servo mencapai 90° | Putar servo +3°/step |
| `OPEN` | Servo = 90° | Tidak ada objek ≥ 5 detik | Reset timer jika ada objek |
| `CLOSING` | Timer habis | Servo mencapai 0° / objek terdeteksi | Putar servo -1°/step |

---

## ⚙️ Konfigurasi Parameter

Semua parameter dapat dikustomisasi melalui konstanta `#define` di bagian atas file:

```cpp
// === PIN CONFIGURATION ===
#define TRIG_PIN      11    // Pin trigger sensor HC-SR04
#define ECHO_PIN      10    // Pin echo sensor HC-SR04
#define SERVO_PIN      3    // Pin sinyal servo motor

// === SERVO ANGLE ===
#define OPEN_ANGLE    90    // Sudut servo posisi pintu terbuka (derajat)
#define CLOSE_ANGLE    0    // Sudut servo posisi pintu tertutup (derajat)

// === TIMING & THRESHOLD ===
#define DIST_THRESHOLD 15   // Jarak deteksi objek (cm) — nilai lebih besar = area lebih luas
#define OPEN_DELAY   5000   // Durasi pintu tetap terbuka setelah objek pergi (ms)

// === SERVO MOTION ===
#define OPEN_STEP      3    // Kecepatan pembukaan pintu (derajat/step) — lebih besar = lebih cepat
#define CLOSE_STEP     1    // Kecepatan penutupan pintu (derajat/step) — lebih kecil = lebih halus
#define SERVO_DELAY   10    // Interval antar step servo (ms)
```

> **Tips Kustomisasi:**
> - Untuk memperlambat gerakan servo, perkecil nilai `OPEN_STEP`/`CLOSE_STEP` atau perbesar `SERVO_DELAY`.
> - Untuk memperluas jangkauan deteksi, perbesar nilai `DIST_THRESHOLD`.
> - Sesuaikan `OPEN_ANGLE` dan `CLOSE_ANGLE` dengan orientasi pemasangan servo.

---

## 📊 Output Serial Monitor

Contoh output yang ditampilkan pada Serial Monitor (baud rate: 9600):

```
Jarak: 45 cm
Jarak: 42 cm
Jarak: 12 cm
STATE: OPENING
Jarak: 10 cm
Jarak: 9 cm
STATE: OPEN
Jarak: 8 cm
Jarak: 32 cm
Jarak: 45 cm
STATE: CLOSING
OBJEK TERDETEKSI - BUKA LAGI
STATE: OPENING
STATE: OPEN
...
STATE: CLOSING
STATE: CLOSED
```

---

## 🛠️ Troubleshooting

| Masalah | Kemungkinan Penyebab | Solusi |
|---------|----------------------|--------|
| Servo tidak bergerak | Kabel signal tidak terhubung | Cek wiring pin 3 ke servo |
| Sensor selalu menampilkan -1 | Kabel TRIG/ECHO terbalik | Cek koneksi pin 11 dan 10 |
| Pintu tidak menutup | `OPEN_DELAY` terlalu besar | Turunkan nilai `OPEN_DELAY` |
| Gerakan servo tersentak | `SERVO_DELAY` terlalu kecil | Naikkan nilai `SERVO_DELAY` |
| Arduino reset saat servo aktif | Arus servo membebani Arduino | Gunakan power supply eksternal untuk servo |
| Jarak tidak akurat | Permukaan objek menyerap gelombang | Gunakan permukaan datar dan padat sebagai pengujian |

---

## 🔮 Pengembangan Lanjutan

Beberapa ide pengembangan yang dapat diterapkan sebagai perbaikan atau penelitian lanjutan:

- [ ] **Penambahan LCD Display** — Menampilkan status pintu dan jarak secara visual
- [ ] **Integrasi Buzzer** — Memberikan notifikasi audio saat pintu membuka/menutup
- [ ] **Sensor PIR sebagai Alternatif** — Menggunakan Passive Infrared Sensor untuk deteksi gerak
- [ ] **Mode Malam (Jam Operasional)** — Menggunakan RTC DS3231 untuk membatasi jam operasi
- [ ] **Konektivitas IoT** — Integrasi ESP8266/ESP32 untuk monitoring jarak jauh via MQTT/HTTP
- [ ] **Logging Data** — Menyimpan log aktivitas pintu ke SD Card untuk analisis
- [ ] **Keamanan Akses** — Menambahkan RFID atau keypad untuk autentikasi pengguna

---

## 📚 Referensi

1. Arduino Documentation. (2024). *Servo Library Reference*. https://www.arduino.cc/reference/en/libraries/servo/
2. Arduino Documentation. (2024). *millis() Function*. https://www.arduino.cc/reference/en/language/functions/time/millis/
3. Elecfreaks. (2022). *HC-SR04 Ultrasonic Sensor Datasheet*. Elecfreaks Technology Co., Ltd.
4. Margolis, M. (2020). *Arduino Cookbook: Recipes to Begin, Expand, and Enhance Your Projects* (3rd ed.). O'Reilly Media.
5. Monk, S. (2021). *Programming Arduino: Getting Started with Sketches* (3rd ed.). McGraw-Hill Education.
6. Gamma, E., Helm, R., Johnson, R., & Vlissides, J. (1994). *Design Patterns: Elements of Reusable Object-Oriented Software*. Addison-Wesley. *(Referensi pola State Machine)*

---

## 📄 Lisensi

Proyek ini dilisensikan di bawah **MIT License** — lihat file [LICENSE](LICENSE) untuk detail lengkap.

```
MIT License — Copyright (c) 2025 Mardiansyah
Bebas digunakan, dimodifikasi, dan didistribusikan dengan menyertakan atribusi.
```

---

## 👨‍💻 Author

<div align="center">

| | |
|---|---|
| **Nama** | Mardiansyah |
| **Role** | Embedded Systems Engineer / Developer |
| **GitHub** | [@mardiansyah](https://github.com/mardiansyah) |
| **Proyek** | Sistem Pintu Otomatis Berbasis Arduino |

</div>

---

<div align="center">

**Dibuat dengan ❤️ menggunakan Arduino**

*"Automation is not about replacing humans — it's about enhancing human experience."*

⭐ Jika proyek ini bermanfaat, berikan bintang pada repositori ini!

</div>
