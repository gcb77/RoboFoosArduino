#include <SoftwareSerial.h>
#include <Arduino.h>

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

struct timeval  tv1, tv2;

SerialClass Serial;

void printTime() {
    gettimeofday(&tv1, NULL);
    printf("%d:%d ", tv1.tv_sec, tv1.tv_usec);
}

int digitalRead(int pin) {
    // printTime();
    // printf("Reading pin %d\n", pin);
    return 1;
}

void digitalWrite(int pin, int value) {
    printTime();
    printf("Writing %d to pin %d\n", value, pin);
}

void pinMode(int pin, int mode) {
    printTime();
    printf("Setting mode %d on pin %d\n", mode, pin);
}

void delayMicroseconds(int ammount) {
    printTime();
    printf("Delaying %d microseconds\n", ammount);
}

int random(int a, int b) {
    printTime();
    int rVal = a + (b-a)/2;
    printf("Returning random value of %d for request of random(%d, %d)\n", rVal, a, b);
    return(rVal);
}

void SerialClass::begin(int baudRate) {
    printTime();
    printf("Serial begin() called\n");
}

void SerialClass::print(const char *str) {
    printTime();
    printf("Serial print: %s", str);
}

void SerialClass::println(const char *str) {
    printTime();
    printf("Serial print: %s\n", str);
}

void SerialClass::println(int val) {
    printTime();
    printf("Serial print: %d\n", val);
}

int SerialClass::available() {
    return 0;
}

char SerialClass::read() {
    return 0;
}

int main(void) {
    setup();
    gettimeofday(&tv2, NULL);
    int start_sec = tv2.tv_sec;
    int end_sec = start_sec + 1;
    while(tv2.tv_sec < end_sec) {
        loop();
        gettimeofday(&tv2, NULL);
    }
}

unsigned long micros() {
    gettimeofday(&tv1, NULL);
    return (unsigned long)tv1.tv_usec;
}