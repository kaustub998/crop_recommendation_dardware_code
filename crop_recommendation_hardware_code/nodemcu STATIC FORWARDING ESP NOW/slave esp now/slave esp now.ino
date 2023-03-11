#include <ESP8266WiFi.h>
#include <espnow.h>

int received_data; // (0 or 1)
int forwarding = 0;

unsigned long lastTime = 0;  
unsigned long timerDelay = 2000;

typedef struct data {
    String node_id;
    String temp;
    String hum;
    String pH;
} data;

data my_data;
data forward_data;

// REPLACE WITH RECEIVER MAC Address
uint8_t broadcastAddress[] = {0xFC, 0xF5, 0xC4, 0x89, 0x13, 0x02};

void OnDataSent(uint8_t *mac_addr, uint8_t sendStatus) {
  Serial.print("Last Packet Send Status: ");
  if (sendStatus == 0){
    Serial.println("Delivery success");
  }
  else{
    Serial.println("Delivery fail");
  }
}

void OnDataRecv(uint8_t * mac, uint8_t *incomingData, uint8_t len) {
  
  Serial.print("Bytes received: ");
  Serial.println(len);
  
  if(len > sizeof(received_data))
  {
    memcpy(&forward_data, incomingData, sizeof(forward_data));
    forwarding = 1;

    Serial.println(forward_data.node_id);
    Serial.println(forward_data.temp);
    Serial.println(forward_data.hum);
  }
  else
  {
    memcpy(&received_data, incomingData, sizeof(received_data));
    Serial.println(received_data);
  }
}

void setup() {
  // put your setup code here, to run once:

  my_data.node_id = "2";
  my_data.temp = "50";
  my_data.hum = "50";
  my_data.pH = "9";
  
  
  Serial.begin(115200);
 
  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  // Init ESP-NOW
  if (esp_now_init() != 0) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  esp_now_set_self_role(ESP_NOW_ROLE_COMBO);
  
  esp_now_register_send_cb(OnDataSent);

  esp_now_add_peer(broadcastAddress, ESP_NOW_ROLE_COMBO,  1, NULL, 0);

  esp_now_register_recv_cb(OnDataRecv);
}

void loop() {
  // put your main code here, to run repeatedly:

  if ((millis() - lastTime) >= timerDelay) {
    // Set values to send

    // Send message via ESP-NOW
    if(received_data == 1)
    {
      esp_now_send(broadcastAddress, (uint8_t *) &received_data, sizeof(received_data));
      esp_now_send(broadcastAddress, (uint8_t *) &my_data, sizeof(my_data));
      
      if(forwarding)
      {
        forwarding = 0;
        esp_now_send(broadcastAddress, (uint8_t *) &forward_data, sizeof(forward_data));
      }
      
      received_data = 0;
    }
    

    lastTime = millis();
  }


}
