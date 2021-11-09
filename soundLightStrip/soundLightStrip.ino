/*
   Rui Santos
   Complete Project Details http://randomnerdtutorials.com
*/

//https://learn.adafruit.com/adafruit-neopixel-uberguide/arduino-library-use
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h>
#endif

#define PIN 9
//#define NUM_LEDS 137
#define NUM_LEDS 40
#define BRIGHTNESS 20

int ledPin = 13;
int sensorPin1 = 7; //input pin for microphone
int sensorPin2 = 6; //input pin for microphone
bool val = 0; //input HIGH/LOW from microphone
bool history[NUM_LEDS]; //stores a history of val
int arrayOff = 0; //this keeps track of where on the array the current cycle is
int i = 0; //counter
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_LEDS, PIN, NEO_GRB + NEO_KHZ800);

void setup() {
  pinMode(sensorPin1, INPUT);
  pinMode(sensorPin2, INPUT);
  for (i = 0; i < NUM_LEDS; i++) history[i] = 0;//zero history array

  strip.setBrightness(BRIGHTNESS);
  strip.begin();
  for (i = 0; i < NUM_LEDS; i++) strip.setPixelColor(i, 0, 0, 0);
  strip.show(); // Initialize all pixels to 'off'
}

int width = 32;
uint8_t wait = 30;

void loop() {
  arrayOff++;
  val = digitalRead(sensorPin1)+digitalRead(sensorPin2); //read from microphone
  // Serial.println (val);

  history[arrayOff % NUM_LEDS] = val; //update history at offset location

  for (i = 0; i < NUM_LEDS; i++) {
    if (history[(arrayOff - i) % NUM_LEDS]) {
      strip.setPixelColor(i, 0, 255, 255);
    } else {
      strip.setPixelColor(i, 0, 0, 0);
    }
  }

  strip.show();
  delay(wait);
}
