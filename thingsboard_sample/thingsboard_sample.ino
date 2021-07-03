#include "ThingsBoard.h"

#include <ESP8266WiFi.h>


#define WIFI_AP             "JULIAN_RIVERA-ext"
#define WIFI_PASSWORD       "Locator90210jrm"

// See https://thingsboard.io/docs/getting-started-guides/helloworld/
// to understand how to obtain an access token
#define TOKEN               "cdJUur9K8VQH9V2CfNZ1"
#define THINGSBOARD_SERVER  "thingsboard.cloud"

// Baud rate for debug serial
#define SERIAL_DEBUG_BAUD   115200

// Initialize ThingsBoard client
WiFiClient espClient;
// Initialize ThingsBoard instance
ThingsBoard tb(espClient);
// the Wifi radio's status
int status = WL_IDLE_STATUS;

//DHT11 Settings
#include "DHT.h"
#define DHTPIN 14     // Digital pin connected to the DHT sensor
#define DHTTYPE DHT11 //DHT Type
DHT dht(DHTPIN, DHTTYPE); //Variable

//Buzzer parameters
#define BUZZERPIN 13
const float buzzerTempe = 32;
int buzzerStatus = 0; //Apagado 1 encendido


void setup() {
  pinMode(BUZZERPIN, OUTPUT);
  doNotToneBuzzer();
  // initialize serial for debugging
  Serial.begin(SERIAL_DEBUG_BAUD);
  Serial.println(F("DHT11 Module!"));
  dht.begin();
  WiFi.begin(WIFI_AP, WIFI_PASSWORD);
  InitWiFi();
}

void doToneBuzzer(){
  analogWrite(BUZZERPIN, LOW); // Send 1KHz sound signal...
  //delay(2000);        // ...for 2 sec
}

void doNotToneBuzzer(){
  analogWrite(BUZZERPIN, HIGH);     // Stop sound...
  //delay(2000);
}

void loop() {
  delay(5000);

  if (WiFi.status() != WL_CONNECTED) {
    reconnect();
  }

  if (!tb.connected()) {
    // Connect to the ThingsBoard
    Serial.print("Connecting to: ");
    Serial.print(THINGSBOARD_SERVER);
    Serial.print(" with token ");
    Serial.println(TOKEN);
    if (!tb.connect(THINGSBOARD_SERVER, TOKEN)) {
      Serial.println("Failed to connect");
      return;
    }
  }

  Serial.println("Sending data...");

  // Uploads new telemetry to ThingsBoard using MQTT.
  // See https://thingsboard.io/docs/reference/mqtt-api/#telemetry-upload-api
  // for more details
  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();
  // Compute heat index in Celsius (isFahreheit = false)
  //float hic = dht.computeHeatIndex(t, h, false);

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t)) {
    Serial.println(F("Failed to read from DHT11 sensor!"));
    return;
  }

  Serial.println("Temperature: "+String(t));
  Serial.println("Humidity: "+String(h));
  //Serial.println("HIC: "+String(hic));
  
  tb.sendTelemetryFloat("temperatura", t);
  tb.sendTelemetryFloat("humedad",h);
  //tb.sendTelemetryFloat("hic",hic);

  //Manipulacion de Buzzer
  if(t>=buzzerTempe){
    buzzerStatus = 1;
    doToneBuzzer();    
  }else{
    buzzerStatus = 0;
    doNotToneBuzzer();
  }
  tb.sendTelemetryFloat("buzzerStatus", buzzerStatus);
  tb.loop();
}

void InitWiFi()
{
  Serial.println("Connecting to AP ...");
  // attempt to connect to WiFi network

  WiFi.begin(WIFI_AP, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connected to AP");
}

void reconnect() {
  // Loop until we're reconnected
  status = WiFi.status();
  if ( status != WL_CONNECTED) {
    WiFi.begin(WIFI_AP, WIFI_PASSWORD);
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }
    Serial.println("Connected to AP");
  }
}
