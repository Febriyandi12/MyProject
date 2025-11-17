#ifndef FM24C64B_H
#define FM24C64B_H

#include <Arduino.h>
#include <Wire.h>

class FM24C64B {
public:
  FM24C64B();
  // i2c_addr: 7-bit base address (usually 0x50 when A0..A2 = GND)
  bool begin(uint8_t i2c_addr = 0x50, TwoWire &wire = Wire);
  // set I2C clock in Hz (will call Wire.setClock)
  void setClock(uint32_t hz);

  // basic read/write single byte
  bool writeByte(uint16_t memAddress, uint8_t data);
  bool readByte(uint16_t memAddress, uint8_t &data);

  // read/write buffer (ini adalah fondasi kita)
  bool writeBuffer(uint16_t memAddress, const uint8_t* buf, size_t len);
  bool readBuffer(uint16_t memAddress, uint8_t* buf, size_t len);

  // ==========================================================
  // --- FUNGSI HELPER BARU (Tambahkan ini) ---
  // ==========================================================
  
  /**
   * @brief Menulis tipe data apa pun (float, int, long, struct) ke FRAM.
   * @tparam T Tipe data (akan dideteksi otomatis)
   * @param memAddress Alamat memori untuk menulis
   * @param value Nilai yang akan ditulis
   * @return true jika sukses
   */
  template <typename T>
  bool writeObject(uint16_t memAddress, const T& value) {
    // Mengonversi pointer object (misal: float*) menjadi pointer byte (uint8_t*)
    // dan menulisnya menggunakan writeBuffer
    return writeBuffer(memAddress, (const uint8_t*)&value, sizeof(T));
  }

  /**
   * @brief Membaca tipe data apa pun (float, int, long, struct) dari FRAM.
   * @tparam T Tipe data (akan dideteksi otomatis)
   * @param memAddress Alamat memori untuk membaca
   * @param value Variabel referensi untuk menyimpan hasil
   * @return true jika sukses
   */
  template <typename T>
  bool readObject(uint16_t memAddress, T& value) {
    // Membaca byte ke dalam object (misal: float) menggunakan readBuffer
    return readBuffer(memAddress, (uint8_t*)&value, sizeof(T));
  }

  // --- Implementasi untuk String ---
  bool writeString(uint16_t memAddress, const String &s);
  bool readString(uint16_t memAddress, String &out, size_t maxLen = 100);

private:
  TwoWire* _wire;
  uint8_t _deviceAddress;
};

#endif