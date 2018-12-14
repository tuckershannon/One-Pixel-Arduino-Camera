// Single RGB Color Sensor Plotter
// Tucker Shannon 2018

import controlP5.*;
ControlP5 cp5;
Slider abc;
int rboost;
int gboost;
int bboost;
import processing.serial.*;
Serial myPort;  
String val; 
float lightVal;
float gridX;
float gridY;
int photoSize = 900;
int origin = photoSize / 2;
int gridSize = photoSize / 100;
int xPos = origin;
int yPos = origin;
int red;
int blue;
int green;
int maxSeen = 1;
float multiplier = 1.0;
int[] reds = {};
int[] greens = {};
int[] blues = {};
int[] xPoss = {};
int[] yPoss = {};

void setup(){ 
  size(900,900);
  background(0);  
  String portName = Serial.list()[0];     // your [number] may be different
  myPort = new Serial(this, portName, 9600);
  myPort.bufferUntil('\n');
  cp5 = new ControlP5(this);
  cp5.addSlider("rboost")
     .setPosition(10,650)
     .setRange(0,200)
     .setValue(100)
     ;
  cp5.addSlider("gboost")
     .setPosition(10,670)
     .setRange(0,200)
     .setValue(100)
     ;
  cp5.addSlider("bboost")
     .setPosition(10,690)
     .setRange(0,200)
     .setValue(100)
     ;
}

void draw(){
    val = myPort.readStringUntil('\n'); 
    String[] list = split(val, ',');
    if (val != null) {     
      red = int(list[2]);
      green = int(list[3]);
      blue = Integer.parseInt(trim(list[4]));
      if (red>maxSeen){
        maxSeen = red;
      }
      if (green>maxSeen){
        maxSeen = green;
      }
      if (blue>maxSeen){
        maxSeen = blue;
      }
      reds = append(reds,int(red));
      blues = append(blues,int(blue));
      greens = append(greens,int(green));
      xPoss = append(xPoss,xPos);
      yPoss = append(yPoss,yPos);
      multiplier = 255.0/maxSeen;
      println("scalling: ",multiplier*maxSeen);   
      println(red, " " ,blue, " ", green);
      colorMode(RGB, maxSeen);
      for (int p = 0; p < xPoss.length; p++){
        fill(reds[p]*float(rboost)/100.0, greens[p]*float(gboost)/100.0, blues[p]*float(bboost)/100.0);
        stroke(reds[p]*float(rboost)/100.0, greens[p]*float(gboost)/100.0, blues[p]*float(bboost)/100.0);
        rect(xPoss[p],yPoss[p],gridSize,gridSize);
      }
      gridX = int(list[0]);
      gridY = int(list[1]);
      xPos = int(origin + gridX * gridSize);
      yPos = int(origin - gridY * gridSize);  
    }
}
