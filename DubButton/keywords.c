#include <avr/pgmspace.h>

const char keyword_0[] PROGMEM = "dub";
const char keyword_1[] PROGMEM = "psy";
const char keyword_2[] PROGMEM = "trance";
const char keyword_3[] PROGMEM = "trip";
const char keyword_4[] PROGMEM = "lofi";
const char keyword_5[] PROGMEM = "roots";
const char keyword_6[] PROGMEM = "nintendo";
const char keyword_7[] PROGMEM = "chill";
const char keyword_8[] PROGMEM = "downtempo";

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
};

const uint8_t num_keywords = sizeof(keywords) / sizeof(*(keywords));
