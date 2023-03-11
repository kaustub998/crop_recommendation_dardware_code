#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>
#include <SoftwareSerial.h>
#include <ArduinoJson.h>

#include "DHT.h"

#define DHTTYPE DHT11   // DHT 11
#define dht_dpin 5

DHT dht(dht_dpin, DHTTYPE);

//======================================WIFI CREDENTIALS for Hotspot==================================
const char* ssid_node = "server1";
const char* password_node = "qwertyuiop"; 
const char* ssid_backend = "merokhet";
const char* password_backend = "hamrokhet";
const char* server_backend = "http://192.168.92.178:8000/from-node";
int max_connection = 8;
//====================================================================================================
String node_id;
String temp;
String humidity;
String pH;
//=============================================define the objects======================================
ESP8266WebServer server_node(80);

//===============================================dafine the IP Address of the hotspot==================
IPAddress ip (10, 10, 10, 1);
IPAddress gateway (10, 10, 10, 1);
IPAddress subnet (255, 255, 255, 0);
//=========================================SETUP PART==================================================
void setup() {
  Serial.begin(115200);
  dht.begin();
  WiFi.mode(WIFI_AP_STA);
  Serial.println("Setup Access point");
  Serial.println("Disconnect from any other modes");
//  WiFi.disconnect();
  Serial.println("stating access point with SSID" +String(ssid_node)); 
  WiFi.softAP(ssid_node, password_node, 1, false, max_connection); 
  WiFi.softAPConfig(ip, gateway, subnet);
  IPAddress myIP = WiFi.softAPIP();
  Serial.println(myIP);
  
  server_node.on("/feed1", feed1);
  server_node.on("/feed2", feed2);
  server_node.begin();

  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid_backend);
  WiFi.begin(ssid_backend, password_backend);     //Begin WiFi
  
 while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print(WiFi.localIP());
  Serial.println("/");
}
//=====================================handle root page================================================
//===================================handle door sensor page===========================================
void feed1(){
  Serial.println("on feed1");
  node_id = server_node.arg("node_id");
  temp = server_node.arg("temp");
  humidity = server_node.arg("humidity");
  
  pH = server_node.arg("pH");

  Serial.println();
  Serial.println("From Slave");
  Serial.print("Node id: ");
  Serial.println(node_id);
  Serial.print("Temp: ");
  Serial.println(temp);
  Serial.print("Humidity: ");
  Serial.println(humidity);
  Serial.print("pH: ");
  Serial.println(pH);

  sendData();
}

void feed2(){
  Serial.println("on feed2");
  node_id = server_node.arg("node_id");
  temp = server_node.arg("temp");
  humidity = server_node.arg("humidity");
  pH = server_node.arg("pH");

  Serial.println();
  Serial.println("From Slave");
  Serial.print("Node id: ");
  Serial.println(node_id);
  Serial.print("Temp: ");
  Serial.println(temp);
  Serial.print("Humidity: ");
  Serial.println(humidity);
  Serial.print("pH: ");
  Serial.println(pH);

  sendData();
}

void sendData(){
  HTTPClient http;
  WiFiClient client;
  http.begin(client, server_backend);
  http.addHeader("Content-Type", "application/json");

  DynamicJsonDocument doc1(256);
  doc1["node_id"] = node_id;
  doc1["temp"] = temp;
  doc1["humidity"] = humidity;
  doc1["pH"] = pH;

  String json1;
  serializeJson(doc1, json1);
 
  int httpResponseCode1 = http.POST(json1);
  Serial.println(httpResponseCode1);
  
  server_node.send(200,"text/plain","Response");

  if (client.connected()) {
    client.stop();
  }
}

void readData(){
  node_id = "1";
  humidity = dht.readHumidity() + 23;    //Read humidity level
  temp = dht.readTemperature();
  pH = "7";

  // Serial.println();
  // Serial.println("From Master");
  // Serial.print("Node id: ");
  // Serial.println(node_id);
  // Serial.print("Temp: ");
  // Serial.println(temp);
  // Serial.print("Humidity: ");
  // Serial.println(humidity);
  // Serial.print("pH: ");
  // Serial.println(pH);

  sendData();
}

//==============================================LOOP===================================================
void loop (){
  server_node.handleClient();
  readData();
//  delay(1000);
}
