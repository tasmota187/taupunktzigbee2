#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_http_server.h"
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

// Interne Messdaten (extern lesbar)
static float T1 = 0, RH1 = 0, T2 = 0, RH2 = 0, taupunkt = 0;
static bool luefter_status = false;
static httpd_handle_t server = NULL;

static const char *TAG = "web";

// Setter für aktuelle Sensorwerte
void web_update_sensorwerte(float t1, float rh1, float t2, float rh2, float dp) {
    T1 = t1;
    RH1 = rh1;
    T2 = t2;
    RH2 = rh2;
    taupunkt = dp;
}

// Extern abrufbar: Lüfterstatus
bool web_luefter_status() {
    return luefter_status;
}

esp_err_t control_get_handler(httpd_req_t *req) {
    char buf[512];
    snprintf(buf, sizeof(buf),
        "<html><head><meta charset='utf-8'><title>Taupunktsteuerung</title></head><body>"
        "<h2>Keller Taupunktregelung</h2>"
        "<p><strong>Sensor 1:</strong> %.1f °C, %.1f %% RH</p>"
        "<p><strong>Sensor 2:</strong> %.1f °C, %.1f %% RH</p>"
        "<p><strong>Taupunkt:</strong> %.1f °C</p>"
        "<p><strong>Lüfter:</strong> <span style='color:%s;'>%s</span></p>"
        "<form method='POST'>"
        "<button name='cmd' value='%s'>Lüfter %s</button>"
        "</form>"
        "</body></html>",
        T1, RH1, T2, RH2, taupunkt,
        luefter_status ? "green" : "red",
        luefter_status ? "AKTIV" : "AUS",
        luefter_status ? "off" : "on",
        luefter_status ? "AUS" : "EIN"
    );

    httpd_resp_set_type(req, "text/html");
    httpd_resp_send(req, buf, HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}

esp_err_t control_post_handler(httpd_req_t *req) {
    char buf[64] = {0};
    int ret = httpd_req_recv(req, buf, MIN(req->content_len, sizeof(buf) - 1));
    if (ret > 0) {
        if (strstr(buf, "cmd=on")) luefter_status = true;
        if (strstr(buf, "cmd=off")) luefter_status = false;
    }
    httpd_resp_set_status(req, "303 See Other");
    httpd_resp_set_hdr(req, "Location", "/");
    httpd_resp_send(req, NULL, 0);
    return ESP_OK;
}

void start_webserver() {
    // SoftAP starten
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));

    wifi_config_t ap_cfg = {
        .ap = {
            .ssid = "Taupunkt-Setup",
            .ssid_len = 0,
            .channel = 1,
            .password = "",
            .max_connection = 4,
            .authmode = WIFI_AUTH_OPEN
        }
    };
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &ap_cfg));
    ESP_ERROR_CHECK(esp_wifi_start());
    ESP_LOGI(TAG, "SoftAP aktiv: SSID 'Taupunkt-Setup'");

    // Webserver starten
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    httpd_uri_t uri_get = {
        .uri = "/", .method = HTTP_GET, .handler = control_get_handler
    };
    httpd_uri_t uri_post = {
        .uri = "/", .method = HTTP_POST, .handler = control_post_handler
    };

    httpd_start(&server, &config);
    httpd_register_uri_handler(server, &uri_get);
    httpd_register_uri_handler(server, &uri_post);
    ESP_LOGI(TAG, "Webserver läuft: http://192.168.4.1/");
}
