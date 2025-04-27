#include <Wire.h>
#include <MPU6050_light.h>
#include <Adafruit_NeoPixel.h>

// === NeoPixel LED Setup ===
#define LED_PIN 2
#define NUM_LEDS 3
Adafruit_NeoPixel strip(NUM_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800);

// === IMU Setup ===
MPU6050 mpu(Wire);

void setup() {
  Serial.begin(9600);
  Wire.begin();

  // IMU Setup
  mpu.begin();
  Serial.println("Calibrating MPU6050...");
  delay(1000);
  mpu.calcOffsets();
  Serial.println("MPU6050 Ready");

  // NeoPixel Setup
  strip.begin();
  strip.show(); // Start with LEDs off
}

void loop() {
  mpu.update(); // Update IMU readings

  // === Read Tilt Angles ===
  float angleX = mpu.getAngleX();
  float angleY = mpu.getAngleY();

  bool dangerouslyTilted = (abs(angleX) > 20 || abs(angleY) > 20);

  // === LED Logic: Tilt only ===
  if (dangerouslyTilted) {
    setColor(255, 0, 0); // 🔴 Solid Red = Danger Tilt
  } else {
    setColor(0, 255, 0); // 🟢 Solid Green = Stable
  }

  // === Debug Print ===
  Serial.println("------ Tilt Readings ------");
  Serial.print("Angle X: "); Serial.print(angleX);
  Serial.print(" | Angle Y: "); Serial.print(angleY);
  Serial.print(" | Angle Z: "); Serial.println(mpu.getAngleZ());
  Serial.print("Tilt Warning: "); Serial.println(dangerouslyTilted ? "YES" : "NO");
  Serial.println("-----------------------------\n");

  delay(200); // Small refresh delay
}

// === Set LED Color Function ===
void setColor(byte r, byte g, byte b) {
  for (int i = 0; i < NUM_LEDS; i++) {
    strip.setPixelColor(i, strip.Color(r, g, b));
  }
  strip.show();
}


