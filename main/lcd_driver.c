#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "lcd_driver.h"
#include "driver/gpio.h"
#include <string.h>

#define LCD_RS GPIO_NUM_14
#define LCD_EN GPIO_NUM_27
#define LCD_D4 GPIO_NUM_26
#define LCD_D5 GPIO_NUM_25
#define LCD_D6 GPIO_NUM_33
#define LCD_D7 GPIO_NUM_32

static void lcd_pulse_enable(void);
static void lcd_send_nibble(uint8_t nibble);
static void lcd_send_command(uint8_t cmd);
static void lcd_send_data(uint8_t data);

void lcd_init(void) {
    gpio_set_direction(LCD_RS, GPIO_MODE_OUTPUT);
    gpio_set_direction(LCD_EN, GPIO_MODE_OUTPUT);
    gpio_set_direction(LCD_D4, GPIO_MODE_OUTPUT);
    gpio_set_direction(LCD_D5, GPIO_MODE_OUTPUT);
    gpio_set_direction(LCD_D6, GPIO_MODE_OUTPUT);
    gpio_set_direction(LCD_D7, GPIO_MODE_OUTPUT);

    vTaskDelay(pdMS_TO_TICKS(50)); // initial power-up delay

    gpio_set_level(LCD_RS, 0);
    lcd_send_nibble(0x03);
    vTaskDelay(pdMS_TO_TICKS(5));
    gpio_set_level(LCD_RS, 0);
    lcd_send_nibble(0x03);
    vTaskDelay(pdMS_TO_TICKS(1));
    gpio_set_level(LCD_RS, 0);
    lcd_send_nibble(0x03);
    vTaskDelay(pdMS_TO_TICKS(1));
    gpio_set_level(LCD_RS, 0);
    lcd_send_nibble(0x02); // 4-bit mode
    

    lcd_send_command(0x28); // 2 lines, 5x8 font
    lcd_send_command(0x0C); // Display ON, cursor OFF
    lcd_send_command(0x06); // Entry mode
    lcd_send_command(0x01); // Clear display
    vTaskDelay(pdMS_TO_TICKS(2));
}

void lcd_clear(void) {
    lcd_send_command(0x01);
    vTaskDelay(pdMS_TO_TICKS(5));
}

void lcd_set_cursor(int row, int col) {
    uint8_t row_offsets[] = {0x00, 0x40};
    lcd_send_command(0x80 | (col + row_offsets[row]));
}

void lcd_write_string(const char *str) {
    while (*str) {
        lcd_send_data((uint8_t)(*str++));
    }
}

static void lcd_send_nibble(uint8_t nibble) {
    gpio_set_level(LCD_D4, (nibble >> 0) & 0x01);
    gpio_set_level(LCD_D5, (nibble >> 1) & 0x01);
    gpio_set_level(LCD_D6, (nibble >> 2) & 0x01);
    gpio_set_level(LCD_D7, (nibble >> 3) & 0x01);
    lcd_pulse_enable();
}

static void lcd_send_command(uint8_t cmd) {
    gpio_set_level(LCD_RS, 0);
    lcd_send_nibble(cmd >> 4);
    lcd_send_nibble(cmd & 0x0F);
    vTaskDelay(pdMS_TO_TICKS(2));
}

static void lcd_send_data(uint8_t data) {
    gpio_set_level(LCD_RS, 1);
    lcd_send_nibble(data >> 4);
    lcd_send_nibble(data & 0x0F);
    vTaskDelay(pdMS_TO_TICKS(2));
}

static void lcd_pulse_enable(void) {
    gpio_set_level(LCD_EN, 0);
    esp_rom_delay_us(10);
    gpio_set_level(LCD_EN, 1);
    esp_rom_delay_us(10);
    gpio_set_level(LCD_EN, 0);
    esp_rom_delay_us(300); // increased for stable pulse
}