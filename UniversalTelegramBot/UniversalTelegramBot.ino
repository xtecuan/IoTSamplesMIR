/*
  Rui Santos
  Complete project details at https://RandomNerdTutorials.com/telegram-group-esp32-esp8266/

  Project created using Brian Lough's Universal Telegram Bot Library: https://github.com/witnessmenow/Universal-Arduino-Telegram-Bot
*/


#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>   // Version 1.3.0 Universal Telegram Bot Library written by Brian Lough: https://github.com/witnessmenow/Universal-Arduino-Telegram-Bot
#include <ArduinoJson.h> //Version de Libreria 6.15.2

// Replace with your network credentials
const char* ssid = "JULIAN_RIVERA-ext";
const char* password = "Locator90210jrm";

// Initialize Telegram BOT
#define BOTtoken "1723433441:AAGI4TRFyvXIVkO1F9kKtRGkYH4DnjbOaV0"  // your Bot Token (Get from Botfather)

// Use @myidbot to find out the chat ID of an individual or a group
// Also note that you need to click "start" on a bot before it can
// message you
#define CHAT_ID "-577515097"

#ifdef ESP8266
X509List cert(TELEGRAM_CERTIFICATE_ROOT);
#endif

WiFiClientSecure client;
UniversalTelegramBot bot(BOTtoken, client);

// Checks for new messages every 1 second.
int botRequestDelay = 1000;
unsigned long lastTimeBotRan;

const int ledPin = 2;
bool ledState = LOW;

// Handle what happens when you receive new messages
void handleNewMessages(int numNewMessages) {
  Serial.println("handleNewMessages");
  Serial.println(String(numNewMessages));

  for (int i = 0; i < numNewMessages; i++) {
    // Chat id of the requester
    String chat_id = String(bot.messages[i].chat_id);
    if (chat_id != CHAT_ID) {
      bot.sendMessage(chat_id, "Unauthorized user", "");
      continue;
    }

    // Print the received message
    String text = bot.messages[i].text;
    Serial.println(text);

    String from_name = bot.messages[i].from_name;

    if (text.equalsIgnoreCase("encender")) {
      bot.sendMessage(chat_id, "El Foco está encendido!, servicio completado...", "");
      ledState = HIGH;
      digitalWrite(ledPin, ledState);
    }

    if (text.equalsIgnoreCase("apagar")) {
      bot.sendMessage(chat_id, "El Foco está apagado!, servicio completado...", "");
      ledState = LOW;
      digitalWrite(ledPin, ledState);
    }

    if (text.equalsIgnoreCase("estado")) {
      if (digitalRead(ledPin)) {
        bot.sendMessage(chat_id, "Estado: Foco Encendido!", "");
      }
      else {
        bot.sendMessage(chat_id, "Estado: Foco Apagado!", "");
      }
    }
  }
}

void setup() {
  Serial.begin(115200);

#ifdef ESP8266
  configTime(0, 0, "pool.ntp.org");      // get UTC time via NTP
  client.setTrustAnchors(&cert); // Add root certificate for api.telegram.org
#endif

  pinMode(ledPin, OUTPUT);

  digitalWrite(ledPin, ledState);

  // Connect to Wi-Fi
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
#ifdef ESP32
  client.setCACert(TELEGRAM_CERTIFICATE_ROOT); // Add root certificate for api.telegram.org
#endif
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }
  // Print ESP32 Local IP Address
  Serial.println(WiFi.localIP());

  bot.sendMessage(CHAT_ID, "Bot Started", "");
}

void loop() {
  if (millis() > lastTimeBotRan + botRequestDelay) {
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);

    while (numNewMessages) {
      Serial.println("got response");
      handleNewMessages(numNewMessages);
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }
    lastTimeBotRan = millis();
  }
}
