/*
  LoRa receiver <RA-02>
  
  Github:   Portia-Lin
  https://github.com/Portia-Lin/man-made-accidents/blob/master/Receiver/Receiver.ino
*/

#include <ArduinoJson.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <SPI.h>
#include <LoRa.h>

#define TFT_CS 6
#define TFT_RST 8
#define TFT_DC 7
#define LORA_CS 10
#define LORA_RST 9
#define LORA_DIO 2
#define BUTTON 3 // default: D5
#define BUZZER 4

byte localAddress = 0xAA;
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

void setup() {
  Serial.begin(9600);
  pinMode(BUZZER, OUTPUT);
  pinMode(BUTTON,INPUT_PULLUP);
  tft.initR(INITR_BLACKTAB);
  tft.setRotation(3);
  startReceiver();
  LoRa.setPins(LORA_CS, LORA_RST, LORA_DIO);
  Serial.println("Starting LoRa Receiver...");
  if (!LoRa.begin(433.925E6)) {
    Serial.println("LoRa init failed. Check your connections.");
    loraFailed();
    while (true) {}
  }
  Serial.println("Start LoRa Receiver Is Successful");
  LoRa.setSpreadingFactor(12);
  LoRa.setSignalBandwidth(250E3);
  LoRa.setPreambleLength(8);
  LoRa.setSyncWord(0x12);
  LoRa.setCodingRate4(4);
  LoRa.enableCrc();
  clearDisplay();
}

void loop() {
  if (digitalRead(BUTTON)==LOW) {
    digitalWrite(BUZZER, 0);
    clearDisplay();
  }
  receiveMessage(LoRa.parsePacket());
}

void receiveMessage(int packetSize) {
  if (packetSize == 0) return;
  String data;
  int recipient = LoRa.read();
  byte sender = LoRa.read();
  byte dataLength = LoRa.read();

  while (LoRa.available()) {
    data += (char)LoRa.read();
  }

  if (dataLength != data.length()) {
    Serial.println("Error: Message length does not match length");
    return;
  }

  if (recipient != localAddress) {
    Serial.println("Error: Recipient address does not match local address");
    return;
  }

  Serial.print("Received data " + data);
  Serial.print(" from 0x" + String(sender, HEX));
  Serial.println(" to 0x" + String(recipient, HEX));
  digitalWrite(BUZZER, HIGH);
  parseMessage(data);
}

void parseMessage(String data) {
  Serial.println(data);
  StaticJsonDocument<200> doc;
  char json[data.length()];
  data.toCharArray(json, data.length()+1);
  DeserializationError error = deserializeJson(doc, json);
  String accident = doc["accident"];
  String address = doc["address"]; 
  Serial.println("Address: " + address);
  Serial.println("Accident: " + accident);
  displayMessage(accident, address); 
}

void startReceiver() {
  tft.fillScreen(ST77XX_WHITE);
  tft.setTextColor(ST77XX_BLACK);
  tft.setTextSize(2);
  tft.setCursor(10, 38);
  tft.println("START");
  tft.setCursor(10, 57);
  tft.println("LORA");
  tft.setCursor(10, 76);
  tft.println("RECEIVER");
  delay(1000);
}

void loraFailed() {
  tft.fillScreen(ST77XX_WHITE);
  tft.setTextColor(ST77XX_BLACK);
  tft.setTextSize(2);
  tft.setCursor(10, 28);
  tft.println("Starting");
  tft.setCursor(10, 47);
  tft.println("LoRa");
  tft.setCursor(10, 66);
  tft.println("Receiver");
  tft.setCursor(10, 83);
  tft.println("Faield!");
}

void displayMessage(String accident, String address) {
  tft.fillScreen(ST77XX_WHITE);
  tft.setTextColor(ST77XX_BLACK);
  tft.setTextSize(2);
  tft.setCursor(10, 28);
  tft.println("WARNING!");
  tft.setCursor(10, 47);
  tft.println(accident);
  tft.setCursor(10, 66);
  tft.println("AT");
  tft.setCursor(10, 85);
  tft.println(address);
}

void clearDisplay() {
  tft.fillScreen(ST77XX_WHITE);
  tft.setTextColor(ST77XX_BLACK);
  tft.setTextSize(2);
  tft.setCursor(10, 47);
  tft.println("NO NEW");
  tft.setCursor(10, 66);
  tft.println("DISASTERS");
}
