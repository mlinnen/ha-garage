#include "mywifi.h"
#include "mymqttbroker.h"
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Cover_SingleLEDStatus.h>
#include <Cover_GarageDoorRelay.h>
#include "ctype.h"

#define MQTT_GARAGE_1_ROOT_TOPIC "/home/garage/1"
#define MQTT_GARAGE_STATE_TOPIC "/cover/state"
#define MQTT_GARAGE_CMD_TOPIC "/cover/cmd"
#define MQTT_GARAGE_1_STATE_TOPIC MQTT_GARAGE_1_ROOT_TOPIC MQTT_GARAGE_STATE_TOPIC
#define MQTT_GARAGE_1_CMD_TOPIC MQTT_GARAGE_1_ROOT_TOPIC MQTT_GARAGE_CMD_TOPIC

const char* ssid = WLAN_SSID;
const char* password = WLAN_PASS;
const char* mqtt_server = MQTT_SERVER;

char message_buff[20];

WiFiClient espClient;
PubSubClient* client;
Cover::GarageDoorRelay* garageActuator;
Cover::SingleLEDStatus* garageLEDStatus;

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  String topicStr(topic);
  topicStr.toLowerCase();

  int i;
  // create character buffer with ending null terminator (string)
  for(i=0; i<length; i++) {
    message_buff[i] = payload[i];
  }
  message_buff[i] = '\0';

  String msgString = String(message_buff);
  msgString.toLowerCase();

  // Parse the message and convert it into a command;
  if (topicStr == MQTT_GARAGE_1_CMD_TOPIC)
  {
    Cover::Command command = Cover::CommandUnknown;
    if (msgString == "open")
    {
      command = Cover::CommandOpen;
    }
    if (msgString == "close")
    {
      command = Cover::CommandClose;
    }
    if (msgString == "stop")
    {
      command = Cover::CommandStop;
    }
    garageActuator->setCommand(command);
  }

  // Parse the message and convert it into a state;
  if (topicStr == MQTT_GARAGE_1_STATE_TOPIC)
  {
    Cover::State state = Cover::StateUnknown;
    if (msgString == "open")
    {
      state = Cover::StateOpen;
    }
    if (msgString == "closed")
    {
      state = Cover::StateClosed;
    }
    if (msgString == "closing")
    {
      state = Cover::StateClosing;
    }
    if (msgString == "opening")
    {
      state = Cover::StateOpening;
    }
    garageActuator->setState(state);
    garageLEDStatus->setState(state);
  }
}

void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void reconnect() {
  // Loop until we're reconnected
  while (!client->connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client->connect(clientId.c_str(),MQTT_USERNAME,MQTT_PASSWORD)) {
      Serial.println("connected");
      
      // Subscribe to messages that this application will responde to
      client->subscribe(MQTT_GARAGE_1_CMD_TOPIC);
      client->subscribe(MQTT_GARAGE_1_STATE_TOPIC);

    } else {
      Serial.print("failed, rc=");
      Serial.print(client->state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}


void setup() {
  // put your setup code here, to run once:
  client = new PubSubClient(espClient);
  garageActuator = new Cover::GarageDoorRelay(12,1500);
  garageLEDStatus = new Cover::SingleLEDStatus(15);

  Serial.begin(115200);
  setup_wifi();
  client->setServer(mqtt_server, 1883);
  client->setCallback(callback);

  // Call all components that need to perform setup
  garageActuator->setup();
  garageLEDStatus->setup();
}

void loop() {
  // put your main code here, to run repeatedly:
  if (!client->connected()) {
    reconnect();
  }

  // Look for any new messages
  client->loop();

  // Execute the loop function for each component
  garageActuator->loop();
  garageLEDStatus->loop();

}
