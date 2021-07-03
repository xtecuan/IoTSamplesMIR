/*
    Creador:
    Rui Santos
  Complete project details at https://RandomNerdTutorials.com/telegram-group-esp32-esp8266/
  Project created using Brian Lough's Universal Telegram Bot Library: https://github.com/witnessmenow/Universal-Arduino-Telegram-Bot
    Adaptador/Traductor al espanniol:
    Julian Rivera-Pineda

*/

//Bibliotecas de WiFi de las ESP8266 Boards version 3.0.1
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
//Biblioteca de Universal Telegram Bot version 1.3.0
//Biblioteca escrita por Brian Lough: https://github.com/witnessmenow/Universal-Arduino-Telegram-Bot
#include <UniversalTelegramBot.h>
//Biblioteca Arduino Json version 6.15.2
#include <ArduinoJson.h>

//Datos de Conexion WiFi ssid y password
const char* ssid = "JULIAN_RIVERA-ext";
const char* password = "Locator90210jrm";

//Datos de Conexion Telegram
//Token de la plataforma Telegram para el Bot, se obtiene con BotFather
#define BOTtoken "1723433441:AAGI4TRFyvXIVkO1F9kKtRGkYH4DnjbOaV0"
//Chat_id del grupo de Telegram al cual se unira el bot
//Puede obtenerse enviando un mensaje al grupo de chat y luego cargando la siguiente URL
//En un browser: https://api.telegram.org/${ACA_ESCRIBA_EL_TEXTO_DE_SU_BOT_TOKEN}/getUpdates
#define CHAT_ID "-577515097"

#ifdef ESP8266
X509List cert(TELEGRAM_CERTIFICATE_ROOT);
#endif

WiFiClientSecure client;
UniversalTelegramBot bot(BOTtoken, client);

//Intervalo en que verifica por nuevos mensajes = cada segundo
int botRequestDelay = 1000;
unsigned long lastTimeBotRan;

//Numero de pin en donde estara la sennal digital que ira al relay
const int ledPin = 2;
bool ledState = LOW;

//Funcion que se utilizara para manejar los comandos hacia el Bombillo LED
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

    //Mensaje recibido con el posible comando
    //Se imprime al puerto Serial
    String text = bot.messages[i].text;
    Serial.println(text);

    String from_name = bot.messages[i].from_name;

    //Listado de comandos que procesara el Bot: encender, apagar y estado (Pueden ir de cualquier forma Mayusculas o Minusculas
    //o Mezcladas)
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
