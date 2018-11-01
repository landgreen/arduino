#include <Adafruit_NeoPixel.h>

const int analogInPin = A11;  // A11 is called #12 on the Flora
const int NUM_LEDS = 24; //30;
const int PIN  = 9;
const int WAIT = 10;
// adjust SENSITIVITY first, then adjust LED_OFFSET until no LEDs light up with no pressure
const int SENSITIVITY = 160;  // smaller means less lights,  bigger means more lights
const int LED_OFFSET = 56;  // adjust this to remove LEDs that stay lit up after changing sensitivity

Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_LEDS, PIN, NEO_GRB + NEO_KHZ800);

int sensorValue = 0;
float stripLength = 0;
int color[NUM_LEDS][3];

void setup() {
  // Serial.begin(9600);
  digitalWrite(analogInPin, HIGH);
  strip.begin();
  strip.show();

  // set up colors array as red
  for (uint16_t i = 0; i < NUM_LEDS; i++) {
    color[i][0] = 255;
    color[i][1] = 0;
    color[i][2] = 0;
  }
}

void loop() {
  // read the analog in value:
  sensorValue = analogRead(analogInPin);
  // Serial.print("sensor = " );
  // Serial.println(sensorValue);

  // smooth the sensorValue and scale to number of LEDs
  stripLength = stripLength * 0.9  + (SENSITIVITY * NUM_LEDS / sensorValue - LED_OFFSET) * 0.1;
  //  stripLength = NUM_LEDS;//light up full strip for testing

  // green spark
  for (uint16_t i = 0; i < NUM_LEDS; i++) {
    if (random(400) == 0 ) {
      color[i][1] = 125;
    } else if (color[i][1] > 1) {
      //green fade out
      color[i][1] -= 2;
    } else {
      color[i][1] = 0;
    }
  }
  //light up LEDs up to stripLength
  for (uint16_t i = 0; i < stripLength; i++) {
    strip.setPixelColor(i, color[i][0], color[i][1], color[i][2]);
  }
  //turn off the rest of the LEDs
  for (uint16_t i = stripLength; i < NUM_LEDS; i++) {
    strip.setPixelColor(i, 0, 0, 0);
  }
  strip.show();
  delay(WAIT);
}
