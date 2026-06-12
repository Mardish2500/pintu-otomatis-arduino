# Changelog

Semua perubahan pada proyek ini didokumentasikan dalam file ini.

Format mengikuti [Keep a Changelog](https://keepachangelog.com/en/1.0.0/)
dan proyek ini menggunakan [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

---

## [1.0.0] — 2025-06-12

### Added
- Implementasi awal sistem pintu otomatis berbasis sensor HC-SR04
- Finite State Machine (FSM) dengan 4 state: `CLOSED`, `OPENING`, `OPEN`, `CLOSING`
- Gerakan servo non-blocking menggunakan `millis()` (menggantikan `delay()`)
- Mekanisme keselamatan: pintu membuka ulang jika objek terdeteksi saat menutup
- Output monitoring state via Serial Monitor (baud rate 9600)
- Konfigurasi modular melalui konstanta `#define`
- Dokumentasi kode lengkap dengan Doxygen-style comments
- README profesional dan akademis
- File LICENSE (MIT)
- File .gitignore untuk proyek Arduino
- CHANGELOG

### Hardware Support
- Arduino Uno / Nano / Mega
- Sensor Ultrasonik HC-SR04
- Servo Motor SG90 / MG996R

---

## [Unreleased]

### Planned
- Penambahan dukungan LCD I2C untuk display status
- Mode hemat daya (low-power mode) dengan sleep Arduino
- Integrasi buzzer notifikasi
- Koneksi IoT via ESP8266/ESP32
