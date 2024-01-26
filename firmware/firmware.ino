#include <SparkFun_GridEYE_Arduino_Library.h>
#include <Wire.h>
#include <Ethernet2.h>
#include <EthernetUdp2.h>
#include <OSCMessage.h>

//----------------------------------------------------
// ETH/UDP setup
byte mac[] = {0x90, 0xA2, 0xDA, 0x10, 0x30, 0xD5};
IPAddress ip(10, 1, 0, 50);
IPAddress remoteIp(10, 1, 0, 8);
unsigned int localPort = 7010;
unsigned int remotePort = 8020;

EthernetUDP Udp;

//----------------------------------------------------
// GridEye Setup
#define HOT  30
#define COLD 25

int pixelTable[64];
char charArray[65]; // 65 to accommodate the null terminator

GridEYE grideye;

void setup() {
  Ethernet.begin(mac, ip);
  Udp.begin(localPort);

  Wire.begin();

  // Initialize the GridEYE object with its address
  grideye.begin(0x69);

  Serial.begin(115200);
}

void processGridEye() {
  for (unsigned char i = 0; i < 64; i++) {
    pixelTable[i] = map(grideye.getPixelTemperature(i), COLD, HOT, 0, 9);
  }
}

void sendOSC() {
  OSCMessage msg(charArray);
  Udp.beginPacket(remoteIp, remotePort);
  msg.send(Udp);
  Udp.endPacket();
  msg.empty();
}

void copyToCharArray() {
  for (int i = 0; i < 64; i++) {
    if (pixelTable[i] >= 0 && pixelTable[i] <= 9) {
      charArray[i] = '0' + pixelTable[i];
    } else if (pixelTable[i] > 9) {
      charArray[i] = '9';
    } else if (pixelTable[i] < 0) {
      charArray[i] = '0';
    }
  }
  charArray[64] = '\0'; // null-terminate the char array
}

void loop() {
  processGridEye();
  copyToCharArray();
  sendOSC();
  delay(10);
}
