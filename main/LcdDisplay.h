#ifndef LCD_DISPLAY
#define LCD_DISPLAY

#include <cstdint>
class I2CBus;

class LcdDisplay {
  uint8_t _lcd_mask;

  uint8_t functionset_mask;
  uint8_t display_mask;
  uint8_t entrymode_mask;

  I2CBus &_i2c;
  uint8_t _address;

  void sendNibble(uint8_t nibble);

  void command(uint8_t command);
  void data(uint8_t data);

public:
  LcdDisplay(I2CBus &i2c, uint8_t address);

  void init();
  void entrymode();
  void functionset();

  void setCursor(uint8_t x, uint8_t y);
  void print(const char *message);
  void clear();
  void home();
  void backlight();
  void display();
};

#endif /*LCD_DISPLAY*/
