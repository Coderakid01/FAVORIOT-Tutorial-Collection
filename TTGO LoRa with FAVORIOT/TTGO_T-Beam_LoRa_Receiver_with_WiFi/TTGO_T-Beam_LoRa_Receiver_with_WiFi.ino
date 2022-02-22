#include <WiFi.h>
#include <HTTPClient.h>

#define SCK   5
#define MISO  19
#define MOSI  27
#define SS    18
#define RST   23
#define DIO0  26

//433E6 for Asia
//868E6 for Europe
//915E6 for North America
#define BAND  923E6

#include <SPI.h>
#include <LoRa.h>


int packetSize, rssi, snr;
String message = "No Message";

// Replace with your network credentials
const char* ssid     = "YOUR_WIFI_NAME";
const char* password = "YOUR_WIFI_PASSWORD";
const char* device_developer_id = "DEVICE DEVELOPER ID";

void startLoRA(){
  int counter;
  //SPI LoRa pins
  SPI.begin(SCK, MISO, MOSI, SS);
  //setup LoRa transceiver module
  LoRa.setPins(SS, RST, DIO0);

  while (!LoRa.begin(BAND) && counter < 10) {
    Serial.print(".");
    counter++;
    delay(500);
  }
  if (counter == 10) {
    // Increment readingID on every new reading
    Serial.println("Starting LoRa failed!"); 
  }
  Serial.println("LoRa Initialization OK!");
  
  LoRa.setSyncWord(0xF3);           // ranges from 0-0xFF, default 0x34, see API docs
  LoRa.setTxPower(7);
  
  delay(2000);
}

void connectWiFi(){

  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void setup()
{
  Serial.begin(115200);
  startLoRA();
  connectWiFi();

}

void loop()
{
  packetSize = LoRa.parsePacket();
  if (packetSize)
  {
    message = "";
    while (LoRa.available()) message += (char)LoRa.read();

    rssi = LoRa.packetRssi();
    snr = LoRa.packetSnr();

    Serial.println("Received packet: " + message);
    Serial.println("RSSI: " + String(rssi));
    Serial.println("SNR: " + String(snr));
    Serial.println();

    String json = "{\"device_developer_id\":\"" + String(device_developer_id) + "\",\"data\":{";
    
    json += "\"Counter\":\"" + String(message) + "\",";
    json += "\"RSSI\":\"" + String(rssi) + "\",";
    json += "\"SNR\":\"" + String(snr) + "\"";
    
    json += "}}";

    HTTPClient http;

    http.begin("https://apiv2.favoriot.com/v2/streams");
    http.addHeader("Content-Type", "application/json");
    http.addHeader("apikey", "YOUR API KEY");

    int httpCode = http.POST(json);

    if(httpCode > 0) {
      String payload = http.getString();
      Serial.println(payload);
    }
    else {
      Serial.println("HTTP Error!");
    }

   //delay(5000);
  }
}
