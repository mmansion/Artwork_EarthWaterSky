#include <SparkFun_GridEYE_Arduino_Library.h>
#include <Wire.h>
#include <SPI.h>
#include <Ethernet2.h>
#include <EthernetUdp2.h>
#include <OSCMessage.h>
//----------------------------------------------------
// ETH/UDP setup
byte mac[] = { 0x90, 0xA2, 0xDA, 0x10, 0x30, 0xD5  };
IPAddress ip(10, 1, 0, 50);
IPAddress remoteIp(10, 1, 0, 8);
unsigned int localPort = 7010;      // local port to listen on
unsigned int remotePort = 8020;

// buffers for receiving and sending data
char packetBuffer[UDP_TX_PACKET_MAX_SIZE]; //buffer to hold incoming packet,
char ReplyBuffer[] = "acknowledged";       // a string to send back

// An EthernetUDP instance to let us send and receive packets over UDP
EthernetUDP Udp;

//----------------------------------------------------
// GridEye Setup
// Use these values (in degrees C) to adjust the contrast
#define HOT  30
#define COLD 25

int combinedPixelTable[128]; // Combined pixel table for two sensors
char charArray[129]; // 129 to accommodate the null terminator

GridEYE grideye1;
GridEYE grideye2;

void setup() {

  Ethernet.begin(mac, ip);
  Udp.begin(localPort);

  Wire.begin();

  // Initialize the two GridEYE objects with their addresses
  grideye1.begin(0x68);
  grideye2.begin(0x69);

  Serial.begin(115200);

}

void processGridEye(GridEYE &grideye, int startIndex) {
  for(unsigned char i = 0; i < 64; i++) {
    combinedPixelTable[startIndex + i] = map(grideye.getPixelTemperature(i), COLD, HOT, 0, 9);
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
  for (int i = 0; i < 128; i++) {
    // Assuring that the values are between 0 and 9
    if (combinedPixelTable[i] >= 0 && combinedPixelTable[i] <= 9) {
      // Convert integer to char and store in charArray
      charArray[i] = '0' + combinedPixelTable[i];
      // Handle error for values outside 0-9
    } else if (combinedPixelTable[i] > 9) {
      charArray[i] = '9';
    } else if (combinedPixelTable[i] < 0) {
      charArray[i] = '0';
    }
  }
  charArray[128] = '\0'; // null-terminate the char array
}


void loop() {
  processGridEye(grideye1, 0);   // first sensor fills indices 0 to 63
  processGridEye(grideye2, 64);  // second sensor fills indices 64 to 127
//  displayCombinedGrid();
//  return;

  // Call the function to copy values to charArray
  copyToCharArray();

  sendOSC(); 
  delay(10);
//
//  // if there's data available, read a packet
//  int packetSize = Udp.parsePacket();
//  if (packetSize)
//  {
//    Serial.print("Received packet of size ");
//    Serial.println(packetSize);
//    Serial.print("From ");
//    IPAddress remote = Udp.remoteIP();
//    for (int i = 0; i < 4; i++)
//    {
//      Serial.print(remote[i], DEC);
//      if (i < 3)
//      {
//        Serial.print(".");
//      }
//    }
//    Serial.print(", port ");
//    Serial.println(Udp.remotePort());
//
//    // read the packet into packetBufffer
//    Udp.read(packetBuffer, UDP_TX_PACKET_MAX_SIZE);
//    Serial.println("Contents:");
//    Serial.println(packetBuffer);
//
//    // send a reply, to the IP address and port that sent us the packet we received
//    Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
//    Udp.write(ReplyBuffer);
//    Udp.endPacket();
//  }
//  delay(10);
  
}
