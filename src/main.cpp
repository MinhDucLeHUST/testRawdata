#include <WiFi.h>
#include <PubSubClient.h>
// #include "IRremote.hpp"
// #include "IRSend.hpp"

#define PUB_TOPIC "data" 
#define SUB_TOPIC "test"
#define LED_SENSE 2           // blink when sense IR coming
#define LED_START 23          // ON to notify start receive IR signal

// const char* ssid = "CAM_TEST2";
// const char* password = "1234567890";
const char* ssid = "P407";
const char* password = "17052000";
// const char* mqtt_server = "192.168.88.107";
const char* mqtt_server = "192.168.1.2";

WiFiClient espClient;
PubSubClient client(espClient);
void connectBroker();
void callback(char* topic, byte *payload, unsigned int length);
void connect_to_broker();
void connectWifi (void);

void setup() {
  Serial.begin(9600);
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
  char data[50];
  Serial.println("-------new message from broker-----");
  Serial.print("topic: ");
  Serial.println(topic);
  Serial.print("message: ");
  Serial.write(payload, length);
  Serial.println();
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
