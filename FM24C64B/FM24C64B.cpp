#include "FM24C64B.h"


FM24C64B::FM24C64B() {
  _wire = nullptr;
  _deviceAddress = 0x50;
}


bool FM24C64B::begin(uint8_t i2c_addr, TwoWire& wire) {
  _wire = &wire;
  _deviceAddress = i2c_addr & 0x7F;
  _wire->begin();
  return true;
}


void FM24C64B::setClock(uint32_t hz) {
  if (_wire) _wire->setClock(hz);
}


bool FM24C64B::writeByte(uint16_t memAddress, uint8_t data) {
  if (!_wire) return false;
  _wire->beginTransmission(_deviceAddress);
  _wire->write((uint8_t)(memAddress >> 8));
  _wire->write((uint8_t)(memAddress & 0xFF));
  _wire->write(data);
  return (_wire->endTransmission() == 0);
}


bool FM24C64B::readByte(uint16_t memAddress, uint8_t& data) {
  if (!_wire) return false;
  _wire->beginTransmission(_deviceAddress);
  _wire->write((uint8_t)(memAddress >> 8));
  _wire->write((uint8_t)(memAddress & 0xFF));
  if (_wire->endTransmission(false) != 0) return false;


  size_t got = _wire->requestFrom((int)_deviceAddress, 1);
  if (got != 1) return false;
  data = _wire->read();
  return true;
}


bool FM24C64B::writeBuffer(uint16_t memAddress, const uint8_t* buf, size_t len) {
  if (!_wire) return false;
  size_t remaining = len;
  const uint8_t* p = buf;
  uint16_t addr = memAddress;


  while (remaining) {
    size_t chunk = remaining;
    if (chunk > 30) chunk = 30;


    _wire->beginTransmission(_deviceAddress);
    _wire->write((uint8_t)(addr >> 8));
    _wire->write((uint8_t)(addr & 0xFF));
    _wire->write(p, chunk);
    if (_wire->endTransmission() != 0) return false;


    remaining -= chunk;
    p += chunk;
    addr += chunk;
  }
  return true;
}

bool FM24C64B::writeString(uint16_t memAddress, const String &s) {
  // Ambil pointer ke string C-style (termasuk null terminator)
  const char* c_str = s.c_str();
  // Dapatkan panjang string + 1 untuk null terminator
  size_t len = s.length() + 1;
  
  // Tulis sebagai buffer byte biasa
  return writeBuffer(memAddress, (const uint8_t*)c_str, len);
}


/**
 * @brief Membaca string (null-terminated) dari FRAM.
 * @param memAddress Alamat memori untuk mulai membaca
 * @param out String object untuk diisi
 * @param maxLen Batas aman untuk membaca agar tidak terjadi loop tak terbatas
 * @return true jika sukses
 */
bool FM24C64B::readString(uint16_t memAddress, String &out, size_t maxLen) {
  if (!_wire) return false;
  
  out = ""; // Kosongkan string
  out.reserve(32); // Alokasikan memori di awal agar lebih efisien

  uint8_t aChar;
  size_t bytesRead = 0;
  
  // Kita baca byte satu per satu sampai menemukan null terminator (0x00)
  while (bytesRead < maxLen) {
    if (!readByte(memAddress + bytesRead, aChar)) {
      // Gagal membaca I2C
      return false; 
    }
    
    if (aChar == 0x00) {
      // Kita menemukan akhir string, sukses!
      return true; 
    }
    
    // Tambahkan karakter ke string
    out += (char)aChar;
    bytesRead++;
  }
  
  // Jika kita sampai di sini, berarti kita mencapai maxLen 
  // tanpa menemukan null terminator.
  // Kita tetap kembalikan true, tapi string mungkin terpotong.
  return true; 
}


bool FM24C64B::readBuffer(uint16_t memAddr, uint8_t* buffer, size_t len) {
  if (!_wire) return false;

  // kirim address high + low byte
  _wire->beginTransmission(_deviceAddress);
  _wire->write((uint8_t)(memAddr >> 8));
  _wire->write((uint8_t)(memAddr & 0xFF));

  // repeated start
  if (_wire->endTransmission(false) != 0) {
    return false;
  }

  // request data
  size_t r = _wire->requestFrom((int)_deviceAddress, (int)len);
  if (r != len) return false;

  for (size_t i = 0; i < len; i++) {
    buffer[i] = _wire->read();
  }

  return true;
}

