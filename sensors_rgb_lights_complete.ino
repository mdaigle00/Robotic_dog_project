#include <Wire.h>
#include <MPU6050_light.h>
#include <Adafruit_NeoPixel.h>

// === Ultrasonic Pins ===
const int trigPin1 = 5;
const int echoPin1 = 4;
const int trigPin2 = 7;
const int echoPin2 = 6;

// === NeoPixel LED Setup ===
#define LED_PIN 2
#define NUM_LEDS 3
Adafruit_NeoPixel strip(NUM_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800);

// === IMU Setup ===
MPU6050 mpu(Wire);

// === Distance Variables ===
unsigned long duration1, duration2;
float distance1, distance2;

void setup() {
  Serial.begin(9600);
  Wire.begin();

  // IMU Setup
  mpu.begin();
  Serial.println("Calibrating MPU6050...");
  delay(1000);
  mpu.calcOffsets();
  Serial.println("MPU6050 Ready");

  // Ultrasonic Setup
  pinMode(trigPin1, OUTPUT);
  pinMode(echoPin1, INPUT);
  pinMode(trigPin2, OUTPUT);
  pinMode(echoPin2, INPUT);

  // NeoPixel Setup
  strip.begin();
  strip.show(); // Start with LEDs off
}

void loop() {
  mpu.update(); // Update IMU readings

  // === Sensor 1 Read ===
  digitalWrite(trigPin1, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin1, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin1, LOW);
  duration1 = pulseIn(echoPin1, HIGH);
  distance1 = duration1 * 0.0133858; // to inches

  // === Sensor 2 Read ===
  digitalWrite(trigPin2, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin2, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin2, LOW);
  duration2 = pulseIn(echoPin2, HIGH);
  distance2 = duration2 * 0.0133858; // to inches

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
  Serial.println("------ Sensor and Tilt Readings ------");
  Serial.print("Distance 1 (D5/D4): "); Serial.print(distance1); Serial.println(" inches");
  Serial.print("Distance 2 (D7/D6): "); Serial.print(distance2); Serial.println(" inches");
  Serial.print("Angle X: "); Serial.print(angleX);
  Serial.print(" | Angle Y: "); Serial.print(angleY);
  Serial.print(" | Angle Z: "); Serial.println(mpu.getAngleZ());
  Serial.print("Tilt Warning: "); Serial.println(dangerouslyTilted ? "YES" : "NO");
  Serial.println("--------------------------------------\n");

  delay(200); // Small refresh delay
}

// === Set LED Color Function ===
void setColor(byte r, byte g, byte b) {
  for (int i = 0; i < NUM_LEDS; i++) {
    strip.setPixelColor(i, strip.Color(r, g, b));
  }
  strip.show();
}

