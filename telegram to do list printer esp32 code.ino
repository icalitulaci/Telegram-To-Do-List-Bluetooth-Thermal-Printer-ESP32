
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <BluetoothSerial.h>

// ================= CONFIG =================
// This is the initial setup for the ESP32 Telegram Printer. 

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

bool isAuthorized(String chatId) {
  for (String id : allowedChats) {
    if (chatId == id) return true;
  }
  return false;
}


// ================= END CONFIG =================


// ---------- CONSTANTS ----------

const unsigned long TELEGRAM_CHECK_INTERVAL = 1000; // this is for checking new messages in telegram
const unsigned long BT_RECOVERY_DELAY = 2000; // this is for delay between bluetooth & wifi


WiFiClientSecure secured_client;
UniversalTelegramBot bot(BOT_TOKEN, secured_client);
BluetoothSerial SerialBT;

unsigned long lastTelegramCheck = 0;



// ----------Connecting to WIFI ----------

void connectWiFi() {

  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  Serial.print("Connecting WiFi");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("WiFi Connected");
  Serial.println(WiFi.localIP());
}

// ---------- Connecting to Bluetooth Printer ----------

bool connectPrinter() {

  Serial.println("Starting Bluetooth...");

  SerialBT.begin("ESP32_PRINTER", true);

  delay(1000);

  SerialBT.setPin(PRINTER_PIN, strlen(PRINTER_PIN));

  Serial.println("Connecting Printer...");

  bool connected = SerialBT.connect(PRINTER_NAME);

  if (connected) {
    Serial.println("Printer Connected");
  } else {
    Serial.println("Printer Connection Failed");
  }

  return connected;
}

// ---------- disconnecting from Bluetooth Printer ----------


void disconnectPrinter() {

  if (SerialBT.hasClient()) {
    SerialBT.disconnect();
    delay(1000);
  }

  SerialBT.end();

  Serial.println("Bluetooth Stopped");

  delay(BT_RECOVERY_DELAY);
}

// ---------- ESC/POS Formating Parameters ----------

void printerInit() {
  SerialBT.write(0x1B);
  SerialBT.write('@');
}

void printLine(String text) {
  SerialBT.print(text);
  SerialBT.print("\n");
}

void feedLines(int count) {
  for (int i = 0; i < count; i++) {
    SerialBT.print("\n");
  }
}

void boldOn() {
  SerialBT.write(0x1B);
  SerialBT.write(0x45);
  SerialBT.write(1);
}

void boldOff() {
  SerialBT.write(0x1B);
  SerialBT.write(0x45);
  SerialBT.write(0);
}

void doubleSizeOn() {
  SerialBT.write(0x1D);
  SerialBT.write(0x21);
  SerialBT.write(0x11);   // 2x width + 2x height
}

void normalSize() {
  SerialBT.write(0x1D);
  SerialBT.write(0x21);
  SerialBT.write(0x00);
}

void centerOn() {
  SerialBT.write(0x1B);
  SerialBT.write(0x61);
  SerialBT.write(1);
}

void leftAlign() {
  SerialBT.write(0x1B);
  SerialBT.write(0x61);
  SerialBT.write(0);
}

void printWrapped(String text, int maxChars) { 

  while (text.length() > 0) {

    if (text.length() <= maxChars) {
      printLine(text);
      break;
    }

    int breakPos = text.lastIndexOf(' ', maxChars);

    if (breakPos <= 0)
      breakPos = maxChars;

    printLine(text.substring(0, breakPos));

    text = text.substring(breakPos);
    text.trim();
  }
}


// ---------- Print Telegram Text ----------
// this helps to format the message that will be printed on the thermal printer. 
// In this scenario the message will only list what the user has sent along with the date of printing. You can modify this to your liking.

void printTelegramText(String sender, String message) {

  if (!connectPrinter()) {
    return;
  }


    printerInit();

    feedLines(2);

    centerOn();
    printLine("================");
    printLine("");

    leftAlign();
    boldOn();
    doubleSizeOn();

    printWrapped(message, 14); // the number 14 is the maximum number of characters that can be printed in a single line. May need to adjust based on printer size

    normalSize();
    boldOff();
    printLine("");
    centerOn();

    printLine("================");
    leftAlign();
    printLine("Printed:" __DATE__);


    feedLines(3);

  Serial.print("Printed: ");
  Serial.println(message);

  delay(1000);

  disconnectPrinter();
}

// ---------- Check new message on telegram ----------

void processNewestMessage() {

  int count = bot.getUpdates(bot.last_message_received + 1);

  if (count <= 0) {
    return;
  }

  int newest = count - 1;

  String chatId = bot.messages[newest].chat_id;


  if (!isAuthorized(chatId)) {
  Serial.println("Unauthorized Chat");
  return;
}

  String sender = bot.messages[newest].from_name;
  String text   = bot.messages[newest].text;

  Serial.println("New Telegram Message:");
  Serial.println(text);

  bot.sendMessage(chatId, "Printing...", "");

  printTelegramText(sender, text);
}

// ---------- SETUP ----------

void setup() {

  Serial.begin(115200);

  connectWiFi();

  secured_client.setInsecure();

  Serial.println("Ready");
}

// ---------- LOOP ----------

void loop() {

  if (millis() - lastTelegramCheck >= TELEGRAM_CHECK_INTERVAL) {

    processNewestMessage();

    lastTelegramCheck = millis();
    Serial.println("Checking Messages");
  }

  if (WiFi.status() != WL_CONNECTED) {
    connectWiFi();
  }
}

