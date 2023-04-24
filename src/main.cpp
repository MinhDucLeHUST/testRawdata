#include <WiFi.h>
#include <PubSubClient.h>
#include "IRremote.hpp"
// #include "IRSend.h"
// #include <IRremoteESP8266.h>

#define PUB_TOPIC "data" 
#define SUB_TOPIC "test"
#define LED_SENSE 2           // blink when sense IR coming
#define LED_START 23          // ON to notify start receive IR signal
#define LED_SEND 15

const char* ssid = "CAM_TEST_EN";
const char* password = "doimatkhauroi";
// const char* ssid = "P407";
// const char* password = "17052000";
const char* mqtt_server = "192.168.88.108";
// const char* mqtt_server = "192.168.1.2";
// uint16_t raw_data[200] = { 0 };
bool send_rawData = true;
int i=0;
// int count_element = 0;

void handleData(char str[]);
void showArray (int size_array);
void stringToUint16(char s[], int sizeArray);
int countElement (char s[]);
uint16_t *data_recieve;
void connectBroker();
void callback(char* topic, byte *payload, unsigned int length);
void connect_to_broker();
void connectWifi (void);
void sendRawData (const uint16_t* rawData);

WiFiClient espClient;
PubSubClient client(espClient);
// IRsend irsend;

void setup() {
  Serial.begin(9600);
  IrSender.begin(LED_SEND);
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
  char data[350];
  Serial.println("\n-------new message from broker-----");
  Serial.print("topic: ");
  Serial.println(topic);
  Serial.print("message: ");
  Serial.write(payload, length);
  Serial.println();
  for(int i=0; i<length;i++){
    data[i] = *(payload+i);
  }
  Serial.printf("  Data = %s\n",data);
  handleData(data);
  // sendRawData(&data_recieve);
  
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
  // char str[] = "";
  int size_array = countElement(str);
  printf("Size = %d\n", size_array);
  char* token;

  data_recieve = (uint16_t*) calloc(size_array, sizeof(uint16_t));

  token = strtok(str, ",");
  while (token != NULL) {
      // printf("\tSplit: %s\t", token);
      stringToUint16(token,size_array);
      token = strtok(NULL, ",");
  }
  // showArray(size_array);
  free(data_recieve);
}

void stringToUint16(char s[], int sizeArray) {
  unsigned int num = strtoul(s, NULL, 10);
  uint16_t num16 = (uint16_t)num;
  // printf("After convert: %u\t", num16);
  data_recieve[i] = num16;
  printf("rawdata[%d] = %u\n", i,data_recieve[i]);
  // i++;
  if(i >= sizeArray-1)
    i=0;
  else 
    i++;

  IrSender.sendRaw(data_recieve,31, 38);
  // sendRawData(data_recieve);
}

void showArray (int size_array){
  for(int i=0;i<size_array;i++){
      printf("%u\t",data_recieve[i]);
  }
}

int countElement (char s[]){
  // const char* c = ",";
  int count_element = 0;
  for(int i=0;i<strlen(s); i++){
      if(s[i] == ','){
          count_element++;
      }
  }
  return count_element+1;
}

void sendRawData (const uint16_t* rawData)
{
  int rawDataSize = sizeof(rawData) / sizeof(uint16_t);
  if(send_rawData){
    IrSender.sendRaw(rawData,31, 38);
    // send_rawData = false;
  }
}