#include <avr/pgmspace.h>

// Note: The keyboard logic only supports ASCII A-Z chars
const char keyword_0[] PROGMEM = "dub";
const char keyword_1[] PROGMEM = "psy";
const char keyword_2[] PROGMEM = "trance";
const char keyword_3[] PROGMEM = "trip";
const char keyword_4[] PROGMEM = "lofi";
const char keyword_5[] PROGMEM = "roots";
const char keyword_6[] PROGMEM = "nintendo";
const char keyword_7[] PROGMEM = "chill";
const char keyword_8[] PROGMEM = "downtempo";
const char keyword_9[] PROGMEM = "vapor";
const char keyword_10[] PROGMEM = "dreamy";
const char keyword_11[] PROGMEM = "sweet";
const char keyword_12[] PROGMEM = "relax";
const char keyword_13[] PROGMEM = "meditation";
const char keyword_14[] PROGMEM = "deep";
const char keyword_15[] PROGMEM = "ambient";
const char keyword_16[] PROGMEM = "adhd";
const char keyword_17[] PROGMEM = "beats";
const char keyword_18[] PROGMEM = "nu";
const char keyword_19[] PROGMEM = "jazz";
const char keyword_20[] PROGMEM = "drum";
const char keyword_21[] PROGMEM = "bass";
const char keyword_22[] PROGMEM = "break";
const char keyword_23[] PROGMEM = "core";
const char keyword_24[] PROGMEM = "modern";
const char keyword_25[] PROGMEM = "old";

const char* const keywords[] PROGMEM = {
  keyword_0,
  keyword_1,
  keyword_2,
  keyword_3,
  keyword_4,
  keyword_5,
  keyword_6,
  keyword_7,
  keyword_8,
  keyword_9,
  keyword_10,
  keyword_11,
  keyword_12,
  keyword_13,
  keyword_14,
  keyword_15,
  keyword_16,
  keyword_17,
  keyword_18,
  keyword_19,
  keyword_20,
  keyword_21,
  keyword_22,
  keyword_23,
  keyword_24,
  keyword_25,
};

const uint8_t num_keywords = sizeof(keywords) / sizeof(*(keywords));
