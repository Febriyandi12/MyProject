
# Pustaka FRAM Universal Arduino (Fm24cxxb)

Sebuah pustaka (library) I2C Arduino yang ringan dan efisien untuk F-RAM (Ferroelectric RAM). Pustaka ini dirancang sebagai driver "universal" yang dapat bekerja dengan chip **FM24C16** (2KB) dan **FM24C64B** (8KB).

Fitur utamanya adalah API yang disederhanakan yang memungkinkan Anda membaca dan menulis **tipe data apa pun** (termasuk `float`, `int`, `long`, dan `struct` kustom) menggunakan satu fungsi *template*.

## Fitur Utama

  * **Dukungan Universal:** Bekerja dengan `FM24C16` (2KB) dan `FM24C64B` (8KB) hanya dengan mengubah satu parameter di `begin()`.
  * **Tulis/Baca Tipe Data Apa Saja:** Menggunakan fungsi *template* `writeObject()` dan `readObject()` untuk menangani **SEMUA** tipe data (`float`, `long`, `struct`, dll) secara otomatis.
  * **Pengalamatan Cerdas:** Secara internal, pustaka ini secara otomatis beralih antara pengalamatan **blok 1-byte** (untuk C16) dan pengalamatan **16-bit** (untuk C64).
  * **Penulisan Buffer Aman:** Secara otomatis memecah penulisan buffer besar menjadi potongan (chunks) **30-byte** untuk mencegah *buffer overflow* pada pustaka `Wire.h`.
  * **Dukungan String:** Termasuk fungsi *helper* `writeString()` dan `readString()` untuk objek `String` Arduino.
  * **Inisialisasi Fleksibel:** Dapat menerima objek `TwoWire` (seperti `Wire1` pada ESP32) saat inisialisasi.

## Instalasi

1.  Unduh repositori ini sebagai file ZIP.
2.  Di Arduino IDE, buka **Sketch** \> **Include Library** \> **Add .ZIP Library...**
3.  Pilih file ZIP yang baru saja Anda unduh.
4.  Anda juga dapat menginstal secara manual dengan menempatkan folder `Fm24cxxb` di dalam folder `libraries` Arduino Anda (biasanya di `Documents/Arduino/libraries`).

-----

## Panduan API

### 1\. Inisialisasi

Anda harus menginisialisasi pustaka dengan memberi tahu tipe chip yang Anda gunakan.

```cpp
#include "Fm24cxxb.h" //
#include <Wire.h>

// Buat objek FRAM
Fm24cxxb fram; //

// Tentukan pin Anda (contoh ESP32)
#define I2C_SDA_PIN 21
#define I2C_SCL_PIN 22

void setup() {
  // Inisialisasi Wire terlebih dahulu (penting untuk pin kustom)
  Wire.begin(I2C_SDA_PIN, I2C_SCL_PIN); //

  // --- PILIH CHIP ANDA DI SINI ---
  // Gunakan CHIP_FM24C64B untuk chip 8KB
  const FRAM_Chip_t chip = CHIP_FM24C64B; //
  
  // atau Gunakan CHIP_FM24C16 untuk chip 2KB
  // const FRAM_Chip_t chip = CHIP_FM24C16;
  
  // Mulai pustaka
  if (!fram.begin(chip, 0x50, Wire)) { //
    Serial.println("GAGAL! FRAM tidak terdeteksi.");
    while(1); //
  }
  
  // (Opsional) Atur kecepatan I2C
  fram.setClock(400000); //
  
  Serial.println("FRAM Siap!");
}
```

### 2\. Penulisan & Pembacaan (Semua Tipe Data)

Ini adalah cara yang disarankan untuk menyimpan dan mengambil data.

#### `char` dan `byte` (1-byte)

Gunakan `writeByte()` dan `readByte()`.

```cpp
// Tulis
char c_tulis = 'A'; //
fram.writeByte(ADDR_CHAR, c_tulis); //

// Baca
uint8_t c_baca; //
fram.readByte(ADDR_CHAR, c_baca); //
Serial.printf("Dibaca: %c\n", (char)c_baca); //
```

#### `int`, `long`, `float`, `unsigned int` (Multi-byte)

Gunakan `writeObject()` dan `readObject()`. Pustaka akan secara otomatis menangani ukuran data.

```cpp
// --- TULIS ---
int i_tulis = -32000; //
fram.writeObject(ADDR_INT, i_tulis); //

long l_tulis = -123456789; //
fram.writeObject(ADDR_LONG, l_tulis); //

float f_tulis = 3.14159; //
fram.writeObject(ADDR_FLOAT, f_tulis); //


// --- BACA ---
int i_baca;
fram.readObject(ADDR_INT, i_baca); //

long l_baca;
fram.readObject(ADDR_LONG, l_baca); //

float f_baca;
fram.readObject(ADDR_FLOAT, f_baca); //
```

#### `String`

Gunakan `writeString()` dan `readString()`. Fungsi ini secara otomatis menangani *null terminator* (`\0`).

```cpp
// --- TULIS ---
String s_tulis = "Tes Universal!"; //
fram.writeString(ADDR_STRING, s_tulis); //

// --- BACA ---
String s_baca;
// 50 adalah batas aman (maxLen) untuk mencegah pembacaan tak terbatas
fram.readString(ADDR_STRING, s_baca, 50); // 
```

#### `struct` (Struktur Data Kustom)

Ini adalah fitur paling kuat. `writeObject()` dan `readObject()` juga dapat menangani `struct` kustom.

```cpp
// Definisikan struct Anda
struct MyConfig {
  int deviceID;
  float calibration; //
};

// --- TULIS ---
MyConfig cfg_tulis = { 101, 1.23f }; //
fram.writeObject(ADDR_STRUCT, cfg_tulis); //

// --- BACA ---
MyConfig cfg_baca; // Buat struct kosong
fram.readObject(ADDR_STRUCT, cfg_baca); //

// Verifikasi
Serial.printf("Struct dibaca: ID=%d, Cal=%.2f\n",
              cfg_baca.deviceID, cfg_baca.calibration); //
```

-----

## Penjelasan Internal: Cara Kerja Pengalamatan

Pustaka ini secara otomatis menangani dua metode pengalamatan yang sangat berbeda di dalam fungsi `writeBuffer` dan `readBuffer`:

1.  **Jika `_chipType == CHIP_FM24C64B`:**

      * Pustaka menggunakan **pengalamatan 16-bit**.
      * Untuk mengakses alamat `memAddr`, ia mengirim:
        `[I2C_ADDR]` `[ADDR_HI]` `[ADDR_LO]` `[DATA...]`

2.  **Jika `_chipType == CHIP_FM24C16`:**

      * Pustaka menggunakan **pengalamatan blok 1-byte**.
      * Untuk mengakses alamat `memAddr`, ia menghitung:
          * `i2c_block_addr` = `0x50` ATAU `(memAddr >> 8)`
          * `offset` = `memAddr & 0xFF` (hanya 1 byte)
      * Ia lalu mengirim:
        `[i2c_block_addr]` `[offset]` `[DATA...]`

Anda tidak perlu khawatir tentang ini; Anda cukup memanggil `fram.writeObject()` dan pustaka akan menanganinya secara otomatis.

-----

## Contoh Program Lengkap

Kode berikut (`FRAM_lib.ino`) adalah contoh sempurna yang menguji penulisan, pembacaan, dan verifikasi semua tipe data yang didukung.

```cpp
/*
 * Program Dummy: Tes Pustaka FRAM Universal
 * Menulis dan Membaca SEMUA tipe data.
 */
#include "Fm24cxxb.h" //
#include <Wire.h>

// --- KONFIGURASI ---
#define I2C_SDA_PIN 21
#define I2C_SCL_PIN 22
#define I2C_FREQ 400000

// --- PILIH CHIP DI SINI! ---
// Ganti ini sesuai chip yang terpasang di hardware Anda
const FRAM_Chip_t CHIP_FM24CXXB = CHIP_FM24C64B; //
// const FRAM_Chip_t CHIP_FM24CXXB = CHIP_FM24C16;

// Buat objek FRAM
Fm24cxxb fram; //

// --- Alamat Memori ---
#define ADDR_CHAR   10
#define ADDR_BYTE   12
#define ADDR_INT    14
#define ADDR_UINT   20
#define ADDR_LONG   26
#define ADDR_ULONG  32
#define ADDR_FLOAT  38
#define ADDR_STRING 44
#define ADDR_STRUCT 100

// Struct kustom untuk tes
struct MyConfig {
  int deviceID;
  float calibration; //
};

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("\n--- Tes Pustaka FRAM Universal ---"); //

  // Inisialisasi Wire dan Library
  Wire.begin(I2C_SDA_PIN, I2C_SCL_PIN); //
  
  // Beri tahu library chip apa yang kita gunakan
  if (!fram.begin(CHIP_FM24CXXB, 0x50, Wire)) { //
    Serial.println("GAGAL! FRAM tidak terdeteksi. Program berhenti.");
    while(1); //
  }
  
  if (CHIP_FM24CXXB == CHIP_FM24C16) {
    Serial.println("Mode: CHIP_FM24C16 (2KB)"); //
  } else {
    Serial.println("Mode: CHIP_FM24C64B (8KB)"); //
  }
  
  fram.setClock(I2C_FREQ); //

  // ===================================
  // --- FASE TULIS ---
  // ===================================
  Serial.println("Menulis semua tipe data ke FRAM..."); //

  char c_tulis = 'A'; //
  fram.writeByte(ADDR_CHAR, c_tulis); //

  byte b_tulis = 0xDE; //
  fram.writeByte(ADDR_BYTE, b_tulis); //

  int i_tulis = -32000; //
  fram.writeObject(ADDR_INT, i_tulis); //

  unsigned int ui_tulis = 65000; //
  fram.writeObject(ADDR_UINT, ui_tulis); //

  long l_tulis = -123456789; //
  fram.writeObject(ADDR_LONG, l_tulis); //

  unsigned long ul_tulis = 3210987654; //
  fram.writeObject(ADDR_ULONG, ul_tulis); //

  float f_tulis = 3.14159; //
  fram.writeObject(ADDR_FLOAT, f_tulis); //

  String s_tulis = "Tes Universal!"; //
  fram.writeString(ADDR_STRING, s_tulis); //

  MyConfig cfg_tulis = { 101, 1.23f }; //
  fram.writeObject(ADDR_STRUCT, cfg_tulis); //

  Serial.println("...Penulisan selesai.");
  Serial.println("-------------------------------------"); //

  // ===================================
  // --- FASE BACA ---
  // ===================================
  Serial.println("Membaca data kembali dari FRAM..."); //

  uint8_t c_baca; //
  fram.readByte(ADDR_CHAR, c_baca); //
  Serial.printf("CHAR:   Ditulis: %c, Dibaca: %c\n", c_tulis, (char)c_baca); //

  byte b_baca; fram.readByte(ADDR_BYTE, b_baca); //
  Serial.printf("BYTE:   Ditulis: 0x%X, Dibaca: 0x%X\n", b_tulis, b_baca); //

  int i_baca; fram.readObject(ADDR_INT, i_baca); //
  Serial.printf("INT:    Ditulis: %d, Dibaca: %d\n", i_tulis, i_baca); //

  unsigned int ui_baca; fram.readObject(ADDR_UINT, ui_baca); //
  Serial.printf("UINT:   Ditulis: %u, Dibaca: %u\n", ui_tulis, ui_baca); //

  long l_baca; fram.readObject(ADDR_LONG, l_baca); //
  Serial.printf("LONG:   Ditulis: %ld, Dibaca: %ld\n", l_tulis, l_baca); //

  unsigned long ul_baca; fram.readObject(ADDR_ULONG, ul_baca); //
  Serial.printf("ULONG:  Ditulis: %lu, Dibaca: %lu\n", ul_tulis, ul_baca); //

  float f_baca; fram.readObject(ADDR_FLOAT, f_baca); //
  Serial.printf("FLOAT:  Ditulis: %f, Dibaca: %f\n", f_tulis, f_baca); //

  String s_baca; fram.readString(ADDR_STRING, s_baca, 50); //
  Serial.printf("STRING: Ditulis: %s, Dibaca: %s\n", s_tulis.c_str(), s_baca.c_str()); //

  MyConfig cfg_baca; fram.readObject(ADDR_STRUCT, cfg_baca); //
  Serial.printf("STRUCT: Ditulis: {ID:%d, Cal:%.2f}, Dibaca: {ID:%d, Cal:%.2f}\n", //
                cfg_tulis.deviceID, cfg_tulis.calibration,
                cfg_baca.deviceID, cfg_baca.calibration);

  // ===================================
  // --- VERIFIKASI ---
  // ===================================
  Serial.println("-------------------------------------"); //
  if (c_tulis == c_baca && b_tulis == b_baca && i_tulis == i_baca &&
      ui_tulis == ui_baca && l_tulis == l_baca && ul_tulis == ul_baca &&
      f_tulis == f_baca && s_tulis == s_baca && 
      cfg_tulis.deviceID == cfg_baca.deviceID) { //
    Serial.println("✅ VERIFIKASI SUKSES! Semua data cocok."); //
  } else {
    Serial.println("❌ VERIFIKASI GAGAL! Data tidak cocok."); //
  }
}

void loop() {
  // Tidak ada loop
}
```

## Lisensi

Proyek ini dilisensikan di bawah Lisensi MIT.