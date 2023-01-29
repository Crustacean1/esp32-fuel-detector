
#include "LcdDisplay.h"
#include "I2CBus.h"
#include "driver/i2c.h"
#include "esp_log.h"
#include "rom/ets_sys.h"
#include <stdint.h>

#define LCD_RS_BIT 0x01
#define LCD_RW_BIT 0x02
#define LCD_ENABLE_BIT 0x04
#define LCD_BACKLIGHT_BIT 0x08

/*COMMANDS*/
#define LCD_CLEARDISPLAY 0x01
#define LCD_RETURNHOME 0x02
#define LCD_ENTRYMODESET 0x04
#define LCD_DISPLAYCONTROL 0x08
#define LCD_FUNCTIONSET 0x20

/*DISPLAY*/
#define LCD_DISPLAYON 0x04
#define LCD_DISPLAYOFF 0x00

/*ENTRYMODE*/
#define LCD_ENTRYLEFT 0x02
#define LCD_ENTRYSHIFTDECREMENT 0x00

/*FUNCTIONSET*/
#define LCD_4BITMODE 0x00
#define LCD_2LINE 0x08

static const char *TAG = "i2c-simple-example";

LcdDisplay::LcdDisplay(I2CBus &i2c, uint8_t address)
    : _i2c(i2c), _address(address) {

  functionset_mask = LCD_4BITMODE | LCD_2LINE;
  display_mask = LCD_DISPLAYON;
  entrymode_mask = LCD_ENTRYLEFT | LCD_ENTRYSHIFTDECREMENT;

  _lcd_mask = 0x0;
}

void LcdDisplay::sendNibble(uint8_t nibble) {
  nibble &= 0xf0;
  nibble |= _lcd_mask;

  _i2c.write(_address, nibble | LCD_ENABLE_BIT);
  ets_delay_us(1);
  _i2c.write(_address, nibble);
  ets_delay_us(5);
}

void LcdDisplay::data(uint8_t data) {
  _lcd_mask |= LCD_RS_BIT;
  _lcd_mask &= (~LCD_RW_BIT);

  sendNibble(data & 0xF0);
  sendNibble(data << 4);
}

void LcdDisplay::command(uint8_t cmd) {
  _lcd_mask &= (~LCD_RS_BIT);
  _lcd_mask &= (~LCD_RW_BIT);
  sendNibble((cmd & 0xF0) & (~LCD_RS_BIT));
  sendNibble((cmd << 4) & (~LCD_RS_BIT));
}

void LcdDisplay::backlight() {
  _lcd_mask |= LCD_BACKLIGHT_BIT;
  return command(0);
}

void LcdDisplay::clear() {
  command(LCD_CLEARDISPLAY);
  ets_delay_us(2000);
}

void LcdDisplay::home() {
  command(LCD_RETURNHOME);
  ets_delay_us(2000);
}

void LcdDisplay::print(const char *message) {
  const char *ptr = message;
  while (*ptr) {
    data(*(ptr++));
  }
}

void LcdDisplay::init() {

  vTaskDelay(100);
  _lcd_mask = 0x0;
  sendNibble(0);
  sendNibble(0x30);
  sendNibble(0x30);
  sendNibble(0x30);
  sendNibble(0x20);

  command(0x28);
  command(0x0C);
  command(0x01);
  command(0x06);
  ESP_LOGI(TAG, "Initialization complete");
}
