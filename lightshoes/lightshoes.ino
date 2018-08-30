#include <Adafruit_NeoPixel.h>

const int analogInPin = A0;  // Analog input pin that the potentiometer is attached to
const int NUM_LEDS = 147;
const int PIN  = 9;
const int WAIT = 0;//5;

Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_LEDS, PIN, NEO_GRB + NEO_KHZ800);

int sensorValue = 0;   // value read from the pot

void setup() {
  // initialize serial communications at 9600 bps:
  Serial.begin(9600);
  digitalWrite(analogInPin, HIGH);
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
}


// array that keeps track of colors
int colors[NUM_LEDS][3];

void loop() {
  // read the analog in value:
  sensorValue = analogRead(analogInPin);
  // print the results to the serial monitor:
  Serial.print("sensor = " );
  Serial.println(sensorValue);

  if (sensorValue < 60) {
    for (uint16_t i = 0; i < NUM_LEDS; i++) {
      //add red to pixel
      colors[i][0] = 255;
      //color decay
      for (uint16_t i = 0; i < NUM_LEDS; i++) {
        if (colors[i][0] > 0) colors[i][0]--;
        if (colors[i][1] > 0) colors[i][1]--;
        if (colors[i][2] > 0) colors[i][2]--;
        strip.setPixelColor(i, colors[i][0], colors[i][1], colors[i][2]);
      }
      strip.show();
      delay(WAIT);
    }
  } else {
    //just color decay
    for (uint16_t i = 0; i < NUM_LEDS; i++) {
      for (uint16_t i = 0; i < NUM_LEDS; i++) {
        if (colors[i][0] > 0) colors[i][0]--;
        if (colors[i][1] > 0) colors[i][1]--;
        if (colors[i][2] > 0) colors[i][2]--;
      }
      strip.setPixelColor(i, colors[i][0], colors[i][1], colors[i][2]);
      strip.show();
      delay(WAIT);
    }
  }
}
