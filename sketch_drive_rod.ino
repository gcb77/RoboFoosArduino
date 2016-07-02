#include <SoftwareSerial.h>
#include <Arduino.h>

const int X_STEP_PIN = 2;
const int X_DIR_PIN = 5;
const int Y_STEP_PIN = 3;
const int Y_DIR_PIN = 6;
const int DISABLE_PIN = 8;
const int X_ENDSTOP_PIN = 9;

//Holds current moves.  Each elements stores the step time in ms
int x_steps_buffer[1000];
int y_steps_buffer[1000];

//Current step position in buffer
int x_step_pos = 0;
int x_step_end = 0;
int y_step_pos = 0;
int y_step_end = 0;

//next pulse time for step
unsigned long current_micros = 0l;
unsigned long x_next_pulse_time = micros();
unsigned long y_next_pulse_time = micros();

//With all 3 jumpers in the driver is configured to run at 1/16 microstepping
const int MICROSTEPS_PER_STEP = 16;
const int STEPS_PER_ROTATION = 200 * MICROSTEPS_PER_STEP;

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

/* 
  Goes the indicated number of steps in the specified direction.  Very
  inneficient since it holds up the CPU while it does it
*/
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

void scheduleSteps(int steps, int dir, int *buffer) {
  int accel_steps = 0;
  int hold_time = 1000;
  int accel_time = 2;
  
  var pos = 0;

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
    
    //Add either a negative value or positive value based on direction
    if(dir == 0) {
      buffer + pos = hold_time;
    } else {
      buffer + pos = hodl_time * -1;
    }
    
    //increment the buffer position
    pos += 1;
  }
}

void doStep(int pin, int dir) {
  //Enable power
  digitalWrite(DISABLE_PIN, HIGH);  
  
  //pulse the motor
  digitalWrite(pin, HIGH);
  //digitalWrite(Y_STEP_PIN, HIGH);
  delayMicroseconds(20);
  digitalWrite(X_STEP_PIN, LOW);
  //digitalWrite(Y_STEP_PIN, LOW);
  delayMicroseconds((hold_time)-20);      
}
  
void loop() {
  //Process serial input
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
  
  
  //Check to see if we're active
  if(is_active > 0 || digitalRead(X_ENDSTOP_PIN) == HIGH) {
    
    //Check to see if current steps are being processed
    if(x_step_end > 0) {
      //Get current time
      current_micros = micros();
      
      //Check to see if its time for the next X pulse
      if(current_micros > x_next_pulse_time) {
        
        //Enable motors
        digitalWrite(DISABLE_PIN, LOW);
        
        //Set the direction pin based on -/+ value of step
        if(x_steps_buffer[x_step_pos] < 0) {
          digitalWrite(X_STEP_DIR, LOW);
        } else {
          digitalWrite(X_STEP_DIR, HIGH);
        }
        
        //Turn on the step pin and hold it for 20 microseconds
        digitalWrite(X_STEP_PIN, HIGH);
        delayMicroseconds(20);
        digitalWrite(X_STEP_PIN, LOW);
        
        //Disable motors
        digitalWrite(DISABLE_PIN, HIGH);
        
        //Increment the step and check if we have more input
        x_step_pos += 1;
        if(x_step_pos < x_step_end) {
          x_next_pulse_time = current_micros + x_steps_buffer[x_step_pos];
        } else {
          //Done with steps, reset it all
          x_step_pos = 0;
          x_step_end = 0;
        }
      }
    } else {
      int x_steps = random(STEPS_PER_ROTATION/10, STEPS_PER_ROTATION * 1.5);
      //Prevent a buffer overrun
      if(x_steps > 500)  x_steps = 500;
      scheduleSteps(x_steps, LOW, x_steps_buffer);
      scheduleSteps(x_steps, HIGH, x_steps_buffer+x_steps);
    }
    //TODO: do Y
  } else {
    //Stop all movement and clear all move buffers
    digitalWrite(DISABLE_PIN, HIGH); 
    x_step_end = 0;
    x_step_pos = 0;
    y_step_end = 0;
    y_step_pos = 0;
  }
}