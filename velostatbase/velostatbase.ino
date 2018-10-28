//https://learn.adafruit.com/adafruit-neopixel-uberguide/arduino-library-use
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h>
#endif

#define PIN 9
#define NUM_LEDS 147
#define BRIGHTNESS 50
const int WAIT = 90;

Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_LEDS, PIN, NEO_GRB + NEO_KHZ800);
int color[3] = {0, 0, 0};

// Analog input pin that the Velostat is connected to
const int ANALOG_IN_PIN = A10;
int sensorValue = 0;
int pressureThreshold = 65;
float smoothSensorValue = 0;
int count = 0;


void setup() {
  // initialize serial communications at 9600 bps:
  Serial.begin(9600);

  strip.setBrightness(BRIGHTNESS);
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'

  //init Analog Pin as PULLUP
  //(meaning it sends out voltage)
  digitalWrite(ANALOG_IN_PIN, HIGH);

}

void loop() {
  // read the analog in value:
  sensorValue = analogRead(ANALOG_IN_PIN);
  smoothSensorValue = smoothSensorValue * 0.99 + sensorValue * 0.01;
  Serial.println(smoothSensorValue);
  if (sensorValue < pressureThreshold) {
    for (int i = 0; i < NUM_LEDS; ++i) {
      strip.setPixelColor(i, 0, i + 255,  255 - i);
    }
    count = 50;
  }
  count--;
  if (count < 1) {
    for (int i = 0; i < NUM_LEDS; ++i) {
      strip.setPixelColor(i, color[0], color[1],  color[2]);
    }
  }
  strip.show();
  //  delay(WAIT);
}
