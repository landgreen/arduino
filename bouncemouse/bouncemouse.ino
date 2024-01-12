#include "Arduino_LED_Matrix.h"
#include <Mouse.h>

ArduinoLEDMatrix matrix;

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

int x = 5;
int y = 0;
int Vx = 1;
int Vy = 1;

void bounce(){
    if (x < 0) {
        x = 1;
        Vx = 1;
    }
    if (x > MAX_X-2) {
        x = MAX_X-1;
        Vx = -1;
    }
    if (y < 0) {
        y = 1;
        Vy = 1;
    }
    if (y > MAX_Y-2) {
        y = MAX_Y-1;
        Vy = -1;
    }
}

void setup() {
  Mouse.begin();
  matrix.begin();
  delay(1000);
}

void loop() {
  //clear
  grid[y][x] = 0;
  //move
  Mouse.move(Vx,Vy);

  x += Vx;
  y += Vy;
  bounce();
  grid[y][x] = 1;
  matrix.renderBitmap(grid, 8, 12);
  delay(100);
}
