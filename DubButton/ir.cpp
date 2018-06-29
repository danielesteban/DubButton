// This is a heavily simplified version of SeeJayDee's IRremote port for the ATtiny:
// https://gist.github.com/SeeJayDee/caa9b5cc29246df44e45b8e7d1b1cdc5
//
// I just refactored out the IRsend class and added support for samsung TVs

#include "ir.h"

#define TOPBIT 0x80000000

void IR::enableIROut(int khz) {
  // Enables IR output. The khz value controls the modulation frequency in kilohertz.
  // The IR output will be on pin 3 (PB4 - Arduino D4) (OC1B).
  // This routine is designed for 36-40KHz; if you use it for other values, it's up to you
  // to make sure it gives reasonable results. (Watch out for overflow / underflow / rounding.)
  // TIMER1 is used in fast PWM mode, with OCR1C controlling the frequency and OCR1B controlling the duty cycle.
  // There is no prescaling, so the output frequency is 8MHz / (2 * OCR1C)

  // Setup output pin
  pinMode(4, OUTPUT);
  digitalWrite(4, LOW);

  // CTC1 = 1: TOP value set to OCR1C
  // CS = 0001: No Prescaling
  TCCR1 = _BV(CTC1) | _BV(CS10);

  // PWM1B = 1: Enable PWM for OCR1B
  GTCCR = _BV(PWM1B);

  // The top value for the timer. The modulation frequency will be F_CPU / OCR1C.
  OCR1C = F_CPU / khz / 1000;
  OCR1B = OCR1C / 3; // 33% duty cycle
}

void IR::mark(int time) {
  // Sends an IR mark for the specified number of microseconds.
  // The mark output is modulated at the PWM frequency.
  GTCCR |= _BV(COM1B1); // Enable pin 3 PWM output (PB4 - Arduino D4)
  delayMicroseconds(time);
}

void IR::space(int time) {
  // Sends an IR space for the specified number of microseconds.
  // A space is no output, so the PWM output is disabled.
  GTCCR &= ~(_BV(COM1B1)); // Disable pin 3 PWM output (PB4 - Arduino D4)
  delayMicroseconds(time);
}

#define SAMSUNG_BITS 32
#define SAMSUNG_HDR_MARK    5000
#define SAMSUNG_HDR_SPACE   5000
#define SAMSUNG_BIT_MARK     560
#define SAMSUNG_ONE_SPACE   1600
#define SAMSUNG_ZERO_SPACE   560

void IR::sendSamsung(unsigned long data) {
  // Note: I commented the next line out so that I can just call it once from arduino's setup()
  // enableIROut(38);
  mark(SAMSUNG_HDR_MARK);
  space(SAMSUNG_HDR_SPACE);
  for (int i = 0; i < SAMSUNG_BITS; i++) {
    if (data & TOPBIT) {
      mark(SAMSUNG_BIT_MARK);
      space(SAMSUNG_ONE_SPACE);
    } else {
      mark(SAMSUNG_BIT_MARK);
      space(SAMSUNG_ZERO_SPACE);
    }
    data <<= 1;
  }
  mark(SAMSUNG_BIT_MARK);
  space(0);
}

// #define NEC_BITS 32
// #define NEC_HDR_MARK	9000
// #define NEC_HDR_SPACE	4500
// #define NEC_BIT_MARK	560
// #define NEC_ONE_SPACE	1600
// #define NEC_ZERO_SPACE	560
//
// void IR::sendNEC(unsigned long data) {
//   enableIROut(38);
//   mark(NEC_HDR_MARK);
//   space(NEC_HDR_SPACE);
//   for (int i = 0; i < NEC_BITS; i++) {
//     if (data & TOPBIT) {
//       mark(NEC_BIT_MARK);
//       space(NEC_ONE_SPACE);
//     } else {
//       mark(NEC_BIT_MARK);
//       space(NEC_ZERO_SPACE);
//     }
//     data <<= 1;
//   }
//   mark(NEC_BIT_MARK);
//   space(0);
// }

// #define SONY_BITS 12
// #define SONY_HDR_MARK	2400
// #define SONY_HDR_SPACE	600
// #define SONY_ONE_MARK	1200
// #define SONY_ZERO_MARK	600
//
// void IR::sendSony(unsigned long data) {
//   enableIROut(40);
//   mark(SONY_HDR_MARK);
//   space(SONY_HDR_SPACE);
//   for (int i = 0; i < SONY_BITS; i++) {
//     if (data & TOPBIT) {
//       mark(SONY_ONE_MARK);
//       space(SONY_HDR_SPACE);
//     } else {
//       mark(SONY_ZERO_MARK);
//       space(SONY_HDR_SPACE);
//     }
//     data <<= 1;
//   }
//   space(0);
// }

// #define RC5_T1		889
//
// // Note: first bit must be a one (start bit)
// void IR::sendRC5(unsigned long data, int nbits) {
//   enableIROut(36);
//   data = data << (32 - nbits);
//   mark(RC5_T1); // First start bit
//   space(RC5_T1); // Second start bit
//   mark(RC5_T1); // Second start bit
//   for (int i = 0; i < nbits; i++) {
//     if (data & TOPBIT) {
//       space(RC5_T1); // 1 is space, then mark
//       mark(RC5_T1);
//     } else {
//       mark(RC5_T1);
//       space(RC5_T1);
//     }
//     data <<= 1;
//   }
//   space(0);
// }

// #define RC6_HDR_MARK	2666
// #define RC6_HDR_SPACE	889
// #define RC6_T1		444
//
// // Caller needs to take care of flipping the toggle bit
// void IR::sendRC6(unsigned long data, int nbits) {
//   enableIROut(36);
//   data = data << (32 - nbits);
//   mark(RC6_HDR_MARK);
//   space(RC6_HDR_SPACE);
//   mark(RC6_T1); // start bit
//   space(RC6_T1);
//   int t;
//   for (int i = 0; i < nbits; i++) {
//     if (i == 3) {
//       // double-wide trailer bit
//       t = 2 * RC6_T1;
//     } else {
//       t = RC6_T1;
//     }
//     if (data & TOPBIT) {
//       mark(t);
//       space(t);
//     } else {
//       space(t);
//       mark(t);
//     }
//
//     data <<= 1;
//   }
//   space(0);
// }

// void IR::sendRaw(unsigned int buf[], int len, int khz) {
//   enableIROut(khz);
//   for (int i = 0; i < len; i++) {
//     if (i & 1) {
//       space(buf[i]);
//     } else {
//       mark(buf[i]);
//     }
//   }
//   space(0);
// }
