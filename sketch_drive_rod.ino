#include <SoftwareSerial.h>
#include <comm.h>
#include <Arduino.h>

const int x_step = 2;
const int x_dir = 5;
const int disable = 8;

//const int steps_per_rotation = 200 * 4;
const int steps_per_rotation = 40;

int hold_high = 300;
int hold_low = 700;
int t;

void setup() {
  pinMode(x_step, OUTPUT);
  pinMode(x_dir, OUTPUT);
  pinMode(disable, OUTPUT);
  Serial.begin(57600);
  Serial.println("RoboFoos Ready!");
  //send: 07766544444517654444460776654444451765444444670776654444451765444446077665444445176544444467
  //digitalWrite(x_dir, HIGH);
  t = 0;
}

void loop() {
  if(Serial.available()){
    char c = Serial.read();
    int n = c-48;
    Serial.print("N is ");
    Serial.println(n);
    if(n == 0) {
      digitalWrite(x_dir, HIGH);
    } else if(n==1){
      digitalWrite(x_dir, LOW);
    } else if(n==2){
      delay(1000);
    } else if(3 < n && n <= 9) {
      //hold_high = n * 100;
      hold_low = n * 100;
      t+=1;
    }
    Serial.print("Running with ");
    Serial.println(hold_low);
  }
  if(t > 0) {

    digitalWrite(disable, LOW);
    //digitalWrite(x_dir, HIGH);

    for (int i = 0; i < steps_per_rotation; i++){    
      digitalWrite(x_step, HIGH);
      delayMicroseconds(hold_high);
      digitalWrite(x_step, LOW);
      delayMicroseconds(hold_low);      
    }
    //digitalWrite(disable, HIGH);  
  
    t -= 1;
  } else {
    digitalWrite(disable, HIGH); 
  }
}
