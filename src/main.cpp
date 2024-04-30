#include <Arduino.h>

// Include necessary libraries here

#include <WiFi.h>
#include <PubSubClient.h>
#include <LiquidCrystal_I2C.h>
#include <ArduinoJson.h>

// Define global variables here

// WiFi credentials
const char *ssid = "IoTPrivate";
const char *password = "iotprivate303";

// MQTT credentials
const char *mqtt_server = "mqtt.iotserver.uz";
const int mqtt_port = 1883;
const char *mqtt_user = "userTTPU";
const char *mqtt_password = "mqttpass";

// MQTT client unique ID
const char *mqtt_client_id = "ttpuIoTTEacherEsp32";

// MQTT topics
const char *globalLedTopic = "ttpu/iot/global/led";
const char *btnPressTopic = "ttpu/iot/teacher/btn";

// pinout
const int redPin = 32;
const int yellowPin = 33;
const int greenPin = 25;
const int buttonPin = 26;

// Define objects
WiFiClient espClient;
PubSubClient client(espClient);
LiquidCrystal_I2C lcd(0x27, 16, 2); // Change I2C address and dimensions if needed

// Function prototypes
void setupWiFi();
void setupMQTT();
void callback(char *topic, byte *payload, unsigned int length);
void publishButtonPress();
void handleLedCommand(const char *command);

/**********************************/
// SETUP //
void setup(void) 
{
  // Initialize peripherals
  pinMode(redPin, OUTPUT);
  pinMode(yellowPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(buttonPin, INPUT_PULLUP);
  lcd.init();
  lcd.backlight();

  // Connect to WiFi
  setupWiFi();

  // Setup MQTT
  setupMQTT();
}

// Connect to WiFi
void setupWiFi() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
}

// Setup MQTT
void setupMQTT() {
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
}

/**********************************/
// LOOP //
void loop(void) 
{
  if (!client.connected()) {
    // Reconnect if not connected to MQTT
    setupMQTT();
  }
  client.loop();
  // Keep WiFi and MQTT connection alive
  if (WiFi.status() != WL_CONNECTED) {
    setupWiFi();
  }
}

// MQTT callback function
void callback(char *topic, byte *payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  // Handle LED control commands
  handleLedCommand((char *)payload);
}

// Publish button press
void publishButtonPress() {
  client.publish(btnPressTopic, "pressed");
}

// Handle LED control commands
void handleLedCommand(const char *command) {
  DynamicJsonDocument doc(1024);
  deserializeJson(doc, command);
  
  if (doc.containsKey("red")) {
    String state = doc["red"];
    digitalWrite(redPin, state == "on" ? HIGH : LOW);
  }
  if (doc.containsKey("yellow")) {
    String state = doc["yellow"];
    digitalWrite(yellowPin, state == "on" ? HIGH : LOW);
  }
  if (doc.containsKey("green")) {
    String state = doc["green"];
    digitalWrite(greenPin, state == "on" ? HIGH : LOW);
  }
}
