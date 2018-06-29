#include <Arduino.h>
#include "ir.h"

// I/O config
#define PRIMARY_BUTTON_PIN 1
#define SECONDARY_BUTTON_PIN 0
#define FEEDBACK_LED_PIN 2

// Youtube app keyboard config
#define KEYBOARD_STRIDE 7
#define KEYBOARD_HEIGHT 4
#define KEYBOARD_SPANISH 1 // Set this flag if your youtube app keyboard has an "ñ"

struct {
  uint8_t pin;
  uint8_t read;
  uint8_t state;
  uint32_t debounce;
} buttons[2] = {{ PRIMARY_BUTTON_PIN, HIGH, 0 }, { SECONDARY_BUTTON_PIN, HIGH, 0 }};

extern const char* const keywords[] PROGMEM;
extern const uint8_t num_keywords;

void setup() {
  // Setup buttons
  for (int i = 0; i < 2; i++) {
    pinMode(buttons[i].pin, INPUT_PULLUP);
  }
  // Setup feedback led
  pinMode(FEEDBACK_LED_PIN, OUTPUT);
  digitalWrite(FEEDBACK_LED_PIN, HIGH);
  // Seed the RNG
  randomSeed(analogRead(A3));
  // Enable IR output
  IR::enableIROut();
}

// Samsung TV IR codes
#define POWER 0xE0E040BF
#define MUTE 0xE0E0F00F
#define SMART_HUB 0xE0E09E61
#define UP 0xE0E006F9
#define LEFT 0xE0E0A659
#define RIGHT 0xE0E046B9
#define DOWN 0xE0E08679
#define ENTER 0xE0E016E9

void send(const uint32_t code, const uint32_t wait) {
  // Pulse feedback led & send IR code
  digitalWrite(FEEDBACK_LED_PIN, LOW);
  IR::sendSamsung(code);
  digitalWrite(FEEDBACK_LED_PIN, HIGH);
  delay(wait);
}

void openYoutube() {
  // Smart hub
  send(SMART_HUB, 10000);
  // Recent -> Featured
  send(LEFT, 1000);
  send(ENTER, 5000);
  // Apps menu
  send(ENTER, 10000);
  // Go thrice to the right
  // You'll may need to modify this to match the
  // youtube app grid position on your TV
  send(RIGHT, 1000);
  send(RIGHT, 1000);
  send(RIGHT, 1000);
  // Start youtube app
  send(ENTER, 20000);
}

void playSomeMusic() {
  // Navigate to search
  send(LEFT, 500);
  send(LEFT, 500);
  send(UP, 1500);
  send(RIGHT, 500);
  send(RIGHT, 500);
  // Initialize query
  uint8_t query[num_keywords];
  for (uint8_t i = 0; i < num_keywords; i++) query[i] = i;
  // Shuffle query
  for (uint8_t j = 0; j < num_keywords; j++) {
    for (uint8_t i = 0; i < num_keywords; i++) {
      uint8_t n = random(0, num_keywords);
      uint8_t temp = query[n];
      query[n] = query[i];
      query[i] = temp;
    }
  }
  // Type search query
  const uint8_t numWords = min(3, num_keywords);
  for (uint8_t i = 0; i < numWords; i++) {
    // Fetch keyword
    const char* keyword = pgm_read_word(&(keywords[query[i]]));
    for (uint8_t l = 0; l < strlen_P(keyword); l++) {
      uint8_t letter = toupper(pgm_read_byte_near(keyword + l)) - 'A';
      #ifdef KEYBOARD_SPANISH
      if (letter > 13) letter += 1;
      #endif
      const uint8_t row = letter / KEYBOARD_STRIDE;
      const uint8_t col = letter % KEYBOARD_STRIDE;
      // Go to letter
      for (uint8_t y = 0; y < row; y++) send(DOWN, 300);
      for (uint8_t x = 0; x < col; x++) send(RIGHT, 300);
      // Submit letter
      send(ENTER, 500);
      // Go back to A
      for (uint8_t y = 0; y < row; y++) send(UP, 300);
      for (uint8_t x = 0; x < col; x++) send(LEFT, 300);
    }
    // Go to spacebar
    for (uint8_t y = 0; y < KEYBOARD_HEIGHT; y++) {
      send(DOWN, 300);
    }
    if (i < numWords - 1) {
      // Submit spacebar
      send(ENTER, 500);
      // Go back to A
      for (uint8_t y = 0; y < KEYBOARD_HEIGHT; y++) {
        send(UP, 300);
      }
    } else {
      // Go to search button
      send(RIGHT, 300);
      // Submit search
      send(ENTER, 1500);
      // Navigate to first video
      send(DOWN, 300);
      // Play video
      send(ENTER, 500);
    }
  }
}

void onButton(const uint8_t button) {
  switch (button) {
    case 0:
      // Primary button
      // Power up the TV
      send(POWER, 15000);
      // Keep it quiet while it loads up
      send(MUTE, 20000);
      // Preload smart hub
      send(SMART_HUB, 10000);
      send(LEFT, 1000);
      send(ENTER, 50000);
      // TV should be ready
      openYoutube();
      // Give it some extra time to load youtube for the first time
      send(MUTE, 40000);
      break;
    case 1:
      // Secondary button
      // TV is already powered & ready
      openYoutube();
      break;
  }
  playSomeMusic();
}

void loop() {
  for (uint8_t i = 0; i < 2; i++) {
    // Update button state
    const uint8_t read = digitalRead(buttons[i].pin);
    const uint32_t now = millis();
    if (buttons[i].read != read) {
      buttons[i].debounce = now;
    }
    if ((now - buttons[i].debounce) > 10) {
      if (buttons[i].state != read) {
        buttons[i].state = read;
        if (read == LOW) {
          onButton(i);
        }
      }
    }
    buttons[i].read = read;
  }
}
