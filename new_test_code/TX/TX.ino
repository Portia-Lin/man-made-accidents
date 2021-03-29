#include <SPI.h>
#include <LoRa.h>

#define LORA_CS 10
#define LORA_RST 9
#define LORA_DIO 2

int counter = 0;
byte localAddress = 0xBB;
byte destinationAddress = 0xAA;

void setup() {
  Serial.begin(9600);
  Serial.println("Starting LoRa Transmitter...");
  LoRa.setPins(LORA_CS, LORA_RST, LORA_DIO);

  if (!LoRa.begin(433E6)) {
    Serial.println("LoRa init failed. Check your connections.");
    while (true) {}
  }
  Serial.println("Start LoRa Transmitter Is Successful");
  LoRa.setSpreadingFactor(12);
  LoRa.setSignalBandwidth(62.5E3);
  LoRa.setPreambleLength(8);
  LoRa.setSyncWord(0x12);
  LoRa.setCodingRate4(8);
  LoRa.enableCrc();
  LoRa.setTxPower(20);
}

void loop() {
  Serial.print("Sending packet: ");
  Serial.println(counter);
  String data = "hello " + String(counter);

  // send packet
  LoRa.beginPacket();
  LoRa.write(destinationAddress);
  LoRa.write(localAddress);
  LoRa.write(data.length());
  LoRa.print(data);
  LoRa.endPacket();

  counter++;
  delay(3000);
}
