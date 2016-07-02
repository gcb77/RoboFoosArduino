class SerialClass {
    public:
      static void begin(int baudRate);
      static void print(const char *str);
      static void println(const char *str);
      static void println(int);
      static int available();
      static char read();
};

extern SerialClass Serial;