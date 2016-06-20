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

void setup() {
  pinMode(X_STEP_PIN, OUTPUT);
  pinMode(X_DIR_PIN, OUTPUT);
  pinMode(Y_STEP_PIN, OUTPUT);
  pinMode(Y_DIR_PIN, OUTPUT);
  pinMode(DISABLE_PIN, OUTPUT);
  pinMode(X_ENDSTOP_PIN, INPUT);
  Serial.begin(57600);
  Serial.println("RoboFoos Ready!");
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
    int x_steps = random(STEPS_PER_ROTATION/10, STEPS_PER_ROTATION * 1.5);
    go(x_steps, LOW);
    go(x_steps, HIGH);    
    digitalWrite(DISABLE_PIN, HIGH);
  } 
  else {
    digitalWrite(DISABLE_PIN, HIGH); 
  }
}


