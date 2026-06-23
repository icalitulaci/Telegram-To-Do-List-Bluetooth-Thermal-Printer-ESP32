# Telegram-To-Do-List-Printer (ESP32 + Bluetooth Thermal Printer)
Send a message to your Telegram bot, and the ESP32 will automatically forward it to a Bluetooth thermal printer, printing your tasks instantly. Great for shopping lists, reminders, notes, or daily to-do lists.

What it can do?
-  Receive messages from Telegram bot
-  Restrict printing to allowed Telegram chat IDs
-  Print messages via Bluetooth thermal printer (ESC/POS supported)
-  Auto WiFi reconnect
-  Continuous message polling
-  Clean formatted output (bold, centered header, wrapped text)

<img src="https://github.com/favicon.ico" width="48">


---

## Hardware Needed
- ESP32 Dev Kit (Will need Wifi and Classic Bluetooth)
- Any generic ESP-POS Bluetooth Thermal Printer ( Size 58mm is used in this case)
- Power Source

## How to set up
1. Install ESP32 Board and UniversalTelegramBot on the Arduino IDE
2. Edit the following values in the code:

```cpp
#define WIFI_SSID        "Your_WiFi_SSID"
#define WIFI_PASSWORD    "Your_WiFi_PASSWORD"

#define BOT_TOKEN        "YOUR_BOT_TOKEN_HERE" // Replace with your Telegram bot token gathered from BotFather

// Only works with bluetooth thermal printer that uses the classic Bluetooth protocol (No APP / not BLE).
#define PRINTER_NAME     "Your_Printer_Name"
#define PRINTER_PIN      "1234"  // Change to "" if your printer does not require a PIN

// insert the chat IDs of the telegram user to avoid any ddos attacks. 
const String allowedChats[] = {
  " 123456789",  // Replace with your Telegram chat ID
  "987654321"   // Add more chat IDs as needed
};

```

3. Change the setting in arduino IDE to accomodate large sketch file
   ```cpp
    > Tools > Partition Scheme> “No OTA (2MB APP / 2MB SPIFFS)
   ```
 4. Upload the sketch and monitor 115200 band
 5. Send your message to the telegram bot and wait for it to print
 6. Enjoy

Example message will look like this
```cpp
================

Buy milk
Eggs
Chicken breast

================
Printed: Jun 23 2026
```

## Common Posible Issue pitfall
- Using the wrong ESP32 (e.g Using Bluetooth Low Energy)
- Certain ESP32 will require to hold BOOT button during uploading
- Printer doesn't support Bluetooth Classic/Require a proprietary app
- Fail to connect/disconnect bluetooth printer? Try adjusting the delay
- Why bother disconnecting bluetooth printer after each print? Because certain model of ESP32 cannot receive the telegram message if the bluetooth remains connected.
- Format looks off --> can adjust the code in "ESC/POS Formating Parameters" and "Print Telegram Text"




#define WIFI_SSID        "Your_WiFi_SSID"
#define WIFI_PASSWORD    "Your_WiFi_PASSWORD"

#define BOT_TOKEN        "YOUR_BOT_TOKEN_HERE"

#define PRINTER_NAME     "Your_Printer_Name"
#define PRINTER_PIN      "1234"  // or "" if not required
