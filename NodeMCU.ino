//IMPORTAZIONE DELLE LIBRERIE UTILIZZATE
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ArduinoJson.h>
#include <LiquidCrystal_I2C.h>


ESP8266WebServer server;

//INSERIMENTO SSID E PASSWORD DELLA RETE
char* ssid = "";
char* password = "";

LiquidCrystal_I2C lcd(0x27, 16, 2);

//INIZIALIZZAZIONE DELLE VARIABILI RELATIVE AI DATI RACCOLTI
double umidita = 0, temperatura = 0, suolo = 0;
int luminosita = 0, qualita = 0;

void setup()
{
  //INIZIALIZZAZIONE CONNESSIONE
  WiFi.begin(ssid,password);
  Serial.begin(9600);
  while(WiFi.status()!=WL_CONNECTED)
  {
    Serial.print(".");
    delay(500);
  }
  Serial.println("");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  server.on("/",handleIndex);
  server.begin();
  
  //INIZIALIZZAZIONE DISPLAY LCD TRAMITE MODULO I2C
  lcd.init();                  
  lcd.backlight();

  lcd.setCursor(0, 0);
  lcd.print("Indirizzo IP:");
  lcd.setCursor(0,1);
  lcd.print(WiFi.localIP());
  delay(8000);
  lcd.clear(); 
}

void loop()
{
  //RICEZIONE DATI TRAMITE ARDUINO JSON
  server.handleClient();
  DynamicJsonDocument doc(1024);
  
  //INVIO RICHIESTA
  doc["type"] = "request";
  serializeJson(doc,Serial);
  boolean messageReady = false;
  String message = "";
  while(messageReady == false) { // blocking but that's ok
    while(Serial.available()) {
      message = Serial.readString();
      messageReady = true;
    }
  }
  //ERRORE DESERIALIZZAZIONE
  DeserializationError error = deserializeJson(doc,message);
  if(error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.c_str());
    return;
  }
  temperatura = doc["temperatura"];
  umidita = doc["umidità"];
  luminosita = doc["luminosità"];
  qualita = doc["qualità aria"];
  suolo = doc["suolo"];
    
  //DISPLAY

  //UMIDITA'
  lcd.setCursor(0, 0);
  lcd.print("Umidita':");
  lcd.setCursor(0,1);
  lcd.print(umidita);
  lcd.print("%");
  delay(2000);
  lcd.clear();

  //TEMPERATURA
  lcd.setCursor(0, 0);
  lcd.print("Temperatura:");
  lcd.setCursor(0,1);
  lcd.print(temperatura);
  lcd.print("C");
  delay(2000);
  lcd.clear();

  //LUMINOSITA'
  lcd.setCursor(0, 0);
  lcd.print("Luminosita':");
  lcd.setCursor(0,1);
  lcd.print(luminosita);
  delay(2000);
  lcd.clear();

  //QUALITA' ARIA
  lcd.setCursor(0, 0);
  lcd.print("Qualita' Aria:");
  lcd.setCursor(0,1);
  lcd.print(qualita);
  lcd.print(" ");
  lcd.print("PPM");
  delay(2000);
  lcd.clear();

  //SUOLO
  lcd.setCursor(0, 0);
  lcd.print("Umidita' Suolo:");
  lcd.setCursor(0,1);
  lcd.print(suolo);
  lcd.print("%");
  delay(2000);
  lcd.clear();
}

//INVIO DATI SU WEB
void handleIndex()
{
  server.send(200,"text/html", SendHTML(temperatura, umidita, luminosita, qualita, suolo));
}

//SITO WEB IN HTML
String SendHTML(float temperatura,float umidita, float luminosita, float qualita, float suolo){
  String ptr = "<!DOCTYPE html> <html>\n";
  ptr +="<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=no\">\n";
  ptr +="<meta http-equiv=\"refresh\" content=\"5\">\n";
  ptr +="<link href=\"https://fonts.googleapis.com/css?family=Open+Sans:300,400,600\" rel=\"stylesheet\">\n";
  ptr +="<title>Serra Domotica</title>\n";
  ptr +="<link rel=\"icon\" type=\"image/x-icon\" href=\"C:/Users/40799/Downloads/favicon.ico\">\n";
  ptr +="<style>html { font-family: 'Open Sans', sans-serif; display: block; margin: 0px auto; text-align: center;color: #333333;}\n";
  ptr +="body{margin-top: 50px;}\n";
  ptr +="h1 {margin: 50px auto 30px;}\n";
  ptr +=".side-by-side{display: inline-block;vertical-align: middle;position: relative;}\n";
  //UMIDITA'
  ptr +=".humidity-icon{background-color: #3498db;width: 30px;height: 30px;border-radius: 50%;line-height: 36px;margin-top:46px;margin-left:-130px;}\n";
  ptr +=".humidity-text{font-weight: 600;padding-left: 15px;font-size: 19px;width: 160px;text-align: left;margin-top:46px;}\n";
  ptr +=".humidity{font-weight: 300;font-size: 60px;position: fixed;color: #3498db;margin-top:20px;margin-left:10px;}\n";
  //TEMPERATURA
  ptr +=".temperature-icon{background-color: #f39c12;width: 30px;height: 30px;border-radius: 50%;line-height: 40px;margin-top:46px;margin-left:-130px;}\n";
  ptr +=".temperature-text{font-weight: 600;padding-left: 15px;font-size: 19px;width: 160px;text-align: left;margin-top:46px;}\n";
  ptr +=".temperature{font-weight: 300;font-size: 60px;position: fixed;color: #f39c12;margin-top:20px;margin-left:10px;}\n";
  //LUMINOSITA'
  ptr +=".brightness-icon{background-color: #ff0000;width: 30px;height: 30px;border-radius: 50%;line-height: 40px;margin-top:46px;margin-left:-130px;}\n";
  ptr +=".brightness-text{font-weight: 600;padding-left: 15px;font-size: 19px;width: 160px;text-align: left;margin-top:46px;}\n";
  ptr +=".brightness{font-weight: 300;font-size: 60px;position: fixed;color: #ff0000;margin-top:20px;margin-left:10px;}\n";
  //QUALITA'
  ptr +=".gas-icon{background-color: #00ff00;width: 30px;height: 30px;border-radius: 50%;line-height: 40px;margin-top:46px;margin-left:-130px;}\n";
  ptr +=".gas-text{font-weight: 600;padding-left: 15px;font-size: 19px;width: 160px;text-align: left;margin-top:46px;}\n";
  ptr +=".gas{font-weight: 300;font-size: 60px;position: fixed;color: #00ff00;margin-top:20px;margin-left:10px;}\n";
  //SUOLO
  ptr +=".ground-icon{background-color: #800000;width: 30px;height: 30px;border-radius: 50%;line-height: 40px;margin-top:46px;margin-left:-130px;}\n";
  ptr +=".ground-text{font-weight: 600;padding-left: 15px;font-size: 19px;width: 160px;text-align: left;margin-top:46px;}\n";
  ptr +=".ground{font-weight: 300;font-size: 60px;position: fixed;color: #800000;margin-top:20px;margin-left:10px;}\n";
  //APPENDICI
  ptr +=".superscript-closer{font-size: 17px;font-weight: 600;position: absolute;right: -20;top: 15px;}\n";
  ptr +=".superscript{font-size: 17px;font-weight: 600;position: absolute;right: -35px;top: 15px;}\n";
  ptr +=".data{padding: 10px;}\n";
  ptr +="</style>\n";
  ptr +="</head>\n";
  ptr +="<body>\n";
  
  ptr +="<div id=\"webpage\">\n";
   
   ptr +="<h1>Serra Domotica</h1>\n";
   ptr +="<p>\n";
   
   ptr +="<script> document.write(new Date().toLocaleDateString())\n";
   ptr +="<script> document.write(new Date().toLocaleTimeString())\n"; 
   ptr +="</script>\n";
   ptr +="</p>\n";
   
   //TEMPERATURA
   ptr +="<div class=\"data\">\n";
   ptr +="<div class=\"side-by-side temperature-icon\">\n";
   ptr +="<svg version=\"1.1\" id=\"Layer_1\" xmlns=\"http://www.w3.org/2000/svg\" xmlns:xlink=\"http://www.w3.org/1999/xlink\" x=\"0px\" y=\"0px\"\n";
   ptr +="width=\"9.915px\" height=\"22px\" viewBox=\"0 0 9.915 22\" enable-background=\"new 0 0 9.915 22\" xml:space=\"preserve\">\n";
   ptr +="<path fill=\"#FFFFFF\" d=\"M3.498,0.53c0.377-0.331,0.877-0.501,1.374-0.527C5.697-0.04,6.522,0.421,6.924,1.142\n";
   ptr +="c0.237,0.399,0.315,0.871,0.311,1.33C7.229,5.856,7.245,9.24,7.227,12.625c1.019,0.539,1.855,1.424,2.301,2.491\n";
   ptr +="c0.491,1.163,0.518,2.514,0.062,3.693c-0.414,1.102-1.24,2.038-2.276,2.594c-1.056,0.583-2.331,0.743-3.501,0.463\n";
   ptr +="c-1.417-0.323-2.659-1.314-3.3-2.617C0.014,18.26-0.115,17.104,0.1,16.022c0.296-1.443,1.274-2.717,2.58-3.394\n";
   ptr +="c0.013-3.44,0-6.881,0.007-10.322C2.674,1.634,2.974,0.955,3.498,0.53z\"/>\n";
   ptr +="</svg>\n";
   ptr +="</div>\n";
   ptr +="<div class=\"side-by-side temperature-text\">Temperatura</div>\n";
   ptr +="<div class=\"side-by-side temperature\">";
   ptr +=(int)temperatura;
   ptr +="<span class=\"superscript-closer\">C</span></div>\n";
   ptr +="</div>\n";
   
   //UMIDITA'
   ptr +="<div class=\"data\">\n";
   ptr +="<div class=\"side-by-side humidity-icon\">\n";
   ptr +="<svg version=\"1.1\" id=\"Layer_2\" xmlns=\"http://www.w3.org/2000/svg\" xmlns:xlink=\"http://www.w3.org/1999/xlink\" x=\"0px\" y=\"0px\"\n\"; width=\"12px\" height=\"17.955px\" viewBox=\"0 0 13 17.955\" enable-background=\"new 0 0 13 17.955\" xml:space=\"preserve\">\n";
   ptr +="<path fill=\"#FFFFFF\" d=\"M1.819,6.217C3.139,4.064,6.5,0,6.5,0s3.363,4.064,4.681,6.217c1.793,2.926,2.133,5.05,1.571,7.057\n";
   ptr +="c-0.438,1.574-2.264,4.681-6.252,4.681c-3.988,0-5.813-3.107-6.252-4.681C-0.313,11.267,0.026,9.143,1.819,6.217\"></path>\n";
   ptr +="</svg>\n";
   ptr +="</div>\n";
   ptr +="<div class=\"side-by-side humidity-text\">Umidita'</div>\n";
   ptr +="<div class=\"side-by-side humidity\">";
   ptr +=(int)umidita;
   ptr +="<span class=\"superscript-closer\">%</span></div>\n";
   ptr +="</div>\n";

  //LUMINOSITA'
   ptr +="<div class=\"data\">\n";
   ptr +="<div class=\"side-by-side brightness-icon\">\n";
   ptr +="<svg xmlns=\"http://www.w3.org/2000/svg\" width=\"25px\" height=\"20px\" fill=\"currentColor\" class=\"bi bi-brightness-high\" viewBox=\"3 0 10 16\" enable-background=\"new 0 0 13 17.955\" xml:space=\"preserve\">\n";
   ptr +="<path fill=\"#FFFFFF\" d=\"M8 11a3 3 0 1 1 0-6 3 3 0 0 1 0 6zm0 1a4 4 0 1 0 0-8 4 4 0 0 0 0 8zM8 0a.5.5 0 0 1 .5.5v2a.5.5 0 0 1-1 0v-2A.5.5 0 0 1 8 0zm0 13a.5.5 0 0 1 .5.5v2a.5.5 0 0 1-1 0v-2A.5.5 0 0 1 8 13zm8-5a.5.5 0 0 1-.5.5h-2a.5.5 0 0 1 0-1h2a.5.5 0 0 1 .5.5zM3 8a.5.5 0 0 1-.5.5h-2a.5.5 0 0 1 0-1h2A.5.5 0 0 1 3 8zm10.657-5.657a.5.5 0 0 1 0 .707l-1.414 1.415a.5.5 0 1 1-.707-.708l1.414-1.414a.5.5 0 0 1 .707 0zm-9.193 9.193a.5.5 0 0 1 0 .707L3.05 13.657a.5.5 0 0 1-.707-.707l1.414-1.414a.5.5 0 0 1 .707 0zm9.193 2.121a.5.5 0 0 1-.707 0l-1.414-1.414a.5.5 0 0 1 .707-.707l1.414 1.414a.5.5 0 0 1 0 .707zM4.464 4.465a.5.5 0 0 1-.707 0L2.343 3.05a.5.5 0 1 1 .707-.707l1.414 1.414a.5.5 0 0 1 0 .708z\"/>\n";
   ptr +="</svg>\n";
   ptr +="</div>\n";
   ptr +="<div class=\"side-by-side brightness-text\">Luminosita'</div>\n";
   ptr +="<div class=\"side-by-side brightness\">";
   ptr +=(int)luminosita;
   ptr +="<span class=\"superscript\"></span></div>\n";
   ptr +="</div>\n";

  //QUALITA'
   ptr +="<div class=\"data\">\n";
   ptr +="<div class=\"side-by-side gas-icon\">\n";
   ptr +="<svg xmlns=\"http://www.w3.org/2000/svg\" width=\"20\" height=\"18\" fill=\"currentColor\" class=\"bi bi-wind\" viewBox=\"0 1 16 16\" enable-background=\"new 0 0 13 17.955\" xml:space=\"preserve\">\n";
   ptr +="<path fill=\"#FFFFFF\" d=\"M12.5 2A2.5 2.5 0 0 0 10 4.5a.5.5 0 0 1-1 0A3.5 3.5 0 1 1 12.5 8H.5a.5.5 0 0 1 0-1h12a2.5 2.5 0 0 0 0-5zm-7 1a1 1 0 0 0-1 1 .5.5 0 0 1-1 0 2 2 0 1 1 2 2h-5a.5.5 0 0 1 0-1h5a1 1 0 0 0 0-2zM0 9.5A.5.5 0 0 1 .5 9h10.042a3 3 0 1 1-3 3 .5.5 0 0 1 1 0 2 2 0 1 0 2-2H.5a.5.5 0 0 1-.5-.5z\">\n";
   ptr +="</svg>\n";
   ptr +="</div>\n";
   ptr +="<div class=\"side-by-side gas-text\">Qualita' aria</div>\n";
   ptr +="<div class=\"side-by-side gas\">";
   ptr +=(int)qualita;
   ptr +="<span class=\"superscript\">PPM</span></div>\n";
   ptr +="</div>\n";

   //SUOLO
   ptr +="<div class=\"data\">\n";
   ptr +="<div class=\"side-by-side ground-icon\">\n";
   ptr +="<svg xmlns=\"http://www.w3.org/2000/svg\" width=\"20\" height=\"18\" fill=\"currentColor\" class=\"bi bi-moisture\" viewBox=\"2 0.3 16 16\">\n";
   ptr +="<path fill=\"#FFFFFF\" d=\"M13.5 0a.5.5 0 0 0 0 1H15v2.75h-.5a.5.5 0 0 0 0 1h.5V7.5h-1.5a.5.5 0 0 0 0 1H15v2.75h-.5a.5.5 0 0 0 0 1h.5V15h-1.5a.5.5 0 0 0 0 1h2a.5.5 0 0 0 .5-.5V.5a.5.5 0 0 0-.5-.5h-2zM7 1.5l.364-.343a.5.5 0 0 0-.728 0l-.002.002-.006.007-.022.023-.08.088a28.458 28.458 0 0 0-1.274 1.517c-.769.983-1.714 2.325-2.385 3.727C2.368 7.564 2 8.682 2 9.733 2 12.614 4.212 15 7 15s5-2.386 5-5.267c0-1.05-.368-2.169-.867-3.212-.671-1.402-1.616-2.744-2.385-3.727a28.458 28.458 0 0 0-1.354-1.605l-.022-.023-.006-.007-.002-.001L7 1.5zm0 0-.364-.343L7 1.5zm-.016.766L7 2.247l.016.019c.24.274.572.667.944 1.144.611.781 1.32 1.776 1.901 2.827H4.14c.58-1.051 1.29-2.046 1.9-2.827.373-.477.706-.87.945-1.144zM3 9.733c0-.755.244-1.612.638-2.496h6.724c.395.884.638 1.741.638 2.496C11 12.117 9.182 14 7 14s-4-1.883-4-4.267z\"/>\n";
   ptr +="</svg>\n";
   ptr +="</div>\n";
   ptr +="<div class=\"side-by-side ground-text\">Umidita' Suolo</div>\n";
   ptr +="<div class=\"side-by-side ground\">";
   ptr +=(int)suolo;
   ptr +="<span class=\"superscript-closer\">%</span></div>\n";
   ptr +="</div>\n";
  
  ptr +="</div>\n";
  ptr +="</body>\n";
  ptr +="</html>\n";
  return ptr;
}
