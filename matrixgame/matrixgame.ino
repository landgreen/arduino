#include <Adafruit_NeoPixel.h>

const int WAIT = 20;         // how many cycles to look for player joystick input
const int LEDPIN = 6;         // pin for LEDS
const int SWITCH_PIN_1 = 2;   // pin for P1 switch
const int SWITCH_PIN_2 = 4;   // pin for P2 switch
const int NUM_LEDS = 256;     // 16x16 = 256
const int BRIGHTNESS = 20;     // 0-100 LED brightness

// Parameter 1 = number of pixels in strip
// Parameter 2 = pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_LEDS, LEDPIN, NEO_GRB + NEO_KHZ800);

void setup() {
  Serial.begin(9600);
  //activate pull-up resistor on the push-button pin
  pinMode(SWITCH_PIN_1, INPUT_PULLUP);
  pinMode(SWITCH_PIN_2, INPUT_PULLUP);
  strip.setBrightness(BRIGHTNESS);
  strip.begin();
  // clear all pixels
  for (int i = 0; i < NUM_LEDS; i++) {
    strip.setPixelColor(i, 0, 0, 0);
  }
  strip.show();
}

// player 1
int p1_position[2] = {5, 5};  //player location x,y
int p1_velocity[2] = {0, 0};  //player velocity x,y
int p1_input[3] = {0, 0, 0};  //player key presses x,y,button down
int p1_color[3] = {0, 150, 200};
//int p1_bullets[4][2] = {{2, 5}, {5, 7}, {3, 7}, {5, 12}};

// player 2
int p2_position[2] = {11, 11};  //player location x,y
int p2_velocity[2] = {0, 0};  //player velocity x,y
int p2_input[3] = {0, 0, 0};  //player key presses x,y,button down
int p2_color[3] = {255, 50, 0};

void loop() {
  playerMove();
  // clear all
  for (int i = 0; i < NUM_LEDS; i++) {
    strip.setPixelColor(i, 0, 0, 0);
  }
  // draw player 1
  drawPlayer(p1_position[0], p1_position[1], p1_color);
  // draw player 2
  drawPlayer(p2_position[0], p2_position[1], p2_color);

  strip.show();
  delay(WAIT);
}


//void drawBullet() {
//  for (int i = 0; i < 4; ++i) {
//    ZigZagPixel(p1_bullets[i][0], p1_bullets[i][1], 255, 0, 0);
//  }
//}

void drawPlayer(int x, int y, int color[3]) {
  ZigZagPixel(x + 1, y + 1, color[0], color[1], color[2]);
  ZigZagPixel(x, y + 1, color[0], color[1], color[2]);
  ZigZagPixel(x - 1, y + 1, color[0], color[1], color[2]);
  ZigZagPixel(x + 1, y, color[0], color[1], color[2]);
  ZigZagPixel(x, y, color[0], color[1], color[2]);
  ZigZagPixel(x - 1, y, color[0], color[1], color[2]);
  ZigZagPixel(x + 1, y - 1, color[0], color[1], color[2]);
  ZigZagPixel(x, y - 1, color[0], color[1], color[2]);
  ZigZagPixel(x - 1, y - 1, color[0], color[1], color[2]);
}

void playerMove() {

  // player 1
  // read input from game controllers
  p1_input[0] = map(analogRead(0), 0, 1023, -3, 4);
  p1_input[1] = map(analogRead(1), 0, 1023, -3, 4);

  // x velocity
  if (p1_input[0] > 1) {
    p1_velocity[0] = 1;
  } else if (p1_input[0] < 0) {
    p1_velocity[0] = -1;
  } else {
    p1_velocity[0] = 0;
  }

  // y velocity
  if (p1_input[1] > 1) {
    p1_velocity[1] = 1;
  } else if (p1_input[1] < 0) {
    p1_velocity[1] = -1;
  } else {
    p1_velocity[1] = 0;
  }

  // move
  p1_position[0] += p1_velocity[0];
  p1_position[1] += p1_velocity[1];

  // wall check
  if (p1_position[0] < 2) {
    p1_position[0] = 1;
  } else if (p1_position[0] > 14) {
    p1_position[0] = 14;
  }
  if (p1_position[1] < 2) {
    p1_position[1] = 1;
  } else if (p1_position[1] > 14) {
    p1_position[1] = 14;
  }

  // player 2
  // read input from game controllers
  p2_input[0] = map(analogRead(2), 0, 1023, -3, 4);
  p2_input[1] = map(analogRead(3), 0, 1023, -3, 4);

  // x velocity
  if (p2_input[0] > 1) {
    p2_velocity[0] = 1;
  } else if (p2_input[0] < 0) {
    p2_velocity[0] = -1;
  } else {
    p2_velocity[0] = 0;
  }

  // y velocity
  if (p2_input[1] > 1) {
    p2_velocity[1] = 1;
  } else if (p2_input[1] < 0) {
    p2_velocity[1] = -1;
  } else {
    p2_velocity[1] = 0;
  }

  // move
  p2_position[0] += p2_velocity[0];
  p2_position[1] += p2_velocity[1];

  // wall check
  if (p2_position[0] < 2) {
    p2_position[0] = 1;
  } else if (p2_position[0] > 14) {
    p2_position[0] = 14;
  }
  if (p2_position[1] < 2) {
    p2_position[1] = 1;
  } else if (p2_position[1] > 14) {
    p2_position[1] = 14;
  }
}





void ZigZagPixel(int x, int y, int r, int g, int b) {
  if (y % 2) {
    strip.setPixelColor(y * 16 + x, r, g,  b);
  } else {
    strip.setPixelColor(y * 16 + 15 - x, r, g,  b);
  }
}

void getZigZagPixel(int x, int y) {
  if (y % 2) {
    Serial.println(strip.getPixelColor(y * 16 + x) );
  } else {
    Serial.println(strip.getPixelColor(y * 16 + 15 - x));
  }
}

void(* resetFunc) (void) = 0; //declare reset function @ address 0
void endGame() {
  delay(5000);
  resetFunc();  //call reset
}

