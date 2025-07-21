#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_vendor.h"

#define SDA_GPIO 10
#define SCL_GPIO 11

static esp_lcd_panel_handle_t panel;

void display_init(){
  i2c_config_t cfg = { .mode=I2C_MODE_MASTER, .sda_io_num=SDA_GPIO, .scl_io_num=SCL_GPIO, .master.clk_speed=400000 };
  i2c_param_config(I2C_NUM_0, &cfg);
  i2c_driver_install(I2C_NUM_0, cfg.mode, 0,0,0);
  esp_lcd_panel_io_i2c_config_t io_cfg = { .i2c_port = I2C_NUM_0, .addr = 0x3C };
  esp_lcd_panel_io_handle_t io = NULL;
  esp_lcd_new_panel_io_i2c(&io, &io_cfg);
  esp_lcd_panel_dev_config_t dev_cfg = { .reset_gpio_num = -1, .color_space = ESP_LCD_COLOR_SPACE_MONOCHROME };
  esp_lcd_new_panel_ssd1306(io, &dev_cfg, &panel);
}

void display_update(float tau){
  char buf[32];
  esp_lcd_panel_clear(panel, true);
  snprintf(buf, sizeof(buf), "Tau: %.1f C", tau);
  // Textposition setzen und schreiben
}
