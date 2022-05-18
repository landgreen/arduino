//todo
// add more interesting death screen
// convert r,g,b into array,  remove duplicate variables


#include <Arduino.h>
#include <RotaryEncoder.h>

#if defined(ARDUINO_AVR_UNO) || defined(ARDUINO_AVR_NANO_EVERY)
// Example for Arduino UNO with input signals on pin 2 and 3
#define PIN_IN1 10
#define PIN_IN2 9

#elif defined(ESP8266)
// Example for ESP8266 NodeMCU with input signals on pin D5 and D6
#define PIN_IN1 D5
#define PIN_IN2 D6

#endif

RotaryEncoder encoder(PIN_IN1, PIN_IN2, RotaryEncoder::LatchMode::TWO03);

//https://learn.adafruit.com/adafruit-neopixel-uberguide/arduino-library-use
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h> // Required for 16 MHz Adafruit Trinket
#endif

#define PIN 11
#define NUMPIXELS 192 // 192 total?

Adafruit_NeoPixel strip(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

#define MAXBRIGHTNESS 50
#define buttonPinR 5
#define buttonPinG 6
#define buttonPinB 7
#define encoderGrowRate 30
#define pixelGrowthRate  0.2

bool isAnyButtonDown = false;
float pixelTimer = 0;
int cycle = 0;
float R = 0, G = 0, B = 0;
int R_int = 0, G_int = 0, B_int = 0;
int isButtonDownR = false, isButtonDownG = false, isButtonDownB = false;
unsigned int target_hue = random(0, 65536);
uint32_t target_color = strip.gamma32(strip.ColorHSV(target_hue)); //(Hue 0 - 65536), (Sat0-255),(Value 0-255)
byte rgb[3];
double hsv[3];
//https://piandmore.wordpress.com/2020/08/15/rgb-to-hsv-and-vice-versa/
void RGBtoHSV (byte rgb[], double hsv[]) {
  byte xMin = rgb[0];
  if (rgb[1] < xMin) {
    xMin = rgb[1];
  }
  if (rgb[2] < xMin) {
    xMin = rgb[2];
  }   byte xMax = rgb[0];   if (rgb[1] > xMax) {
    xMax = rgb[1];
  }
  if (rgb[2] > xMax) {
    xMax = rgb[2];
  }
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
  if (hsv[0] < 0) {
    hsv[0] += 360;
  }
}

int mod(int a, int n) { //modulus function that works more like math. (ignores negative)
  return (a % n + n) % n;
}

void setup() {
  // Serial.begin(115200);
  // while (! Serial);
  // Serial.println("SimplePollRotator example for the RotaryEncoder library.");
  strip.begin(); // INITIALIZE NeoPixel strip object (REQUIRED)
  strip.setBrightness(MAXBRIGHTNESS);
  pinMode(buttonPinR, INPUT);
  pinMode(buttonPinG, INPUT);
  pinMode(buttonPinB, INPUT);
}

void loop() {
  // strip.clear(); // Set all pixel colors to 'off'
  // for (int i = 0; i < NUMPIXELS; i++) {
  //   target_color = strip.gamma32(strip.ColorHSV(random(0,65536)));
  //   strip.setPixelColor(i, target_color);
  // }
  //   strip.show();   // Send the updated pixel colors to the hardware.

  cycle++;
  pixelTimer += pixelGrowthRate;
  //check for color collision
  if (pixelTimer > (NUMPIXELS - 16)) {
    rgb[0] = min(MAXBRIGHTNESS, strip.getPixelColor(0) >> 16 & 255); // red
    rgb[1] = min(MAXBRIGHTNESS, strip.getPixelColor(5) >> 8 & 255); // green
    rgb[2] = min(MAXBRIGHTNESS, strip.getPixelColor(10) & 255); // blue
    RGBtoHSV(rgb, hsv);
    const float taget_hue_360 = float(target_hue) / 65536 * 360; //convert to 360 hue
    const int diff = mod(int(hsv[0] - taget_hue_360 + 180), 360) - 180; //find difference between each color arc hue
    const int sat = abs(R_int - G_int) + abs(B_int - G_int) + abs(R_int - B_int);
    Serial.println("r+g+b,diff,sat");
    Serial.println(R_int + G_int + B_int);
    Serial.println(diff);
    Serial.println(sat);
    //check if too bright,  too white,too different from target
    if ((R_int + G_int + B_int > 700) || abs(diff) > 50 || sat < 60) { //lose and reset
      for (int i = 0; i < 16; i++) {
        // strip.setPixelColor(random(0,NUMPIXELS-1), 0,0,0);//random place on strip goes white
        target_hue += 1000;
        target_color = strip.gamma32(strip.ColorHSV(target_hue)); //(Hue 0 - 65536), (Sat0-255),(Value 0-255)
        strip.setPixelColor(16 - i, target_color);
        strip.show();   // Send the updated pixel colors to the hardware.
        delay(200);
      }
      delay(1000);
      //wait until player presses and releases a button
      while (1) {
        if (digitalRead(buttonPinR) || digitalRead(buttonPinG) || digitalRead(buttonPinB)) {
          break;
        //   isAnyButtonDown = true;
        // } else if (isAnyButtonDown) {
        }
      }
      delay(500);
    } else { //survive and reset
      for (int i = 0; i < NUMPIXELS; i += 3) {
        strip.setPixelColor(i, 0, 0, 0);
        strip.setPixelColor(i + 1, 0, 0, 0);
        strip.setPixelColor(i + 2, 0, 0, 0);
        strip.show();
      }
    }

    strip.clear(); // Set all pixel colors to 'off'
    strip.show();

    //resetting
    R = 0;
    G = 0;
    B = 0;
    target_hue = random(0, 65536);
    target_color = strip.gamma32(strip.ColorHSV(target_hue)); //(Hue 0 - 65536), (Sat0-255),(Value 0-255)
    pixelTimer = 0;
  }

  //check buttons
  // if (digitalRead(buttonPinR)) {
  //   if (isButtonDownR) {
  //     R += encoderGrowRate;
  //   } else {
  //     R = -16;
  //   }
  //   isButtonDownR = true;
  // } else {
  //   isButtonDownR = false;
  // }
  // if (digitalRead(buttonPinG)) {
  //   if (isButtonDownG) {
  //     G += encoderGrowRate;
  //   } else {
  //     G = -16;
  //   }
  //   isButtonDownG = true;
  // } else {
  //   isButtonDownG = false;
  // }
  // if (digitalRead(buttonPinB)) {
  //   if (isButtonDownB) {
  //     B += encoderGrowRate;
  //   } else {
  //     B = -16;
  //   }
  //   isButtonDownB = true;
  // } else {
  //   isButtonDownB = false;
  // }

  static int pos = 0;
  encoder.tick();
  int newPos = encoder.getPosition();
  if (pos != newPos) {
    pos = newPos;
    const int direction = -(int)(encoder.getDirection());
    // Serial.print("pos:");
    // Serial.print(newPos);
    // Serial.print(" dir:");
    // Serial.println((int)(encoder.getDirection()));
    if (digitalRead(buttonPinR)) {
      R += 255/4 * direction;
      if (R>255) R = 255;
      if (R<0) R = 0;
    }
    if (digitalRead(buttonPinG)) {
      G += 255/4 * direction;
      if (G>255) G = 255;
      if (G<0) G = 0;
    }
    if (digitalRead(buttonPinB)) {
      B += 255/4 * direction;
      if (B>255) B = 255;
      if (B<0) B = 0;
    }
  }


  

  R_int = int(constrain(R, 0, 255));
  G_int = int(constrain(G, 0, 255));
  B_int = int(constrain(B, 0, 255));

  // if(R_int>0)  R_map+=1;
  // if(G_int>0)  G_map+=1;
  // if(B_int>0)  B_map+=1;
  //clear
  for (int i = 0; i < 16; i++)  strip.setPixelColor(i, 0, 0, 0);
  // draw    //map(value, fromLow, fromHigh, toLow, toHigh)
  for (int i = 0; i < map(R_int, 0, 255, R_int > 0, 4); i++) strip.setPixelColor(i, strip.gamma32(strip.ColorHSV(0))); //(Hue 0 - 65536), (Sat0-255),(Value 0-255)
  for (int i = 4; i < 4 + map(G_int, 0, 255, G_int > 0, 4); i++) strip.setPixelColor(i, strip.gamma32(strip.ColorHSV(22574))); //(Hue 0 - 65536), (Sat0-255),(Value 0-255)
  for (int i = 8; i < 8 + map(B_int, 0, 255, B_int > 0, 4); i++) strip.setPixelColor(i, strip.gamma32(strip.ColorHSV(46421))); //(Hue 0 - 65536), (Sat0-255),(Value 0-255)
  if (R_int + G_int + B_int > 0)
  rgb[0] = R_int;
  rgb[1] = G_int;
  rgb[2] = B_int;
  RGBtoHSV(rgb, hsv);
  // for (int i = 12; i < 16; i++)  strip.setPixelColor(i, strip.gamma32(strip.ColorHSV(hsv[0]*182)));// /360*65536

  for (int i = 12; i < 16; i++)  strip.setPixelColor(i, strip.Color( R_int, G_int, B_int ));


  //RGB indicator pixels at the head of the strip
  // if (isButtonDownR & (R_int == 255) & (cycle % 20 > 10)) {
  //   for (int i = 0; i < 5; i++) strip.setPixelColor(i, strip.Color( 0, 0, 0 ));
  // } else {
  //   for (int i = 0; i < 5; i++) strip.setPixelColor(i, strip.gamma32(strip.ColorHSV(0, 255, R_int))); //(Hue 0 - 65536), (Sat0-255),(Value 0-255)
  // }
  // if (isButtonDownG & (G_int == 255) & (cycle % 20 > 10)) {
  //   for (int i = 5; i < 10; i++) strip.setPixelColor(i, strip.Color( 0, 0, 0 ));
  // } else {
  //   for (int i = 5; i < 10; i++) strip.setPixelColor(i, strip.gamma32(strip.ColorHSV(22574, 255, G_int)));
  // }
  // if (isButtonDownB & (B_int == 255) & (cycle % 20 > 10)) {
  //   for (int i = 10; i < 15; i++) strip.setPixelColor(i, strip.Color( 0, 0, 0 ));
  // } else {
  //   for (int i = 10; i < 15; i++) strip.setPixelColor(i, strip.gamma32(strip.ColorHSV(46421, 255, B_int)));
  // }
  // for (int i = 15; i < 20; i++)  strip.setPixelColor(i, strip.Color( R_int, G_int, B_int ));

  strip.setPixelColor(NUMPIXELS - pixelTimer, target_color);
  strip.show();   // Send the updated pixel colors to the hardware.
}
