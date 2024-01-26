import hypermedia.net.*;
import controlP5.*;
import codeanticode.syphon.*; // Import the Syphon library
UDP udp;
int cols = 8;
int rows = 16;
float[][] grid = new float[rows][cols];
float[][] targetGrid = new float[rows][cols];
ControlP5 cp5;
int lowValue = 0;
int highValue = 255;
Boolean showGui = false;
int lastReceiveTime = 0;
int timeout = 2000; // Set timeout for 2000 milliseconds or 2 seconds
SyphonServer server; // Declare a SyphonServer object

void setup() {
  size(400, 800, P3D);//disable P3D to visualize in processing
  server = new SyphonServer(this, "grideye1"); // Initialize the Syphon server
  
  cp5 = new ControlP5(this);
  
  cp5.addSlider("lowValue")
    .setPosition(20, 100)
    .setRange(0, 255)
    .setValue(0);
  
  cp5.addSlider("highValue")
    .setPosition(200, 100)
    .setRange(0, 255)
    .setValue(255);
    
  cp5.hide();
    
  udp = new UDP(this, 8020);
  udp.listen(true);
  lastReceiveTime = millis();
}

void draw() {
  background(255);
  float cellwidth = width / float(cols);
  float cellheight = (height) / float(rows); // reserved 100px height for sliders
  
  // If timeout period is reached, start fading to black
  if(millis() - lastReceiveTime > timeout) {
    for (int i = 0; i < rows / 2; i++) {
      for (int j = 0; j < cols; j++) {
        targetGrid[i][j] -= 0.1; // Decrease the value to fade to black
        targetGrid[i][j] = constrain(targetGrid[i][j], 0, 9);
      }
    }
  }
  
  for (int i = 0; i < rows; i++) {
    float originalY = map(i, 0, rows-1, 0, (rows/2)-1);
    int y1 = int(originalY);
    int y2 = min(y1 + 1, (rows/2) - 1);
    float lerpFactorY = originalY - y1;
    
    for (int j = 0; j < cols; j++) {
      grid[i][j] = lerp(grid[i][j], lerp(targetGrid[y1][j], targetGrid[y2][j], lerpFactorY), 0.05);
      fill(map(grid[i][j], 0, 9, lowValue, highValue));
      rect(j * cellwidth, i * cellheight, cellwidth, cellheight);
    }
  }
  server.sendScreen(); // Send the frame to the Syphon server after drawing
}

void receive(byte[] data, String ip, int port) {
  lastReceiveTime = millis(); // Update the last received time when new data comes in
  String msg = new String(data);
  for (int i = 0; i < rows / 2; i++) {
    for (int j = 0; j < cols; j++) {
      int index = i * cols + j;
      if (index < msg.length()) {
        char ch = msg.charAt(index);
        if (Character.isDigit(ch)) {
          targetGrid[i][j] = ch - '0';
        }
      }
    }
  }
}

void keyPressed() {
  if (key == ' ') {
    showGui = !showGui;
    if (showGui) {
      cp5.show();
    } else {
      cp5.hide();
    }
  }
}
