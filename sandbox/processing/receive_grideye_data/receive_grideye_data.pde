import hypermedia.net.*;
import controlP5.*;

UDP udp;
int cols = 8;
int rows = 16;
float[][] grid = new float[rows][cols];
float[][] targetGrid = new float[rows][cols];
ControlP5 cp5;
int lowValue = 255;
int highValue = 0;
Boolean showGui = false;

void setup() {
  size(400, 800);
  
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
}

void draw() {
  background(255);
  float cellwidth = width / float(cols);
  float cellheight = (height) / float(rows); // reserved 100px height for sliders
  for (int i = 0; i < rows; i++) {
    for (int j = 0; j < cols; j++) {
      grid[i][j] = lerp(grid[i][j], targetGrid[i][j], 0.05);
      //stroke(255,0,0);
      fill(map(grid[i][j], 0, 9, lowValue, highValue));
      rect(j * cellwidth, i * cellheight, cellwidth, cellheight);
    }
  }
}

void receive(byte[] data, String ip, int port) {
  String msg = new String(data);
  for (int i = 0; i < rows; i++) {
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
