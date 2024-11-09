#include <time.h>
#include <WiFi.h>
#include <Wire.h>
#include <WebServer.h>
#include <AutoConnectCore.h>
#include <ArduinoJson.h>
#include <PubSubClient.h>


#define DELAY 100
#define NUM_ADDR 16
#define SLAVE_ADDR 9


const char *mqtt_server = "breadboard.achri.dk";
const int mqtt_port = 1883;

const char *topic_program = "program";
const char *topic_command = "command";
const char *command_key = "command";
const char *filter_string = "{\"command\":true,\"address\":true,\"data\":true,\"0\":true,\"1\":true,\"2\":true,\"3\":true,\"4\":true,\"5\":true,\"6\":true,\"7\":true,\"8\":true,\"9\":true,\"10\":true,\"11\":true,\"12\":true,\"13\":true,\"14\":true,\"15\":true}";
const char *command_programFromEEPROM = "PROGRAM";
const char *command_immediateWrite = "WRITE";


WiFiClient wifiClient;
PubSubClient MQTTClient(wifiClient);
StaticJsonDocument<512> jsonFilter;


WebServer Server;
AutoConnect WiFiSetupPortal(Server);
AutoConnectConfig config;


void setup() {
  Wire.begin();

  Serial.begin(115200);
  Serial.setTimeout(30000);

  Wire.begin();
  delay(DELAY);

  setupWifi();
  setupMqtt();
}

void onWIFIConnected(IPAddress& ipaddr) {
  Serial.printf("WiiFi connected with %s, IP:%s\n", WiFi.SSID().c_str(), ipaddr.toString().c_str());
  // if (WiFi.getMode() & WIFI_AP) {
  //   WiFi.softAPdisconnect(true);
  //   WiFi.enableAP(false);
  //   Serial.printf("SoftAP:%s shut down\n", WiFi.softAPSSID().c_str());
  // }
}

void setupWifi() {
  config.title = "Gluten PC";
  config.apid = "Gluten-PC";
  config.psk  = "";
  // config.autoRise = false;
  config.retainPortal = true;
  WiFiSetupPortal.config(config);
  WiFiSetupPortal.onConnect(onWIFIConnected);
  WiFiSetupPortal.disableMenu(AC_MENUITEM_HOME);
  WiFiSetupPortal.begin();

  // config.autoRise = true;   // Enable the launch of the captive portal.
  // WiFiSetupPortal.config(config);    // Don't forget it.

  Serial.println("Web Server started:" + WiFi.localIP().toString());
}

void setupMqtt() {  
  deserializeJson(jsonFilter, filter_string);

  MQTTClient.setServer(mqtt_server, mqtt_port);
  MQTTClient.setBufferSize(2048);
  MQTTClient.setCallback(mqttCallback);
}

void loop() {
  WiFiSetupPortal.handleClient();
  if (!MQTTClient.connected()) {
    mqttReconnect();
  }
  MQTTClient.loop();
}

void sendLine(uint8_t addr, uint8_t data) {
  Wire.beginTransmission(SLAVE_ADDR);
  Wire.write(addr);
  Wire.write(data);
  Wire.endTransmission();
  delay(DELAY);
}

void commandProgram() {
  sendLine(0x10, 0x00);
}

void immidiateWrite(uint8_t addr, uint8_t data) {
  Serial.println(addr);
  Serial.println(0x20 | addr);
  Serial.println(data);
  sendLine(0x20 | addr, data);
}

void mqttCallback(char *topic, byte *payload, unsigned int length) {
  Serial.print("Message with length "); Serial.print(length); Serial.print(" recieved on topic "); Serial.print(topic); Serial.println(":");

  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  StaticJsonDocument<512> doc;
  DeserializationError err = deserializeJson(doc, payload, DeserializationOption::Filter(jsonFilter));
  if (err != DeserializationError::Ok) {
    Serial.print("JSON deserialization failed with error: "); Serial.println(err.c_str());
    return;
  }

  if (strcmp(topic, topic_command) == 0) {
    Serial.println("COMMAND topic");

    if (doc.containsKey(command_key)) {
      if (strcmp(doc[command_key], command_programFromEEPROM) == 0) {
        commandProgram();
      } else if (strcmp(doc[command_key], command_immediateWrite) == 0) {
        immidiateWrite(doc["address"].as<unsigned int>(), doc["data"].as<unsigned int>());
      } else {
        Serial.print("Unknown command: "); Serial.println(doc[command_key].as<const char*>());
      }
    }

  } else if (strcmp(topic, topic_program) == 0) {
    Serial.println("PROGRAM topic");
    serializeJson(doc, Serial);

    for (uint8_t addr = 0; addr < NUM_ADDR; addr++) {
      char addrStr[3];
      sprintf(addrStr, "%d", addr);
      if (doc.containsKey(addrStr)) {
        sendLine(addr, doc[addrStr]);
      }
    }
  } else {
    Serial.println("Unknown topic");
  }
}

void mqttReconnect() {
  // Loop until we're reconnected
  while (!MQTTClient.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (MQTTClient.connect("breadboard_wemos")) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      MQTTClient.publish("outTopic", "hello world");
      // ... and resubscribe
      MQTTClient.subscribe(topic_program);
      MQTTClient.subscribe(topic_command);
    } else {
      Serial.print("failed, rc=");
      Serial.print(MQTTClient.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}
