#include <ArduinoJson.h>
#include "DHT.h"

#define DHTPIN 2
#define DHTTYPE DHT11
#define fanPin 13


String message = "";
bool messageReady = false;
DHT dht(DHTPIN, DHTTYPE);

int photocellPin = A0;     // the cell and 10K pulldown are connected to a0
int gasPin = A1;
int groundPin = A2;
     // the analog reading from the sensor divider


void setup() {
  pinMode(fanPin, OUTPUT);
  Serial.begin(9600);
}

void loop() {

  //GAS
  int gas = analogRead(gasPin); 

  //FOTORESISTENZA
  int brightness = analogRead(photocellPin);  
  brightness = 1023 - brightness;

  //DHT11
  int temperature = dht.readTemperature();
  int humidity = dht.readHumidity();

  //TERRENO
  int ground = analogRead(groundPin);
  ground = constrain(ground,400,1023);  //Keep the ranges!
  ground = map(ground,400,1023,100,0);  //Map value : 400 will be 100 and 1023 will be 0

  //ACCENSIONE/SPEGNIMENTO VENTOLA
  if (temperature>=23){
    analogWrite(fanPin, 255);
  }
  else if(temperature<23){
    analogWrite(fanPin, 0);
  }
  
  
  // Monitor serial communication
  while(Serial.available()) {
    message = Serial.readString();
    messageReady = true;
  }
  // Only process message if there's one
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
