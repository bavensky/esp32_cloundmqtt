#include <WiFi.h>
#include <PubSubClient.h>

const char* ssid = "xxxx";
const char* password =  "xxxx";

const char* mqttServer = "xxxx";
const int mqttPort = 00000;
const char* mqttUser = "xxxx";
const char* mqttPassword = "xxxx";

#define LED_PIN 2

WiFiClient espClient;
PubSubClient client(espClient);

void callback(char* topic, byte* payload, unsigned int length) {
  payload[length] = '\0';
  String topic_str = topic, payload_str = (char*)payload;
  Serial.println("[" + topic_str + "]: " + payload_str);

  digitalWrite(LED_PIN, (payload_str == "ON") ? HIGH : LOW);
}

void setup() {

  Serial.begin(115200);
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
      delay(2000);

    }
  }

  client.subscribe("/CMMC/esp32Device");
}

uint32_t pevTime = 0;
uint32_t count;
char bufferData[100];
String dataSend;

void loop() {
  client.loop();

  uint32_t curTime = millis();
  if (curTime - pevTime >= 1000) {
    pevTime = curTime;

    count += 1;
    sprintf(bufferData, "count: %lu", count);
    dataSend = bufferData;
    
    Serial.print("data: ");
    Serial.println(dataSend);
    
    client.publish("esp32/temperature", dataSend.c_str());

  }
}
