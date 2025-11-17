
-----

# Pustaka FRAM I2C FM24C64B (Arduino)

Pustaka (library) Arduino yang ringan dan efisien untuk membaca dan menulis ke chip F-RAM (Ferroelectric RAM) **FM24C64B** 64-Kbit (8KB) menggunakan I2C.

Pustaka ini dirancang agar sangat serbaguna, menggunakan *C++ template* untuk memungkinkan pembacaan dan penulisan tipe data apa pun (termasuk `float`, `long`, dan `struct` kustom) hanya dengan satu perintah.

## Fitur Utama

  * **Sangat Serbaguna:** Menggunakan fungsi *template* `writeObject()` dan `readObject()` untuk membaca/menulis **tipe data apa pun** (`float`, `int`, `long`, `bool`, `struct`) secara langsung.
  * **Penulisan Buffer Cerdas:** Fungsi `writeBuffer()` secara otomatis memecah data besar menjadi **potongan (chunks) 30-byte** untuk mencegah *buffer overflow* pada *library* `Wire.h` standar.
  * **Dukungan String:** Menyertakan fungsi *helper* `writeString()` dan `readString()` untuk objek `String` Arduino (menangani *null-terminator* secara otomatis).
  * **Pengalamatan 16-bit:** Mendukung penuh rentang alamat 8KB (0-8191) dari chip FM24C64B.
  * **Ringan:** Tidak memiliki *buffer* internal dan bergantung langsung pada `Wire.h`, sehingga hemat memori.
  * **Fleksibel:** Dapat digunakan dengan *port* I2C kustom (misalnya `Wire1` pada ESP32) dengan meneruskan objek `TwoWire` saat `begin()`.

-----

## Instalasi

1.  Unduh repositori ini sebagai file ZIP.
2.  Di Arduino IDE, buka **Sketch** \> **Include Library** \> **Add .ZIP Library...**
3.  Pilih file ZIP yang baru saja Anda unduh.
4.  Pustaka ini juga dapat diinstal secara manual dengan menempatkan folder `FM24C64B` di dalam folder `libraries` Arduino Anda (biasanya di `Documents/Arduino/libraries`).

-----

## Inisialisasi Pustaka

Anda harus menginisialisasi *library* `Wire` terlebih dahulu, terutama jika menggunakan ESP32 dengan pin kustom.

```cpp
#include "FM24C64B.h"
#include <Wire.h>

// Tentukan pin I2C Anda (contoh untuk ESP32)
#define I2C_SDA_PIN 21
#define I2C_SCL_PIN 22
#define I2C_FREQ 400000

// Buat objek FRAM
FM24C64B fram;

void setup() {
  Serial.begin(115200);

  // 1. Inisialisasi Wire (penting untuk ESP32)
  Wire.begin(I2C_SDA_PIN, I2C_SCL_PIN);
  
  [cite_start]// 2. Mulai library, teruskan objek Wire [cite: 5]
  //    (0x50 adalah alamat I2C default jika A0, A1, A2 = GND)
  fram.begin(0x50, Wire); 
  
  // 3. (Opsional) Atur kecepatan I2C
  fram.setClock(I2C_FREQ);

  Serial.println("FRAM Siap!");
}
```

-----

## Cara Penulisan & Pembacaan Data

Pustaka ini mempermudah penyimpanan tipe data kompleks.

### 1\. Tipe Data Primitif (float, long, int, dll)

Gunakan fungsi *template* `writeObject()` dan `readObject()`. Pustaka akan secara otomatis menghitung ukuran data (`sizeof`) dan menanganinya.

[cite\_start]**Contoh:** [cite: 7, 8, 13, 14]

```cpp
// --- TULIS ---
float f_tulis = -123.456f;
unsigned long l_tulis = 987654321UL;

fram.writeObject(0x100, f_tulis);   // Menulis float (4 byte) ke alamat 100
fram.writeObject(0x110, l_tulis);  // Menulis ulong (4 byte) ke alamat 110

// --- BACA ---
float f_baca = 0;
unsigned long l_baca = 0;

fram.readObject(0x100, f_baca);
fram.readObject(0x110, l_baca);

Serial.printf("Float dibaca: %f\n", f_baca);
Serial.printf("Long dibaca: %lu\n", l_baca);
```

### 2\. Objek String Arduino

Gunakan `writeString()` dan `readString()`. Fungsi ini secara otomatis menambahkan dan membaca *null-terminator* (`\0`).

[cite\_start]**Contoh:** [cite: 9, 15]

```cpp
// --- TULIS ---
String s_tulis = "Halo dari ESP32!";
fram.writeString(0x120, s_tulis);

// --- BACA ---
String s_baca = "";
[cite_start]// 50 adalah batas aman (maxLen) untuk mencegah pembacaan tak terbatas [cite: 16]
fram.readString(0x120, s_baca, 50); 

Serial.printf("String dibaca: \"%s\"\n", s_baca.c_str());
```

### 3\. Struct (Struktur Data Kustom)

Ini adalah fitur paling kuat. Anda dapat menggunakan `writeObject()` dan `readObject()` untuk menyimpan seluruh objek `struct` dalam satu panggilan.

**Contoh:**
[cite\_start]Pertama, definisikan `struct` Anda: [cite: 3]

```cpp
struct MyConfig {
  int deviceID;
  float calibration;
  char name[20];
};
```

[cite\_start]Kemudian, tulis dan baca struct tersebut: [cite: 10, 11, 12, 17]

```cpp
// --- TULIS ---
MyConfig config_tulis = { 101, 1.23, "Sensor_A" };
Serial.printf("Menulis struct: ID=%d, Cal=%.2f, Name=%s\n", 
              config_tulis.deviceID, config_tulis.calibration, config_tulis.name);
fram.writeObject(0x200, config_tulis);

// --- BACA ---
MyConfig config_baca; // Buat struct kosong
fram.readObject(0x200, config_baca);

Serial.printf("Struct dibaca: ID=%d, Cal=%.2f, Name=%s\n", 
              config_baca.deviceID, config_baca.calibration, config_baca.name);
```

### 4\. Level Rendah (Byte Tunggal & Buffer)

Anda juga dapat menggunakan fungsi level rendah jika diperlukan:

  * `fram.writeByte(alamat, data);`
  * `fram.readByte(alamat, variabel_data);`
  * `fram.writeBuffer(alamat, buffer_byte, panjang_buffer);`
  * `fram.readBuffer(alamat, buffer_byte, panjang_buffer);`

-----

## Mengenai Pengalamatan (Addressing)

  * **Rentang:** Chip FM24C64B memiliki **8192 byte** memori. Alamat yang valid adalah dari `0` hingga `8191`.
  * **Otomatis:** Pustaka ini secara otomatis menggunakan pengalamatan 16-bit (2-byte) yang diperlukan oleh chip ini.
  * **Manajemen Memori:** Anda bertanggung jawab untuk mengatur *layout* memori Anda. Pastikan data tidak tumpang tindih.

[cite\_start]**Contoh Layout Memori (dari file .ino):** [cite: 2]

```cpp
// Alamat didefinisikan sebagai titik AWAL
#define ADDR_FLOAT  100 // Menggunakan 4 byte (alamat 100, 101, 102, 103)
#define ADDR_LONG   110 // Menggunakan 4 byte (alamat 110, 111, 112, 113)
#define ADDR_STRING 120 // Menggunakan (panjang string + 1) byte (misal 17 byte)
#define ADDR_STRUCT 200 // Menggunakan sizeof(MyConfig) byte (misal 26 byte)
```

-----

## Contoh Program Lengkap (`.ino`)

[cite\_start]Berikut adalah contoh lengkap yang menguji penulisan dan pembacaan semua tipe data. [cite: 1]

```cpp
#include "FM24C64B.h"
#include <Wire.h>

// --- KONFIGURASI ---
// Ganti pin ini jika Anda menggunakan ESP32
#define I2C_SDA_PIN 21
#define I2C_SCL_PIN 22
#define I2C_FREQ 400000

// Buat objek FRAM
FM24C64B fram;

// Definisikan alamat
#define ADDR_FLOAT  100
#define ADDR_LONG   110
#define ADDR_STRING 120
#define ADDR_STRUCT 200

// Kita bahkan bisa menyimpan/membaca struct!
struct MyConfig {
  int deviceID;
  float calibration;
  char name[20];
};

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("\n--- Tes Baca/Tulis Semua Tipe Data ---");

  // Inisialisasi Wire untuk ESP32
  Wire.begin(I2C_SDA_PIN, I2C_SCL_PIN);
  
  // Mulai library (begin() sekarang mengambil objek Wire)
  fram.begin(0x50, Wire); 
  
  fram.setClock(I2C_FREQ);

  // ===================================
  // --- FASE TULIS ---
  // ===================================
  Serial.println("Menulis data ke FRAM...");

  // 1. Tulis FLOAT
  float f_tulis = -123.456f;
  Serial.printf("Menulis float: %f\n", f_tulis);
  fram.writeObject(ADDR_FLOAT, f_tulis);

  // 2. Tulis UNSIGNED LONG
  unsigned long l_tulis = 987654321UL;
  Serial.printf("Menulis long: %lu\n", l_tulis);
  fram.writeObject(ADDR_LONG, l_tulis);

  // 3. Tulis STRING
  String s_tulis = "Halo dari ESP32!";
  Serial.printf("Menulis string: \"%s\"\n", s_tulis.c_str());
  fram.writeString(ADDR_STRING, s_tulis);

  // 4. Tulis STRUCT
  MyConfig cfg_tulis = { 101, 1.23, "Sensor_A" };
  Serial.printf("Menulis struct: ID=%d, Cal=%.2f, Name=%s\n", 
                cfg_tulis.deviceID, cfg_tulis.calibration, cfg_tulis.name);
  fram.writeObject(ADDR_STRUCT, cfg_tulis);

  Serial.println("...Penulisan selesai.");
  Serial.println("-------------------------------------");

  // ===================================
  // --- FASE BACA ---
  // ===================================
  Serial.println("Membaca data kembali dari FRAM...");

  // 1. Baca FLOAT
  float f_baca = 0;
  fram.readObject(ADDR_FLOAT, f_baca);
  Serial.printf("Float dibaca: %f\n", f_baca);

  // 2. Baca UNSIGNED LONG
  unsigned long l_baca = 0;
  fram.readObject(ADDR_LONG, l_baca);
  Serial.printf("Long dibaca: %lu\n", l_baca);

  // 3. Baca STRING
  String s_baca = "";
  fram.readString(ADDR_STRING, s_baca, 50); // 50 adalah batas aman
  Serial.printf("String dibaca: \"%s\"\n", s_baca.c_str());

  // 4. Baca STRUCT
  MyConfig cfg_baca;
  fram.readObject(ADDR_STRUCT, cfg_baca);
  Serial.printf("Struct dibaca: ID=%d, Cal=%.2f, Name=%s\n", 
                cfg_baca.deviceID, cfg_baca.calibration, cfg_baca.name);

  // ===================================
  // --- VERIFIKASI ---
  // ===================================
  Serial.println("-------------------------------------");
  if (f_tulis == f_baca && 
      l_tulis == l_baca && 
      s_tulis == s_baca && 
      cfg_tulis.deviceID == cfg_baca.deviceID &&
      strcmp(cfg_tulis.name, cfg_baca.name) == 0) 
  {
    Serial.println("VERIFIKASI SUKSES! Semua data cocok.");
  } else {
    Serial.println("VERIFIKASI GAGAL! Data tidak cocok.");
  }
}

void loop() {
  // Tidak ada loop
}
```

## Lisensi

Proyek ini dilisensikan di bawah Lisensi MIT. Anda bebas menggunakan, memodifikasi, dan mendistribusikan kode ini.