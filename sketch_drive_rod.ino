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
  int dirPin;
  int stepPin;
  int upDown;
  int burnDelay;
  long last;
  long next;
} status;

status motor1Status;
status motor2Status;

void setup() {
  pinMode(X_STEP_PIN, OUTPUT);
  pinMode(X_DIR_PIN, OUTPUT);
  pinMode(Y_STEP_PIN, OUTPUT);
  pinMode(Y_DIR_PIN, OUTPUT);
  pinMode(DISABLE_PIN, OUTPUT);
  pinMode(X_ENDSTOP_PIN, INPUT);
  motor1Status.stepPin = X_STEP_PIN;
  motor1Status.dirPin = X_DIR_PIN;
  motor2Status.stepPin = Y_STEP_PIN;
  motor2Status.dirPin = Y_DIR_PIN;
  Serial.begin(57600);
  Serial.println("RoboFoos Ready! Enter 1-9 to run with different delays, 0 to stop!");
  digitalWrite(X_DIR_PIN, HIGH);
  is_active = 0;
}

void goNew(int steps, int dir, struct status_struct *motStatus) {
  motStatus->numSteps = steps;
  motStatus->currentStep = 0;
  motStatus->dir = dir;
  motStatus->upDown = 0;
  motStatus->burnDelay = 1000;
  motStatus->last = micros();
  motStatus->next = motor1Status.last;
}

void checkMaps(struct status_struct *motStatus) {
  long cTime = motStatus->last;
  motStatus->last = micros();

  int delay = 0;

  //Only do stuff if the time is right
  digitalWrite(motStatus->dirPin, motStatus->dir);
  if(motStatus->next <= cTime) {
    if(motStatus->currentStep > motStatus->numSteps && motStatus->upDown == 0) {
      //Motor finished the current steps
      if(motStatus->dir == LOW) {
        //Set to high for the return
        motStatus->dir = HIGH;
        motStatus->currentStep = 0;
      } else {
        //This was the return, so reset to 0
        motStatus->numSteps = 0;
        motStatus->currentStep = 0;
      }
    } else {
      if(motStatus->upDown == 0) {
        motStatus->upDown = 1;
        digitalWrite(motStatus->stepPin, HIGH);
        motStatus->next = cTime + 20;
      } else {
        motStatus->upDown = 0;
        digitalWrite(motStatus->stepPin, LOW);

        delay = step_time;
        if(motStatus->burnDelay > 0) {
          delay += motStatus->burnDelay;
          motStatus->burnDelay -= 50;
        }
        motStatus->next = cTime + delay;

        motStatus->currentStep++;
      }
    }
  }
}

void loop() {
  if(Serial.available()){
    char c = Serial.read();
    int n = c-48;
    Serial.print("N is ");
    Serial.println(n);
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
      //Serial.println("Calling goNew for 1");
      goNew(x_steps, LOW, &motor1Status);
    }
    if(motor2Status.numSteps == 0) {
      int x_steps = random(STEPS_PER_ROTATION/10, STEPS_PER_ROTATION * 1.5);
     // Serial.println("Calling goNew for 2");
      goNew(x_steps, LOW, &motor2Status);
    }

    // Check maps and move rods
    checkMaps(&motor1Status);
    checkMaps(&motor2Status);
  }
  else {
    digitalWrite(DISABLE_PIN, HIGH);
  }
}


