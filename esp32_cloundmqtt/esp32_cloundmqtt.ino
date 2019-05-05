#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

const char* ssid = "xxxxx";
const char* password =  "xxxxx";

const char* mqttServer = "xxxxx";
const int mqttPort = 00000;
const char* mqttUser = "xxxxx";
const char* mqttPassword = "xxxxx";
const char* mqttName = "superman";

#define LED_PIN 2

WiFiClient espClient;
PubSubClient client(espClient);


void callback(char* topic, byte* payload, unsigned int length) {
  payload[length] = '\0';
  String topic_str = topic, payload_str = (char*)payload;
  Serial.println("[" + topic_str + "]: " + payload_str);

  if (payload_str == "ON") {
    digitalWrite(LED_PIN, LOW);
  } else {
    digitalWrite(LED_PIN, HIGH);
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);

  WiFi.begin(ssid, password);


  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Connecting to WiFi..");
  }

  Serial.println("Connected to the WiFi network");

  client.setServer(mqttServer, mqttPort);

  while (!client.connected()) {
    Serial.println("Connecting to MQTT...");

    if (client.connect("ESP32Client", mqttUser, mqttPassword )) {
      Serial.println("connected");
    } else {
      Serial.print("failed with state ");
      Serial.println(client.state());
      delay(1000);
    }
  }

  client.subscribe("CMMC/esp32Device");
  client.setCallback(callback);
}

uint32_t pevTime = 0;
uint32_t count;

char bufferData[100];
String dataSend;
String output;

StaticJsonBuffer<200> jsonBuffer;
JsonObject& deviceData = jsonBuffer.createObject();

void loop() {
  client.loop();

  uint32_t curTime = millis();
  if (curTime - pevTime >= 1000) {
    pevTime = curTime;

    count += 1;

    deviceData["name"] = mqttName;
    deviceData["count"] = count;
    deviceData.printTo(output);

    Serial.print("data: ");
    Serial.println(output);

    client.publish("CMMC/data", output.c_str());
  }
}
