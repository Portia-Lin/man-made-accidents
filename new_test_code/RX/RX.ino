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
#define BUTTON 3

byte localAddress = 0xAA;
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

void setup() {
  Serial.begin(9600);
  pinMode(BUTTON,INPUT_PULLUP);
  tft.initR(INITR_BLACKTAB);
  tft.setRotation(1);
  startReceiver();
  LoRa.setPins(LORA_CS, LORA_RST, LORA_DIO);
  Serial.println("Starting LoRa Receiver...");
  if (!LoRa.begin(433E6)) {
    Serial.println("LoRa init failed. Check your connections.");
    loraFailed();
    while (true) {}
  }
  Serial.println("Start LoRa Receiver Is Successful");
  LoRa.setSpreadingFactor(12);
  LoRa.setSignalBandwidth(62.5E3);
  LoRa.setPreambleLength(8);
  LoRa.setSyncWord(0x12);
  LoRa.setCodingRate4(8);
  LoRa.enableCrc();
  //LoRa.setGain(1);
  clearDisplay();
}

void loop() {
  if (digitalRead(BUTTON)==LOW) {
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
  int rssi = LoRa.packetRssi();
  float snr = LoRa.packetSnr();
  
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
  Serial.print("' with RSSI ");
  Serial.println(rssi);
  
  displayMessage(data, rssi, snr);
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

void displayMessage(String packet, int rssi, float snr) {
  tft.fillScreen(ST77XX_WHITE);
  tft.setTextColor(ST77XX_BLACK);
  tft.setTextSize(2);
  tft.setCursor(10, 28);
  tft.println("Packet:");
  tft.setCursor(10, 47);
  tft.setTextColor(ST77XX_RED);
  tft.println(packet);
  tft.setCursor(10, 66);
  tft.setTextColor(ST77XX_BLACK);
  tft.println("RSSI:");
  tft.setCursor(70, 66);
  tft.println("SNR:");
  tft.setCursor(10, 85);
  tft.setTextColor(ST77XX_RED);
  tft.println(rssi);
  tft.setCursor(70, 85);
  tft.println(snr);
}

void clearDisplay() {
  tft.fillScreen(ST77XX_WHITE);
  tft.setTextColor(ST77XX_BLACK);
  tft.setTextSize(2);
  tft.setCursor(10, 47);
  tft.println("NO NEW");
  tft.setCursor(10, 66);
  tft.println("PACKETS");
}
