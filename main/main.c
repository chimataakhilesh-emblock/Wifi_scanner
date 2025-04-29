#include "lcd_driver.h" 
#include "wifi_scanner.h" 
#include "driver/gpio.h" 
#include "freertos/FreeRTOS.h" 
#include "freertos/task.h" 
#include "esp_log.h" 
#include "esp_sntp.h" 
#include "esp_event.h" 
#include "esp_wifi.h" 
#include "nvs_flash.h" 
#include <string.h> 
#include <stdio.h> 
#include <time.h>

#define BUTTON_NEXT GPIO_NUM_4
#define BUTTON_RESCAN GPIO_NUM_13

#define MAX_APS 20 
#define WIFI_SSID "Airtel_Home" 
#define WIFI_PASS "Rithwin@2016"

static wifi_ap_info_t ap_list[MAX_APS]; 
static int ap_count = 0; 
static int current_ap = 0;

static const char *TAG = "main";

void IRAM_ATTR button_next_isr_handler(void *arg) { 
    static uint32_t last_time = 0;
    uint32_t now = xTaskGetTickCountFromISR();
    if((now - last_time > pdMS_TO_TICKS(200)) && (ap_count > 0)) {
        current_ap = (current_ap + 1) % ap_count; 
        last_time = now;
    }
}

void IRAM_ATTR button_rescan_isr_handler(void *arg) {
     current_ap = 0;
     ap_count = wifi_scan_networks(ap_list, MAX_APS); 
    
    }

void setup_buttons(void) { 
    gpio_config_t io_conf = { 
        .intr_type = GPIO_INTR_POSEDGE, 
        .mode = GPIO_MODE_INPUT, 
        .pin_bit_mask = (1ULL << BUTTON_NEXT) | (1ULL << BUTTON_RESCAN), 
        .pull_up_en = 1, 
        .pull_down_en = 0, 
    }; 
    gpio_config(&io_conf);

gpio_install_isr_service(0);
gpio_isr_handler_add(BUTTON_NEXT, button_next_isr_handler, NULL);
gpio_isr_handler_add(BUTTON_RESCAN, button_rescan_isr_handler, NULL);

}

void sync_time_with_ntp(void) { 
    ESP_LOGI(TAG, "Connecting to WiFi for time sync");

wifi_config_t wifi_config = {
    .sta = {
        .ssid = WIFI_SSID,
        .password = WIFI_PASS,
    },
};

ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
ESP_ERROR_CHECK(esp_wifi_connect());

ESP_LOGI(TAG, "Waiting for IP and SNTP sync...");
sntp_setoperatingmode(SNTP_OPMODE_POLL);
sntp_setservername(0, "pool.ntp.org");
sntp_init();

time_t now = 0;
struct tm timeinfo = { 0 };
int retry = 0;

while (timeinfo.tm_year < (2020 - 1900) && retry++ < 10) {
    ESP_LOGI(TAG, "Waiting for time to be set... (%d/10)", retry);
    vTaskDelay(pdMS_TO_TICKS(400));
    time(&now);
    localtime_r(&now, &timeinfo);
}

esp_wifi_disconnect();
ESP_LOGI(TAG, "Time synced and Wi-Fi disconnected.");

}

void show_ap_on_lcd(void) { lcd_clear();

if (ap_count == 0) {
    lcd_write_string("No networks");
    lcd_set_cursor(1, 0);
    lcd_write_string("found...");
    return;
}

wifi_ap_info_t *ap = &ap_list[current_ap];
char line1[17];
snprintf(line1, sizeof(line1), "%.7s %2ddB %c",
         ap->ssid, ap->rssi,
         ap->is_open ? 'O' : 'L');
lcd_write_string(line1);

time_t now;
struct tm timeinfo;
time(&now);
localtime_r(&now, &timeinfo);

char line2[17];
strftime(line2, sizeof(line2), "%a %d-%m", &timeinfo);
lcd_set_cursor(1, 0);
lcd_write_string(line2);

}

void app_main(void) { 
    ESP_ERROR_CHECK(nvs_flash_init()); 
    ESP_ERROR_CHECK(esp_netif_init()); 
    ESP_ERROR_CHECK(esp_event_loop_create_default());

lcd_init();
lcd_clear();
setup_buttons();

wifi_init_scan();
sync_time_with_ntp();
ap_count = wifi_scan_networks(ap_list, MAX_APS);

while (1) {
    show_ap_on_lcd();
    vTaskDelay(pdMS_TO_TICKS(100));
}

}