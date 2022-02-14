/*
 *goal: This programs will collect data from DHT12 sensor and post it in website 
 *web : ifri-meteo.herokuapp.com
**/

#include <ESP8266WiFi.h>
#include <WiFiClient.h> 
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>
#include "DHT.h"

//sensor config
#define DHTTYPE DHT11
uint8_t DHTPin = D4; 
// Initialize DHT sensor
DHT dht(DHTPin, DHTTYPE);

//Led 
uint8_t LedPin = D1; 

//Enter your wifi credentials
const char *ssid = "BTHOTSPOT-6647";
const char *password = "sh4d0w.n3t!256/B";

//Web server on which we're going to save data
String host = "http://ifri-meteo.herokuapp.com/iot_reader/index.php";

//Others data
String stationZone = "Calavi";
String temperature = "";
String humidite = "";
String vent = "NaN";
String luminosite = "NaN";
String postData = "";


void setup() {
  delay(1000);
  Serial.begin(57600);
  //Prevents reconnection issue (taking too long to connect)
  WiFi.mode(WIFI_OFF);        
  delay(1000);
  //This line hides the viewing of ESP as wifi hotspot
  WiFi.mode(WIFI_STA);        

  //Connect to our wifi
  WiFi.begin(ssid, password);     

  Serial.println("\n\nConnecting ...");
  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(250);
    Serial.print(".");
  }

  //connection successful
  Serial.print("\r\n");
  Serial.print("Connected to:\t");
  Serial.println(WiFi.SSID());
  Serial.print("IP Adresse:\t");
  Serial.println(WiFi.localIP());

  //init sensor
  pinMode(DHTPin, INPUT);
  pinMode(LedPin, OUTPUT);
  digitalWrite(LedPin, LOW);
  dht.begin();   
}


void loop() {
  HTTPClient http;
  WiFiClient wifiClient;

  //read sensor
  temperature.concat(dht.readTemperature());
  int humRounded = dht.readHumidity();
  humidite.concat(humRounded);
  digitalWrite(LedPin, HIGH);

  //Post Data
  postData = "lieu=" + stationZone + "&temperature=" + temperature + "&humidite=" + humidite + "&luminosite=" + luminosite + "&vent=" + vent;
  
  //Specify request destination
  http.begin(wifiClient, host+"?"+postData);

  //Send the request
  int httpCode = http.GET();

  // httpCode will be negative if an error occur
  if (httpCode > 0) {
    Serial.printf("[HTTP] POST... code: %d\n", httpCode);

    // file found at server
    if (httpCode == HTTP_CODE_OK) {
      const String& payload = http.getString();
      Serial.println("received payload:\n<<");
      Serial.println(payload);
      Serial.println(">>");
    }
  } 
  else {
    Serial.printf("[HTTP] POST... failed, error: %s\n", http.errorToString(httpCode).c_str());
  }
  http.end();

  //Post Data at every 5 secondss
  delay(1000);  
  digitalWrite(LedPin, LOW);
}
//=======================================================================
