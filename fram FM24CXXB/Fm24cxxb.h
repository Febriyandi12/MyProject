/*
 * Pustaka FRAM Universal (Fm24cxxb.h)
 * Menggunakan API template 'writeObject' untuk semua tipe data.
 */
#ifndef Fm24cxxb_H
#define Fm24cxxb_H

#include <Arduino.h>
#include <Wire.h>

/**
 * @brief Tipe chip yang didukung.
 * Pilih salah satu saat memanggil begin().
 */
enum FRAM_Chip_t {
    CHIP_FM24C16,  // 16-Kbit (2KB), pengalamatan blok 1-byte
    CHIP_FM24C64B  // 64-Kbit (8KB), pengalamatan 16-bit
};

class Fm24cxxb {
public:
  Fm24cxxb();

  /**
   * @brief Inisialisasi library.
   * @param chipType Tipe chip yang Anda gunakan (CHIP_FM24C16 atau CHIP_FM24C64B)
   * @param i2c_addr Alamat I2C dasar (biasanya 0x50)
   * @param wire Objek TwoWire (default: Wire)
   * @return true jika chip terdeteksi
   */
  bool begin(FRAM_Chip_t chipType, uint8_t i2c_addr = 0x50, TwoWire &wire = Wire);

  /**
   * @brief Mengatur kecepatan I2C (opsional)
   */
  void setClock(uint32_t hz);

  /**
   * @brief Menulis 1 byte (char/byte).
   */
  bool writeByte(uint16_t memAddress, uint8_t data);

  /**
   * @brief Membaca 1 byte (char/byte).
   */
  bool readByte(uint16_t memAddress, uint8_t &data);

  /**
   * @brief Menulis tipe data apa pun (int, long, float, struct).
   * Menggunakan template untuk menangani semua tipe secara otomatis.
   */
  template <typename T>
  bool writeObject(uint16_t memAddress, const T& value) {
    return writeBuffer(memAddress, (const uint8_t*)&value, sizeof(T));
  }

  /**
   * @brief Membaca tipe data apa pun (int, long, float, struct).
   * Menggunakan template untuk menangani semua tipe secara otomatis.
   */
  template <typename T>
  bool readObject(uint16_t memAddress, T& value) {
    return readBuffer(memAddress, (uint8_t*)&value, sizeof(T));
  }

  /**
   * @brief Menulis objek String Arduino (menangani null terminator).
   */
  bool writeString(uint16_t memAddress, const String &s);

  /**
   * @brief Membaca objek String Arduino.
   */
  bool readString(uint16_t memAddress, String &out, size_t maxLen = 100);

  bool writeBuffer(uint16_t memAddress, const uint8_t* buf, size_t len);
  bool readBuffer(uint16_t memAddress, uint8_t* buf, size_t len);

private:
  TwoWire* _wire;
  uint8_t _deviceAddress; // Alamat I2C dasar
  FRAM_Chip_t _chipType;  // Menyimpan tipe chip yang dipilih
};

#endif