#include "lcd_driver.h"
#include "wifi_scanner.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_log.h" // Include for ESP_LOGI
#include "esp_event.h"
#include "esp_wifi.h"
#include "nvs_flash.h"
#include <string.h>
#include <stdio.h>

#define BUTTON_NEXT GPIO_NUM_4
#define BUTTON_RESCAN GPIO_NUM_13

#define MAX_APS 20

static wifi_ap_info_t ap_list[MAX_APS];
static int ap_count = 0;
static int current_ap = 0;

static const char *TAG = "main";

static QueueHandle_t scan_queue;

void IRAM_ATTR button_next_isr_handler(void *arg) {
    static uint32_t last_time = 0;
    uint32_t now = xTaskGetTickCountFromISR();
    if ((now - last_time > pdMS_TO_TICKS(200)) && (ap_count > 0)) {
        current_ap = (current_ap + 1) % ap_count;
        last_time = now;
    }
}

void IRAM_ATTR button_rescan_isr_handler(void *arg) {
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    uint32_t rescan_signal = 1;
    xQueueSendFromISR(scan_queue, &rescan_signal, &xHigherPriorityTaskWoken);
    if (xHigherPriorityTaskWoken) {
        portYIELD_FROM_ISR();
    }
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

void scan_task(void *pvParameters) {
    while (1) {
        uint32_t rescan_signal;
        if (xQueueReceive(scan_queue, &rescan_signal, portMAX_DELAY)) {
            ESP_LOGI(TAG, "Rescan triggered by button.");
            current_ap = 0;
            ap_count = wifi_scan_networks(ap_list, MAX_APS);
            ESP_LOGI(TAG, "Scan complete. Found %d networks.", ap_count);
            for (int i = 0; i < ap_count; i++) {
                ESP_LOGI(TAG, "Network %d: SSID=%s, RSSI=%d, Open=%s",
                         i + 1, (char *)ap_list[i].ssid, ap_list[i].rssi, ap_list[i].is_open ? "Yes" : "No");
            }
        }
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

void show_ap_on_lcd(void) {
    lcd_clear();

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

    lcd_set_cursor(1, 0);
    char line2[17];
    snprintf(line2, sizeof(line2), "AP %d/%d", current_ap + 1, ap_count);
    lcd_write_string(line2);
}

void app_main(void) {
    ESP_ERROR_CHECK(nvs_flash_init());
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    // Initialize LCD with a common pinout - ADJUST THESE IF YOUR WIRING IS DIFFERENT!
    lcd_init();
    lcd_clear();
    ESP_LOGI(TAG, "LCD Initialized");

    setup_buttons();
    ESP_LOGI(TAG, "Buttons Setup");

    wifi_init_scan();
    ESP_LOGI(TAG, "WiFi Scanner Initialized");
    ap_count = wifi_scan_networks(ap_list, MAX_APS);
    ESP_LOGI(TAG, "Initial Scan Complete. Found %d networks.", ap_count);
    for (int i = 0; i < ap_count; i++) {
        ESP_LOGI(TAG, "Network %d: SSID=%s, RSSI=%d, Open=%s",
                 i + 1, (char *)ap_list[i].ssid, ap_list[i].rssi, ap_list[i].is_open ? "Yes" : "No");
    }

    scan_queue = xQueueCreate(1, sizeof(uint32_t));
    if (scan_queue == NULL) {
        ESP_LOGE(TAG, "Failed to create scan queue.");
        return;
    }

    BaseType_t xReturned;
    xReturned = xTaskCreate(
        scan_task,
        "ScanTask",
        4096,
        NULL,
        10,
        NULL);
    if (xReturned == pdFAIL) {
        ESP_LOGE(TAG, "Failed to create scan task.");
        return;
    }
    ESP_LOGI(TAG, "Scan Task Created");

    while (1) {
        show_ap_on_lcd();
        vTaskDelay(pdMS_TO_TICKS(200));
    }
}