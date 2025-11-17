<<<<<<< HEAD
﻿# ESP32 Flasher & Monitor Tool

Sebuah _software_ GUI sederhana yang dikembangkan untuk menyederhanakan proses _flashing_ (upload firmware), _erase_, dan _monitoring_ serial pada board ESP32.

Aplikasi ini dirancang khusus untuk lingkungan pabrik (Quality Control) agar operator dapat memprogram perangkat dengan cepat dan andal tanpa memerlukan setup teknis yang rumit.

Dikembangkan oleh: **FEBRI R&D**

## ✨ Fitur Utama

-   **Upload Sekali Klik:** Menggabungkan proses _flashing_ `bootloader`, `partitions`, dan `app` ke alamat memori yang benar secara otomatis.
    
-   **Erase Total:** Tombol sekali klik untuk menghapus seluruh memori flash ESP32.
    
-   **Serial Monitor & Logger:** Monitor serial terintegrasi untuk melihat output dari ESP32.
    
-   **Rekam ke Excel:** Merekam (logging) data serial monitor langsung ke file `.xlsx` lengkap dengan _timestamp_ untuk kebutuhan arsip dan analisis QC.
    
-   **Notifikasi Cerdas:** Popup yang ramah pengguna untuk status sukses, gagal, atau error umum (seperti "Port Sibuk" atau "Gagal Terhubung").
    
-   **Antarmuka Jelas:** Label yang jelas memisahkan "Serial Monitor" (output ESP32) dari "Process Log" (output _software_).
    

----------

## 👨‍💻 Untuk Developer: Menjalankan dari Script (.py)

Bagian ini menjelaskan cara menjalankan aplikasi ini di lingkungan pengembangan.

### 1. Prasyarat

-   [Python 3.10+](https://www.python.org/downloads/)
    
-   `pip` (Python package manager)
    
-   `git` (opsional, untuk kloning)
    

### 2. Langkah-langkah Instalasi

1.  **Clone Repository:**
    
    Bash
    
    ```
    git clone [URL_GITHUB_ANDA_DI_SINI]
    cd [NAMA_FOLDER_PROYEK_ANDA]
    
    ```
    
2.  Siapkan File Biner Esensial:
    
    Pastikan Anda menempatkan file bootloader.bin dan partitions.bin standar untuk ESP32 di dalam folder root proyek (di sebelah flasher_app.py).
    
3.  Install Library yang Dibutuhkan:
    
    Aplikasi ini bergantung pada beberapa library Python. Install semuanya menggunakan pip:
    
    Bash
    
    ```
    pip install customtkinter pyserial openpyxl
    
    ```
    

### 3. Cara Menjalankan

Setelah semua instalasi selesai, jalankan _script_ dengan perintah:

Bash

```
python flasher_app.py

```

----------

## 🏭 Untuk Operator : Panduan Pengguna Aplikasi (.exe)

Bagian ini menjelaskan cara menggunakan aplikasi yang sudah jadi (`.exe`) untuk keperluan produksi atau QC.

### 1. Instalasi

Tidak ada instalasi yang rumit.

Cukup copy file flasher_app.exe (yang sudah dibuat) ke komputer operator.

### 2. Tata Cara Penggunaan

1.  **Hubungkan Perangkat:** Colokkan board ESP32 ke komputer via kabel USB.
    
2.  **Buka Aplikasi:** Klik dua kali pada file `flasher_app.exe`.
    
3.  **Pilih COM Port:**
    
    -   Klik menu _dropdown_ **"COM Port"**.
        
    -   Pilih port yang benar (biasanya memiliki nama seperti `Silicon Labs` atau `CH340`).
        
    -   Jika port tidak muncul, klik tombol **"Refresh"**.
        
4.  **Info Bantuan:**
    
    -   Jika bingung, klik tombol **"Info"** di kanan atas untuk melihat panduan singkat di dalam aplikasi.
        

#### Untuk UPLOAD FIRMWARE (Aplikasi Tes)

1.  Klik tombol `...` (titik tiga) di sebelah "Firmware".
    
2.  Pilih file `.bin` aplikasi yang akan di-upload (misal: `tes_qc.bin`).
    
3.  Klik tombol besar **"UPLOAD FIRMWARE"**.
    
4.  Sebuah _progress bar_ akan bergerak, dan log proses akan muncul di kotak "Process Log" di bawah.
    
5.  Tunggu hingga muncul popup **"Upload Berhasil!"**.
    
6.  **Jika Gagal:** Popup akan menampilkan pesan error yang mudah dimengerti (misal: "Gagal Terhubung ke ESP32"). Coba tahan tombol "BOOT" di board sambil mengulang proses upload.
    

#### Untuk MELIHAT OUTPUT SERIAL (Logging)

1.  Pilih **"Baud Rate"** yang benar sesuai program Anda (umumnya `115200`).
    
2.  Klik tombol **"Connect Serial"**.
    
3.  Output dari ESP32 akan mulai muncul di kotak **"Serial Monitor & Logger"**.
    

#### Untuk MEREKAM LOG KE EXCEL

1.  Pastikan Anda sudah "Connect Serial" (langkah di atas).
    
2.  Klik tombol hijau **"Start Log"** untuk mulai merekam.
    
3.  Aplikasi akan menyimpan semua data serial yang masuk.
    
4.  Anda bisa menekan **"Pause Log"** untuk menjeda rekaman sementara.
    
5.  Jika tes sudah selesai, klik tombol merah **"Stop & Save"**.
    
6.  Aplikasi akan membuka jendela untuk menyimpan file `.xlsx`. Beri nama file Anda dan klik "Save".
    

----------

## 📦 Cara Build (Export ke .exe)

Jika Anda seorang _developer_ dan ingin meng-kompilasi ulang `.exe` dari script `.py`:

### Metode 1: `auto-py-to-exe` (Disarankan)

1.  Install: `pip install auto-py-to-exe`
    
2.  Jalankan: `auto-py-to-exe` di terminal.
    
3.  Atur konfigurasi di GUI:
    
    -   **Script Location:** Pilih `flasher_app.py`.
        
    -   **Onefile:** Pilih "One File".
        
    -   **Console Window:** Pilih "Window Based (hide the console)".
        
    -   **Additional Files:**
        
        -   "Add File": Pilih `bootloader.bin` (Destination: `.`)
            
        -   "Add File": Pilih `partitions.bin` (Destination: `.`)
            
        -   "Add Folder": Pilih folder `customtkinter` dari `site-packages` (Destination: `customtkinter/`)
            
4.  Klik "CONVERT .PY TO .EXE".
    

### Metode 2: PyInstaller (Command Line)

1.  Temukan path `customtkinter` Anda (jalankan `pip show customtkinter` dan lihat `Location`).
    
2.  Jalankan perintah ini (ganti `[PATH_ANDA]`):
    
    Bash
    
    ```
    pyinstaller --onefile --windowed --add-data "[PATH_ANDA_KE_SITE-PACKAGES]\customtkinter;customtkinter" --add-data "bootloader.bin;." --add-data "partitions.bin;." flasher_app.py
    
    ```
## 📦 Alur Kerja Rilis: Dari `.py` ke Installer `setup.exe`

Panduan ini mencatat langkah-langkah lengkap untuk mengubah script Python `flasher_app.py` menjadi satu file `setup.exe` yang dapat didistribusikan ke komputer operator QC.

Proses ini dibagi menjadi dua fase utama:

1.  **Fase 1 (Bundling):** Mengubah `flasher_app.py` (dan semua library-nya) menjadi satu file `flasher_app.exe`.
    
2.  **Fase 2 (Installer):** Membungkus `flasher_app.exe` menjadi `setup.exe` profesional dengan wizard instalasi.
    

----------

### Fase 1: Mengkonversi Script `.py` menjadi `.exe` (Bundling)

Kita menggunakan **`auto-py-to-exe`** untuk proses ini. Ini adalah antarmuka grafis (GUI) untuk PyInstaller yang memudahkan konfigurasi.

#### A. Persiapan (Hanya Dilakukan Sekali)

1.  Install auto-py-to-exe:
    
    Buka PowerShell/Terminal dan jalankan:
    
    Bash
    
    ```
    pip install auto-py-to-exe
    
    ```
    
2.  Pastikan Semua Library Terinstal:
    
    Pastikan environment Python Anda memiliki semua library yang dibutuhkan oleh script:
    
    Bash
    
    ```
    pip install customtkinter pyserial openpyxl
    
    ```
    

#### B. Langkah-Langkah Konversi

1.  Jalankan Aplikasi:
    
    Buka PowerShell/Terminal dan ketik:
    
    Bash
    
    ```
    auto-py-to-exe
    
    ```
    
    Ini akan membuka _tab_ baru di browser Anda.
    
2.  **Isi Konfigurasi di GUI:**
    
    -   **Script Location:** Klik "Browse", pilih file `flasher_app.py` Anda.
        
    -   **Onefile:** Pilih **"One File"**.
        
    -   **Console Window:** Pilih **"Window Based (hide the console)"**.
        
    -   **Additional Files:** Ini adalah bagian **paling penting** untuk menambahkan file-file yang tidak terdeteksi otomatis.
        
        -   Klik **"Add File"** -> Pilih `bootloader.bin`
            
            -   Kotak Kanan (Destination): Isi dengan `.` (satu titik)
                
        -   Klik **"Add File"** -> Pilih `partitions.bin`
            
            -   Kotak Kanan (Destination): Isi dengan `.` (satu titik)
                
        -   Klik **"Add Folder"** -> _Browse_ ke folder `customtkinter`
            
            -   Kotak Kanan (Destination): Isi dengan `customtkinter/`
                
            -   **Cara Cepat Menemukan Folder:** Jalankan `pip show customtkinter` di terminal, lihat `Location:`, dan _browse_ ke folder itu (misal: `C:\...\[python-env]\Lib\site-packages\customtkinter`).
                
3.  **Konversi:**
    
    -   Klik tombol biru besar **"CONVERT .PY TO .EXE"**.
        
    -   Tunggu proses selesai di kotak "Output".
        
4.  **Hasil:**
    
    -   Klik "Open Output Folder".
        
    -   Di dalamnya, Anda akan menemukan file `flasher_app.exe`. File ini sudah mandiri dan berisi `openpyxl`, `customtkinter`, dll.
        

----------

### Fase 2: Membuat Installer `.exe` (Inno Setup)

Kita menggunakan **`Inno Setup`** untuk membungkus `flasher_app.exe` (dari Fase 1) menjadi `setup.exe` yang profesional.

#### A. Persiapan (Hanya Dilakukan Sekali)

1.  **Install Inno Setup:**
    
    -   Download dan install dari situs resminya: **[Inno Setup Downloads](https://jrsoftware.org/isinfo.php)**
        

#### B. Langkah-Langkah Membuat Installer

1.  **Jalankan Inno Setup Compiler** (dari Start Menu).
    
2.  **Mulai Wizard:**
    
    -   Saat muncul _welcome screen_, pilih **"Create a new script file using the Script Wizard"**.
        
    -   Klik "OK".
        
3.  **Lengkapi Wizard:**
    
    -   **Application Information:**
        
        -   **Application Name:** `ESP32 Flasher & Monitor`
            
        -   **Application Version:** `1.0` (atau versi Anda)
            
        -   **Application Publisher:** `Febri R&D`
            
        -   (Biarkan sisanya _default_) -> Klik **Next**.
            
    -   **Application Files:**
        
        -   **(PENTING) Application main executable file:** Klik "Browse..." dan pilih file `flasher_app.exe` yang Anda buat di **Fase 1**.
            
        -   **(PENTING) Other application files:** **JANGAN TAMBAHKAN APA-APA LAGI.** Klik "Next". (Semua _library_ sudah ada _di dalam_ `flasher_app.exe`).
            
    -   **Application Shortcuts:**
        
        -   Centang **"Create a desktop shortcut"** jika Anda ingin ada ikon di Desktop.
            
        -   Klik **Next**.
            
    -   **Application Documentation:** (Bisa dikosongi) -> Klik **Next**.
        
    -   **Setup Languages:** (Biarkan _default_) -> Klik **Next**.
        
    -   **Compiler Settings:** (Biarkan _default_) -> Klik **Next**.
        
    -   **Script Selesai:** Klik **Finish**.
        
4.  **Compile (Kompilasi):**
    
    -   Sebuah _script_ (`.iss`) akan dibuatkan untuk Anda.
        
    -   Tekan tombol **"Compile"** (Ikon petir hijau) di _toolbar_ atas, atau tekan `Ctrl+F9`.
        
    -   Simpan _script_ `.iss` Anda (untuk diedit di masa depan).
        
    -   Inno Setup akan mulai bekerja.
        
5.  **Selesai:**
    
    -   Setelah selesai, sebuah folder `Output` akan dibuat (di sebelah _script_ `.iss` Anda).
        
    -   Di dalamnya, Anda akan menemukan satu file `setup.exe` (atau `unins000.exe`, tergantung nama _default_). File inilah yang Anda berikan ke tim QC untuk di-install di komputer mereka.
=======
﻿Tentu, ini adalah draf dokumentasi yang sangat baik untuk proyek Anda, yang ditulis dalam format `README.md` untuk GitHub.

Dokumentasi ini menjelaskan *library* Anda, arsitekturnya (termasuk *chunking*), dan cara menggunakan fungsi *template* Anda (`writeObject`/`readObject`) untuk menangani semua tipe data dengan mudah.

Anda bisa menyalin dan menempelkan teks di bawah ini langsung ke file `README.md` di repositori GitHub Anda.

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
[cite_start]// Definisikan alamat [cite: 2]
#define ADDR_FLOAT  100
#define ADDR_LONG   110
#define ADDR_STRING 120
#define ADDR_STRUCT 200

// Kita bahkan bisa menyimpan/membaca struct!
[cite_start]struct MyConfig { [cite: 3]
  int deviceID;
  float calibration;
  char name[20];
};

void setup() {
  Serial.begin(115200);
  delay(1000);
  [cite_start]Serial.println("\n--- Tes Baca/Tulis Semua Tipe Data ---"); [cite: 4]

  // Inisialisasi Wire untuk ESP32
  Wire.begin(I2C_SDA_PIN, I2C_SCL_PIN);
  
  [cite_start]// Mulai library (begin() sekarang mengambil objek Wire) [cite: 5]
  fram.begin(0x50, Wire); 
  
  fram.setClock(I2C_FREQ);

  // ===================================
  [cite_start]// --- FASE TULIS --- [cite: 6]
  // ===================================
  Serial.println("Menulis data ke FRAM...");

  [cite_start]// 1. Tulis FLOAT [cite: 7]
  float f_tulis = -123.456f;
  Serial.printf("Menulis float: %f\n", f_tulis);
  fram.writeObject(ADDR_FLOAT, f_tulis);

  [cite_start]// 2. Tulis UNSIGNED LONG [cite: 8]
  unsigned long l_tulis = 987654321UL;
  Serial.printf("Menulis long: %lu\n", l_tulis);
  fram.writeObject(ADDR_LONG, l_tulis);

  [cite_start]// 3. Tulis STRING [cite: 9]
  String s_tulis = "Halo dari ESP32!";
  Serial.printf("Menulis string: \"%s\"\n", s_tulis.c_str());
  fram.writeString(ADDR_STRING, s_tulis);

  [cite_start]// 4. Tulis STRUCT [cite: 10]
  MyConfig cfg_tulis = { 101, 1.23, "Sensor_A" };
  [cite_start]Serial.printf("Menulis struct: ID=%d, Cal=%.2f, Name=%s\n",  [cite: 11]
                cfg_tulis.deviceID, cfg_tulis.calibration, cfg_tulis.name);
  [cite_start]fram.writeObject(ADDR_STRUCT, cfg_tulis); [cite: 12]

  Serial.println("...Penulisan selesai.");
  Serial.println("-------------------------------------");

  // ===================================
  // --- FASE BACA ---
  // ===================================
  Serial.println("Membaca data kembali dari FRAM...");

  [cite_start]// 1. Baca FLOAT [cite: 13]
  float f_baca = 0;
  fram.readObject(ADDR_FLOAT, f_baca);
  Serial.printf("Float dibaca: %f\n", f_baca);

  [cite_start]// 2. Baca UNSIGNED LONG [cite: 14]
  unsigned long l_baca = 0;
  fram.readObject(ADDR_LONG, l_baca);
  Serial.printf("Long dibaca: %lu\n", l_baca);

  [cite_start]// 3. Baca STRING [cite: 15]
  String s_baca = "";
  fram.readString(ADDR_STRING, s_baca, 50); [cite_start]// 50 adalah batas aman [cite: 16]
  Serial.printf("String dibaca: \"%s\"\n", s_baca.c_str());

  // 4. Baca STRUCT
  MyConfig cfg_baca;
  fram.readObject(ADDR_STRUCT, cfg_baca);
  [cite_start]Serial.printf("Struct dibaca: ID=%d, Cal=%.2f, Name=%s\n",  [cite: 17]
                cfg_baca.deviceID, cfg_baca.calibration, cfg_baca.name);

  // ===================================
  [cite_start]// --- VERIFIKASI --- [cite: 18]
  // ===================================
  Serial.println("-------------------------------------");
  [cite_start]if (f_tulis == f_baca &&  [cite: 19]
      l_tulis == l_baca && 
      s_tulis == s_baca && 
      cfg_tulis.deviceID == cfg_baca.deviceID &&
      strcmp(cfg_tulis.name, cfg_baca.name) == 0) 
  {
    Serial.println("VERIFIKASI SUKSES! Semua data cocok.");
  [cite_start]} else { [cite: 20]
    Serial.println("VERIFIKASI GAGAL! Data tidak cocok.");
  }
}

void loop() {
  // Tidak ada loop
}
```

## Lisensi

Proyek ini dilisensikan di bawah Lisensi MIT. Anda bebas menggunakan, memodifikasi, dan mendistribusikan kode ini.
>>>>>>> 033cb3d67f07b219ba8da6b73b2e43a1dfbf7f8d
