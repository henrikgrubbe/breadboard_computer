#include <Wire.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

const char* ssid = "FDF_Hotspot";
// const char* password = "foobar";
const char* mqtt_server = "it.fantastiskefroe.dk";
const int mqtt_port = 1883;

const char* topic_program = "progRAM";
const char* topic_command = "command";

const char* command_programFromEEPROM = "PROGRAM";

#define DELAY 100

#define SLAVE_ADDR 9


WiFiClient wifiClient;
PubSubClient client(wifiClient);


void setup() {
  

  Wire.begin();

  Serial.begin(115200);
  Serial.setTimeout(30000);

  Wire.begin();
  delay(DELAY);

  setupWifi();
  setupMqtt();
}

void setupWifi() {
  WiFi.mode(WIFI_STA);
  // WiFi.begin(ssid, password);
  WiFi.begin(ssid);
  Serial.println("\nConnecting");

  while(WiFi.status() != WL_CONNECTED){
      Serial.print(".");
      delay(100);
  }

  Serial.println("\nConnected to the WiFi network");
  Serial.print("Local ESP32 IP: ");
  Serial.println(WiFi.localIP());
}

void setupMqtt() {
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(mqttCallback);
}

void loop() {
  if (!client.connected()) {
    mqttReconnect();
  }
  client.loop();
}

void sendLine(uint8_t addr, uint8_t data) {
  Wire.beginTransmission(SLAVE_ADDR);
  Wire.write(addr);
  Wire.write(data);
  Wire.endTransmission();
  delay(DELAY);
}

void commandProgram() {
  sendLine(0x10,0x00);
}




void mqttCallback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
    Serial.print(topic);
  Serial.print("] ");
  
  for (int i=0;i<length;i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
  

  DynamicJsonDocument doc(length+10);
  DeserializationError err = deserializeJson(doc, payload);
  if(err != DeserializationError::Ok) {
    Serial.println("invalid json document");
    return;
  }



  if(strcmp(topic, topic_command) == 0) {
    Serial.println("COMMAND topic");

    const char* command = doc.as<const char*>();
    if(strcmp(command, command_programFromEEPROM) == 0) {
      commandProgram();
    }

  }
  else if(strcmp(topic, topic_program) == 0) {
    Serial.println("DATA topic");

    if(doc.containsKey("0"))  { sendLine( 0,doc["0"]); }
    if(doc.containsKey("1"))  { sendLine( 1,doc["1"]); }
    if(doc.containsKey("2"))  { sendLine( 2,doc["2"]); }
    if(doc.containsKey("3"))  { sendLine( 3,doc["3"]); }
    if(doc.containsKey("4"))  { sendLine( 4,doc["4"]); }
    if(doc.containsKey("5"))  { sendLine( 5,doc["5"]); }
    if(doc.containsKey("6"))  { sendLine( 6,doc["6"]); }
    if(doc.containsKey("7"))  { sendLine( 7,doc["7"]); }
    if(doc.containsKey("8"))  { sendLine( 8,doc["8"]); }
    if(doc.containsKey("9"))  { sendLine( 9,doc["9"]); }
    if(doc.containsKey("10")) { sendLine(10,doc["10"]); }
    if(doc.containsKey("11")) { sendLine(11,doc["11"]); }
  }
  else {
    Serial.println("Unknown topic");
  }
}


void mqttReconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("breadboard_wemos")) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("outTopic","hello world");
      // ... and resubscribe
      client.subscribe(topic_program);
      client.subscribe(topic_command);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}
