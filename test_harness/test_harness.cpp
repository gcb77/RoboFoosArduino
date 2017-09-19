#include <SoftwareSerial.h>
#include <Arduino.h>

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

SerialClass Serial;

long clockTime = 0;

int digitalRead(int pin) {
    printf("Reading pin %d\n", pin);
    return 1;
}

void digitalWrite(int pin, int value) {
    printf("Writing %d to pin %d\n", value, pin);
}

void pinMode(int pin, int mode) {
    printf("Setting mode %d on pin %d\n", mode, pin);
}

long micros() {
    clockTime++;
}

void delayMicroseconds(int ammount) {
    printf("Delaying %d microseconds\n", ammount);
}

int random(int a, int b) {
    int rVal = a + (b-a)/2;
    printf("Returning random value of %d for request of random(%d, %d)\n", rVal, a, b);
    return(rVal);
}

void SerialClass::begin(int baudRate) {
    printf("Serial begin() called\n");
}

void SerialClass::print(const char *str) {
    printf("%s", str);
}

void SerialClass::println(const char *str) {
    printf("%s\n", str);
}

void SerialClass::println(int val) {
    printf("%d\n", val);
}

void SerialClass::print(long val) {
    printf("%d", val);
}

int SerialClass::available() {
    return 0;
}

char SerialClass::read() {
    return 0;
}

int main(void) {
    int num = 10000;
    setup();
    while(num-- > 0) {
      clockTime++;
      loop();
    }
}