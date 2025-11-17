#ifndef FM24C16_H
#define FM24C16_H

#include <Arduino.h>
#include <Wire.h>
// ... (Semua #define Anda tetap sama) ...
#define FM24C16_DEFAULT_TIMEOUT     1000
#define FM24C16_MAX_RETRIES         5
#define FM24C16_RETRY_DELAY_BASE    10
#define FM24C16_BLOCK_SIZE          256
#define FM24C16_TOTAL_SIZE          2048
#define FM24C16_PAGE_SIZE           16
#define CRC16_POLYNOMIAL            0x8005
#define CRC16_INIT_VALUE            0xFFFF


class FM24C16 {
public:
    FM24C16();
    ~FM24C16();
    
    void begin(int sda_pin = 21, int scl_pin = 22, int i2c_freq = 400000);
    bool isConnected();
    
    bool writeULong(uint16_t address, unsigned long value); //
    bool readULong(uint16_t address, unsigned long &value); //
    bool writeFloat(uint16_t address, float value); //
    bool readFloat(uint16_t address, float &value); //
    bool writeString(uint16_t address, const String &s);
    bool readString(uint16_t address, String &out, size_t maxLen = 100);
    bool writeByte(uint16_t address, uint8_t value);
    bool readByte(uint16_t address, uint8_t &value);

    /**
     * @brief Menulis tipe data apa pun (int, long, char, struct) ke FRAM.
     * @tparam T Tipe data (akan dideteksi otomatis)
     * @note Fungsi ini akan selalu menggunakan CRC.
     */
    template <typename T>
    bool writeObject(uint16_t address, const T& value) {
        // Validasi alamat (pastikan data + 2 byte CRC muat)
        if ((address + sizeof(T) + 2) > FM24C16_TOTAL_SIZE) {
             logError("writeObject", "Address out of bounds");
             return false;
        }
        // Gunakan fungsi 'write' inti dengan CRC
        return write(address, (const uint8_t*)&value, sizeof(T), true);
    }

    /**
     * @brief Membaca tipe data apa pun (int, long, char, struct) dari FRAM.
     * @tparam T Tipe data (akan dideteksi otomatis)
     * @note Fungsi ini akan selalu memverifikasi CRC.
     */
    template <typename T>
    bool readObject(uint16_t address, T& value) {
        // Validasi alamat
        if ((address + sizeof(T) + 2) > FM24C16_TOTAL_SIZE) {
             logError("readObject", "Address out of bounds");
             return false;
        }
        // Gunakan fungsi 'read' inti dengan verifikasi CRC
        return read(address, (uint8_t*)&value, sizeof(T), true);
    }

    // --- FUNGSI INTI (Core) ---
    bool write(uint16_t address, const uint8_t* data, size_t length, bool enable_crc = true); //
    bool read(uint16_t address, uint8_t* data, size_t length, bool check_crc = true); //
    
    bool eraseAll();
    const char* getLastError();
    void printDebugInfo();

private:
    bool m_initialized;
    int m_sda_pin;
    int m_scl_pin;
    int m_i2c_freq;
    char m_error_msg[128];
    unsigned long m_last_op_time;
    
    // ... (Semua fungsi private Anda tetap sama) ...
    uint8_t getI2CAddress(uint16_t memory_address);
    uint16_t calculateCRC16(const uint8_t* data, size_t length);
    bool wireWrite(uint8_t i2c_addr, const uint8_t* data, size_t length);
    bool wireRead(uint8_t i2c_addr, uint8_t* data, size_t length);
    bool wireWriteRead(uint8_t i2c_addr, const uint8_t* write_data, size_t write_len, 
                       uint8_t* read_data, size_t read_len);
    void logError(const char* context, const char* detail = "");
};

#endif