#define LOW 0
#define HIGH 1

#define INPUT 0
#define OUTPUT 1

void setup();
void loop();
int digitalRead(int pin);
void digitalWrite(int pin, int signal);
void pinMode(int pin, int mode);
void delayMicroseconds(int ammount);

int random(int a, int b);