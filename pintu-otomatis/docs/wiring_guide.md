# Panduan Wiring — Sistem Pintu Otomatis

**Author:** Mardiansyah  
**Versi Dokumen:** 1.0.0

---

## Daftar Komponen

| Komponen | Jumlah | Keterangan |
|----------|--------|------------|
| Arduino Uno/Nano/Mega | 1 | Mikrokontroler utama |
| HC-SR04 | 1 | Sensor ultrasonik |
| Servo SG90 / MG996R | 1 | Aktuator pintu |
| Breadboard | 1 | Opsional, untuk prototyping |
| Kabel Jumper | ~10 | Male-to-Male / Male-to-Female |

---

## Koneksi HC-SR04

```
HC-SR04          Arduino
──────────────────────────
VCC       →      5V
GND       →      GND
TRIG      →      Pin 11
ECHO      →      Pin 10
```

> ⚠️ **Peringatan:** HC-SR04 beroperasi pada logika 5V. Jika menggunakan
> Arduino 3.3V (seperti Arduino Due), pasang voltage divider resistor pada
> pin ECHO agar tidak merusak mikrokontroler.

---

## Koneksi Servo Motor

```
Servo            Arduino
──────────────────────────
Merah (VCC)  →   5V
Hitam (GND)  →   GND
Kuning/Oranye → Pin 3 (PWM)
```

> ⚠️ **Catatan Penting:** Servo motor SG90 menarik arus hingga 500 mA saat
> bergerak. Jika menggunakan servo yang lebih besar (MG996R ~2.5A), gunakan
> catu daya eksternal 5V dan hubungkan GND-nya ke GND Arduino.

---

## Diagram Blok Sistem

```
                 ┌─────────────────────────────────┐
                 │         ARDUINO UNO/NANO          │
                 │                                   │
  HC-SR04        │  Pin 11 ──── TRIG               │
  ┌──────┐       │  Pin 10 ──── ECHO               │       Servo
  │ VCC  │──5V───┤                                   │       ┌──────┐
  │ GND  │──GND──┤  Pin  3 ──── Signal ─────────────┼───────│Signal│
  │ TRIG │───────┤                                   │       │ VCC  │──5V
  │ ECHO │───────┤  5V   ──── VCC Servo             │       │ GND  │──GND
  └──────┘       │  GND  ──── GND Servo             │       └──────┘
                 └─────────────────────────────────┘
                                │
                          USB / Serial
                                │
                         PC / Laptop
                       (Serial Monitor)
```

---

## Tips Prototyping

1. Uji sensor HC-SR04 terlebih dahulu secara terpisah sebelum menggabungkan ke sistem lengkap.
2. Gunakan `Serial.println(distance)` untuk memverifikasi pembacaan jarak akurat.
3. Sesuaikan `DIST_THRESHOLD` sesuai kebutuhan jangkauan deteksi nyata.
4. Pastikan servo motor tidak mengalami *binding* (hambatan mekanik) pada posisi 0° dan 90°.
