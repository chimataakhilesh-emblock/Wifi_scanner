#ifndef LCD_DRIVER_H
#define LCD_DRIVER_H

void lcd_init(void);
void lcd_clear(void);
void lcd_set_cursor(int row, int col);
void lcd_write_string(const char *str);


#endif // LCD_DRIVER_H 