#include <ESP8266WiFi.h>
#include <Adafruit_BMP085.h>
#include "DHT.h"
#include <Wire.h>
#include <BH1750.h>

#define DHTTYPE DHT11   // DHT 11
#define dht_dpin 0      //GPIO-0 D3 pin of nodemcu

const char* ssid     = "SSID";
const char* password = "Password";
const char* host = "api.example.com";
const char* api_key = "APIToken";
const char* node_id = "ESP8266";
const int httpPort = 443;

DHT dht(dht_dpin, DHTTYPE);
BH1750 lightMeter;
Adafruit_BMP085 bmp;

void setup() {
  Serial.begin(9600);
  if (!bmp.begin()) {
    Serial.println("Could not find a valid BMP085 sensor, check wiring!");
    while (1) {}
  }
  
  dht.begin();
  delay(10);

  Wire.begin();
  lightMeter.begin();

  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  Serial.print("connecting to ");
  Serial.println(host);

  // Use WiFiClient class to create TCP connections
  WiFiClientSecure client;

  client.setInsecure();

  if (!client.connect(host, httpPort)) { //works!
    Serial.println("connection failed");
    return;
  }

  float h = dht.readHumidity(); //Humidity level
  float t = dht.readTemperature(); //Temperature in celcius
  float p = bmp.readPressure();
  float lux = lightMeter.readLightLevel();

  // We now create a URI for the request
  String url = "/weather-pi-project/backend/api/import.php/";
  url += "?api_key=";
  url += api_key;
  url += "&node_id=";
  url += node_id;
  url += "&temperature=";
  url += String(t);
  url += "&humidity=";
  url += String(h);
  url += "&pressure=";
  url += String(p/100);
  url += "&light=";
  url += String(lux);

  Serial.print("Requesting URL: ");
  Serial.println(url);

  // This will send the request to the server
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "Connection: close\r\n\r\n");

  Serial.println();
  Serial.println("closing connection");

  delay(3600000);
}
