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

int counter;
String message;

void setup()
{
  Serial.begin(115200);
  SPI.begin(SCK, MISO, MOSI, SS);
  LoRa.setPins(SS, RST, DIO0);

  if (!LoRa.begin(BAND))
  {
    Serial.println("Starting LoRa failed!");
    while (1);
  }
  LoRa.setSyncWord(0xF3);           // ranges from 0-0xFF, default 0x34, see API docs
  LoRa.setTxPower(7);
}

void loop()
{
  counter++;

  message = "RAW DATA VALUE : " + String(counter);
  Serial.println("Sending: " + message);

  LoRa.beginPacket();
  LoRa.print(message);
  LoRa.endPacket();
  delay(1000);
}
