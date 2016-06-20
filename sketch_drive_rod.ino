#include <SoftwareSerial.h>
#include <Arduino.h>

const int X_STEP_PIN = 2;
const int X_DIR_PIN = 5;
const int DISABLE_PIN = 8;

/*
  With all 3 jumpers in the driver is configured to run at 1/16 microstepping
*/
const int STEPS_PER_ROTATION = 200 * 16;

int step_time = 500;
int is_active;

void setup() {
  pinMode(X_STEP_PIN, OUTPUT);
  pinMode(X_DIR_PIN, OUTPUT);
  pinMode(DISABLE_PIN, OUTPUT);
  Serial.begin(57600);
  Serial.println("RoboFoos Ready!");
  digitalWrite(X_DIR_PIN, HIGH);
  is_active = 0;
}

void go(int steps, int dir) {
  if(dir <= 0) {
    digitalWrite(X_DIR_PIN, LOW);
  } else {
    digitalWrite(X_DIR_PIN, HIGH);
  }

  for (int i = 0; i < steps; i++){    
    int hold_time = step_time;
    if(i==0 || i == steps-1){
      hold_time = 800;
    } else if (i == 1 || i == steps-2) {
      hold_time = 750;
    } else if (i == 2 || i == steps-3) {
      hold_time = 600;
    } else if (i == 3 || i == steps-4) {
      hold_time = 500;
    } else if (i == 4 || i == steps-5) {
      hold_time = 400;
    } else if (i == 5 || i == steps-6) {
      hold_time = 300;
    } else if (i == 6 || i == steps-7) {
      hold_time = 200;
    } else if (i == 7 || i == steps-8) {
      hold_time = 100;
    } else if (i == 8 || i == steps-9) {
      hold_time = 70;
    } else if (i == 9 || i == steps-10) {
      hold_time = 50;
    }
    digitalWrite(X_STEP_PIN, HIGH);
    delayMicroseconds(20);
    digitalWrite(X_STEP_PIN, LOW);
    delayMicroseconds((hold_time)-20);      
  }
  //digitalWrite(DISABLE_PIN, HIGH);  
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
  if(is_active > 0) {
    digitalWrite(DISABLE_PIN, LOW);
      go(STEPS_PER_ROTATION, 0);
      go(STEPS_PER_ROTATION, 1);
    digitalWrite(DISABLE_PIN, HIGH);
  } else {
    digitalWrite(DISABLE_PIN, HIGH); 
  }
}

