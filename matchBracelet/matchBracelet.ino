#include <esp_now.h>
#include <WiFi.h>

#define whiteLed1      3
#define whiteLed2      4
#define blueLed1       10
#define blueLed2       1
#define buzzer         0
#define epeePullDown 5
#define foilPullUp  6
#define deviceMode           2 // Usar con pull-down físico

//FIE REGULATIONS
#define timeSpanEpeeTouch 2000  //In ms
#define timeSpanEpeeRiposte 45  

//Functions
void notifyOk();

//Control Variables
enum Mode{
  Fencing,
  Riposte,
  Point
}currentMode = Fencing;

unsigned long stateStartTime = 0;
bool waitingForSignal = false;
unsigned long receivedAt = 0;

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
  Serial.begin(115200);

  //Pin configuration
  pinMode(whiteLed1, OUTPUT);
  pinMode(whiteLed2, OUTPUT);
  pinMode(blueLed1, OUTPUT);
  pinMode(blueLed2, OUTPUT);
  pinMode(buzzer,OUTPUT);

  pinMode(epeePullDown,INPUT_PULLDOWN);
  pinMode(foilPullUp,INPUT_PULLUP);
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

  notifyOk();
}

void loop() {
  switch(currentMode){
    case Mode::Fencing:
      //Detect attack
      if(digitalRead(epeePullDown)==HIGH){
        unsigned long pulse = pulseIn(epeePullDown,HIGH);
        if(pulse>timeSpanEpeeTouch){

          //Detected Point

          //TODO Add the lights and buzzer.

          structMessage msg = {true};
          esp_now_send(broadcastAddress,(uint8_t *)&struct,sizeof(StructMessage)));

          currentMode = Mode::Point;
          stateStartTime=milis();
        }
      }
      break;
    case Mode::Point:
      //Wait 3s
      if(milis()-stateStartTime>3000){
        //TODO Turn off retroalimentation
        currentMode=Mode::Fencing;
      }
      break;
    case Mode::Riposte:
      //The enemy scored a point, time for riposte
      if(millis()-receivedAt <= timeSpanEpeeRiposte){
        if(digitalRead(epeePullDown) == HIGH){
          unsigned long pulse = pulseIn(epeePullDown,HIGH);
          if(pulse>timeSpanEpeeTouch){
            //Touch in correct time
            //TODO: Add retroalimentation
            currentMode = Mode::Point;

          }
        }
      } else {
        //No riposte
        //TODO: ADD RETROALIMETATION
        stateStartTime = millis();
        currentMode = Mode::Point; //Just to wait 
      }
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
