#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include "DHT.h"

#define DHTTYPE DHT11   // DHT 11
#define dht_dpin 5

DHT dht(dht_dpin, DHTTYPE);
int phval = 0; 
unsigned long int avgval; 
int buffer_arr[10],temp;

//=============================================MOTION SENSOR DEVICE ID==============================================
String node_id = "3";
//==================================================================================================================
String data;
//============================================WIFI CREDENTIALS OF HUB================================================
const char* ssid = "server1";
const char* password = "qwertyuiop";
String host = "http://10.10.10.1/feed1?";
//============================================defining the object====================================================
WiFiClient client;
HTTPClient http;
//============================================defining the IP Address of the sensor==================================
IPAddress ip (10, 10, 10, 32);
IPAddress gateway (10, 10, 10, 1);
IPAddress subnet (255, 255, 255, 0);

//============================================SETUP PART=============================================================
void setup(void) {
  dht.begin();
  Serial.begin(115200);
  ESP.eraseConfig();
  WiFi.persistent(false);
  Serial.println("SET ESP IN STA MODE");
  WiFi.mode(WIFI_STA);
  Serial.println("- connecting to wifi");
  WiFi.config(ip, gateway, subnet); 
  WiFi.begin(ssid, password);
  Serial.println(".");
  Serial.println("- wifi connected");
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

}
//============================================READ THE STATE OF SENSOR================================================
void readdata(){
  
 float h =dht.readHumidity();  //Humidity level
 float t =dht.readTemperature();  //Temperature in celcius 
// float f =0.0;  //Temperature in fahrenheit

for(int i=0;i<10;i++) 
    { 
        buffer_arr[i]=analogRead(A0);
        delay(30);
        }
    for(int i=0;i<9;i++)
    {
        for(int j=i+1;j<10;j++)
        {
          if(buffer_arr[i]>buffer_arr[j])
          {
            temp=buffer_arr[i];
            buffer_arr[i]=buffer_arr[j];
            buffer_arr[j]=temp;
          }
        }
    }
  
    avgval=0;
    for(int i=2;i<8;i++){
      avgval+=buffer_arr[i];
    }
    avgval = avgval / 6;

 float pH = ((avgval-272.52)/65.11);
// float m_p = 0.0; //Moisture Percentage 

// h = dht.readHumidity();    //Read humidity level
// t = dht.readTemperature(); //Read temperature in celcius
// f = (t * 1.8) + 32;        //Temperature converted to Fahrenheit
// m_p = ( 100.00 - ( (analogRead(moisture_pin)/1023.00) * 100.00 ) );

 data = "node_id=";
 data += String(node_id);
 data += "&temp=";
 data += String(t);
 data += "&humidity=";
 data += String(h);
 data += "&pH=";
 data += String(pH);

 http.begin(client, host);
 http.addHeader("content-type","application/x-www-form-urlencoded");
 http.POST(data);
 http.writeToStream(&Serial);
 http.end();
 Serial.println("data stream: "+data);
}

//============================================LOOP PART================================================
void loop () {
  readdata();
//  delay(2000);
}
