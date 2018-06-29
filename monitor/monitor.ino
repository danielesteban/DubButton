// This is a simple sketch to debug/monitor the IR output of the DubButton.
// Upload it to any arduino board that has a hardware serial,
// Connect an IR Receiver to it and open the serial monitor.

#include <IRremote.h>

#define RECEIVER_PIN 11
IRrecv irrecv(RECEIVER_PIN);

// Samsung TV IR codes
#define POWER 0xE0E040BF
#define MUTE 0xE0E0F00F
#define SMART_HUB 0xE0E09E61
#define UP 0xE0E006F9
#define LEFT 0xE0E0A659
#define RIGHT 0xE0E046B9
#define DOWN 0xE0E08679
#define ENTER 0xE0E016E9

void setup() {
  // Start receiving codes
  Serial.begin(115200);
  irrecv.enableIRIn();
}

void loop() {
  decode_results results;
  if (irrecv.decode(&results)) {
    if (results.decode_type == SAMSUNG) {
      // Dump the code
      switch (results.value) {
        case POWER:
          Serial.println("POWER");
          break;
        case MUTE:
          Serial.println("MUTE");
          break;
        case SMART_HUB:
          Serial.println("SMART_HUB");
          break;
        case UP:
          Serial.println("UP");
          break;
        case LEFT:
          Serial.println("LEFT");
          break;
        case RIGHT:
          Serial.println("RIGHT");
          break;
        case DOWN:
          Serial.println("DOWN");
          break;
        case ENTER:
          Serial.println("ENTER");
          break;
        default:
          Serial.println("UNKNOWN");
          break;
      }
    }
    irrecv.resume();
  }
}
