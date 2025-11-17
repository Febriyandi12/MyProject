/*
 * Program Dummy: Tes Tulis/Baca SEMUA Tipe Data
 * Menggunakan library FM24C64B yang sudah ada.
 */

#include "FM24C64B.h"
#include <Wire.h>

// --- KONFIGURASI ---
#define I2C_SDA_PIN 21
#define I2C_SCL_PIN 22
#define I2C_FREQ 400000

FM24C64B fram;

// --- Alamat Memori ---
#define ADDR_STRING 10   // Ukuran 4 byte
#define ADDR_CHAR   100  // Ukuran 1 byte
#define ADDR_BYTE   102  // Ukuran 1 byte
#define ADDR_INT    104  // Ukuran 4 byte 
#define ADDR_UINT   110  // Ukuran 4 byte
#define ADDR_LONG   116  // Ukuran 4 byte
#define ADDR_ULONG  122  // Ukuran 4 byte
#define ADDR_FLOAT  128  // Ukuran 4 byte

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("\n--- Tes Tulis/Baca Semua Tipe Data ---");

  // Inisialisasi Wire dan Library
  Wire.begin(I2C_SDA_PIN, I2C_SCL_PIN);
  fram.begin(0x50, Wire); //
  fram.setClock(I2C_FREQ); //

  // ===================================
  // --- FASE TULIS ---
  // ===================================
  Serial.println("Menulis semua tipe data ke FRAM...");

  char c_tulis = 'C';
  fram.writeByte(ADDR_CHAR, c_tulis); //

  String s_tulis = "Saya Melakukan untuk baca tulis dengan INi";
  fram.writeString(ADDR_STRING, s_tulis);

  byte b_tulis = 0xDE;
  fram.writeByte(ADDR_BYTE, b_tulis); //

  int i_tulis = -32000;
  fram.writeObject(ADDR_INT, i_tulis); //

  unsigned int ui_tulis = 65000;
  fram.writeObject(ADDR_UINT, ui_tulis); //

  long l_tulis = -123456789;
  fram.writeObject(ADDR_LONG, l_tulis); //

  unsigned long ul_tulis = 3210987654;
  fram.writeObject(ADDR_ULONG, ul_tulis); //

  float f_tulis = 3.14159;
  fram.writeObject(ADDR_FLOAT, f_tulis); //

  Serial.println("...Penulisan selesai.");
  Serial.println("-------------------------------------");

  // ===================================
  // --- FASE BACA ---
  // ===================================
  Serial.println("Membaca data kembali dari FRAM...");

  uint8_t c_baca = 0;
  fram.readByte(ADDR_CHAR, c_baca); //
  Serial.printf("CHAR:   Ditulis: %c, Dibaca: %c\n", c_tulis, c_baca);

  String s_baca = "";
  fram.readString(ADDR_STRING, s_baca);
  Serial.printf("String:   Ditulis: %s, Dibaca: %s\n", s_tulis.c_str(), s_baca.c_str());

  byte b_baca = 0;
  fram.readByte(ADDR_BYTE, b_baca); //
  Serial.printf("BYTE:   Ditulis: 0x%X, Dibaca: 0x%X\n", b_tulis, b_baca);

  int i_baca = 0;
  fram.readObject(ADDR_INT, i_baca); //
  Serial.printf("INT:    Ditulis: %d, Dibaca: %d\n", i_tulis, i_baca);

  unsigned int ui_baca = 0;
  fram.readObject(ADDR_UINT, ui_baca); //
  Serial.printf("UINT:   Ditulis: %u, Dibaca: %u\n", ui_tulis, ui_baca);

  long l_baca = 0;
  fram.readObject(ADDR_LONG, l_baca); //
  Serial.printf("LONG:   Ditulis: %ld, Dibaca: %ld\n", l_tulis, l_baca);

  unsigned long ul_baca = 0;
  fram.readObject(ADDR_ULONG, ul_baca); //
  Serial.printf("ULONG:  Ditulis: %lu, Dibaca: %lu\n", ul_tulis, ul_baca);

  float f_baca = 0;
  fram.readObject(ADDR_FLOAT, f_baca); //
  Serial.printf("FLOAT:  Ditulis: %f, Dibaca: %f\n", f_tulis, f_baca);

  // ===================================
  // --- VERIFIKASI ---
  // ===================================
  Serial.println("-------------------------------------");
  
  // <-- KESALAHAN 3: Tambahkan pengecekan string (s_tulis == s_baca)
  if (c_tulis == c_baca && s_tulis == s_baca && b_tulis == b_baca && 
      i_tulis == i_baca && ui_tulis == ui_baca && l_tulis == l_baca && 
      ul_tulis == ul_baca && f_tulis == f_baca) { // <-- DIPERBAIKI
    Serial.println("✅ VERIFIKASI SUKSES! Semua data cocok.");
  } else {
    Serial.println("❌ VERIFIKASI GAGAL! Data tidak cocok.");
  }
}

void loop() {
  // Tidak ada loop
}