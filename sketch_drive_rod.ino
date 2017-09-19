#include <SoftwareSerial.h>
#include <Arduino.h>

const int X_STEP_PIN = 2;
const int X_DIR_PIN = 5;
const int Y_STEP_PIN = 3;
const int Y_DIR_PIN = 6;
const int DISABLE_PIN = 8;
const int X_ENDSTOP_PIN = 9;

/*
  With all 3 jumpers in the driver is configured to run at 1/16 microstepping
 */
const int STEPS_PER_ROTATION = 200 * 16;

int step_time = 150;
int is_active;

typedef struct status_struct {
  int numSteps;
  int currentStep;
  int dir;
  int upDown;
  long last;
  long next;
} status;

status motor1Status;

void setup() {
  pinMode(X_STEP_PIN, OUTPUT);
  pinMode(X_DIR_PIN, OUTPUT);
  pinMode(Y_STEP_PIN, OUTPUT);
  pinMode(Y_DIR_PIN, OUTPUT);
  pinMode(DISABLE_PIN, OUTPUT);
  pinMode(X_ENDSTOP_PIN, INPUT);
  Serial.begin(57600);
  Serial.println("RoboFoos Ready! Enter 1-9 to run with different delays, 0 to stop!");
  digitalWrite(X_DIR_PIN, HIGH);
  is_active = 0;
}

void go(int steps, int dir) {
  int accel_steps = 0;
  int hold_time = 1000;
  int accel_time = 2;


  digitalWrite(X_DIR_PIN, dir);
  //digitalWrite(Y_DIR_PIN, dir);

  for (int i = 0; i < steps; i++){
    //determine pulse time
    if(i > steps - accel_steps) {
      //In decelleration phase
      hold_time += accel_time;
    }
    else if(hold_time > step_time) {
      //In accelleration phase
      hold_time -= accel_time;
    }
    else if(hold_time < step_time) {
      hold_time = step_time;
    }
    //pulse the motor
    digitalWrite(X_STEP_PIN, HIGH);
    //digitalWrite(Y_STEP_PIN, HIGH);
    delayMicroseconds(20);
    digitalWrite(X_STEP_PIN, LOW);
    //digitalWrite(Y_STEP_PIN, LOW);
    delayMicroseconds((hold_time)-20);
  }
  //digitalWrite(DISABLE_PIN, HIGH);
}


void goNew(int steps, int dir) {
  motor1Status.numSteps = steps;
  motor1Status.currentStep = 0;
  motor1Status.dir = dir;
  motor1Status.upDown = 0;
  motor1Status.last = millis();
  motor1Status.next = motor1Status.last;
}

void checkMaps() {
  long cTime = millis();

  //Serial.print("C:");
  //Serial.print(cTime);
  //Serial.print(" N:");
  //Serial.print(motor1Status.next);
  //Serial.println("");

  //Only do stuff if the time is right
  digitalWrite(X_DIR_PIN, motor1Status.dir);
  if(motor1Status.next <= cTime) {
    if(motor1Status.currentStep > motor1Status.numSteps && motor1Status.upDown == 0) {
      //Motor finished the current steps
      if(motor1Status.dir == LOW) {
        //Set to high for the return
        motor1Status.dir = HIGH;
        motor1Status.currentStep = 0;
      } else {
        //This was the return, so reset to 0
        motor1Status.numSteps = 0;
        motor1Status.currentStep = 0;
      }
    } else {
      motor1Status.last = cTime;
      if(motor1Status.upDown == 0) {
        motor1Status.upDown = 1;
        digitalWrite(X_STEP_PIN, HIGH);
        motor1Status.next = cTime + 20;
      } else {
        motor1Status.upDown = 0;
        digitalWrite(X_STEP_PIN, LOW);
        motor1Status.next = cTime + 300;
        motor1Status.currentStep++;
      }
    }
  }
}

void loop() {
  if(Serial.available()){
    char c = Serial.read();
    int n = c-48;
    //Serial.print("N is ");
    //Serial.println(n);
    if(n == 0) {
      is_active = 0;
    }
    else if(1 < n && n <= 9) {
      step_time = n * 30;
      is_active = 1;
    }
    Serial.print("Running with ");
    Serial.println(step_time);
  }
  if(is_active > 0 || digitalRead(X_ENDSTOP_PIN) == HIGH) {
    digitalWrite(DISABLE_PIN, LOW);

    //Initialize maps to move rod
    if(motor1Status.numSteps == 0) {
       int x_steps = random(STEPS_PER_ROTATION/10, STEPS_PER_ROTATION * 1.5);
       Serial.println("Calling goNew");
       goNew(x_steps, LOW);
    }
    // int x_steps = random(STEPS_PER_ROTATION/10, STEPS_PER_ROTATION * 1.5);
    // go(x_steps, LOW);
    // go(x_steps, HIGH);
    // digitalWrite(DISABLE_PIN, HIGH);

    // Check maps and move rods
    checkMaps();
  }
  else {
    digitalWrite(DISABLE_PIN, HIGH);
  }
  //Serial.println(motor1Status.currentStep);
}


