//Arduino RGB Color Sensor Camera
//Tucker Shannon 2018
//For use with Adafruit RGB Color Sensor TCS34725

#include "Adafruit_TCS34725.h"

//Setting up pins for motor stepper motors
int yMotorPins[] = {11, 10, 9, 8};
int xMotorPins[] = {7, 6, 5, 4};
int photoPin = 3;
//Stepper motor rotation sequence
int Seq[][4] = {{1, 0, 0, 0},
                {1, 1, 0, 0}, 
                {0, 1, 0, 0}, 
                {0, 1, 1, 0}, 
                {0, 0, 1, 0}, 
                {0, 0, 1, 1}, 
                {0, 0, 0, 1}, 
                {1, 0, 0, 1}};         
int nSteps[] {0, 0};
//Setting up viewing angle and grid size
float viewingAngle = 20.0;
float photoSize = 40.0;
float gridSize = tan((viewingAngle/360.0)*2*3.14159265359)/photoSize; 
float radPerStep = (2.0 * 3.14159265359) / 4076.0;
float currentTheta = 0.0001;
float currentPhi = 0.0001;
float currentX = 0.0;
float currentY = 0.0;
float currentDX = sin(radPerStep) / cos(currentTheta);
float currentDY = sin(radPerStep) / cos(currentPhi);
float startPosition = 0.0;
float nextX;
float nextY;
int gridX = 0;
int gridY = 0;
int seqStepY = 0;
int seqStepX = 0;

//color sensor Adafruit_TCS34725
Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_154MS, TCS34725_GAIN_60X);



void setup() {
  Serial.begin(9600);
  for (int nPin = 0; nPin < 4; nPin++) {
    pinMode(yMotorPins[nPin], OUTPUT);
    pinMode(xMotorPins[nPin], OUTPUT);
  }
}



void loop() {
  //wait 1 second before starting
  delay(5000);
  takePhoto();
  returnHome();
  while (true){delay(1000);}
}


int takePhoto(){
  //moveN = number of grid spaces to move from center
  float moveN = 1;
  //spiral from center outwards
  for (int nRowsCols = 0;  nRowsCols < photoSize/2.0; nRowsCols++) {
    for (int x = 0; x < moveN; x++) {
      nextX = currentX + gridSize;
      gridX = gridX + 1;
      moveToNew();
    }
    for (int x = 0; x < moveN; x++) {
      nextY = currentY + gridSize;
      gridY = gridY + 1;
      moveToNew();
    }
    moveN++;
    for (int x = 0; x < moveN; x++) {
      nextX = currentX - gridSize;
      gridX = gridX - 1;
      moveToNew();
    }
    for (int x = 0; x < moveN; x++) {
      nextY = currentY - gridSize;
      gridY = gridY - 1;
      moveToNew();
    }
    moveN++;
  }
}



int moveToNew() {
  while (abs(currentX - nextX) > currentDX || abs(currentY - nextY) > currentDY) {
    currentDX = sin(radPerStep) / cos(currentTheta);
    currentDY = sin(radPerStep) / cos(currentPhi);
    currentX = tan(currentTheta);
    currentY = tan(currentPhi) / cos(currentTheta);
    if (abs(currentX - nextX) > currentDX) {
      if (currentX < nextX) {
        takeStep(1, "left");
        currentTheta = currentTheta + radPerStep;
      }
      if (currentX > nextX) {
        takeStep(1, "right");
        currentTheta = currentTheta - radPerStep;
      }
    }
    if (abs(currentY - nextY) > currentDY) {
      if (currentY < nextY) {
        takeStep(2, "left");
        currentPhi = currentPhi + radPerStep;
      }
      if (currentY > nextY) {
        takeStep(2, "right");
        currentPhi = currentPhi - radPerStep;
      }
    }
  }
  //logging the data for the "pixel" to be used in image
  //format: x,y,r,g,b
  Serial.print(gridX);
  Serial.print(",");
  Serial.print(gridY);
  Serial.print(",");
  uint16_t clr, red, green, blue;
  tcs.getRawData(&red, &green, &blue, &clr);
  Serial.print(red);
  Serial.print(",");
  Serial.print(green);
  Serial.print(",");
  Serial.println(blue);
}


//this function takes in a stepper motor number (1 or 2) and a direction (1 or 2)

int takeStep(int motor, char dir[]) {
  int xPin;
  int yPin;
  if (motor == 1) {
    for (int nPin = 0; nPin < 4; nPin++) {
      xPin = xMotorPins[nPin];
      if (Seq[seqStepX][nPin] != 0) {digitalWrite(xPin, HIGH);}
      else {digitalWrite(xPin, LOW);}
    }
  }
  else if (motor == 2) {
    for (int nPin = 0; nPin < 4; nPin++) {
      yPin = yMotorPins[nPin];
      if (Seq[seqStepY][nPin] != 0) {digitalWrite(yPin, HIGH);}
      else {digitalWrite(yPin, LOW);}
    }
  }
  delay(2);
  if (dir == "left") {
    if (motor == 2) {
      nSteps[0]++;
      if (seqStepY == 7) {seqStepY = 0;}
      else {seqStepY++;}
    }
    if (motor == 1) {
      nSteps[1]++;
      if (seqStepX == 7) {seqStepX = 0;}
      else {seqStepX++;}
    }
  }
  if (dir == "right") {
    if (motor == 2) {
      nSteps[0]--;
      if (seqStepY == 0) {seqStepY = 7;}
      else {seqStepY--;}
    }
    if (motor == 1) {
      nSteps[1]--;
      if (seqStepX == 0) {seqStepX = 7;}
      else {seqStepX--;}
    }
  }
}

int returnHome() {
  while (abs(nSteps[0]) > 1 or abs(nSteps[1]) > 1) {
    if (nSteps[0] > 1){takeStep(2, "right");}
    if (nSteps[0] < 1){takeStep(2, "left");}
    if (nSteps[1] > 1){takeStep(1, "right");}
    if (nSteps[1] < 1){takeStep(1, "left");}
  }
}
