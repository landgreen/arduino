#include <Arduino.h>
#include <RotaryEncoder.h>
#include <Adafruit_NeoPixel.h>

// --- HARDWARE CONFIGURATION ---
#define PIN_STRIP 11
#define PIN_ROTARY_IN1 10
#define PIN_ROTARY_IN2 9
#define PIN_ROTARY_BTN 8
#define PIN_R_BTN 5
#define PIN_G_BTN 6
#define PIN_B_BTN 7

#define NUMPIXELS 192

// --- GAME SETTINGS ---
#define PIXEL_EDGE 12           // Hit box size
#define ROTARY_RATE 16          // Color change speed
#define PIXEL_GROW_RATE 0.07    // Game speed
#define MATCH_THRESHOLD 15000   // Difficulty (Lower = Harder)
#define MAX_POWER_MA 800        // Power Limit (mA)

// --- OBJECTS ---
// LatchMode::TWO03 is usually best, but if rotary is skipping, try FOUR3
RotaryEncoder encoder(PIN_ROTARY_IN1, PIN_ROTARY_IN2, RotaryEncoder::LatchMode::TWO03);
Adafruit_NeoPixel strip(NUMPIXELS, PIN_STRIP, NEO_GRB + NEO_KHZ800);

// --- VARIABLES ---
float inputR = 0, inputG = 0, inputB = 0;

// Target
uint8_t targetR, targetG, targetB;
uint32_t targetColor32;
uint32_t targetTailColors[3];

// Game State
float pixelTimer = 0;
int cycle = 0;
bool inputActive[] = {0, 0, 0};
int consecutiveWins = 0;

// Difficulty Scaling
float currentSpeed = PIXEL_GROW_RATE;

// --- HELPER PROTOTYPES ---
uint32_t applyDimming(uint32_t color, float factor);
void generateNewTarget();
void handleInputs();
void checkCollisions();
void renderStrip();
void playDeathAnimation();
void blinkZone(int r, int g, int b);

void setup() {
  Serial.begin(9600);
  strip.begin();
  strip.setBrightness(255);
  
  pinMode(PIN_R_BTN, INPUT);
  pinMode(PIN_G_BTN, INPUT);
  pinMode(PIN_B_BTN, INPUT);
  pinMode(PIN_ROTARY_BTN, INPUT_PULLUP);
  
  randomSeed(analogRead(0));
  generateNewTarget();
}

void loop() {
  cycle++;
  
  // 1. Game Speed (with progressive difficulty)
  float speed = currentSpeed;
  if (!digitalRead(PIN_ROTARY_BTN)) speed *= 10.0;
  pixelTimer += speed;

  // 2. Logic
  handleInputs();
  checkCollisions();

  // 3. Render
  renderStrip();
}

void handleInputs() {
  inputActive[0] = digitalRead(PIN_R_BTN);
  inputActive[1] = digitalRead(PIN_G_BTN);
  inputActive[2] = digitalRead(PIN_B_BTN);

  encoder.tick();
  int direction = (int)(encoder.getDirection());

  if (direction != 0) {
    // Invert the direction so clockwise increases values
    int change = -direction * ROTARY_RATE;
    if (inputActive[0]) inputR = constrain(inputR + change, 0, 255);
    if (inputActive[1]) inputG = constrain(inputG + change, 0, 255);
    if (inputActive[2]) inputB = constrain(inputB + change, 0, 255);
  }
}

void checkCollisions() {
  if (pixelTimer > (NUMPIXELS - PIXEL_EDGE)) {
    
    // Squared Euclidean Distance
    long dR = (long)inputR - targetR;
    long dG = (long)inputG - targetG;
    long dB = (long)inputB - targetB;
    long totalDistance = (dR * dR) + (dG * dG) + (dB * dB);
    
    Serial.print("Match Distance: "); 
    Serial.print(totalDistance);
    Serial.print(" | Consecutive Wins: ");
    Serial.println(consecutiveWins);

    if (totalDistance <= MATCH_THRESHOLD) {
       // WIN
       consecutiveWins++;
       blinkZone(0, 255, 0); // Green flash
       
       // Progressive difficulty - speed up every 3 wins
       if (consecutiveWins % 3 == 0 && consecutiveWins > 0) {
         currentSpeed = min(currentSpeed * 1.15, PIXEL_GROW_RATE * 3.0); // Cap at 3x speed
         Serial.print("Speed increased! Current multiplier: ");
         Serial.println(currentSpeed / PIXEL_GROW_RATE);
       }
       
    } else {
       // LOSE
       consecutiveWins = 0; // Reset streak
       currentSpeed = PIXEL_GROW_RATE; // Reset speed
       blinkZone(255, 0, 0); // Red flash
       playDeathAnimation();
       inputR = 0; inputG = 0; inputB = 0; // Reset player inputs
    }

    generateNewTarget();
    pixelTimer = 0;
  }
}

void generateNewTarget() {
  uint16_t h = random(0, 65536);
  uint8_t s = random(150, 255); // High saturation for distinct colors
  uint8_t v = 255;
  
  targetColor32 = strip.gamma32(strip.ColorHSV(h, s, v));
  
  // Save RGB components for comparison
  targetR = (uint8_t)(targetColor32 >> 16);
  targetG = (uint8_t)(targetColor32 >> 8);
  targetB = (uint8_t)targetColor32;

  targetTailColors[0] = strip.gamma32(strip.ColorHSV(h, s, 150));
  targetTailColors[1] = strip.gamma32(strip.ColorHSV(h, s, 100));
  targetTailColors[2] = strip.gamma32(strip.ColorHSV(h, s, 50));
}

// --- RENDERING ---

void renderStrip() {
  strip.clear();

  // --- POWER CALCULATION START ---
  float playerSum = (inputR + inputG + inputB) * PIXEL_EDGE;
  float targetSum = (float)(targetR + targetG + targetB);
  const int TAIL_LEN = 20;
  float snakeSum = targetSum * (1 + TAIL_LEN); 
  float totalColorUnits = playerSum + snakeSum;
  float estimatedMA = totalColorUnits * 0.08;

  float powerScale = 1.0;
  if (estimatedMA > MAX_POWER_MA) {
    powerScale = (float)MAX_POWER_MA / estimatedMA;
  }
  // --- POWER CALCULATION END ---

  // 1. Draw Player Zone (Apply Scaling)
  uint32_t playerColor = strip.Color(
    (uint8_t)(inputR * powerScale), 
    (uint8_t)(inputG * powerScale), 
    (uint8_t)(inputB * powerScale)
  );
  strip.fill(playerColor, 0, PIXEL_EDGE);
  
  // Show button state on first 3 pixels
  uint32_t buttonColor = strip.Color(
    inputActive[0] ? 255 : 0,  // Red button
    inputActive[1] ? 255 : 0,  // Green button
    inputActive[2] ? 255 : 0   // Blue button
  );
  strip.fill(buttonColor, 0, 3);

  // 2. Draw Enemy Snake (Apply Scaling)
  int headPos = NUMPIXELS - (int)pixelTimer;

  // Pre-calculate scaled snake colors
  uint32_t scaledTarget = applyDimming(targetColor32, powerScale);
  uint32_t scaledTail[3];
  for(int i=0; i<3; i++) scaledTail[i] = applyDimming(targetTailColors[i], powerScale);

  // Draw Head & Body
  for(int i = 0; i < TAIL_LEN; i++) {
     int p = headPos + i;
     if(p < NUMPIXELS && p >= 0) {
       strip.setPixelColor(p, scaledTarget);
     }
  }

  // Draw Tail Fade
  for(int i = 0; i < 3; i++) {
    int p = headPos + TAIL_LEN + i;
    if (p < NUMPIXELS && p >= 0) {
      strip.setPixelColor(p, scaledTail[i]);
    }
  }

  strip.show();
}

// Helper to dim a packed color without shifting hue
uint32_t applyDimming(uint32_t color, float factor) {
  if (factor >= 1.0) return color;
  uint8_t r = (uint8_t)(color >> 16);
  uint8_t g = (uint8_t)(color >> 8);
  uint8_t b = (uint8_t)color;
  return strip.Color((uint8_t)(r * factor), (uint8_t)(g * factor), (uint8_t)(b * factor));
}

// --- ANIMATIONS ---

void blinkZone(int r, int g, int b) {
  for(int k=0; k<3; k++) {
    strip.fill(strip.Color(r,g,b), 0, PIXEL_EDGE);
    strip.show();
    delay(50);
    strip.fill(0, 0, PIXEL_EDGE);
    strip.show();
    delay(50);
  }
}

void playDeathAnimation() {
   // 1. Red Explosion from player zone
   for (int i = 0; i < PIXEL_EDGE + 10; i++) {
      strip.setPixelColor(i, 255, 0, 0); 
      strip.show();
      delay(20);
   }
   
   // 2. Wait a moment
   delay(500);

   // 3. Screensaver Loop
   long hueOffset = 0;
   
   while(true) {
     // Check if ANY button is pressed to exit
     if(digitalRead(PIN_R_BTN) || digitalRead(PIN_G_BTN) || digitalRead(PIN_B_BTN)) {
       delay(200); 
       while(digitalRead(PIN_R_BTN) || digitalRead(PIN_G_BTN) || digitalRead(PIN_B_BTN)); 
       strip.clear();
       return; 
     }

     // Draw Rainbow
     hueOffset += 256; 
     
     for (int i = 0; i < NUMPIXELS; i++) {
        uint16_t pixelHue = hueOffset + (i * 65536L / 64);
        strip.setPixelColor(i, strip.gamma32(strip.ColorHSV(pixelHue, 255, 100)));
     }
     strip.show();
     delay(10);
   }
}