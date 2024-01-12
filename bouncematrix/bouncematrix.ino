#include "Arduino_LED_Matrix.h"
//12x8 LED Matrix
ArduinoLEDMatrix matrix;

int x = 0;
int y = 0;
int Vx = 1;
int Vy = 1;
const int speed = 1;
const int width = 12;
const int height = 8;

byte frame[8][12] = {
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
};

void setup() {
  Serial.begin(115200);
  matrix.begin();
}

void bounce(){
    if (x < 0) {
        x = 0;
        Vx = speed;
    }
    if (x > width) {
        x = 0;
        Vx = -speed;
    }
    if (y < 0) {
        y = 0;
        Vy = speed;
    }
    if (y > height) {
        y = 0;
        Vy = -speed;
    }
}

void loop(){
    // matrix.clear();
    // matrix.setPixel(x, y, true);
    // matrix.show();

    matrix.renderBitmap(frame, 8, 12);
    delay(100);
    x += Vx;
    y += Vy;
    bounce();
}