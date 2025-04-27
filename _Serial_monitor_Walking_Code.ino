#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

// Servo Driver
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver(0x40);
#define SERVOMIN 150
#define SERVOMAX 600

bool isWalking = false;
bool turningLeft = false;
bool turningRight = false;

// Servo Angles
int currentAngles[12] = {
  80, 125, 55,  // Left Back (Hip, Ankle, Shoulder)
  80,  25, 75,  // Right Back
  85,  30, 60,  // Left Front
  90,  20, 65   // Right Front
};

int neutralAngles[12] = {
  80, 125, 55,
  80,  25, 75,
  85,  30, 60,
  90,  20, 65
};

void moveSmooth(int servoNum, int targetAngle, int steps, int delayTime) {
  int startAngle = currentAngles[servoNum];
  for (int i = 0; i <= steps; i++) {
    int angle = map(i, 0, steps, startAngle, targetAngle);
    pwm.setPWM(servoNum, 0, map(angle, 0, 180, SERVOMIN, SERVOMAX));
    delay(delayTime);
  }
  currentAngles[servoNum] = targetAngle;
}

void standNeutral() {
  for (int i = 0; i < 12; i++) {
    moveSmooth(i, neutralAngles[i], 10, 10);
  }
}

void setup() {
  Serial.begin(9600);
  Wire.begin();
  pwm.begin();
  pwm.setPWMFreq(50);
  delay(1000);

  standNeutral();
  Serial.println("🟢 READY | Type: w = walk, a = left, d = right, s = stop");
}

void loop() {
  if (Serial.available()) {
    char command = Serial.read();
    command = tolower(command); // Normalize to lowercase

    if (command == 'w') {
      isWalking = true;
      turningLeft = false;
      turningRight = false;
      Serial.println("🚶 Walking...");
    } else if (command == 'a') {
      turningLeft = true;
      turningRight = false;
      isWalking = false;
      Serial.println("↩️ Turning Left...");
    } else if (command == 'd') {
      turningRight = true;
      turningLeft = false;
      isWalking = false;
      Serial.println("↪️ Turning Right...");
    } else if (command == 's') {
      isWalking = false;
      turningLeft = false;
      turningRight = false;
      Serial.println("🛑 Stopped. Resetting to neutral...");
      standNeutral();
    }
  }

  if (isWalking) {
    // Step 1: LF + RB
    moveSmooth(7, 50, 10, 10);
    moveSmooth(8, 25, 10, 10);
    moveSmooth(5, 95, 10, 10);
    moveSmooth(4, 15, 10, 10);
    delay(250);
    moveSmooth(7, neutralAngles[7], 10, 10);
    moveSmooth(8, neutralAngles[8], 10, 10);
    moveSmooth(5, neutralAngles[5], 10, 10);
    moveSmooth(4, neutralAngles[4], 10, 10);
    delay(250);

    // Step 2: RF + LB
    moveSmooth(11, 85, 10, 10);
    moveSmooth(10, 30, 10, 10);
    moveSmooth(2, 35, 10, 10);
    moveSmooth(1, 135, 10, 10);
    delay(250);
    moveSmooth(11, neutralAngles[11], 10, 10);
    moveSmooth(10, neutralAngles[10], 10, 10);
    moveSmooth(2, neutralAngles[2], 10, 10);
    moveSmooth(1, neutralAngles[1], 10, 10);
    delay(250);
  }

  if (turningLeft) {
    moveSmooth(7, 45, 10, 10);
    moveSmooth(8, 20, 10, 10);
    moveSmooth(2, 40, 10, 10);
    moveSmooth(1, 130, 10, 10);
    delay(500);
    moveSmooth(7, neutralAngles[7], 10, 10);
    moveSmooth(8, neutralAngles[8], 10, 10);
    moveSmooth(2, neutralAngles[2], 10, 10);
    moveSmooth(1, neutralAngles[1], 10, 10);
    delay(500);
  }

  if (turningRight) {
    moveSmooth(11, 85, 10, 10);
    moveSmooth(10, 35, 10, 10);
    moveSmooth(5, 90, 10, 10);
    moveSmooth(4, 20, 10, 10);
    delay(500);
    moveSmooth(11, neutralAngles[11], 10, 10);
    moveSmooth(10, neutralAngles[10], 10, 10);
    moveSmooth(5, neutralAngles[5], 10, 10);
    moveSmooth(4, neutralAngles[4], 10, 10);
    delay(500);
  }
}
