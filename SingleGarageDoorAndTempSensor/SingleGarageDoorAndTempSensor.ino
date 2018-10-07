#include "mywifi.h"
#include "mymqttbroker.h"
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Cover_DualSwitchSensor.h>
#include <Sensor_SimpleDHT11.h>
#include "ctype.h"

#define MQTT_GARAGE_1_ROOT_TOPIC "/home/garage/1"
#define MQTT_GARAGE_STATE_TOPIC "/cover/state"
#define MQTT_GARAGE_CMD_TOPIC "/cover/cmd"
#define MQTT_GARAGE_1_STATE_TOPIC MQTT_GARAGE_1_ROOT_TOPIC MQTT_GARAGE_STATE_TOPIC
#define MQTT_GARAGE_1_CMD_TOPIC MQTT_GARAGE_1_ROOT_TOPIC MQTT_GARAGE_CMD_TOPIC
#define MQTT_TEMP_STATE_TOPIC "/temp/state"
#define MQTT_HUMID_STATE_TOPIC "/humidity/state"
#define MQTT_GARAGE_1_TEMP_STATE_TOPIC MQTT_GARAGE_1_ROOT_TOPIC MQTT_TEMP_STATE_TOPIC
#define MQTT_GARAGE_1_HUMID_STATE_TOPIC MQTT_GARAGE_1_ROOT_TOPIC MQTT_HUMID_STATE_TOPIC

const char* ssid = WLAN_SSID;
const char* password = WLAN_PASS;
const char* mqtt_server = MQTT_SERVER;

char message_buff[20];

WiFiClient espClient;
PubSubClient* client;
Cover::DualSwitchSensor* garageCoverSensor;
Sensor::SensorSimpleDHT11 *dht11Sensor;

String temp_str; 
String hum_str;

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

  // Parse any messages and convert it into a command;
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
      // Note: this device does not subscribe to anything

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
  garageCoverSensor = new Cover::DualSwitchSensor(13, 14);
  dht11Sensor = new Sensor::SensorSimpleDHT11(2, false, 10);

  Serial.begin(115200);
  setup_wifi();
  client->setServer(mqtt_server, 1883);
  client->setCallback(callback);

  // Call all components that need to perform setup
  garageCoverSensor->setup();
  dht11Sensor->setup();
}

void loop() {
  // put your main code here, to run repeatedly:
  if (!client->connected()) {
    reconnect();
  }

  // Look for any new messages
  client->loop();

  // Execute the loop function for each component
  garageCoverSensor->loop();

  // Check if the state changed and publish a message if it has
  if (garageCoverSensor->getStateChanged())
  {
    Cover::State state = garageCoverSensor->getCurrentState();
    if (state == Cover::StateOpen)
      client->publish(MQTT_GARAGE_1_STATE_TOPIC, "open");
    if (state == Cover::StateOpening)
      client->publish(MQTT_GARAGE_1_STATE_TOPIC, "opening");
    if (state == Cover::StateClosed)
      client->publish(MQTT_GARAGE_1_STATE_TOPIC, "closed");
    if (state == Cover::StateClosing)
      client->publish(MQTT_GARAGE_1_STATE_TOPIC, "closing");
    if (state == Cover::StateUnknown)
      client->publish(MQTT_GARAGE_1_STATE_TOPIC, "unknown");
  }

  if (dht11Sensor->loop())
  {
    // We have new sensor values available so publish them
    temp_str = String(dht11Sensor->getTemperature()); 
    hum_str = String(dht11Sensor->getHumidity()); 
    
    temp_str.toCharArray(message_buff, temp_str.length() + 1); 
    client->publish(MQTT_GARAGE_1_TEMP_STATE_TOPIC, message_buff);

    hum_str.toCharArray(message_buff, hum_str.length() + 1); 
    client->publish(MQTT_GARAGE_1_HUMID_STATE_TOPIC, message_buff);
  }
}
