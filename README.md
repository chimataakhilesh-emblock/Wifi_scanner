# Wifi_scanner
ESP32 + 16x2 LCD Display Project (4-bit GPIO Mode)

This project uses an ESP32 (WROOM Kit) with a 16x2 HD44780 LCD in 4-bit mode. It displays:

Available Wi-Fi networks (with SSID, signal strength, and lock status),

Current date and day (fetched using SNTP),

Navigation using two buttons.

---

Hardware Setup

LCD Wiring (HD44780 - 16x2)

---

Button Pins

Use a 10k pull-down resistor for each button if needed.


---

Folder Structure

project_root/
│
├── main/
│   ├── lcd_driver.c        # Handles LCD display
│   ├── lcd_driver.h
│   ├── wifi_scanner.c      # Scans and stores Wi-Fi SSIDs
│   ├── wifi_scanner.h
│   ├── main.c              # Controls buttons + LCD update
├── CMakeLists.txt
├── sdkconfig
└── README.md              # This file


---

Key Functionalities

1. LCD Initialization

Uses 4-bit communication.

Initializes with timing delays (as per HD44780 datasheet).

Functions include:

lcd_init()

lcd_clear()

lcd_set_cursor(row, col)

lcd_write_string("text")




---

2. Wi-Fi Scanning

Scans nearby SSIDs.

Collects:

SSID name

Signal strength (RSSI)

Lock status (open/secured)




---

3. SNTP Time Sync

Connects to internet only to get current time.

Uses SNTP server (e.g., pool.ntp.org).

Updates and displays:

Day

Date (DD-MM-YYYY)




---

4. Button Control

Button 1 → Move to next Wi-Fi network

Button 2 → Re-scan and update list

If end of list reached, it stays on last network



---

To Build and Flash

idf.py set-target esp32
idf.py menuconfig  # Optional Wi-Fi setup if needed
idf.py build
idf.py -p /dev/ttyUSB0 flash monitor


---

Troubleshooting

Black Squares on LCD?

RW pin must be GND

Ensure contrast is adjusted properly

Double check GPIO pin assignments

Add delays after LCD commands (5–10ms)

LCD might not be initialized correctly – review lcd_init() steps



---

Would you like this as a downloadable .md file or want to continue editing it?