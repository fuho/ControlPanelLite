//
// Created by Ondrej Dolejsi on 5/12/20.
//

#include <Arduino.h>
#include "MMButton.h"
#include "MMButtonEvent.h"

char data[59];


// MMButton Event
MMButtonEvent::MMButtonEvent(
    MMButtonEventType type,
    int pin,
    unsigned long millis,
    unsigned long micros,
    unsigned long duration,
    unsigned long count
) :
    type(type),
    pin(pin),
    millis(millis),
    micros(micros),
    duration(duration),
    count(count) {}

char *MMButtonEvent::repr() {
  //ex: "DOWN      ,     12345,     12345,     12345,         5"
  switch (type) {
    case MMButtonEventType::DOWN:
      sprintf(data, "%2d:%-12s%11lu%11lu%11lu%11lu", pin, "DOWN", millis, micros, duration, count);
      break;
    case MMButtonEventType::DOWN_LONG:
      sprintf(data, "%2d:%-12s%11lu%11lu%11lu%11lu", pin, "DOWN_LONG", millis, micros, duration, count);
      break;
    case MMButtonEventType::CONTINUE:
      sprintf(data, "%2d:%-12s%11lu%11lu%11lu%11lu", pin, "CONTINUE", millis, micros, duration, count);
      break;
    case MMButtonEventType::UP:
      sprintf(data, "%2d:%-12s%11lu%11lu%11lu%11lu", pin, "UP", millis, micros, duration, count);
      break;
    case MMButtonEventType::PRESS:
      sprintf(data, "%2d:%-12s%11lu%11lu%11lu%11lu", pin, "PRESS", millis, micros, duration, count);
      break;
    case MMButtonEventType::PRESS_LONG:
      sprintf(data, "%2d:%-12s%11lu%11lu%11lu%11lu", pin, "PRESS_LONG", millis, micros, duration, count);
      break;
    case MMButtonEventType::PRESS_DOUBLE:
      sprintf(data, "%2d:%-12s%11lu%11lu%11lu%11lu", pin, "PRESS_DOUBLE", millis, micros, duration, count);
      break;
    case MMButtonEventType::PRESS_CANCELLED:
      sprintf(data, "%2d:%-12s%11lu%11lu%11lu%11lu", pin, "type", millis, micros, duration, count);
      break;
  }
  return data;
}