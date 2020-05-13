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

unsigned long ms;
unsigned long us;

void MMButton::_update() {
  ms = millis();
  us = micros();
  _lastCheckMs = ms;
  _lastCheckMicros = us;
  MMButtonState raw = getRawState();

  unsigned long duration;
  switch (_buttonState) {
    case MMButtonState::UP:
      if (raw == MMButtonState::UP) {
        // Button stays UP, do nothing
      } else if (raw == MMButtonState::DOWN) {
        _buttonState = MMButtonState::UP_DOWN;
        _lastUpDownMs = ms;
        _lastUpDownMicros = us;
      }
      break;
    case MMButtonState::UP_DOWN:
      if (raw == MMButtonState::UP) {
        // Cancelling transition from  UP to DOWN
        _buttonState = MMButtonState::UP;
      } else {
        // Button successfully flipped from UP to DOWN
        _buttonState = MMButtonState::DOWN;
        _downCount++;
        _lastDown = ms;
        _lastDownMicros = us;
        _emitEvent(MMButtonEvent(MMButtonEventType::DOWN, _pin, ms, us, 0, _downCount));
      }
      break;
    case MMButtonState::DOWN:
      if (raw == MMButtonState::UP) {
        // Entering transition from DOWN to UP
        _buttonState = MMButtonState::DOWN_UP;
      } else {
        // Button stays DOWN check how long has it been and act accordingly
        if (ms - _lastDown > _longDownMs) {
          _buttonState = MMButtonState::DOWN_LONG;
          _downLongCount++;
          _continueCount = 0;
          _lastDownLong = ms;
          _lastDownLongMicros = us;
          duration = _lastDownLong - _lastDown;
          _emitEvent(MMButtonEvent(MMButtonEventType::DOWN_LONG, _pin, ms, us, duration, _downLongCount));
        }
      }
      break;
    case MMButtonState::DOWN_LONG:
      if (raw == MMButtonState::UP) {
        // Entering transition from DOWN_LONG to UP
        _buttonState = MMButtonState::DOWN_UP;
      } else {
        // Button stays DOWN check how long has it been and act accordingly
        if (ms > (_lastDownLong + (_continueRateMs * (1 + _continueCount)))) {
          // Button down long enough for CONTINUE
          _lastContinue = ms;
          _lastContinueMicros = us;
          duration = ms - _lastContinue;
          _continueCount++;
          _emitEvent(MMButtonEvent(MMButtonEventType::CONTINUE, _pin, ms, us, duration, _continueCount));
        }
      }
      break;
    case MMButtonState::DOWN_UP:
      if (raw == MMButtonState::DOWN) {
        // Cancelling transition from  DOWN to UP
        _buttonState = MMButtonState::DOWN;
        break;
      } else {
        // Button successfully flipped from DOWN to UP
        _buttonState = MMButtonState::UP;

        _lastUp = ms;
        _lastUpMicros = us;
        _upCount++;
        _continueCount = 0;
        duration = ms - _lastDown;


        // Check if it's double press (doing it here before _lastPress overwritten as it's necessary to calculate...
        // ... pressDoubleDuration, but the event has to go out after UP, PRESS and PRESS_LONG
        bool isPressDouble = ms < _lastPress + _doublePressMaxMs;
        unsigned long pressDoubleDuration = 0;
        if (isPressDouble) {
          _lastPressDouble = ms;
          _lastPressDoubleMicros = us;
          pressDoubleDuration = ms - _lastPress;
        }

        // UP
        _emitEvent(MMButtonEvent(MMButtonEventType::UP, _pin, ms, us, 0, _upCount));

        // PRESS
        _lastPress = ms;
        _lastPressMicros = us;
        _pressCount++;
        _emitEvent(MMButtonEvent(MMButtonEventType::PRESS, _pin, ms, us, duration, _pressCount));

        // PRESS_LONG
        if (duration > _longDownMs) {
          // If last press was long press
          _lastPressLong = ms;
          _lastPressLongMicros = us;
          _pressLongCount++;
          _emitEvent(MMButtonEvent(MMButtonEventType::PRESS_LONG, _pin, ms, us, duration, _pressLongCount));
        }

        //PRESS_DOUBLE
        if (isPressDouble) {
          _pressDoubleCount++;
          _emitEvent(
              MMButtonEvent(MMButtonEventType::PRESS_DOUBLE, _pin, ms, us, pressDoubleDuration, _pressDoubleCount)
          );
        }
        break;
      }
  }
}


MMButtonState MMButton::getRawState() const {
  if (digitalRead(_pin)) {
    return MMButtonState::UP;
  } else {
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


#pragma clang diagnostic pop

//char logMsg[128];
//sprintf(logMsg, "_lastPress:%lu, _doublePressMaxMs:%lu, ms:%lu", _lastPress, _doublePressMaxMs, ms);
//Serial.println(logMsg);


