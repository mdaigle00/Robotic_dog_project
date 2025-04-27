#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
#include <PS2X_lib.h>

// Servo Driver
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver(0x40);
#define SERVOMIN 150
#define SERVOMAX 600

// PS2 Controller Pins
#define PS2_DAT 7
#define PS2_CMD 8
#define PS2_SEL 9
#define PS2_CLK 10

PS2X ps2x;

bool isWalking = false;
bool turningLeft = false;
bool turningRight = false;

// Servo Angles
int currentAngles[12] = {
  80, 130, 55,  // Left Back (Hip, Ankle, Shoulder)
  80,  25, 75,  // Right Back
  85,  30, 60,  // Left Front
  90,  20, 65   // Right Front
};

int neutralAngles[12] = {
  80, 135, 55,
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

  // ⭐ Give time to manually pair PS2 controller
  Serial.println("🔄 Waiting 10 seconds to pair PS2 controller...");
  delay(10000); // <-- TIME TO TURN ON CONTROLLER + PRESS MODE

  int error = ps2x.config_gamepad(PS2_CLK, PS2_CMD, PS2_SEL, PS2_DAT);
  if (error == 0) {
    Serial.println("✅ PS2 Controller connected!");
  } else {
    Serial.print("❌ PS2 Controller error: ");
    Serial.println(error);
  }

  standNeutral();
  Serial.println("🎮 Use D-Pad: UP = walk | LEFT = turn left | RIGHT = turn right | DOWN = stop");
}

void loop() {
  ps2x.read_gamepad();

  /*if (ps2x.ButtonPressed(PSB_PAD_UP)) {
    isWalking = true;
    turningLeft = false;
    turningRight = false;
    Serial.println("🚶 Walking...");
  } else if (ps2x.ButtonPressed(PSB_PAD_LEFT)) {
    turningLeft = true;
    turningRight = false;
    isWalking = false;
    Serial.println("↩️ Turning Left...");
  } else if (ps2x.ButtonPressed(PSB_PAD_RIGHT)) {
    turningRight = true;
    turningLeft = false;
    isWalking = false;
    Serial.println("↪️ Turning Right...");
  } else if (ps2x.ButtonPressed(PSB_PAD_DOWN)) {
    isWalking = false;
    turningLeft = false;
    turningRight = false;
    Serial.println(8"🛑 Stopped. Resetting to neutral...");
    standNeutral();
  }*/

  isWalking = true;

  if (isWalking) {
    // Step 1: LF + RB
    moveSmooth(7, 50, 10, 10);   // LF shoulder
    moveSmooth(8, 25, 10, 10);   // LF ankle
    moveSmooth(5, 95, 10, 10);   // RB shoulder
    moveSmooth(4, 15, 10, 10);   // RB ankle
    delay(250);
    moveSmooth(7, neutralAngles[7], 10, 10);
    moveSmooth(8, neutralAngles[8], 10, 10);
    moveSmooth(5, neutralAngles[5], 10, 10);
    moveSmooth(4, neutralAngles[4], 10, 10);
    delay(250);

    // Step 2: RF + LB
    moveSmooth(11, 85, 10, 10); // RF shoulder
    moveSmooth(10, 30, 10, 10); // RF ankle
    moveSmooth(2, 35, 10, 10);  // LB shoulder
    moveSmooth(1, 120, 10, 10); // LB ankle
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

