#include <SPI.h>
#include <LoRa.h>

#define LORA_CS 10
#define LORA_RST 9
#define LORA_DIO 2
#define IN 4

byte localAddress = 0xBB;
byte destinationAddress1 = 0xAB;
byte destinationAddress2 = 0xAA;
boolean check = 0;
String data = "{\"accident\":\"FIRE\",\"address\":\"Chudnivska,103\"}";

void setup() {
  Serial.begin(9600);
  Serial.println("Starting LoRa Transmitter...");
  LoRa.setPins(LORA_CS, LORA_RST, LORA_DIO);
  if (!LoRa.begin(433.925E6)) {
    Serial.println("LoRa init failed. Check your connections.");
    while (true) {}
  }
  Serial.println("Start LoRa Transmitter Is Successful");
  LoRa.setSpreadingFactor(12);
  LoRa.setSignalBandwidth(250E3);
  LoRa.setPreambleLength(8);
  LoRa.setSyncWord(0x12);
  LoRa.setCodingRate4(4);
  LoRa.enableCrc();
  LoRa.setTxPower(20);
}

void loop() {
  checkSensor();
}

void checkSensor() {
  if (digitalRead(IN) == HIGH && check == 0) {
    Serial.println("Sensor detect the fire!");
    sendToRX(data, destinationAddress1);
    sendToRX(data, destinationAddress2);
    check = 1;
  }
  if (digitalRead(IN) == LOW && check == 1) {
    check = 0;
  }
}

void sendToRX(String data, byte destinationAddress) {
  Serial.println("Sending message to: " + String(destinationAddress, HEX));
  LoRa.beginPacket();
  LoRa.write(destinationAddress);
  LoRa.write(localAddress);
  LoRa.write(data.length());
  LoRa.print(data);
  LoRa.endPacket();
}
