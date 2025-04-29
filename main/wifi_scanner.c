#include "wifi_scanner.h"
#include "esp_wifi.h"
#include "esp_log.h"
#include <string.h>

#define MAX_APS 20
static const char *TAG = "wifi_scanner";

void wifi_init_scan(void) {
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_start());
}

int wifi_scan_networks(wifi_ap_info_t *ap_list, int max_aps) {
    wifi_scan_config_t scan_config = {
        .ssid = NULL,
        .bssid = NULL,
        .channel = 0,
        .show_hidden = true
    };

    ESP_ERROR_CHECK(esp_wifi_scan_start(&scan_config, true));

    uint16_t number = max_aps;
    wifi_ap_record_t records[MAX_APS];
    ESP_ERROR_CHECK(esp_wifi_scan_get_ap_records(&number, records));

    for (int i = 0; i < number; i++) {
        strncpy(ap_list[i].ssid, (char *)records[i].ssid, sizeof(ap_list[i].ssid) - 1);
        ap_list[i].ssid[sizeof(ap_list[i].ssid) - 1] = '\0';
        ap_list[i].rssi = records[i].rssi;
        ap_list[i].is_open = (records[i].authmode == WIFI_AUTH_OPEN);
    }

    return number;
}