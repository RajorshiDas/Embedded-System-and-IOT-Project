🚨 ESP32 Vibration Alert & GPS Location Sender with Telegram Integration

This project uses an ESP32, a vibration sensor, a GPS module, and a 16x2 I2C LCD to detect vibration patterns and send alerts or live GPS locations to a Telegram bot. It’s useful for emergency alerts, security systems, or personal safety devices.

📱 Features

Triple Tap (3x) → Sends a “Help!” alert via Telegram

Quad Tap (4x) → Sends a “Call me!” message via Telegram

Five Tap (5x) → Reads GPS coordinates and sends a Google Maps link

LCD Display → Shows system status and incoming Telegram messages

Wi-Fi Connection → Uses your phone’s hotspot or local router

Telegram Bot → Communicates alerts and receives commands

🧠 How It Works

The vibration sensor detects quick consecutive taps.

Depending on how many taps are detected, it performs a specific action:

3 taps → Send “Help!”

4 taps → Send “Call me!”

5 taps → Fetch GPS and send current location link

The GPS module (via TinyGPS++) provides real-time latitude and longitude.

The ESP32 sends the message to Telegram using HTTPS requests through the Telegram Bot API.

The LCD display updates status messages such as connection, tap detection, and message sending.

⚙️ Hardware Components
Component	Description
ESP32	Main microcontroller with Wi-Fi
Vibration Sensor	Detects tap/vibration patterns
GPS Module (e.g., NEO-6M)	Provides latitude & longitude
I2C LCD (16x2)	Displays system messages
Jumper Wires	For connections
Power Source	USB or external 5V
🔌 Pin Connections
Module	ESP32 Pin	Description
Vibration Sensor	D14	Signal pin
GPS TX	D27	TX from GPS to ESP32 RX
GPS RX	D26	RX from GPS to ESP32 TX
LCD I2C	SDA (21), SCL (22)	Default I2C pins
🧩 Libraries Used

Install the following libraries in the Arduino IDE:

WiFi.h (built-in)

WiFiClientSecure.h (built-in)

HTTPClient.h (built-in)

ArduinoJson (for parsing Telegram responses)

LiquidCrystal_I2C (for the LCD)

TinyGPSPlus (for GPS data parsing)

🪄 Setup Instructions

Create a Telegram Bot

Open Telegram and search for @BotFather

Send /newbot and follow the instructions

Copy your bot token

Get Your Chat ID

Start your bot by sending any message to it

Open:

https://api.telegram.org/bot<YourBotToken>/getUpdates


Look for "chat":{"id":...} in the response

Configure the Code
Edit the following fields in the code:

const char* ssid = "YourWiFiSSID";
const char* password = "YourWiFiPassword";
const char* botToken = "YourBotToken";
String chatID = "YourChatID";


Upload the Code

Select ESP32 Dev Module in Arduino IDE

Set proper COM port

Upload the sketch

Monitor Serial Output
Open Serial Monitor at 115200 baud to see connection and GPS logs.

📟 LCD Display Messages
Display Message	Meaning
Connecting WiFi	Trying to connect to hotspot/router
WiFi Connected	Successfully connected
Vibration Ready	Ready to detect taps
3 taps detected	Help alert sent
4 taps detected	Call alert sent
5 taps detected	Sending GPS location
GPS sent!	Location message sent successfully
GPS failed!	GPS data not available
📡 Telegram Commands (Optional)

You can also send messages to the ESP32 via Telegram.
When a message is received, it shows on the LCD display (first 16 characters).

🧭 Example Telegram Output
Help! 3 vibrations detected!
Call me! 4 vibrations detected!
My Location: https://maps.google.com/?q=23.780000,90.410000

⚠️ Notes

Ensure the GPS module has a clear view of the sky for accurate readings.

Keep the vibration sensor sensitive enough but not too noisy.

You can modify vibration count thresholds to add more actions.

🧑‍💻 Author

Developed by: Rajorshi
Platform: Arduino (ESP32)
Language: C++
Purpose: Safety alert system using Telegram and GPS
