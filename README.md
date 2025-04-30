ESP32 Wi-Fi Scanner with LCD Display

This project uses an ESP32, a 16x2 HD44780 LCD, and two buttons to:

Scan available Wi-Fi networks

Display each network's:

SSID (Wi-Fi name)

Signal strength (RSSI)

Open or Locked status


Navigate through networks with one button

Rescan Wi-Fi with another button


All of this is done using ESP-IDF, with a custom LCD driver (no prebuilt libraries).

Project Structure

project/
├── main/
│   ├── main.c            // Main logic: buttons, display
│   ├── lcd_driver.c/h    // Custom LCD control (4-bit mode)
│   ├── wifi_scanner.c/h  // Wi-Fi scanning logic
├── CMakeLists.txt
├── sdkconfig
└── README.md


---

How it Works

1. LCD Driver

Custom low-level GPIO control for HD44780 LCD in 4-bit mode

lcd_write_string(line, string) displays text on LCD

2. Wi-Fi Scanner

Uses ESP32 station mode to scan Wi-Fi

Displays:

SSID

RSSI (signal strength)

Security status (L for locked, O for open)

3. Buttons

Button 1 (GPIO0): Scroll to next Wi-Fi network

Button 2 (GPIO2): Rescan for available networks
---

How to Build & Flash

1. Set up ESP-IDF

Follow ESP-IDF Getting Started Guide

2. Build & Flash

idf.py build
idf.py -p /dev/ttyUSB0 flash monitor

3. Watch the LCD

First line: Wi-Fi name, signal, and lock status

Second line: (currently unused or optional message)
---

Example LCD Output

SSID: HomeWiFi  -68dB L
(Second line can be blank or say "Wi-Fi 1 of 4")

---

Future Plans

Add date and day display via SNTP (Internet time)

Add signal strength bars

Optional connection to a selected Wi-Fi
---