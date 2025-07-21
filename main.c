#include "sensors.h"
#include "display.h"
#include "esp_zb_core.h"
#include "esp_zb_zcl_on_off.h"

#define EP 10
#define TAU_THRESH 12.0

static uint16_t dest_addr=0; static uint8_t dest_ep=0;

void zb_signal_handler(esp_zb_app_signal_t *sig){
  if(esp_zb_app_signal_get_type(sig)==ZB_BDB_SIGNAL_STEERING)
    printf("Pairing complete\n");
}

void app_main(){
  sensors_init(); display_init();

  esp_zb_cfg_t zb_cfg = ESP_ZB_ZC_CONFIG();
  esp_zb_init(&zb_cfg);

  esp_zb_endpoint_register(EP, NULL, NULL);
  esp_zb_zcl_on_off_register(EP, ESP_ZB_ZCL_ON_OFF_SERVER_ROLE);
  esp_zb_bdb_set_signal_handler(zb_signal_handler);
  esp_zb_bdb_start_top_level_commissioning(ZB_BDB_NETWORK_STEERING);
  esp_zb_start();

  while(1){
    float T1,RH1,T2,RH2;
    if(bme_read1(&T1,&RH1)&&bme_read2(&T2,&RH2)){
      float dp=fminf(calc_taupunkt(T1,RH1), calc_taupunkt(T2,RH2));
      display_update(dp);

      esp_zb_zcl_on_off_cmd_req_t req = {
        .address_mode = ESP_ZB_APS_ADDR_MODE_16_ENDP_PRESENT,
        .dst_addr.short_addr = dest_addr, .dst_endpoint = dest_ep,
        .src_endpoint = EP,
        .on_off_cmd_id = (dp>=TAU_THRESH ? ESP_ZB_ZCL_CMD_ON_OFF_ON_ID : ESP_ZB_ZCL_CMD_ON_OFF_OFF_ID)
      };
      esp_zb_zcl_on_off_cmd_req(&req);
    }
    esp_zb_main_loop_iteration();
    vTaskDelay(pdMS_TO_TICKS(5000));
  }
}
