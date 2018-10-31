#include <Adafruit_NeoPixel.h>

const int analogInPin = A11;  // Analog input pin that the potentiometer is attached to
const int NUM_LEDS = 30;
const int PIN  = 9;
const int WAIT = 10;
const int SENSITIVITY = 160;  // smaller means less lights,  bigger means more lights
const int LED_OFFSET = 56;  // adjust this to remove LEDs that stay lit up after chaning sensitivity

Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_LEDS, PIN, NEO_GRB + NEO_KHZ800);

int sensorValue = 0;   // value read from the pot
float stripLength = 0;
int color[NUM_LEDS][3];

void setup() {
  // initialize serial communications at 9600 bps:
  Serial.begin(9600);
  digitalWrite(analogInPin, HIGH);
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'


  // set up colors array
  for (uint16_t i = 0; i < NUM_LEDS; i++) {
    color[i][0] = 255;
    color[i][1] = 0;
    color[i][2] = 0;
  }
}

void fireColors() {
  for (uint16_t i = 0; i < NUM_LEDS; i++) {

    // yellow spark
    if (random(400) == 0 ) {
      color[i][1] = 125;
    } else {
      //green fade out
      if (color[i][1] > 1) {
        color[i][1] -= 2;
      } else {
        color[i][1] = 0;
      }
    }
  }
}

void loop() {
  // read the analog in value:
  sensorValue = analogRead(analogInPin);
  // print the results to the serial monitor:
   Serial.print("sensor = " );
   Serial.println(sensorValue);

  // smoothing
  stripLength = stripLength * 0.9  + (SENSITIVITY * NUM_LEDS / sensorValue - LED_OFFSET) * 0.1;
  //  Serial.print("striplength = " );
  //  Serial.println(stripLength);
  //  stripLength = NUM_LEDS;//light up full strip for testing

  fireColors();
  for (uint16_t i = 0; i < stripLength; i++) {
    // lava colors
    // strip.setPixelColor(i, 200+random(55), random(120),random(20));
    strip.setPixelColor(i, color[i][0], color[i][1], color[i][2]);
  }
  for (uint16_t i = stripLength; i < NUM_LEDS; i++) {
    strip.setPixelColor(i, 0, 0, 0);
  }
  strip.show();
  delay(WAIT);


  // lights roll in and out when over threshold
  //  if (sensorValue < 60) {
  //    for (uint16_t i = 0; i < NUM_LEDS; i++) {
  //      //add red to pixel
  //      strip.setPixelColor(i, 255, 0, 0);
  //      strip.show();
  //      delay(WAIT);
  //    }
  //    for (uint16_t i = 0; i < NUM_LEDS; i++) {
  //      //add red to pixel
  //      strip.setPixelColor(i, 0, 0, 0);
  //      strip.show();
  //      delay(WAIT);
  //    }
  //  }
}
