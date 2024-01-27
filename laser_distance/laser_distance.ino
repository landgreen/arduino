#include <Adafruit_VL53L1X.h>
#include <Wire.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width,  in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels

// Declare an SSD1306 display object connected to I2C
Adafruit_SSD1306 oled(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

#define IRQ_PIN 2
#define XSHUT_PIN 3
#define LED_PIN 13 // Change this to the pin where your LED is connected


Adafruit_VL53L1X vl53 = Adafruit_VL53L1X(XSHUT_PIN, IRQ_PIN);

void setup() {
  pinMode(LED_PIN, OUTPUT); // Set LED pin as OUTPUT

 
  Serial.begin(115200);
  Wire.begin();

  if (!vl53.begin(0x29, &Wire)) {
    Serial.print(F("Error on init of VL sensor: "));
    Serial.println(vl53.vl_status);
    while (1) delay(10);
  }

  Serial.println(F("VL53L1X sensor OK!"));

  Serial.print(F("Sensor ID: 0x"));
  Serial.println(vl53.sensorID(), HEX);

  if (!vl53.startRanging()) {
    Serial.print(F("Couldn't start ranging: "));
    Serial.println(vl53.vl_status);
    while (1) delay(10);
  }

  Serial.println(F("Ranging started"));

  // Valid timing budgets: 15, 20, 33, 50, 100, 200, and 500ms!
  vl53.setTimingBudget(50);
  Serial.print(F("Timing budget (ms): "));
  Serial.println(vl53.getTimingBudget());

  // Initialize OLED display with address 0x3C for 128x64
  if (!oled.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    while (true);
  }

  delay(2000);         // Wait for initializing
  oled.clearDisplay(); // Clear display

  oled.setTextSize(1);          // Text size
  oled.setTextColor(WHITE);     // Text color
  oled.setCursor(0, 10);        // Position to display
  oled.println("Board initialized"); // Text to display
  oled.display();               // Show on OLED
}

void loop() {
  int16_t distance;
  if (vl53.dataReady()) {
    // New measurement for the taking!
    distance = vl53.distance();
    if (distance == -1 || distance > 3200) {
      // Something went wrong!
      Serial.print(F("Couldn't get distance: "));
      Serial.println(vl53.vl_status);
     
      oled.clearDisplay();
      oled.setTextSize(1);
      oled.setTextColor(WHITE);
      oled.setCursor(0, 10);
      oled.print("OUT OF SENSOR RANGE (");
      oled.print(distance);
      oled.println("mm)");
      oled.display();
   
      return;
    }

    // Display distance on OLED
    oled.clearDisplay();
    oled.setTextSize(1);
    oled.setTextColor(WHITE);
    oled.setCursor(0, 10);
    oled.print("Distance: ");
    oled.print(distance);
    oled.println(" mm");
    oled.display();
   
    // Control LED based on distance
    if (distance < 100) {
      digitalWrite(LED_PIN, HIGH); // Turn on LED
    } else {
      digitalWrite(LED_PIN, LOW);  // Turn off LED
    }


    // Data is read out, time for another reading!
    vl53.clearInterrupt();
  }
}