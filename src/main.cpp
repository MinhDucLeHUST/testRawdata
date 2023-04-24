#include <WiFi.h>
#include <PubSubClient.h>
#include "IRremote.hpp"
#include "IRReceive.hpp"

#define PUB_TOPIC "data" 
#define SUB_TOPIC "test"
#define LED_SENSE 2           // blink when sense IR coming
#define LED_START 23          // ON to notify start receive IR signal
#define LED_SEND 15           // Config LED to send IR signal
#define LED_RECIEVE 13        // Config LED to receive IR signal

// const char* ssid = "CAM_TEST_EN";
// const char* password = "doimatkhauroi";
const char* ssid = "P407";
const char* password = "17052000";
// const char* mqtt_server = "192.168.88.108";
const char* mqtt_server = "192.168.1.2";      
int i=0;

void handleData(char str[]);
void stringToUint16(char s[], int sizeArray);
int countElement (char s[]);
uint16_t *data_recieve;
void connectBroker();
void callback(char* topic, byte *payload, unsigned int length);
void connect_to_broker();
void connectWifi (void);
void receiveData ();

WiFiClient espClient;
PubSubClient client(espClient);
// IRsend irsend;

void setup() {
  Serial.begin(9600);
  IrSender.begin(LED_SEND);
  IrReceiver.begin(LED_RECIEVE,ENABLE_LED_FEEDBACK);
  // IrReceiver.enableIRIn();
  // irsend.begin(15);
  pinMode(LED_SENSE, OUTPUT);
  pinMode(LED_START, OUTPUT);
  connectWifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  connectBroker();
}

void loop() {
  client.loop();
  if (!client.connected()) {
    connectBroker();
  }
  receiveData();
}

void connectBroker() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("ESP32Client")) {
      Serial.println("connected");
      client.subscribe(SUB_TOPIC);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void callback(char* topic, byte *payload, unsigned int length) {
  char data[1000];
  Serial.println("\n-------New message from broker-----");
  Serial.print("Topic: ");
  Serial.println(topic);
  Serial.print("Message: ");
  Serial.write(payload, length);
  Serial.println();
  for(int i=0; i<length;i++){
    data[i] = *(payload+i);
  }
  Serial.printf("  Data = %s\n",data);
  handleData(data);
  
}

void connectWifi (void){
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.printf("IP address: %s",WiFi.localIP());
}

void handleData(char str[]){
  int size_array = countElement(str);
  printf("Size = %d\n", size_array);
  char* token;

  data_recieve = (uint16_t*) calloc(size_array, sizeof(uint16_t));

  token = strtok(str, ",");
  while (token != NULL) {
      stringToUint16(token,size_array);
      token = strtok(NULL, ",");
  }
  IrSender.sendRaw(data_recieve,size_array, 38);
  free(data_recieve);
}

void stringToUint16(char s[], int sizeArray) {
  unsigned int num = strtoul(s, NULL, 10);
  uint16_t num16 = (uint16_t)num;
  data_recieve[i] = num16;
  printf("rawdata[%d] = %u\n", i,data_recieve[i]);
  if(i >= sizeArray-1)
    i=0;
  else 
    i++;
}

int countElement (char s[]){
  int count_element = 0;
  for(int i=0;i<strlen(s); i++){
      if(s[i] == ','){
          count_element++;
      }
  }
  return count_element+1;
}

void receiveData (){
  String temp = "";
  if (IrReceiver.decode()) {
    int rawlen = IrReceiver.decodedIRData.rawDataPtr->rawlen-1;
    uint8_t rawCode[RAW_BUFFER_LENGTH];
    IrReceiver.compensateAndStoreIRResultInArray(rawCode);
    for (int i = 0; i < rawlen; i++) {
      printf("%u ", rawCode[i]);
    }
    // Serial.printf("Data = %", temp.substring(0,temp.length()-1));
    Serial.printf("\nLength = %d\n", rawlen);
    Serial.printf("Name of protocol: %s", ProtocolNames[IrReceiver.decodedIRData.protocol]);
    IrReceiver.resume(); // Enable receiving of the next value
  }
}