#include <SoftwareSerial.h>
#include <Arduino.h>

const int X_STEP_PIN = 2;
const int X_DIR_PIN = 5;
const int Y_STEP_PIN = 3;
const int Y_DIR_PIN = 6;
const int DISABLE_PIN = 8;
const int X_ENDSTOP_PIN = 9;
const int Y_ENDSTOP_PIN = 10;

const int STEPS_BUFFER_SIZE = 10000;

//Holds current moves.  Each elements stores the step time in ms
int x_steps_buffer[STEPS_BUFFER_SIZE];
int y_steps_buffer[STEPS_BUFFER_SIZE];

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
      *buffer = hold_time;
    } else {
      *buffer = hold_time * -1;
    }

    //increment the buffer position
    buffer++;
  }
}

long doStep(unsigned long current_micros, int dir_pin, int step_pin, int *buffer, int *pos, int *pos_end) {
  //Enable motors
  digitalWrite(DISABLE_PIN, LOW);

  //Set the direction pin based on -/+ value of step
  if(*(buffer+*pos) < 0) {
    digitalWrite(dir_pin, LOW);
  } else {
    digitalWrite(dir_pin, HIGH);
  }

  //Turn on the step pin and hold it for 20 microseconds
  digitalWrite(step_pin, HIGH);
  delayMicroseconds(20);
  digitalWrite(step_pin, LOW);

  //Disable motors
  digitalWrite(DISABLE_PIN, HIGH);

  //Increment the step and check if we have more input
  *pos += 1;
  if(*pos < *pos_end) {
    return current_micros + *(buffer+*pos);
  } else {
    //Done with steps, reset it all
    *pos = 0;
    *pos_end = 0;
    return 0;
  }
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

  //Check to see if the X axis is active
  if(is_active > 0 || digitalRead(X_ENDSTOP_PIN) == HIGH) {
    //Get current time
    current_micros = micros();

    //Check to see if current steps are being processed
    if(x_step_end > 0) {
      //Check to see if its time for the next X pulse
      if(current_micros > x_next_pulse_time) {
        x_next_pulse_time = doStep(current_micros, X_DIR_PIN, X_STEP_PIN, x_steps_buffer, &x_step_pos, &x_step_end);
      }
    } else {
      int x_steps = random(STEPS_PER_ROTATION/10, STEPS_PER_ROTATION * 1.5);
      //Prevent a buffer overrun
      if(x_steps > STEPS_BUFFER_SIZE/2)  x_steps = STEPS_BUFFER_SIZE/2;
      scheduleSteps(x_steps, LOW, x_steps_buffer);
      scheduleSteps(x_steps, HIGH, x_steps_buffer+x_steps);
      x_step_end = x_steps * 2;
    }
  }

 //Do the same for the Y axis
  if(is_active > 0 || digitalRead(Y_ENDSTOP_PIN) == HIGH) {
    //Get current time
    current_micros = micros();

    //Check to see if current steps are being processed
    if(y_step_end > 0) {
      //Check to see if its time for the next X pulse
      if(current_micros > y_next_pulse_time) {
        y_next_pulse_time = doStep(current_micros, Y_DIR_PIN, Y_STEP_PIN, y_steps_buffer, &y_step_pos, &y_step_end);
      }
    } else {
      int y_steps = random(STEPS_PER_ROTATION/10, STEPS_PER_ROTATION * 1.5);
      //Prevent a buffer overrun
      if(y_steps > STEPS_BUFFER_SIZE/2)  y_steps = STEPS_BUFFER_SIZE/2;
      scheduleSteps(y_steps, LOW, y_steps_buffer);
      scheduleSteps(y_steps, HIGH, y_steps_buffer+y_steps);
      y_step_end = y_steps * 2;
    }
  } else {
    //Stop all movement and clear all move buffers
    digitalWrite(DISABLE_PIN, HIGH);
    x_step_end = 0;
    x_step_pos = 0;
    y_step_end = 0;
    y_step_pos = 0;
  }
}