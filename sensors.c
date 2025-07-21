#include "driver/i2c.h"
#include <stdio.h>
#include <math.h>

#define I2C_NUM I2C_NUM_0
#define SDA_GPIO 10
#define SCL_GPIO 11

void sensors_init(){
  i2c_config_t c = { .mode = I2C_MODE_MASTER, .sda_io_num=SDA_GPIO, .scl_io_num=SCL_GPIO, .master.clk_speed=100000 };
  i2c_param_config(I2C_NUM, &c);
  i2c_driver_install(I2C_NUM, c.mode, 0,0,0);
}

float calc_taupunkt(float T, float RH){
  const float a=17.27, b=237.7;
  float alpha=(a*T/(b+T)) + logf(RH/100.0);
  return (b*alpha)/(a-alpha);
}

// Stub-Funktionen zum Auslesen der beiden Sensoren (0x76, 0x77)
bool bme_read(uint8_t addr, float *T, float *RH){
  // Implementiere I2C-Lesezugriff gemäß BME280 Datenblatt
  // Einfachheitshalber intern via Adafruit-Code portieren
  return false;
}
bool bme_read1(float* T, float* RH){ return bme_read(0x76, T, RH); }
bool bme_read2(float* T, float* RH){ return bme_read(0x77, T, RH); }
