/* todo

make the color display a health meter that reduces after each wrong color match
mount hardware on wood?  acrylic?
use wires to fix strip down, screws for the rest, 
add color filters, diffraction gratings above parts of strip
*/

#include <Arduino.h>
#include <RotaryEncoder.h>
#define PIN_IN1 10
#define PIN_IN2 9
#define PIN_ROTARY_BUTTON 8
RotaryEncoder encoder(PIN_IN1, PIN_IN2, RotaryEncoder::LatchMode::TWO03);

#include <Adafruit_NeoPixel.h> //https://learn.adafruit.com/adafruit-neopixel-uberguide/arduino-library-use
#define PIN 11
#define NUMPIXELS 192
Adafruit_NeoPixel strip(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);
#define MAXBRIGHTNESS 100

#define PIN_R_BUTTON 5
#define PIN_G_BUTTON 6
#define PIN_B_BUTTON 7
#define PIXEL_EDGE 9 //where the incoming color wave collides with your input colors
#define ROTARY_RATE 16 //how fast do the colors change when you twist the dial
#define PIXEL_GROW_RATE 0.07 //difficulty / how long do you have to match colors

bool inputMode[] = {0, 0, 0};
float pixelTimer = 0;
int cycle = 0;
float R = 0, G = 0, B = 0;
int R_int = 0, G_int = 0, B_int = 0;
int target_sat = random(0, 255);
unsigned int target_hue = random(0, 65536);
uint32_t target_color = strip.gamma32(strip.ColorHSV(target_hue, target_sat, 255)); //(Hue 0 - 65536), (Sat0-255),(Value 0-255)
byte rgb[3];
double hsv[3];

void setup() {
  // Serial.begin(9600);
  strip.begin(); // INITIALIZE NeoPixel strip object (REQUIRED)
  strip.setBrightness(MAXBRIGHTNESS);
  pinMode(PIN_R_BUTTON, INPUT);
  pinMode(PIN_G_BUTTON, INPUT);
  pinMode(PIN_B_BUTTON, INPUT);
  pinMode(PIN_ROTARY_BUTTON, INPUT_PULLUP);
}

void loop() {
  cycle++;
  pixelTimer += PIXEL_GROW_RATE;
  if (!digitalRead(PIN_ROTARY_BUTTON)) pixelTimer += PIXEL_GROW_RATE*10;
  colorCollision();
  colorInput();
  output_to_strip();
}


void output_to_strip(){
  if (R_int + G_int + B_int > 0) {
    rgb[0] = R_int;
    rgb[1] = G_int;
    rgb[2] = B_int;
  }
  RGBtoHSV(rgb, hsv);
  for (int i = 3; i < PIXEL_EDGE; i++)  strip.setPixelColor(i, strip.Color( R_int, G_int, B_int ));
  strip.setPixelColor(NUMPIXELS - pixelTimer, target_color);
  strip.show();   // Send the updated pixel colors to the hardware.
}
void colorInput() {//check buttons and set color input mode
  if (digitalRead(PIN_R_BUTTON) || digitalRead(PIN_G_BUTTON) || digitalRead(PIN_B_BUTTON)) {
    inputMode[0] = digitalRead(PIN_R_BUTTON);
    inputMode[1] = digitalRead(PIN_G_BUTTON);
    inputMode[2] = digitalRead(PIN_B_BUTTON);
  }

  static int pos = 0;
  encoder.tick();
  int newPos = encoder.getPosition();
  if (pos != newPos) {
    pos = newPos;
    const int direction = -(int)(encoder.getDirection());
    if (inputMode[0]) {
      R += ROTARY_RATE * direction;
      if (R > 255) R = 255;
      if (R < 0) R = 0;
    }
    if (inputMode[1]) {
      G += ROTARY_RATE * direction;
      if (G > 255) G = 255;
      if (G < 0) G = 0;
    }
    if (inputMode[2]) {
      B += ROTARY_RATE * direction;
      if (B > 255) B = 255;
      if (B < 0) B = 0;
    }
  }

  R_int = int(constrain(R, 0, 255));
  G_int = int(constrain(G, 0, 255));
  B_int = int(constrain(B, 0, 255));
  for (int i = 0; i < 3; i++)  strip.setPixelColor(i, 255 * inputMode[0], 255 * inputMode[1], 255 * inputMode[2]);
}
void colorCollision() {
  if (pixelTimer > (NUMPIXELS - PIXEL_EDGE)) {
    const uint32_t color = strip.getPixelColor(PIXEL_EDGE - 1);
    rgb[0] = min(MAXBRIGHTNESS, color >> 16 & 255); // red
    rgb[1] = min(MAXBRIGHTNESS, color >> 8 & 255); // green
    rgb[2] = min(MAXBRIGHTNESS, color & 255); // blue
    RGBtoHSV(rgb, hsv);
    const float taget_hue_360 = float(target_hue) / 65536 * 360; //convert to 360 hue
    const int diff = mod(int(hsv[0] - taget_hue_360 + 180), 360) - 180; //find difference between each color arc hue
    // const int sat = abs(R_int - G_int) + abs(B_int - G_int) + abs(R_int - B_int);

    //lose and reset
    //check if too different from hue(but not too white), too different in saturation, too low in value (too dim)
    if ((abs(diff) > 60 && target_sat > 80) || abs(hsv[1] - target_sat) > 100 || hsv[2] < 15) {
      //death animation
      for (int i = 0; i < PIXEL_EDGE; i++) {
        strip.setPixelColor(PIXEL_EDGE - i-1, 0,0,0);
        strip.show();   // Send the updated pixel colors to the hardware.
        delay(400);
      }
      cycle = 0;
      idleFadeIn();
      idle();
      delay(500);
    } else { //survive and reset
      for (int i = 0; i < NUMPIXELS; i += 3) {
        strip.setPixelColor(i, 0, 0, 0);
        strip.setPixelColor(i + 1, 0, 0, 0);
        strip.setPixelColor(i + 2, 0, 0, 0);
        strip.show();
      }
    }
    //resetting
    strip.clear();
    strip.show();
    R = 0;
    G = 0;
    B = 0;
    target_sat = random(0, 255);
    target_hue = random(0, 65536);
    target_color = strip.gamma32(strip.ColorHSV(target_hue, target_sat, 255)); //(Hue 0 - 65536), (Sat0-255),(Value 0-255)
    pixelTimer = 0;
  }
}

void idleFadeIn() {
  while (1) { //wait until player presses and releases a button
    if (cycle > 255) break;
    if (digitalRead(PIN_R_BUTTON) || digitalRead(PIN_G_BUTTON) || digitalRead(PIN_B_BUTTON)) break;
    for (int i = 0; i < NUMPIXELS; i++) {
      const int a = i + cycle;
      strip.setPixelColor(i, strip.gamma32(strip.ColorHSV(32768+65536*sin(a*0.02),min(255, 255 + 170*sin(cycle*0.01)), cycle))); //180+75*sin(a*0.1)
    }
    cycle++;
    strip.show();   // Send the updated pixel colors to the hardware.
    // delay(10);
  }
}
void idle() {
  while (1) { //wait until player presses and releases a button
    if (digitalRead(PIN_R_BUTTON) || digitalRead(PIN_G_BUTTON) || digitalRead(PIN_B_BUTTON)) break;
    for (int i = 0; i < NUMPIXELS; i++) {
      const int a = i + cycle;
      strip.setPixelColor(i, strip.gamma32(strip.ColorHSV(32768+65536*sin(a*0.02),min(255, 255 + 170*sin(cycle*0.01)), 255))); //180+75*sin(a*0.1)
    }
    cycle++;
    strip.show();   // Send the updated pixel colors to the hardware.
    // delay(10);
  }
}

void RGBtoHSV (byte rgb[], double hsv[]) { //https://piandmore.wordpress.com/2020/08/15/rgb-to-hsv-and-vice-versa/
  byte xMin = rgb[0];
  if (rgb[1] < xMin)   xMin = rgb[1];
  if (rgb[2] < xMin) {
    xMin = rgb[2];
  }   byte xMax = rgb[0];   if (rgb[1] > xMax) {
    xMax = rgb[1];
  }
  if (rgb[2] > xMax)   xMax = rgb[2];
  hsv[2] = xMax;
  byte delta = xMax - xMin;
  if (xMax != 0) {
    hsv[1] = (int)(delta) * 255 / xMax;
  } else {
    hsv[0] = 0;
    hsv[1] = 0;
    return;
  }
  if (rgb[0] == xMax) {
    hsv[0] = (rgb[1] - rgb[2]) * 60 / delta;
  } else if (rgb[1] == xMax) {
    hsv[0] = 120 + (rgb[2] - rgb[0]) * 60 / delta;
  } else {
    hsv[0] = 240 + (rgb[0] - rgb[1]) * 60 / delta;
  }
  if (hsv[0] < 0)   hsv[0] += 360;
}
int mod(int a, int n) { //modulus function that works more like math. (ignores negative)
  return (a % n + n) % n;
}

void log() {
  const uint32_t color = strip.getPixelColor(PIXEL_EDGE - 1);
  rgb[0] = min(MAXBRIGHTNESS, color >> 16 & 255); // red
  rgb[1] = min(MAXBRIGHTNESS, color >> 8 & 255); // green
  rgb[2] = min(MAXBRIGHTNESS, color & 255); // blue
  RGBtoHSV(rgb, hsv);
  const float taget_hue_360 = float(target_hue) / 65536 * 360; //convert to 360 hue
  const int diff = mod(int(hsv[0] - taget_hue_360 + 180), 360) - 180; //find difference between each color arc hue
  const int sat = abs(R_int - G_int) + abs(B_int - G_int) + abs(R_int - B_int);
  Serial.println("---log---");
  Serial.print("sat diff: ");
  Serial.println(abs(hsv[1] - target_sat) / 3);
  Serial.print("hue diff: ");
  Serial.println(abs(diff));
  Serial.print("1=miss, 0=safe: ");
  Serial.println(abs(diff) + abs(hsv[1] - target_sat) / 3 > 100 || hsv[2] < 15);
  Serial.println("...");
}