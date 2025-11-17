/*
 * Program Dummy: Tes Tulis/Baca SEMUA Tipe Data
 * Menggunakan library FM24C16 yang sudah diperbarui.
 */

#include "FM24C16.h"
#include <Wire.h>

// --- KONFIGURASI ---
#define I2C_SDA_PIN 21
#define I2C_SCL_PIN 22
#define I2C_FREQ 400000

FM24C16 fram;

// --- Alamat Memori ---
// (di ESP32, int, uint, long, ulong, float = 4 byte)
// (char, byte = 1 byte)
// Total memori hanya 2048 byte!
#define ADDR_CHAR   100  // 1 byte
#define ADDR_BYTE   102  // 1 byte
#define ADDR_INT    104  // 4 byte + 2 CRC
#define ADDR_UINT   112  // 4 byte + 2 CRC
#define ADDR_LONG   120  // 4 byte + 2 CRC
#define ADDR_ULONG  128  // 4 byte + 2 CRC
#define ADDR_FLOAT  136  // 4 byte + 2 CRC
#define ADDR_STRING 150  // <-- DITAMBAHKAN (Alamat untuk String)

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("\n--- Tes Tulis/Baca Semua Tipe Data (FM24C16) ---");

  // Inisialisasi Wire dan Library
  Wire.begin(I2C_SDA_PIN, I2C_SCL_PIN);
  fram.begin(I2C_SDA_PIN, I2C_SCL_PIN, I2C_FREQ);

  if (!fram.isConnected()) {
    Serial.println("Gagal terhubung ke FRAM!");
    while(1);
  }

  // ===================================
  // --- FASE TULIS ---
  // ===================================
  Serial.println("Menulis semua tipe data ke FRAM...");

  char c_tulis = 'A';
  fram.writeByte(ADDR_CHAR, (uint8_t)c_tulis); // Gunakan writeByte

  byte b_tulis = 0xDE;
  fram.writeByte(ADDR_BYTE, b_tulis); // Gunakan writeByte

  int i_tulis = -32000;
  fram.writeObject(ADDR_INT, i_tulis); // Gunakan writeObject

  unsigned int ui_tulis = 65000;
  fram.writeObject(ADDR_UINT, ui_tulis); // Gunakan writeObject

  long l_tulis = -123456789;
  fram.writeObject(ADDR_LONG, l_tulis); // Gunakan writeObject

  unsigned long ul_tulis = 3210987654;
  fram.writeULong(ADDR_ULONG, ul_tulis); // Gunakan fungsi asli

  float f_tulis = 3.14159;
  fram.writeFloat(ADDR_FLOAT, f_tulis); // Gunakan fungsi asli

  // --- TULIS STRING (DITAMBAHKAN) ---
  String s_tulis = "Halo dari FM24C16!";
  fram.writeString(ADDR_STRING, s_tulis);
  Serial.println("Menulis String: " + s_tulis);

  Serial.println("...Penulisan selesai.");
  Serial.println("-------------------------------------");

  // ===================================
  // --- FASE BACA ---
  // ===================================
  Serial.println("Membaca data kembali dari FRAM...");

  uint8_t c_baca_raw = 0;
  fram.readByte(ADDR_CHAR, c_baca_raw);
  char c_baca = (char)c_baca_raw;
  Serial.printf("CHAR:   Ditulis: %c, Dibaca: %c\n", c_tulis, c_baca);

  byte b_baca = 0;
  fram.readByte(ADDR_BYTE, b_baca);
  Serial.printf("BYTE:   Ditulis: 0x%X, Dibaca: 0x%X\n", b_tulis, b_baca);

  int i_baca = 0;
  fram.readObject(ADDR_INT, i_baca);
  Serial.printf("INT:    Ditulis: %d, Dibaca: %d\n", i_tulis, i_baca);

  unsigned int ui_baca = 0;
  fram.readObject(ADDR_UINT, ui_baca);
  Serial.printf("UINT:   Ditulis: %u, Dibaca: %u\n", ui_tulis, ui_baca);

  long l_baca = 0;
  fram.readObject(ADDR_LONG, l_baca);
  Serial.printf("LONG:   Ditulis: %ld, Dibaca: %ld\n", l_tulis, l_baca);

  unsigned long ul_baca = 0;
  fram.readULong(ADDR_ULONG, ul_baca);
  Serial.printf("ULONG:  Ditulis: %lu, Dibaca: %lu\n", ul_tulis, ul_baca);

  float f_baca = 0;
  fram.readFloat(ADDR_FLOAT, f_baca);
  Serial.printf("FLOAT:  Ditulis: %f, Dibaca: %f\n", f_tulis, f_baca);

  // --- BACA STRING (DITAMBAHKAN) ---
  String s_baca = "";
  fram.readString(ADDR_STRING, s_baca, 50); // 50 = batas aman
  Serial.printf("STRING: Ditulis: %s, Dibaca: %s\n", s_tulis.c_str(), s_baca.c_str());

  // ===================================
  // --- VERIFIKASI ---
  // ===================================
  Serial.println("-------------------------------------");
  
  // --- VERIFIKASI (DITAMBAHKAN s_tulis == s_baca) ---
  if (c_tulis == c_baca && b_tulis == b_baca && i_tulis == i_baca &&
      ui_tulis == ui_baca && l_tulis == l_baca && ul_tulis == ul_baca &&
      f_tulis == f_baca && s_tulis == s_baca) { // <-- DIPERBAIKI
    Serial.println("✅ VERIFIKASI SUKSES! Semua data cocok.");
  } else {
    Serial.println("❌ VERIFIKASI GAGAL! Data tidak cocok.");
  }
}

void loop() {
  // Tidak ada loop
}