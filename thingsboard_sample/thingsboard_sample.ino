#include "ThingsBoard.h"
#include <ESP8266WiFi.h>


#define WIFI_AP             "Negocios Tecnologicos"
#define WIFI_PASSWORD       "Negocios$2018"


// access token PARA LA
#define TOKEN               "cdJUur9K8VQH9V2CfNZ1"
#define THINGSBOARD_SERVER  "thingsboard.cloud"

// Baud rate for debug serial
#define SERIAL_DEBUG_BAUD   115200

// Iniciando el cliente en  ThingsBoard
WiFiClient espClient;
// Iniciando la instancia ThingsBoard
ThingsBoard tb(espClient);
// Wifi radio's status
int status = WL_IDLE_STATUS;

//DHT11 Settings
#include "DHT.h"
#define DHTPIN 14     // Digital pin conectado al sensor DHT
#define DHTTYPE DHT11 //DHT Tipo
DHT dht(DHTPIN, DHTTYPE); //Variable

//Buzzer parametros
#define BUZZERPIN 13
const float buzzerTempe = 27;
int buzzerStatus = 0; //0 Apagado 1 encendido


void setup() {
  pinMode(BUZZERPIN, OUTPUT);
  // Inicializando serial para debugging
  Serial.begin(SERIAL_DEBUG_BAUD);
  Serial.println(F("DHT11 Module!"));
  dht.begin();
  WiFi.begin(WIFI_AP, WIFI_PASSWORD);
  InitWiFi();
}

void doToneBuzzer(){
  
  digitalWrite (BUZZERPIN, HIGH); 
  delay(2000);        // ...esperando 2 sec
}

void doNotToneBuzzer(){
  
  digitalWrite (BUZZERPIN, LOW); 
  Serial.println("No se enciende");
  delay(2000);
}

void loop() {
  delay(5000);

  if (WiFi.status() != WL_CONNECTED) {
    reconnect();
  }

  if (!tb.connected()) {
    // Connectar a la nube ThingsBoard
    Serial.print("Connecting to: ");
    Serial.print(THINGSBOARD_SERVER);
    Serial.print(" with token ");
    Serial.println(TOKEN);
    if (!tb.connect(THINGSBOARD_SERVER, TOKEN)) {
      Serial.println("Failed to connect");
      return;
    }
  }

  Serial.println("Enviando datos...");

 
  float h = dht.readHumidity();
  
  float t = dht.readTemperature();
  
  if (isnan(h) || isnan(t)) {
    Serial.println(F("Falla para leer el sensor DHT11!"));
    return;
  }

  Serial.println("Temperature: "+String(t));
  Serial.println("Humidity: "+String(h));
  
  
  tb.sendTelemetryInt("temperatura", t);
  tb.sendTelemetryFloat("humedad",h);
  

  //Manipulacion de Buzzer
  if(t>=buzzerTempe){  //Si la temperatura es mayor que el valor definido se activa 
    buzzerStatus = 1;
    doToneBuzzer();
    
  }else{
    
    buzzerStatus = 0;
    doNotToneBuzzer();
   
   delay(2000);
    
    
  }
  tb.sendTelemetryFloat("buzzerStatus", buzzerStatus);
  tb.loop();
}

void InitWiFi()
{
  Serial.println("Conectado a nuestro AP ...");
  

  WiFi.begin(WIFI_AP, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Conectado a nuestro AP");
}

void reconnect() {
  
  status = WiFi.status();
  if ( status != WL_CONNECTED) {
    WiFi.begin(WIFI_AP, WIFI_PASSWORD);
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }
    Serial.println("Conectado a nuestro AP");
  }
}