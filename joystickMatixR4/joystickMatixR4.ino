#include <Mouse.h>
//https://www.arduino.cc/reference/en/language/functions/usb/mouse/

#include "Arduino_LED_Matrix.h"
ArduinoLEDMatrix matrix;
//https://docs.arduino.cc/tutorials/uno-r4-wifi/led-matrix


// Arduino pin numbers
const int SW_pin = 8; // digital pin connected to switch output
const int X_pin = 0; // analog pin connected to X output
const int Y_pin = 1; // analog pin connected to Y output
int x = 512;
int y = 512;

// grid dimensions. should not be larger than 8x8
#define MAX_Y 8
#define MAX_X 12
uint8_t grid[MAX_Y][MAX_X] = {
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, 
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, 
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, 
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, 
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
};


void setup() {
  pinMode(SW_pin, INPUT);
  // digitalWrite(SW_pin, HIGH);
  // Serial.begin(9600);
  matrix.begin();
  delay(2000);
}

void loop() {
  grid[y][x] = 0; //clear last cycle pixel
  x = map(analogRead(X_pin), 0, 1023, 0, 11);
  y = map(analogRead(Y_pin), 0, 1023, 0, 7);
  grid[y][x] = 1;//pixel on
  matrix.renderBitmap(grid, 8, 12);

  Mouse.move(x-5,y-3);
  // mouse click not working
  // if (digitalRead(SW_pin) == HIGH) {
  //   Mouse.click();
  // }
  delay(5);
}