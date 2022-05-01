#include <esp_now.h>
#include <WiFi.h>
#include <MQTT.h>

const char ssid[] = "YOUR WIFI NAME";
const char pass[] = "YOUR WIFI PASSWORD";

WiFiClient net;
MQTTClient client;

unsigned long lastMillis = 0;

typedef struct struct_message {
    int temp;
    int humid;
} struct_message;

struct_message myData;

void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  memcpy(&myData, incomingData, sizeof(myData));
  Serial.print("Bytes received: ");
  Serial.println(len);
  Serial.println("Temperature: " + (String)myData.temp);
  Serial.println("Humidity: " + (String)myData.humid); 

  String 
  jsonEspNowData = "{"; 
  jsonEspNowData += "\"temperature\":" + (String)myData.temp + ",";
  jsonEspNowData += "\"humidity\":" + (String)myData.humid;
  jsonEspNowData += "}";
  Serial.println("ESP-NOW Node JSON Payload > " + jsonEspNowData + "\n");
  client.publish("HIBISCUS_SENSE/DATA", jsonEspNowData);
}

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_AP_STA);
  WiFi.begin(ssid, pass);

  client.begin("test.mosquitto.org", net);
  client.connect("GATEWAY");
  
  esp_now_init();
  esp_now_register_recv_cb(OnDataRecv);
}

void loop() {
  client.loop();
  delay(10);

  if (!client.connected()) {
    client.connect("GATEWAY");
  }
}
