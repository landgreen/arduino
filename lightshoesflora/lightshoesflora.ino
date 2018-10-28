#include <Adafruit_NeoPixel.h>

const int analogInPin = A11;  // Analog input pin that the potentiometer is attached to
const int NUM_LEDS = 23;
const int PIN  = 9;
const int WAIT = 10;//5;
const int THRESHOLD = 65;

Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_LEDS, PIN, NEO_GRB + NEO_KHZ800);

int sensorValue = 0;   // value read from the pot
float stripLength = 0;

void setup() {
  // initialize serial communications at 9600 bps:
  Serial.begin(9600);
  digitalWrite(analogInPin, HIGH);
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
}

void loop() {
  // read the analog in value:
  sensorValue = analogRead(analogInPin);
  // print the results to the serial monitor:
  Serial.print("sensor = " );
  Serial.println(sensorValue);
  // smoothing
  stripLength = stripLength * 0.9  + (THRESHOLD * NUM_LEDS / sensorValue - 11) * 0.1;
  // stripLength =  40* NUM_LEDS / sensorValue-11;
  Serial.print("striplength = " );
  Serial.println(stripLength);
  for (uint16_t i = 0; i < stripLength; i++) {
    strip.setPixelColor(i, 0, 25, 255);
  }
  for (uint16_t i = stripLength; i < NUM_LEDS; i++) {
    strip.setPixelColor(i, 0, 0, 0);
  }
  strip.show();
  delay(WAIT);

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
