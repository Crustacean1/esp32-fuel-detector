#include "I2CBus.h"
#include "driver/i2c.h"
#include "esp_log.h"

static const char *TAG = "i2c-simple-example";

#define I2C_MASTER_SCL_IO                                                      \
  CONFIG_I2C_MASTER_SCL /*!< GPIO number used for I2C master clock */
#define I2C_MASTER_SDA_IO                                                      \
  CONFIG_I2C_MASTER_SDA /*!< GPIO number used for I2C master data  */
#define I2C_MASTER_NUM                                                         \
  0 /*!< I2C master i2c port number, the number of i2c peripheral interfaces   \
available will depend on the chip */

#define I2C_MASTER_FREQ_HZ 400000   /*!< I2C master clock frequency */
#define I2C_MASTER_TX_BUF_DISABLE 0 /*!< I2C master doesn't need buffer */
#define I2C_MASTER_RX_BUF_DISABLE 0 /*!< I2C master doesn't need buffer */
#define I2C_MASTER_TIMEOUT_MS 1000

I2CBus::I2CBus() {}

void I2CBus::write(uint8_t address, uint8_t value) {

  init();
  _buffer[0] = value;
  i2c_port_t i2c_master_port = (i2c_port_t)I2C_MASTER_NUM;
  i2c_cmd_handle_t link = i2c_cmd_link_create();

  esp_err_t result = 0;
  ESP_ERROR_CHECK(i2c_master_start(link));
  ESP_ERROR_CHECK(i2c_master_write_byte(link, address << 1 | 0, true));
  ESP_ERROR_CHECK(i2c_master_write_byte(link, value, true));
  ESP_ERROR_CHECK(i2c_master_stop(link));

  if (result == ESP_OK) {
    ESP_ERROR_CHECK(i2c_master_cmd_begin(i2c_master_port, link, 100));
  }
  i2c_cmd_link_delete(link);
}

bool I2CBus::ping(uint8_t address) {

  i2c_port_t i2c_master_port = (i2c_port_t)I2C_MASTER_NUM;
  i2c_cmd_handle_t link = i2c_cmd_link_create();

  i2c_master_start(link);
  i2c_master_write_byte(link, address << 1 | 0, true);
  i2c_master_stop(link);

  esp_err_t result = i2c_master_cmd_begin(i2c_master_port, link, 100);
  if (result == ESP_OK) {
    return true;
  } else if (result != ESP_FAIL) {
    ESP_LOGI(TAG, "Invalid response on address %uc", address);
  }
  return false;

  i2c_cmd_link_delete(link);
}

void I2CBus::init() {

  if (_initialized) {
    return;
  }
  _initialized = true;

  i2c_port_t i2c_master_port = (i2c_port_t)I2C_MASTER_NUM;

  i2c_config_t conf = {.mode = I2C_MODE_MASTER,
                       .sda_io_num = I2C_MASTER_SDA_IO,
                       .scl_io_num = I2C_MASTER_SCL_IO,
                       .sda_pullup_en = GPIO_PULLUP_ENABLE,
                       .scl_pullup_en = GPIO_PULLUP_ENABLE,
                       .master = {.clk_speed = I2C_MASTER_FREQ_HZ},
                       .clk_flags = 0};

  i2c_param_config(i2c_master_port, &conf);

  ESP_ERROR_CHECK(i2c_driver_install(i2c_master_port, conf.mode,
                                     I2C_MASTER_RX_BUF_DISABLE,
                                     I2C_MASTER_TX_BUF_DISABLE, 0));
}

void I2CBus::scan() {
  init();
  ESP_LOGI(TAG, "Started scanning");
  for (uint8_t addr = 0x01; addr < 128; ++addr) {
    if (ping(addr)) {
      ESP_LOGI(TAG, "Found device at address: 0x%02X", addr);
    }
  }
  ESP_LOGI(TAG, "Scanning done");
}
