#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <TinyGPS++.h>

// ===== Wi-Fi & Telegram =====
const char* ssid = "POCO X4 Pro 5G";
const char* password = "123456789";
const char* botToken = "8387416036:AAFsFJzytuBUuUQFLpTxjbeEl1tFxLwh6yw";
String chatID = "5749660077";

// ===== Pins & Variables =====
const int VIB_PIN = 14;
int lastState = HIGH;
int tapCount = 0;
unsigned long lastTapTime = 0;

const unsigned long DEBOUNCE_MS = 200;
const unsigned long TAP_WAIT_MS = 800;

long lastUpdateId = 0;
unsigned long lastTelegramCheck = 0;
const unsigned long TELEGRAM_INTERVAL = 5000;

// ===== LCD Setup =====
LiquidCrystal_I2C lcd(0x27,16,2);

// ===== GPS Setup =====
#define GPS_TX_PIN 27
#define GPS_RX_PIN 26
TinyGPSPlus gps;
float lat = 0;
float lng = 0;

// ===== Function Prototypes =====
void detectVibration();
void sendTelegramMessage(String text);
void getTelegramMessages();
void getGPSData();

void setup() {
  Serial.begin(115200);
  pinMode(VIB_PIN, INPUT_PULLUP);
  lastState = digitalRead(VIB_PIN);

  // ===== LCD Init =====
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Connecting WiFi");

  // ===== Wi-Fi Connect =====
  WiFi.begin(ssid, password);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi Connected!");
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("WiFi Connected");
  delay(1000);

  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Vibration Ready");

  // ===== GPS Init =====
  Serial1.begin(9600, SERIAL_8N1, GPS_TX_PIN, GPS_RX_PIN);
  Serial.println("GPS Initialized...");
}

void loop() {
  detectVibration();

  // Periodic Telegram check
  unsigned long now = millis();
  if (now - lastTelegramCheck > TELEGRAM_INTERVAL) {
    getTelegramMessages();
    lastTelegramCheck = now;
  }

  delay(10);
}

// ===== Detect vibration patterns =====
void detectVibration() {
  int currentState = digitalRead(VIB_PIN);
  unsigned long now = millis();

  if (lastState == HIGH && currentState == LOW && now - lastTapTime > DEBOUNCE_MS) {
    tapCount++;
    lastTapTime = now;
    Serial.print("Tap #"); Serial.println(tapCount);
  }
  lastState = currentState;

  if (tapCount > 0 && now - lastTapTime > TAP_WAIT_MS) {
    if (tapCount == 3) {
      sendTelegramMessage("Help! 3 vibrations detected!");
      lcd.clear(); lcd.setCursor(0,0); lcd.print("3 taps detected");
      lcd.setCursor(0,1); lcd.print("Msg sent: Help");
    } else if (tapCount == 4) {
      sendTelegramMessage("Call me! 4 vibrations detected!");
      lcd.clear(); lcd.setCursor(0,0); lcd.print("4 taps detected");
      lcd.setCursor(0,1); lcd.print("Msg sent: Call");
    } else if (tapCount == 5) {
      lcd.clear(); lcd.setCursor(0,0); lcd.print("5 taps detected");
      lcd.setCursor(0,1); lcd.print("Sending GPS...");
      getGPSData();

      if (lat != 0 && lng != 0) {
        String gpsMessage = "My Location: https://maps.google.com/?q=" + String(lat,6) + "," + String(lng,6);
        sendTelegramMessage(gpsMessage);
        lcd.clear();
        lcd.setCursor(0,0); lcd.print("GPS sent!");
        lcd.setCursor(0,1); lcd.print(String(lat,2)+","+String(lng,2));
      } else {
        sendTelegramMessage("GPS location not available!");
        lcd.clear(); lcd.setCursor(0,0); lcd.print("GPS failed!");
      }
    }
    tapCount = 0;
  }
}

// ===== Send message to Telegram =====
void sendTelegramMessage(String text) {
  if (WiFi.status() != WL_CONNECTED) return;

  WiFiClientSecure client;
  client.setInsecure();  // Skip SSL check
  HTTPClient https;

  String url = "https://api.telegram.org/bot" + String(botToken) + "/sendMessage?chat_id=" + chatID + "&text=" + text;
  if (https.begin(client, url)) {
    int code = https.GET();
    if (code > 0) Serial.println("Message sent successfully.");
    else Serial.print("Error sending message: "), Serial.println(code);
    https.end();
  } else Serial.println("Unable to connect to Telegram server");
}

// ===== Get messages from Telegram =====
void getTelegramMessages() {
  if (WiFi.status() != WL_CONNECTED) return;

  WiFiClientSecure client;
  client.setInsecure();
  HTTPClient https;

  String url = "https://api.telegram.org/bot" + String(botToken) + "/getUpdates?offset=" + String(lastUpdateId + 1);
  if (https.begin(client, url)) {
    int code = https.GET();
    if (code > 0) {
      String payload = https.getString();
      DynamicJsonDocument doc(4096);
      DeserializationError error = deserializeJson(doc, payload);
      if (!error) {
        JsonArray result = doc["result"].as<JsonArray>();
        for (JsonObject messageObj : result) {
          long updateId = messageObj["update_id"];

          // ===== FIX: Use username OR first_name + last_name =====
          String fromUser;
          if (messageObj["message"]["from"].containsKey("username"))
            fromUser = messageObj["message"]["from"]["username"].as<String>();
          else {
            fromUser = messageObj["message"]["from"]["first_name"].as<String>();
            if (messageObj["message"]["from"].containsKey("last_name"))
              fromUser += " " + messageObj["message"]["from"]["last_name"].as<String>();
          }

          String text = messageObj["message"]["text"];
          String fromChatId = String(messageObj["message"]["chat"]["id"]);

          if (fromChatId == chatID) {
            Serial.print("Message from "); Serial.print(fromUser); Serial.print(": "); Serial.println(text);
            lcd.clear(); lcd.setCursor(0,0); lcd.print("From:"); lcd.print(fromUser);
            lcd.setCursor(0,1); lcd.print(text.substring(0,16));
          }
          lastUpdateId = updateId;
        }
      }
    }
    https.end();
  }
}

// ===== Read GPS data =====
void getGPSData() {
  Serial.println("Reading GPS data...");
  lat = 0; lng = 0;
  unsigned long start = millis();
  const unsigned long timeout = 30000; // wait up to 30s

  while (millis() - start < timeout) {
    while (Serial1.available() > 0) gps.encode(Serial1.read());
    if (gps.location.isUpdated() && gps.location.isValid()) {
      lat = gps.location.lat();
      lng = gps.location.lng();
      Serial.print("Latitude: "); Serial.println(lat,6);
      Serial.print("Longitude: "); Serial.println(lng,6);
      return;
    }
    delay(100);
  }
  Serial.println("GPS data not available after 30 seconds.");
}
