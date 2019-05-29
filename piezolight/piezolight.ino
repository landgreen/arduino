//https://learn.adafruit.com/adafruit-neopixel-uberguide/arduino-library-use
#include <Adafruit_NeoPixel.h>

const int knockSensor = A0; // the piezo is connected to analog pin 0
int sensorReading = 0;      // variable to store the value read from the sensor pin
int out = 0;

const byte PIN = 9;
const byte NUM_LEDS = 147;
const byte BRIGHTNESS = 18;   //about 25 max with no flicker
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_LEDS, PIN, NEO_GRB + NEO_KHZ800);

void setup() {
  Serial.begin(57600);
  strip.setBrightness(BRIGHTNESS);
  strip.begin();
  for (int i = 0; i < NUM_LEDS; i++) {
    strip.setPixelColor(i, 0, 0, 0);
  }
  strip.show();
}

void loop() {
  sensorReading = analogRead(knockSensor);
  out = sensorReading;
  out = map(sensorReading, 200, 1500, 0, NUM_LEDS);
  out = max(0, min(NUM_LEDS, out));
  Serial.println(sensorReading);

    // set all LEDs to off
    for (int i = 0; i < NUM_LEDS; i++) {
        strip.setPixelColor(i, 0, 0, 0);
    }

    //set some LEDs to on 
    for (int i = 0; i < out; i++) {
        strip.setPixelColor(i, 0, 100, 255);
    }
      strip.show();
  //      delay(1000);
}






