//IMPORTAZIONE DELLE LIBRERIE UTILIZZATE
#include <ArduinoJson.h>
#include "DHT.h"

//DEFINIZIONE DEI PIN PER DHT11 E VENTOLA
#define DHTPIN 2
#define DHTTYPE DHT11
#define fanPin 13


String message = "";
bool messageReady = false;
DHT dht(DHTPIN, DHTTYPE);

//DICHIARAZIONE PIN ANALOGICI
int photocellPin = A0;
int gasPin = A1;
int groundPin = A2;


void setup() {
  pinMode(fanPin, OUTPUT);
  Serial.begin(9600);
}

void loop() {

  //QUALITÀ ARIA
  int gas = analogRead(gasPin); 

  //LUMINOSITÀ
  int brightness = analogRead(photocellPin);  
  brightness = 1023 - brightness;

  //TEMPERATURA/UMIDITÀ
  int temperature = dht.readTemperature();
  int humidity = dht.readHumidity();

  //SUOLO
  int ground = analogRead(groundPin);
  ground = constrain(ground,400,1023);
  ground = map(ground,400,1023,100,0);

  //ACCENSIONE/SPEGNIMENTO VENTOLA
  if (temperature>=23){
    analogWrite(fanPin, 255);
  }
  else if(temperature<23){
    analogWrite(fanPin, 0);
  }
  
  
  //COMUNICAZIONE SERIALE CON ARDUINO JSON
  while(Serial.available()) {
    message = Serial.readString();
    messageReady = true;
  }
  //INVIO MESSAGGIO SE PRESENTE
  if(messageReady) {
    // The only messages we'll parse will be formatted in JSON
    DynamicJsonDocument doc(1024); // ArduinoJson version 6+
    // Attempt to deserialize the message
    DeserializationError error = deserializeJson(doc,message);
    if(error) {
      Serial.print(F("deserializeJson() failed: "));
      Serial.println(error.c_str());
      messageReady = false;
      return;
    }
    if(doc["type"] == "request") { 
      doc["type"] = "response";
      // Get data from analog sensors
      doc["temperatura"] = temperature;
      doc["umidità"] = humidity;
      doc["luminosità"] = brightness;
      doc["qualità aria"] = gas;
      doc["terreno"] = ground;
      serializeJson(doc,Serial);
    }
    messageReady = false;
  }
}
