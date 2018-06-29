#include <Arduino.h>
#include "ir.h"

// I/O config
#define PRIMARY_BUTTON_PIN 1
#define SECONDARY_BUTTON_PIN 0
#define FEEDBACK_LED_PIN 2

// Youtube app keyboard config
#define KEYBOARD_STRIDE 7
#define KEYBOARD_HEIGHT 4
#define KEYBOARD_SPANISH 1 // Set this flag if your youtube app keyboard has an "Ñ"

struct {
  uint8_t pin;
  uint8_t read;
  uint8_t state;
  uint32_t debounce;
} buttons[2] = {{ PRIMARY_BUTTON_PIN, HIGH, HIGH, 0 }, { SECONDARY_BUTTON_PIN, HIGH, HIGH, 0 }};

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

void navigateGrid(const uint8_t col, const uint8_t row, uint8_t& currentCol, uint8_t& currentRow) {
  // Navigate through the keyboard grid
  {
    const int8_t distance = (int8_t) row - currentRow;
    if (distance != 0) {
      const uint32_t direction = distance > 0 ? DOWN : UP;
      for (uint8_t y = 0; y < abs(distance); y++) send(direction, 500);
      currentRow = row;
    }
  }
  {
    const int8_t distance = (int8_t) col - currentCol;
    if (distance != 0) {
      const uint32_t direction = distance > 0 ? RIGHT : LEFT;
      for (uint8_t x = 0; x < abs(distance); x++) send(direction, 500);
      currentCol = col;
    }
  }
}

void openYoutube() {
  // Smart hub
  send(SMART_HUB, 10000);
  // Recent -> Featured
  send(LEFT, 5000);
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
  send(LEFT, 1000);
  send(LEFT, 1000);
  send(UP, 3000);
  // Navigate to keyboard
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
  uint8_t col = 0;
  uint8_t row = 0;
  const uint8_t numWords = min(3, num_keywords);
  for (uint8_t i = 0; i < numWords; i++) {
    // Fetch keyword
    const char* keyword = pgm_read_word(&(keywords[query[i]]));
    for (uint8_t l = 0; l < strlen_P(keyword); l++) {
      uint8_t letter = toupper(pgm_read_byte_near(keyword + l)) - 'A';
      #ifdef KEYBOARD_SPANISH
      // Skip "Ñ"
      if (letter > 13) letter++;
      #endif
      // Navigate to letter
      navigateGrid(letter % KEYBOARD_STRIDE, letter / KEYBOARD_STRIDE, col, row);
      // Submit letter
      send(ENTER, 500);
    }
    // Navigate to spacebar
    navigateGrid(col, KEYBOARD_HEIGHT, col, row);
    if (i < numWords - 1) {
      // Submit spacebar
      send(ENTER, 500);
    }
  }
  // Navigate to search button
  send(RIGHT, 500);
  // Submit search
  send(ENTER, 3000);
  // Play video
  send(ENTER, 0);
}

void onButton(const uint8_t button) {
  switch (button) {
    case 0:
      // Primary button
      // Power up the TV
      send(POWER, 15000);
      // Keep it quiet while it loads up
      send(MUTE, 60000);
      // Preload smart hub
      send(SMART_HUB, 15000);
      send(LEFT, 5000);
      send(ENTER, 40000);
      // TV should be ready
      openYoutube();
      // Give it some extra time to load youtube for the first time
      send(MUTE, 20000);
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
  const uint32_t now = millis();
  for (uint8_t i = 0; i < 2; i++) {
    // Update button state
    const uint8_t state = digitalRead(buttons[i].pin);
    if (buttons[i].read != state) {
      buttons[i].debounce = now;
    }
    buttons[i].read = state;
    if (buttons[i].state != state && (now - buttons[i].debounce) > 10) {
      buttons[i].state = state;
      if (state == LOW) {
        onButton(i);
      }
    }
  }
}
