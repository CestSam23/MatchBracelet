#include <esp_now.h>
#include <WiFi.h>

#define whiteLed1      3
#define whiteLed2      4
#define blueLed1       0
#define blueLed2       1
#define buzzer         10
#define epeePullDown 5
 
#define deviceMode           2 // Usar con pull-down físico

//FIE REGULATIONS
#define timeSpanEpeeTouch 2  //In ms
#define timeSpanEpeeRiposte 2000  

//Functions
void notifyOk();
void point();
void noPoint();
void turnOff();

//Control Variables
enum Mode{
  Fencing,
  Riposte,
  Point
}currentMode = Fencing;

bool isDetecting = false;
unsigned long detectStart = 0;
unsigned long pulseStart = 0;
bool countingPulse = false;

unsigned long stateStartTime = 0;
bool waitingForSignal = false;
unsigned long receivedAt = 0;

//Struct to be sent
typedef struct StructMessage{
  bool status;
};

//Address, hexadecimal of the other esp32{0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}
/*Mac oponente: 28:37:2f:73:53:7c */uint8_t broadcastAddress[] ={0X28, 0X37,0X2F,0X73,0X53,0X7C};

esp_now_peer_info_t peerInfo;

//Callback when data is sent
void onDataSent(const uint8_t *mac_addr, esp_now_send_status_t status){

}

//Callback when data is received
void onDataRecv(const uint8_t *mac,const uint8_t *incomingData, int len){
  StructMessage received;
  memcpy(&received, incomingData, sizeof(received));

  if(received.status){
    currentMode = Mode::Riposte;
    receivedAt = millis();
  }
}

void onDataRecv(const esp_now_recv_info_t *recvInfo,const uint8_t *incomingData, int len){
  char macStr[18];
  snprintf(macStr, sizeof(macStr),
           "%02X:%02X:%02X:%02X:%02X:%02X",
           recvInfo->src_addr[0], recvInfo->src_addr[1], recvInfo->src_addr[2],
           recvInfo->src_addr[3], recvInfo->src_addr[4], recvInfo->src_addr[5]);

  Serial.print("Datos recibidos de: ");
  Serial.println(macStr);

  StructMessage received;
  memcpy(&received, incomingData, sizeof(received));

  if(received.status){
    currentMode = Mode::Riposte;
    receivedAt = millis();
  }
}

void setup() {
  Serial.begin(115200);

  //Pin configuration
  pinMode(whiteLed1, OUTPUT);
  pinMode(whiteLed2, OUTPUT);
  pinMode(blueLed1, OUTPUT);
  pinMode(blueLed2, OUTPUT);
  pinMode(buzzer,OUTPUT);

  pinMode(epeePullDown,INPUT);
//  pinMode(foilPullUp,INPUT);
  pinMode(deviceMode,INPUT);
  
  //Esp Now communication protocol
  WiFi.mode(WIFI_STA);
  if(esp_now_init() != ESP_OK){
    Serial.println("Error initializing");
    digitalWrite(buzzer,HIGH);
    delay(500);
    digitalWrite(buzzer,LOW); 
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
  esp_now_register_recv_cb(onDataRecv);

  notifyOk();
}

void loop() {
  switch(currentMode){
    case Mode::Fencing:
      if(digitalRead(epeePullDown)==HIGH && !isDetecting){
        isDetecting = true;
        detectStart = millis();
      }
      if(isDetecting){
        unsigned long duration = millis() - detectStart;
        if(digitalRead(epeePullDown)==LOW){
          unsigned long duration = millis() - detectStart;
          Serial.print("Detection: ");
          Serial.println(duration);

          isDetecting=false;
        } else {
          unsigned long pulseDuration = millis() - detectStart;
          if(pulseDuration>=timeSpanEpeeTouch){
            Serial.println("Detection higher than FIE regulatations");
            isDetecting=false;
            duration = 3;
            
          }
        }
        if(duration>timeSpanEpeeTouch){
          StructMessage msg = {true};
          esp_err_t result = esp_now_send(broadcastAddress, (uint8_t*)&msg, sizeof(msg));
          if (result == ESP_OK) {
            Serial.println("Sent with success");
          } else {
            Serial.print("Error sending the data: ");
            Serial.println(result);
          }

          currentMode = Mode::Point;
          stateStartTime = millis();

          point();
        }
      }
      break;
    case Mode::Point:
      //Wait 3s
      if(millis()-stateStartTime>3000){
        currentMode=Mode::Fencing;
        turnOff();
      }
      break;
    case Mode::Riposte:
      Serial.print("RIPOSTE!:");
      if (millis() - receivedAt <= timeSpanEpeeRiposte) {
        Serial.println(millis()-receivedAt);
        if (!isDetecting && digitalRead(epeePullDown) == HIGH) {
          isDetecting = true;
          detectStart = millis();
        }

        if (isDetecting) {
          unsigned long duration = millis() - detectStart;

          if (digitalRead(epeePullDown) == LOW) {
            isDetecting = false; // No alcanzó el tiempo requerido
          } else if (duration >= timeSpanEpeeTouch) {
            // TOQUE válido en tiempo de riposte
            currentMode = Mode::Point;
            stateStartTime = millis();
            isDetecting = false;
            point();
          }
        }
      } else {
        // Se acabó el tiempo de riposte
        Serial.println("No Riposte");
        currentMode = Mode::Point;
        stateStartTime = millis();
        noPoint();
      }
      break;
  }
}

void point(){
  digitalWrite(blueLed1,HIGH);
  digitalWrite(blueLed2,HIGH);
  digitalWrite(buzzer,HIGH);
  delay(1000);
  digitalWrite(buzzer,LOW);
}

void noPoint(){
  digitalWrite(whiteLed1,HIGH);
  digitalWrite(whiteLed2,HIGH);
  digitalWrite(buzzer,HIGH);
  delay(300);
  digitalWrite(buzzer,LOW);
  delay(300);
  digitalWrite(buzzer,HIGH);
  delay(300);
  digitalWrite(buzzer,LOW);
}

void turnOff(){
  digitalWrite(whiteLed1,LOW);
  digitalWrite(whiteLed2,LOW);
  digitalWrite(blueLed1,LOW);
  digitalWrite(blueLed2,LOW);
  digitalWrite(buzzer,LOW);
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
