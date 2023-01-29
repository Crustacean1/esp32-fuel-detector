#include "I2CBus.h"
#include "LcdDisplay.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
extern "C" {
#include "vl53l0x.h"
}
#include <stdio.h>

#define I2C_MASTER_SCL_IO                                                      \
  CONFIG_I2C_MASTER_SCL /*!< GPIO number used for I2C master clock */
#define I2C_MASTER_SDA_IO                                                      \
  CONFIG_I2C_MASTER_SDA /*!< GPIO number used for I2C master data  */
#define I2C_MASTER_NUM                                                         \
  0 /*!< I2C master i2c port number, the number of i2c peripheral interfaces   \
available will depend on the chip */
#define I2C_DUMMY_XSHUT 21
#define I2C_DUMMY_GPIO1 22

static const char *TAG = "i2c-simple-example";

extern "C" {
void app_main(void) {
  /*I2CBus i2c;
  i2c.scan();
  ESP_LOGI(TAG, "starting lcd display");
  LcdDisplay lcd(i2c, 0x27);
  lcd.init();
  lcd.clear();
  lcd.backlight();
  char message[80];
  int counter = 0;
  while (true) {
    sprintf(message, "Counter: %d", counter);
    lcd.print(message);
    vTaskDelay(100);
    lcd.clear();
    counter += 1;
  }*/

  int8_t sensorAddress = 0x29;

  vl53l0x_t *sensor =
      vl53l0x_config(I2C_MASTER_NUM, I2C_MASTER_SCL_IO, I2C_MASTER_SDA_IO,
                     I2C_DUMMY_XSHUT, sensorAddress, I2C_DUMMY_GPIO1);

  if (sensor == NULL) {
    ESP_LOGI(TAG, "Couldn't initialize sensor, check if device is present "
                  "under address 0x29");
    return;
  }
  ESP_LOGI(TAG, "started");
  const char *init_result = vl53l0x_init(sensor);
  ESP_LOGI(TAG, "Received pointer: %p", init_result);

  uint16_t distance;
  for (int i = 0; i < 100; ++i) {
    distance = vl53l0x_readRangeSingleMillimeters(sensor);
    ESP_LOGI(TAG, "Measured distance: %d", distance);
    vTaskDelay(20);
  }
  vl53l0x_end(sensor);
}
}
