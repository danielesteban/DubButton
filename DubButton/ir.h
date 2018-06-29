#ifndef IR_h
#define IR_h

#include <Arduino.h>

namespace IR {
  void enableIROut(int khz = 38);
  void sendSamsung(unsigned long data);
  // void sendNEC(unsigned long data);
  // void sendSony(unsigned long data);
  // void sendRC5(unsigned long data, int nbits);
  // void sendRC6(unsigned long data, int nbits);
  // void sendRaw(unsigned int buf[], int len, int khz);
  void mark(int usec);
  void space(int usec);
};

#endif // IR_h
