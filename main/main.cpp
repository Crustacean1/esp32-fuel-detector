#include "I2CBus.h"
#include "LcdDisplay.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <stdio.h>

static const char *TAG = "i2c-simple-example";

extern "C" {
void app_main(void) {
  I2CBus i2c;
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
    ESP_LOGI(TAG, "LOOP %d", counter);
    lcd.print(message);
    vTaskDelay(100);
    lcd.clear();
    counter += 1;
  }
}
}
