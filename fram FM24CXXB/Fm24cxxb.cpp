/*
 * Pustaka FRAM Universal (Fm24cxxb.cpp)
 */
#include "Fm24cxxb.h"

Fm24cxxb::Fm24cxxb() {
  _wire = nullptr;
  _deviceAddress = 0x50;
  _chipType = CHIP_FM24C64B; // Default
}

bool Fm24cxxb::begin(FRAM_Chip_t chipType, uint8_t i2c_addr, TwoWire& wire) {
  _wire = &wire;
  _chipType = chipType;
  _deviceAddress = i2c_addr & 0x7F; // Pastikan 7-bit
  _wire->begin();
  
  // Cek koneksi sederhana
  Wire.beginTransmission(_deviceAddress);
  if (Wire.endTransmission() != 0) {
    // Jika gagal, coba cek blok C16 lain jika itu yang dipilih
    if (_chipType == CHIP_FM24C16) {
      for (uint8_t block = 1; block < 8; block++) {
        Wire.beginTransmission(_deviceAddress | block);
        if (Wire.endTransmission() == 0) return true; // Ditemukan
      }
    }
    return false; // Gagal total
  }
  return true; // Sukses di alamat dasar
}

void Fm24cxxb::setClock(uint32_t hz) {
  if (_wire) _wire->setClock(hz);
}

bool Fm24cxxb::writeByte(uint16_t memAddress, uint8_t data) {
  return writeBuffer(memAddress, &data, 1);
}

bool Fm24cxxb::readByte(uint16_t memAddress, uint8_t& data) {
  return readBuffer(memAddress, &data, 1);
}

bool Fm24cxxb::writeString(uint16_t memAddress, const String &s) {
  const char* c_str = s.c_str();
  size_t len = s.length() + 1; // +1 untuk null terminator
  return writeBuffer(memAddress, (const uint8_t*)c_str, len);
}

bool Fm24cxxb::readString(uint16_t memAddress, String &out, size_t maxLen) {
  if (!_wire) return false;
  out = "";
  out.reserve(32); 
  uint8_t aChar;
  size_t bytesRead = 0;
  while (bytesRead < maxLen) {
    if (!readByte(memAddress + bytesRead, aChar)) return false; // Gagal I2C
    if (aChar == 0x00) return true; // Akhir string
    out += (char)aChar;
    bytesRead++;
  }
  return true; 
}
================================

bool Fm24cxxb::writeBuffer(uint16_t memAddress, const uint8_t* buf, size_t len) {
  if (!_wire) return false;
  size_t remaining = len;
  const uint8_t* p = buf;
  uint16_t addr = memAddress;

  while (remaining) {
    size_t chunk = remaining;
    // 30 = batas aman (32 byte buffer - 2 byte alamat)
    if (chunk > 30) chunk = 30;

    // --- INI ADALAH LOGIKA UTAMA YANG MEMBEDAKAN ---
    if (_chipType == CHIP_FM24C64B) {
      // --- LOGIKA FM24C64B (16-bit address) ---
      _wire->beginTransmission(_deviceAddress);
      _wire->write((uint8_t)(addr >> 8)); // Alamat High Byte
      _wire->write((uint8_t)(addr & 0xFF)); // Alamat Low Byte
      _wire->write(p, chunk);
      if (_wire->endTransmission() != 0) return false;
      
    } else {
      // --- LOGIKA FM24C16 (block address) ---
      // Hitung alamat I2C blok (0x50 sampai 0x57)
      uint8_t i2c_block_addr = (_deviceAddress & 0xF8) | ((addr >> 8) & 0x07);
      _wire->beginTransmission(i2c_block_addr);
      _wire->write((uint8_t)(addr & 0xFF)); // Alamat 1-byte (offset)
      _wire->write(p, chunk);
      if (_wire->endTransmission() != 0) return false;
    }
    // -------------------------------------------------

    remaining -= chunk;
    p += chunk;
    addr += chunk;
  }
  return true;
}

bool Fm24cxxb::readBuffer(uint16_t memAddr, uint8_t* buffer, size_t len) {
  if (!_wire) return false;

  uint8_t i2c_addr_to_use;
  
  // --- 1. Set Address Pointer ---
  if (_chipType == CHIP_FM24C64B) {
    // --- LOGIKA FM24C64B (16-bit address) ---
    i2c_addr_to_use = _deviceAddress;
    _wire->beginTransmission(i2c_addr_to_use);
    _wire->write((uint8_t)(memAddr >> 8)); // HI
    _wire->write((uint8_t)(memAddr & 0xFF)); // LO
    if (_wire->endTransmission(false) != 0) return false; // repeated start
    
  } else {
    // --- LOGIKA FM24C16 (block address) ---
    i2c_addr_to_use = (_deviceAddress & 0xF8) | ((memAddr >> 8) & 0x07);
    _wire->beginTransmission(i2c_addr_to_use);
    _wire->write((uint8_t)(memAddr & 0xFF)); // OFFSET
    if (_wire->endTransmission(false) != 0) return false; // repeated start
  }

  // --- 2. Request Data dari alamat yang benar ---
  size_t r = _wire->requestFrom((int)i2c_addr_to_use, (int)len);
  if (r != len) return false;
  
  // --- 3. Read Data ---
  for (size_t i = 0; i < len; i++) {
    buffer[i] = _wire->read();
  }
  return true;
}