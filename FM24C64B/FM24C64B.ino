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