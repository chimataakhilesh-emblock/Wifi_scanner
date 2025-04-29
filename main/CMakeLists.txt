idf_component_register(SRCS "wifi_scanner.c" "main.c" "lcd_driver.c"
                      INCLUDE_DIRS "."
                      PRIV_REQUIRES esp_timer esp_wifi esp_netif nvs_flash esp_event esp_driver_gpio)