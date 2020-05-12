/*
MMButton library
===============
version 0.0.1 (May 2020)
copyright (c) 2020 fuho
https://github.com/fuho

This program is free software: you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free Software
Foundation, either version 3 of the License, or (at your option) any later
version. This program is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License at
http://www.gnu.org/licenses .

This library uses VirtualDelay library
from http://www.avdweb.nl/arduino/libraries/virtualdelay.html
*/

#include "MMButton.h"
#include <Arduino.h>


#pragma clang diagnostic push
#pragma ide diagnostic ignored "UnusedGlobalDeclarationInspection"

MMButton::MMButton(int pin, ListenerType listener) {
  _pin = pin;
  _listener = listener;
  pinMode(pin, INPUT_PULLUP);
  _buttonState = getRawState();
}

MMButton::MMButton(
    int pin,
    ListenerType listener,
    unsigned long debounceMs,
    unsigned long longDownMs,
    unsigned long continueRateMs,
    unsigned long doublePressMaxMs,
    unsigned long cancelPressMinMs
) : _pin(pin),
    _debounceMs(debounceMs),
    _longDownMs(longDownMs),
    _continueRateMs(continueRateMs),
    _doublePressMaxMs(doublePressMaxMs),
    _cancelPressMinMs(cancelPressMinMs) {}

void log(const String &text) {
  //Serial.print(millis());
  //Serial.println(" " + text);
}

void MMButton::tick() {
  if (_shouldUpdate()) {
    _update();
  }
}

bool MMButton::_shouldUpdate() const {
  if (_lastCheckMs + _debounceMs < millis()) {
    return true;
  }
  // Cover the case where every 0xffffffffms (49.71027 days) the millis() counter overflows :)
  return millis() < _lastCheckMs && (0xffffffff - _lastCheckMs + millis()) >= _debounceMs;
}

void MMButton::_emitEvent(MMButtonEvent event) {
  if (_listener) {
    _listener(event);
  }
}

void MMButton::_update() {
  //TODO: Implement; state
  MMButtonState raw = getRawState();
  _lastCheckMs = millis();
  switch (_buttonState) {
    case MMButtonState::UP:
      log("...UP");
      if (raw == MMButtonState::UP) {
        // Button stays UP, do nothing
        return;
      } else if (raw == MMButtonState::DOWN) {
        _buttonState = MMButtonState::UP_DOWN;
        return;
      }
      break;
    case MMButtonState::UP_DOWN:
      log("...UP_DOWN");
      if (raw == MMButtonState::UP) {
        // Cancelling transition from  UP to DOWN
        _buttonState = MMButtonState::UP;
      } else {
        // Button successfully flipped from UP to DOWN
        _buttonState = MMButtonState::DOWN;
        _downCount++;
        _lastDown = millis();
        _lastDownMicros = micros();
        _emitEvent(MMButtonEvent(MMButtonEventType::DOWN, _lastDown, _lastDownMicros, 0, _downCount));
        return;
      }
      break;
    case MMButtonState::DOWN:
      log("...DOWN");
      if (raw == MMButtonState::UP) {
        // Entering transition from DOWN to UP
        _buttonState = MMButtonState::DOWN_UP;
      } else {
        // Button stays DOWN check how long has it been and act accordingly
        if (millis() - _lastDown > _longDownMs) {
          _buttonState = MMButtonState::DOWN_LONG;
          _continueCount = 0;
          _lastDownLong = millis();
          _lastDownLongMicros = micros();
          _emitEvent(
              MMButtonEvent(MMButtonEventType::DOWN_LONG, _lastDownLong, _lastDownLongMicros, _lastDown - _lastDownLong,
                            _downLongCount));
        }
        return;
      }
      break;
    case MMButtonState::DOWN_LONG:
      log("...DOWN_LONG");
      if (raw == MMButtonState::UP) {
        // Entering transition from DOWN_LONG to UP
        _buttonState = MMButtonState::DOWN_UP;
      } else {
        // Button stays DOWN check how long has it been and act accordingly
        if (_lastDownLong + _continueRateMs * (1 + _continueCount) <= millis()) {
          // do nothing
        } else {
          unsigned long duration1;
          if (_continueCount) {
            duration1 = _lastDownLong;
          } else {
            duration1 = _lastContinue;
          }
          _lastContinue = millis();
          _lastContinueMicros = micros();
          _continueCount++;
          _emitEvent(
              MMButtonEvent(MMButtonEventType::CONTINUE, _lastContinue, _lastContinueMicros, duration1, _continueCount)
          );

        }
        return;
      }
      break;
    case MMButtonState::DOWN_UP:
      log("...DOWN_UP");
      if (raw == MMButtonState::DOWN) {
        // Cancelling transition from  DOWN to UP
        _buttonState = MMButtonState::DOWN;
      } else {
        // Button successfully flipped from DOWN to UP
        _buttonState = MMButtonState::UP;
        _upCount++;
        _lastUp = millis();
        _lastUpMicros = micros();
        bool isLongPress = false;
        bool isDoublePress = false;
        unsigned long duration = _lastUp - _lastDown;
        if (duration > _longDownMs) {
          // If last press was long press
          isLongPress = true;
          _pressLongCount++;
          _lastPressLong = _lastUp;
          _lastPressLongMicros = _lastUpMicros;
        }
        if (_lastUp - _lastPress > _doublePressMaxMs) {
          isDoublePress = true;
          _pressDoubleCount++;
          _lastPressDouble = _lastUp;
          _lastPressDoubleMicros = _lastUpMicros;
        }
        _lastPress = _lastUp;
        _upCount++;
        _emitEvent(MMButtonEvent(MMButtonEventType::UP, _lastUp, _lastUpMicros, 0, _upCount));
        _pressCount++;
        _emitEvent(MMButtonEvent(MMButtonEventType::PRESS, _lastUp, _lastUpMicros, duration, _pressCount));
        if (isLongPress) {
          _emitEvent(
              MMButtonEvent(MMButtonEventType::PRESS_LONG, _lastUp, _lastUpMicros, duration, _pressLongCount)
          );
        }
        if (isDoublePress) {
          _emitEvent(
              MMButtonEvent(MMButtonEventType::PRESS_DOUBLE, _lastUp, _lastUpMicros, duration, _pressDoubleCount)
          );
        }
      }
      return;
      break;
  }
}


MMButtonState MMButton::getRawState() const {
  if (digitalRead(_pin)) {
    log("raw: UP");
    return MMButtonState::UP;
  } else {
    log("raw: DOWN");
    return MMButtonState::DOWN;
  }
}

bool MMButton::isDown() {
  return _buttonState == MMButtonState::DOWN;
}

bool MMButton::isUP() {
  return _buttonState == MMButtonState::UP;
}

void MMButton::setDebounceTime(unsigned long debounceMs) {
  _debounceMs = debounceMs;
  tick();
}

void MMButton::setLongPressMs(unsigned long longPressMs) {
  _longDownMs = longPressMs;
  tick();
}

void MMButton::setContinueRateMs(unsigned long continueRateMs) {
  _continueRateMs = continueRateMs;
  tick();
}

void MMButton::setDoublePressMs(unsigned long doublePressMs) {
  _doublePressMaxMs = doublePressMs;
  tick();
}

void MMButton::setCancelPressMs(unsigned long cancelPressMs) {
  _cancelPressMinMs = cancelPressMs;
  tick();
}

void MMButton::resetPressCounter() {
  _pressCount = 0;
  tick();
}

unsigned long MMButton::getStableMs() {
  tick();
  switch (_buttonState) {
    case MMButtonState::UP:
      //TODO: Cover for millis() overflow
      return millis() - _lastUp;
    case MMButtonState::UP_DOWN:
      return 0;
    case MMButtonState::DOWN:
      //TODO: Cover for millis() overflow
      return millis() - _lastDown;
    case MMButtonState::DOWN_UP:
      return 0;
    case MMButtonState::DOWN_LONG:
      return millis() - _lastDownLong;
  }
}

unsigned long MMButton::getPressCount() {
  tick();
  return _pressCount;
}

void MMButton::setEventListener(ListenerType listener) {
  _listener = listener;
}

void MMButton::removeEventListener() {
  _listener = nullptr;
}


// MMButton Event
MMButtonEvent::MMButtonEvent(
    MMButtonEventType type,
    unsigned long millis,
    unsigned long micros,
    unsigned long duration,
    unsigned long count
) :
    type(type),
    millis(millis),
    micros(micros),
    duration(duration),
    count(count) {}

#pragma clang diagnostic pop
