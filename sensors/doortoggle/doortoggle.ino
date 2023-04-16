//  Here's an example code snippet that demonstrates how to use a magnetic reed switch with an ESP8266, connect to WiFi, and send data using MQTT

#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// WiFi credentials
const char* ssid = "your_SSID_here";
const char* password = "your_WIFI_password_here";

// MQTT broker settings
const char* mqtt_server = "your_broker_server_IP_address_here";
const int mqtt_port = 1883;
const char* mqtt_username = "your_mqtt_username_here";
const char* mqtt_password = "your_mqtt_password_here";

// MQTT topic and message
const char* mqtt_topic = "your_mqtt_topic_here";
char mqtt_message[50];

// Magnetic reed switch settings
const int switchPin = D1;
int switchState = LOW;
int count = 0;

// WiFi client and MQTT client
WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

void setup() {
  Serial.begin(9600);
  
  // Connect to WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi.");
  
  // Connect to MQTT broker
  mqttClient.setServer(mqtt_server, mqtt_port);
  mqttClient.setCredentials(mqtt_username, mqtt_password);
  while (!mqttClient.connected()) {
    if (mqttClient.connect("ESP8266Client")) {
      Serial.println("Connected to MQTT broker.");
    } else {
      Serial.println("Failed to connect to MQTT broker. Retrying...");
      delay(1000);
    }
  }

  // Set up magnetic reed switch
  pinMode(switchPin, INPUT_PULLUP);
}

void loop() {
  switchState = digitalRead(switchPin);
  if (switchState == HIGH) {
    count++;
    Serial.print("Door opened. Count = ");
    Serial.println(count);
    
    // Send count to MQTT broker
    snprintf(mqtt_message, 50, "%d", count);
    mqttClient.publish(mqtt_topic, mqtt_message);
  }
  delay(100);
  
  // Check for MQTT messages
  mqttClient.loop();
}
