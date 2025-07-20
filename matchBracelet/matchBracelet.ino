#include <esp_now.h>
#include <WiFi.h>



//Address, hexadecimal {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}
uint8_t broadcastAddress[] ={};
esp_now_peer_info_t peerInfo;

//Callback when data is sent
void onDataSent(const uint8_t *mac_addr, esp_now_send_status_t status){

}

//Callback when data is received
void onDataRecv(const uint8_t *mac,const uint8_t *incomingData, int len){

}

void setup() {
  
  //Esp Now communication protocol
  WiFi.mode(WIFI_STA);
  if(esp_now_init() != ESP_OK){
    Serial.println("Error initializing");
    return;
  }
  //Register for send cb to get status of transmitted packet
  esp_now_register_send_cb(onDataSent);

  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  if(esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }

  //Register for a callback function that will be called when data is received
  esp_now_register_recv_cb(esp_now_recv_cb_t(onDataRecv));
  Serial.begin(115200);

}

void loop() {
  // put your main code here, to run repeatedly:

  //TO SEND DATA USE
  //esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *)&struct, sizeof(struct));
  //if(result==ESP_OK){...} else{...}

}
