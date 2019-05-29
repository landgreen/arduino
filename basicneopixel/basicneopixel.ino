//https://learn.adafruit.com/adafruit-neopixel-uberguide/arduino-library-use
#include <Adafruit_NeoPixel.h>


const byte PIN = 9;
const byte NUM_LEDS = 147;
const byte BRIGHTNESS = 18;   //about 25 max with no flicker
bool toggle = 0;
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_LEDS, PIN, NEO_GRB + NEO_KHZ800);

void setup() {
  Serial.begin(9600);
  strip.setBrightness(BRIGHTNESS);
  strip.begin();
  //  for (int i = 0; i < NUM_LEDS; i++) {
  //      strip.setPixelColor(i, 255, 200, 255);
  //  }
}

void loop() {
  strip.show();
  delay(1000);
  if (toggle) {
    toggle = false;

  for (int i = 0; i < NUM_LEDS; i++) {
    if ((i / 3) % 2) {
      strip.setPixelColor(i, 0, 100, 255);
    } else {
      strip.setPixelColor(i, 0, 255, 100);
    }
  }
    
  } else {
    toggle = true;

      for (int i = 0; i < NUM_LEDS; i++) {
    if ((i / 3) % 2) {
      strip.setPixelColor(i, 0, 255, 100);
    } else {
      strip.setPixelColor(i, 0, 100, 255);
    }
  }
  }
}






