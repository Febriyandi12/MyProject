
#include "Fm24cxxb.h"
#include <Wire.h>

// --- KONFIGURASI ---
#define I2C_SDA_PIN 21
#define I2C_SCL_PIN 22
#define I2C_FREQ 400000

// --- PILIH CHIP DI SINI! ---
// Ganti ini sesuai chip yang terpasang di hardware Anda
const FRAM_Chip_t CHIP_FM24CXXB = CHIP_FM24C64B;
// const FRAM_Chip_t CHIP_FM24CXXB = CHIP_FM24C16;

// Buat objek FRAM
Fm24cxxb fram;

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
  float calibration;
};

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("\n--- Tes Pustaka FRAM Universal ---");

  // Inisialisasi Wire dan Library
  Wire.begin(I2C_SDA_PIN, I2C_SCL_PIN);
  
  // Beri tahu library chip apa yang kita gunakan
  if (!fram.begin(CHIP_FM24CXXB, 0x50, Wire)) {
    Serial.println("GAGAL! FRAM tidak terdeteksi. Program berhenti.");
    while(1);
  }
  
  if (CHIP_FM24CXXB == CHIP_FM24C16) {
    Serial.println("Mode: CHIP_FM24C16 (2KB)");
  } else {
    Serial.println("Mode: CHIP_FM24C64B (8KB)");
  }
  
  fram.setClock(I2C_FREQ);

  // ===================================
  // --- FASE TULIS ---
  // ===================================
  Serial.println("Menulis semua tipe data ke FRAM...");

  char c_tulis = 'A';
  fram.writeByte(ADDR_CHAR, c_tulis);

  byte b_tulis = 0xDE;
  fram.writeByte(ADDR_BYTE, b_tulis);

  int i_tulis = -32000;
  fram.writeObject(ADDR_INT, i_tulis); // API Template

  unsigned int ui_tulis = 65000;
  fram.writeObject(ADDR_UINT, ui_tulis); // API Template

  long l_tulis = -123456789;
  fram.writeObject(ADDR_LONG, l_tulis); // API Template

  unsigned long ul_tulis = 3210987654;
  fram.writeObject(ADDR_ULONG, ul_tulis); // API Template

  float f_tulis = 3.14159;
  fram.writeObject(ADDR_FLOAT, f_tulis); // API Template

  String s_tulis = "Tes Universal!";
  fram.writeString(ADDR_STRING, s_tulis);

  MyConfig cfg_tulis = { 101, 1.23f };
  fram.writeObject(ADDR_STRUCT, cfg_tulis); // API Template

  Serial.println("...Penulisan selesai.");
  Serial.println("-------------------------------------");

  // ===================================
  // --- FASE BACA ---
  // ===================================
  Serial.println("Membaca data kembali dari FRAM...");

  uint8_t c_baca; fram.readByte(ADDR_CHAR, c_baca);
  Serial.printf("CHAR:   Ditulis: %c, Dibaca: %c\n", c_tulis, (char)c_baca);

  byte b_baca; fram.readByte(ADDR_BYTE, b_baca);
  Serial.printf("BYTE:   Ditulis: 0x%X, Dibaca: 0x%X\n", b_tulis, b_baca);

  int i_baca; fram.readObject(ADDR_INT, i_baca);
  Serial.printf("INT:    Ditulis: %d, Dibaca: %d\n", i_tulis, i_baca);

  unsigned int ui_baca; fram.readObject(ADDR_UINT, ui_baca);
  Serial.printf("UINT:   Ditulis: %u, Dibaca: %u\n", ui_tulis, ui_baca);

  long l_baca; fram.readObject(ADDR_LONG, l_baca);
  Serial.printf("LONG:   Ditulis: %ld, Dibaca: %ld\n", l_tulis, l_baca);

  unsigned long ul_baca; fram.readObject(ADDR_ULONG, ul_baca);
  Serial.printf("ULONG:  Ditulis: %lu, Dibaca: %lu\n", ul_tulis, ul_baca);

  float f_baca; fram.readObject(ADDR_FLOAT, f_baca);
  Serial.printf("FLOAT:  Ditulis: %f, Dibaca: %f\n", f_tulis, f_baca);

  String s_baca; fram.readString(ADDR_STRING, s_baca, 50);
  Serial.printf("STRING: Ditulis: %s, Dibaca: %s\n", s_tulis.c_str(), s_baca.c_str());

  MyConfig cfg_baca; fram.readObject(ADDR_STRUCT, cfg_baca);
  Serial.printf("STRUCT: Ditulis: {ID:%d, Cal:%.2f}, Dibaca: {ID:%d, Cal:%.2f}\n",
                cfg_tulis.deviceID, cfg_tulis.calibration,
                cfg_baca.deviceID, cfg_baca.calibration);

  // ===================================
  // --- VERIFIKASI ---
  // ===================================
  Serial.println("-------------------------------------");
  if (c_tulis == c_baca && b_tulis == b_baca && i_tulis == i_baca &&
      ui_tulis == ui_baca && l_tulis == l_baca && ul_tulis == ul_baca &&
      f_tulis == f_baca && s_tulis == s_baca && 
      cfg_tulis.deviceID == cfg_baca.deviceID) {
    Serial.println("✅ VERIFIKASI SUKSES! Semua data cocok.");
  } else {
    Serial.println("❌ VERIFIKASI GAGAL! Data tidak cocok.");
  }
}

void loop() {
  // Tidak ada loop
}