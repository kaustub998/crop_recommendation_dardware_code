#include <ESP8266WiFi.h>
#include <espnow.h>

int send_data = 0;

unsigned long lastTime = 0;  
unsigned long timerDelay = 2000;

typedef struct data {
    int node_id;
    float temp;
    float hum;
} data;

data received_data; 

// REPLACE WITH RECEIVER MAC Address
uint8_t broadcastAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

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
  memcpy(&received_data, incomingData, sizeof(received_data));
  Serial.print("Bytes received: ");
  Serial.println(len);
  Serial.println(received_data.node_id);
  Serial.println(received_data.temp);
  Serial.println(received_data.hum);
}

void setup() {
  // put your setup code here, to run once:

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
    send_data = 1;

    // Send message via ESP-NOW
    esp_now_send(broadcastAddress, (uint8_t *) &send_data, sizeof(send_data));

    lastTime = millis();
  }
}
