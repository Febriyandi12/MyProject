#include "FM24C16.h"
#include "esp_log.h"

static const char* TAG = "FM24C16";

FM24C16::FM24C16() : m_initialized(false), m_sda_pin(21), m_scl_pin(22), m_i2c_freq(400000) {
    memset(m_error_msg, 0, sizeof(m_error_msg));
}

FM24C16::~FM24C16() {
}

void FM24C16::begin(int sda_pin, int scl_pin, int i2c_freq) {
    m_sda_pin = sda_pin;
    m_scl_pin = scl_pin;
    m_i2c_freq = i2c_freq;
    
    Serial.println("[FM24C16] Initializing driver...");
    
    Wire.begin(m_sda_pin, m_scl_pin);
    Wire.setClock(m_i2c_freq);
    
    m_initialized = isConnected();
    
    if (m_initialized) {
        Serial.printf("[FM24C16] Initialized successfully at %d Hz\n", m_i2c_freq);
    } else {
        Serial.println("[FM24C16] ERROR: Device not detected. Check wiring and pull-ups!");
    }
}

uint8_t FM24C16::getI2CAddress(uint16_t memory_address) {
    uint8_t block = (memory_address >> 8) & 0x07;
    return 0x50 | block;
}

uint16_t FM24C16::calculateCRC16(const uint8_t* data, size_t length) {
    uint16_t crc = CRC16_INIT_VALUE;
    for (size_t i = 0; i < length; i++) {
        crc ^= (uint16_t)data[i] << 8;
        for (uint8_t j = 0; j < 8; j++) {
            if (crc & 0x8000) {
                crc = (crc << 1) ^ CRC16_POLYNOMIAL;
            } else {
                crc = crc << 1;
            }
        }
    }
    return crc;
}

void FM24C16::logError(const char* context, const char* detail) {
    snprintf(m_error_msg, sizeof(m_error_msg), "[%s] Error: %s", context, detail);
    Serial.printf("[FM24C16] %s\n", m_error_msg);
}

bool FM24C16::wireWrite(uint8_t i2c_addr, const uint8_t* data, size_t length) {
    if (!m_initialized) return false;
    
    Wire.beginTransmission(i2c_addr);
    Wire.write(data, length);
    uint8_t error = Wire.endTransmission(true);
    
    if (error != 0) {
        logError("wireWrite", (String("endTransmission failed: ") + String(error)).c_str());
        return false;
    }
    return true;
}

bool FM24C16::wireRead(uint8_t i2c_addr, uint8_t* data, size_t length) {
    if (!m_initialized) return false;
    
    size_t bytesRead = Wire.requestFrom(i2c_addr, length, true);
    if (bytesRead != length) {
        logError("wireRead", (String("requestFrom failed: expected ") + 
                String(length) + ", got " + String(bytesRead)).c_str());
        return false;
    }
    
    for (size_t i = 0; i < length; i++) {
        if (!Wire.available()) {
            logError("wireRead", "Data not available");
            return false;
        }
        data[i] = Wire.read();
    }
    return true;
}

bool FM24C16::wireWriteRead(uint8_t i2c_addr, const uint8_t* write_data, size_t write_len,
                            uint8_t* read_data, size_t read_len) {
    // Retry mechanism
    for (int retry = 0; retry < FM24C16_MAX_RETRIES; retry++) {
        // Write address
        Wire.beginTransmission(i2c_addr);
        Wire.write(write_data, write_len);
        if (Wire.endTransmission(false) != 0) {
            delay(FM24C16_RETRY_DELAY_BASE * (retry + 1));
            continue;
        }
        
        // Read data
        if (wireRead(i2c_addr, read_data, read_len)) {
            return true;
        }
        
        delay(FM24C16_RETRY_DELAY_BASE * (retry + 1));
    }
    
    logError("wireWriteRead", "Max retries exceeded");
    return false;
}

bool FM24C16::isConnected() {
    for (uint8_t block = 0; block < 8; block++) {
        uint8_t i2c_addr = 0x50 | block;
        
        Wire.beginTransmission(i2c_addr);
        if (Wire.endTransmission(true) == 0) {
            Serial.printf("[FM24C16] Device detected at block %d (I2C: 0x%02X)\n", block, i2c_addr);
            return true;
        }
    }
    
    logError("isConnected", "No device found on any block");
    return false;
}


bool FM24C16::writeULong(uint16_t address, unsigned long value) {
    // Validasi alamat harus kelipatan 4 dan dalam batas
    if ((address % 4 != 0) || (address + 4 > FM24C16_TOTAL_SIZE)) {
        snprintf(m_error_msg, sizeof(m_error_msg), 
                 "writeULong: Alamat harus kelipatan 4 dan <= 2044. Diberikan: %d", address);
        ESP_LOGE(TAG, "%s", m_error_msg);
        return false;
    }
    
    // Konversi unsigned long ke array byte (little-endian)
    uint8_t data[4] = {
        (uint8_t)(value & 0xFF),           // Byte 0 - Least Significant Byte (LSB)
        (uint8_t)((value >> 8) & 0xFF),    // Byte 1
        (uint8_t)((value >> 16) & 0xFF),   // Byte 2
        (uint8_t)((value >> 24) & 0xFF)    // Byte 3 - Most Significant Byte (MSB)
    };
    
    ESP_LOGD(TAG, "writeULong: Menulis 0x%08lX ke alamat 0x%03X", value, address);
    
    // Gunakan fungsi write() yang sudah ada (dengan CRC)
    bool success = write(address, data, 4, true);
    
    if (success) {
        ESP_LOGD(TAG, "writeULong: Sukses");
    } else {
        logError("writeULong", "Gagal menulis data unsigned long");
    }
    
    return success;
}

bool FM24C16::readULong(uint16_t address, unsigned long &value) {
  if ((address % 4 != 0) || (address + 4 > FM24C16_TOTAL_SIZE)) {
    snprintf(m_error_msg, sizeof(m_error_msg), 
              "readULong: Alamat harus kelipatan 4 dan <= 2044. Diberikan: %d", address);
    ESP_LOGE(TAG, "%s", m_error_msg);
    return false;
  }
  
  uint8_t data[4];
  unsigned long temp_value = 0;
  
  ESP_LOGD(TAG, "readULong: Membaca dari alamat 0x%03X", address);
  
  // Gunakan fungsi read() yang sudah ada (dengan CRC)
  bool success = read(address, data, 4, true);
  
  if (!success) {
      logError("readULong", "Gagal membaca data unsgned long");
      return false;
  }
  
  // Konversi array byte kembali ke unsigned long (little-endian)
  temp_value = ((unsigned long)data[0]) |
                ((unsigned long)data[1] << 8) |
                ((unsigned long)data[2] << 16) |
                ((unsigned long)data[3] << 24);
  
  // Assign ke variabel reference
  value = temp_value;
  
  ESP_LOGD(TAG, "readULong: Nilai 0x%08lX dibaca", value);
  return true;
}

bool FM24C16::writeFloat(uint16_t address, float value) {
    // Validasi alamat harus kelipatan 4 dan dalam batas
    if ((address % 4 != 0) || (address + 4 > FM24C16_TOTAL_SIZE)) {
        snprintf(m_error_msg, sizeof(m_error_msg), 
                 "writeFloat: Alamat harus kelipatan 4 dan <= 2044. Diberikan: %d", address);
        ESP_LOGE(TAG, "%s", m_error_msg);
        return false;
    }
    
    // Gunakan union untuk konversi float → uint8_t[4] (type-safe)
    union FloatToBytes {
        float f;
        uint8_t b[4];
    } converter;
    
    converter.f = value;
    
    ESP_LOGD(TAG, "writeFloat: Menulis %f (0x%08lX) ke alamat 0x%03X", 
             value, *(uint32_t*)&converter.f, address);
    
    // Gunakan fungsi write() existing (dengan CRC)
    bool success = write(address, converter.b, 4, true);
    
    if (success) {
        ESP_LOGD(TAG, "writeFloat: Sukses");
    } else {
        logError("writeFloat", "Gagal menulis data float");
    }
    
    return success;
}

bool FM24C16::readFloat(uint16_t address, float &value) {
    // Validasi alamat harus kelipatan 4 dan dalam batas
    if ((address % 4 != 0) || (address + 4 > FM24C16_TOTAL_SIZE)) {
        snprintf(m_error_msg, sizeof(m_error_msg), 
                 "readFloat: Alamat harus kelipatan 4 dan <= 2044. Diberikan: %d", address);
        ESP_LOGE(TAG, "%s", m_error_msg);
        return false;
    }
    
    union FloatToBytes {
        float f;
        uint8_t b[4];
    } converter;
    
    ESP_LOGD(TAG, "readFloat: Membaca dari alamat 0x%03X", address);
    
    // Gunakan fungsi read() existing (dengan CRC)
    bool success = read(address, converter.b, 4, true);
    
    if (!success) {
        logError("readFloat", "Gagal membaca data float");
        return false;
    }
    
    // Assign ke variabel reference
    value = converter.f;
    
    ESP_LOGD(TAG, "readFloat: Nilai %f (0x%08lX) dibaca", 
             value, *(uint32_t*)&converter.f);
    return true;
}

bool FM24C16::write(uint16_t address, const uint8_t* data, size_t length, bool enable_crc) {
    if (!m_initialized) {
        logError("write", "Driver not initialized");
        return false;
    }
    
    if (address + length > FM24C16_TOTAL_SIZE) {
        logError("write", "Address out of bounds");
        return false;
    }
    
    uint8_t i2c_addr = getI2CAddress(address);
    uint8_t offset = address & 0xFF;
    
    //Serial.printf("[FM24C16] Writing %d bytes to 0x%03X (block %d, offset 0x%02X)\n", 
                  // length, address, (address >> 8) & 0x07, offset);
    
    // Format: [Address byte] + [Data] + [Optional CRC]
    size_t write_size = 1 + length + (enable_crc ? 2 : 0);
    uint8_t* write_buf = (uint8_t*)malloc(write_size);
    if (!write_buf) {
        logError("write", "Memory allocation failed");
        return false;
    }
    
    write_buf[0] = offset;
    memcpy(&write_buf[1], data, length);
    
    if (enable_crc) {
        uint16_t crc = calculateCRC16(data, length);
        write_buf[1 + length] = (crc >> 8) & 0xFF;
        write_buf[1 + length + 1] = crc & 0xFF;
        //Serial.printf("[FM24C16] CRC: 0x%04X\n", crc);
    }
    
    bool success = wireWrite(i2c_addr, write_buf, write_size);
    free(write_buf);
    
    if (success) {
        Serial.println("[FM24C16] Write successful");
    } else {
        logError("write", "Operation failed");
    }
    
    return success;
}

bool FM24C16::read(uint16_t address, uint8_t* data, size_t length, bool check_crc) {
    if (!m_initialized) {
        logError("read", "Driver not initialized");
        return false;
    }
    
    if (address + length > FM24C16_TOTAL_SIZE) {
        logError("read", "Address out of bounds");
        return false;
    }
    
    uint8_t i2c_addr = getI2CAddress(address);
    uint8_t offset = address & 0xFF;
    
    //Serial.printf("[FM24C16] Reading %d bytes from 0x%03X (block %d, offset 0x%02X)\n", 
                  // length, address, (address >> 8) & 0x07, offset);
    
    size_t read_size = length + (check_crc ? 2 : 0);
    uint8_t* read_buf = (uint8_t*)malloc(read_size);
    if (!read_buf) {
        logError("read", "Memory allocation failed");
        return false;
    }
    
    bool success = wireWriteRead(i2c_addr, &offset, 1, read_buf, read_size);
    if (!success) {
        free(read_buf);
        return false;
    }
    
    memcpy(data, read_buf, length);
    
    if (check_crc) {
        uint16_t crc_read = (read_buf[length] << 8) | read_buf[length + 1];
        uint16_t crc_calc = calculateCRC16(data, length);
        
        if (crc_read != crc_calc) {
            snprintf(m_error_msg, sizeof(m_error_msg), 
                     "CRC mismatch: read=0x%04X, calc=0x%04X", crc_read, crc_calc);
            logError("read", m_error_msg);
            free(read_buf);
            return false;
        }
        //Serial.printf("[FM24C16] CRC verification passed: 0x%04X\n", crc_calc);
    }
    
    free(read_buf);
    Serial.println("[FM24C16] Read successful");
    return true;
}

bool FM24C16::eraseAll() {
    if (!m_initialized) {
        logError("eraseAll", "Driver not initialized");
        return false;
    }
    
    //Serial.println("[FM24C16] Erasing entire memory (writing zeros)...");
    
    uint8_t zero[FM24C16_PAGE_SIZE] = {0};
    for (uint16_t addr = 0; addr < FM24C16_TOTAL_SIZE; addr += FM24C16_PAGE_SIZE) {
        if (!write(addr, zero, FM24C16_PAGE_SIZE, false)) {
            return false;
        }
        //Serial.printf("[FM24C16] Erased block at 0x%03X\n", addr);
        delay(1);  // Small delay to prevent watchdog
    }
    
    Serial.println("[FM24C16] Erase completed successfully");
    return true;
}

const char* FM24C16::getLastError() {
    return m_error_msg;
}

bool FM24C16::writeByte(uint16_t address, uint8_t value) {
    // Tulis 1 byte, tanpa CRC (CRC pada 1 byte itu berlebihan)
    // 'write' adalah fungsi inti dari library Anda
    return write(address, &value, 1, false);
}

/**
 * @brief Membaca 1 byte (char/byte) tanpa CRC.
 */
bool FM24C16::readByte(uint16_t address, uint8_t &value) {
    // Baca 1 byte, tanpa CRC
    // 'read' adalah fungsi inti dari library Anda
    return read(address, &value, 1, false);
}

bool FM24C16::writeString(uint16_t address, const String &s) {
    const char* c_str = s.c_str();
    // +1 untuk menyertakan karakter null '\0'
    size_t len = s.length() + 1; 
    
    // Validasi alamat
    if (address + len > FM24C16_TOTAL_SIZE) { 
        logError("writeString", "Address out of bounds");
        return false;
    }
    
    // Tulis string (termasuk null) TANPA CRC
    // karena readString akan membaca byte-by-byte
    return write(address, (const uint8_t*)c_str, len, false); 
}

/**
 * @brief Membaca String object (TANPA CRC)
 */
bool FM24C16::readString(uint16_t address, String &out, size_t maxLen) {
    out = ""; // Kosongkan string
    out.reserve(32); // Alokasi memori awal
    
    uint8_t aChar;
    size_t bytesRead = 0;
    
    // Kita baca byte-by-byte sampai menemukan null terminator (0x00)
    while (bytesRead < maxLen) {
        // readByte() (dari giliran sebelumnya) tidak menggunakan CRC
        if (!readByte(address + bytesRead, aChar)) { 
            return false; // Gagal I2C
        }
        
        if (aChar == 0x00) {
            return true; // Sukses, menemukan akhir string
        }
        
        out += (char)aChar;
        bytesRead++;
    }
    
    // Mencapai maxLen tanpa menemukan null
    return true; 
}

void FM24C16::printDebugInfo() {
    Serial.println("\n===== FM24C16 Debug Info =====");
    Serial.printf("Initialized: %s\n", m_initialized ? "YES" : "NO");
    Serial.printf("SDA Pin: %d\n", m_sda_pin);
    Serial.printf("SCL Pin: %d\n", m_scl_pin);
    Serial.printf("Frequency: %d Hz\n", m_i2c_freq);
    Serial.printf("Last Error: %s\n", m_error_msg);
    Serial.printf("Total Memory: %d bytes\n", FM24C16_TOTAL_SIZE);
    Serial.println("==============================\n");
}