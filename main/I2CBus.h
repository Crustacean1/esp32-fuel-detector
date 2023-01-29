#ifndef I2C_BUS
#define I2C_BUS

#include <cstdint>

class I2CBus {
private:
  uint8_t _buffer[1];
  bool _initialized = false;

  void init();
  bool ping(uint8_t address);

public:
  I2CBus();

  void write(uint8_t address, uint8_t value);
  void write(uint8_t address, uint8_t *data, uint32_t data_size);
  void scan();
};

#endif /*I2C_BUS*/
