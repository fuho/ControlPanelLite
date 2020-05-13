/*
MMButton library
===============
version 1.0.0 (May 2020)
copyright (c) 2020 fuho
https://github.com/fuho

This program is free software: you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free Software
Foundation, either version 3 of the License, or (at your option) any later
version. This program is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License at
http://www.gnu.org/licenses .
*/

#include <Arduino.h>
#include "MMButton.h"
#include "MMButtonEvent.h"

char data[64];


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
      sprintf(data, "%2d:%-15s%11lu%11lu%11lu%11lu", pin, "DOWN", millis, micros, duration, count);
      break;
    case MMButtonEventType::DOWN_LONG:
      sprintf(data, "%2d:%-15s%11lu%11lu%11lu%11lu", pin, "DOWN_LONG", millis, micros, duration, count);
      break;
    case MMButtonEventType::CONTINUE:
      sprintf(data, "%2d:%-15s%11lu%11lu%11lu%11lu", pin, "CONTINUE", millis, micros, duration, count);
      break;
    case MMButtonEventType::UP:
      sprintf(data, "%2d:%-15s%11lu%11lu%11lu%11lu", pin, "UP", millis, micros, duration, count);
      break;
    case MMButtonEventType::PRESS:
      sprintf(data, "%2d:%-15s%11lu%11lu%11lu%11lu", pin, "PRESS", millis, micros, duration, count);
      break;
    case MMButtonEventType::PRESS_LONG:
      sprintf(data, "%2d:%-15s%11lu%11lu%11lu%11lu", pin, "PRESS_LONG", millis, micros, duration, count);
      break;
    case MMButtonEventType::PRESS_DOUBLE:
      sprintf(data, "%2d:%-15s%11lu%11lu%11lu%11lu", pin, "PRESS_DOUBLE", millis, micros, duration, count);
      break;
    case MMButtonEventType::PRESS_CANCELLED:
      sprintf(data, "%2d:%-15s%11lu%11lu%11lu%11lu", pin, "PRESS_CANCELLED", millis, micros, duration, count);
      break;
  }
  return data;
}