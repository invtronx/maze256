#include <LedControl.h>
#include <FastLED.h>

#include "Comms.hpp"
#include "Compass.hpp"
#include "Direction.hpp"
#include "Macros.hpp"
#include "Processor.hpp"
#include "Renderer.hpp"
#include "State.hpp"

// Global Variables
CRGB leds[VISION_NUM_LEDS];
LedControl compassDisplay(COMPASS_DIN, COMPASS_CLK, COMPASS_CS, 1);
Renderer renderer(compassDisplay);
State &state = State::getInstance();

// Function Declarations
static void listen();

void setup() {
  Serial.begin(115200);

  FastLED.addLeds<NEOPIXEL, VISION_DATA_PIN>(leds, VISION_NUM_LEDS);
  FastLED.setBrightness(16);                            // [0, 255]
  compassDisplay.shutdown(COMPASS_LED_INDEX, false);
  compassDisplay.setIntensity(COMPASS_LED_INDEX, 8);    // [0, 15]
  compassDisplay.clearDisplay(COMPASS_LED_INDEX);
}

void loop() {
  listen();

  static unsigned long timeout = 0;
  if (millis() > timeout) {
    renderer.renderWideVision(state.wideVision);
    renderer.renderCompass(state.compassDegree);
    FastLED.show();
    timeout = millis() + 200;
  }
}

static void listen() {
  static byte content[64];
  static int contentSz = 0;
  if (Serial.available() > 0) {
    Comms::Type type = Comms::process(content, contentSz);
    if (type != Comms::Type::Invalid)
      Processor::process(type, content, contentSz);
  }
}
