#include <Arduino.h>
#include <config.h>

#include <MMButton.h>
#include "MMButtonEvent.h"


void onButtonEvent(MMButtonEvent event) {
  Serial.println(event.repr());
/*
  switch (event.type) {

    case MMButtonEventType::DOWN:
      Serial.println("down");
      break;
    case MMButtonEventType::DOWN_LONG:
      Serial.println("down_long");
      break;
    case MMButtonEventType::CONTINUE:
      Serial.println("continue");
      break;
    case MMButtonEventType::UP:
      Serial.println("up");
      break;
    case MMButtonEventType::PRESS:
      Serial.println("press");
      break;
    case MMButtonEventType::PRESS_LONG:
      Serial.println("press_long");
      break;
    case MMButtonEventType::PRESS_DOUBLE:
      Serial.println("press_double");
      break;
    case MMButtonEventType::PRESS_CANCELLED:
      Serial.println("press_cancelled");
      break;
  }
*/
}

MMButton red = MMButton(BUTTON_RED, onButtonEvent);
MMButton green = MMButton(BUTTON_GREEN, onButtonEvent);
MMButton blue = MMButton(BUTTON_BLUE, onButtonEvent);
MMButton yellow = MMButton(BUTTON_YELLOW, onButtonEvent);


void setup() {
  Serial.begin(115200);
  Serial.println("SET-UP");
}

void loop() {
  red.tick();
  green.tick();
  blue.tick();
  yellow.tick();
}
