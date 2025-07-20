#include <esp_now.h>
#include <WiFi.h>

/**



1 Entrada PullDown (Espada)
1 Entrada PullUp (Florete)
2 Entrada GPIO

1 Entrada (deviceMode)
1 sntrada GPIO

5 Salidas GPIO
3 Entrada GPIO
8 PINES GPIO

**/

#define whiteLed1      3
#define whiteLed2      4
#define blueLed1       10
#define blueLed2       1
#define buzzer         0
#define espadaPullDown 5
#define floretePullUp  6
#define deviceMode           2 // Usar con pull-down físico

//Functions
void notifyOk();

//Variables
enum Mode{
  Fencing,
  Riposte,
  Point
}currentMode;

//Struct to be sent

typedef struct StructMessage{
  bool status;
} structMessage;

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

  //Pin configuration
  pinMode(whiteLed1, OUTPUT);
  pinMode(whiteLed2, OUTPUT);
  pinMode(blueLed1, OUTPUT);
  pinMode(blueLed2, OUTPUT);
  pinMode(buzzer,OUTPUT);

  pinMode(espadaPullDown,INPUT_PULLDOWN);
  pinMode(floretePullUp,INPUT_PULLUP);
  pinMode(deviceMode,INPUT_PULLDOWN);
  
  //Esp Now communication protocol
  WiFi.mode(WIFI_STA);
  if(esp_now_init() != ESP_OK){
    Serial.println("Error initializing");
    digitalWrite(buzzer,HIGH);
    delay(500);
    digitalWrite(buzzer,LOW); 
    return;
  }

  notifyOk();
  //When its succesfully connected, we turn on leds.
  
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

  switch(currentMode){
    case Mode::Fencing:
      break;
    case Mode::Point:
      break;
    case Mode::Riposte:
      break;
  }


  
  //TO SEND DATA USE
  //esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *)&struct, sizeof(struct));
  //if(result==ESP_OK){...} else{...}

}

void notifyOk(){
  digitalWrite(whiteLed1,HIGH);
  digitalWrite(whiteLed2,HIGH);
  digitalWrite(blueLed1,HIGH);
  digitalWrite(blueLed2,HIGH);
  digitalWrite(buzzer,HIGH);
  delay(400);
  digitalWrite(buzzer,LOW);
  delay(200);
  digitalWrite(buzzer,HIGH);
  delay(200);
  digitalWrite(whiteLed1,LOW);
  digitalWrite(whiteLed2,LOW);
  digitalWrite(blueLed1,LOW);
  digitalWrite(blueLed2,LOW);
  digitalWrite(buzzer,LOW);
}
