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

#pragma clang diagnostic push
#pragma ide diagnostic ignored "UnusedStructInspection"
#pragma ide diagnostic ignored "UnusedGlobalDeclarationInspection"

#ifndef MMButton_h
#define MMButton_h

#define DEBUG true

#include <Arduino.h>
#include "../lib/VirtualDelay/avdweb_VirtualDelay.h"

#define SFX_END "END"
#define SFX_REPEAT "REP"

enum class MMButtonEventType {
    DOWN, DOWN_LONG, CONTINUE, UP, PRESS, PRESS_LONG, PRESS_DOUBLE, PRESS_CANCELLED
};

enum class MMButtonState {
    UP, UP_DOWN, DOWN, DOWN_LONG, DOWN_UP
};


class MMButtonEvent {
public:
    MMButtonEventType type;
    unsigned long millis;
    unsigned long micros;
    unsigned long duration;
    unsigned long count;

    explicit MMButtonEvent(
        MMButtonEventType type,
        unsigned long _millis,
        unsigned long _micros,
        unsigned long _duration,
        unsigned long _count
    );

};

extern "C" {
typedef void (*ListenerType)(MMButtonEvent);
}

class MMButton {
public:
    void tick();

    explicit MMButton(int pin, ListenerType listener);

    MMButton(
        int pin,
        ListenerType listener,
        unsigned long debounceMs,
        unsigned long longDownMs,
        unsigned long continueRateMs,
        unsigned long doublePressMaxMs,
        unsigned long cancelPressMinMs
    );


    /**
     * @return  Returns true if the button is currently depressed
     */
    bool isDown();

    /**
     * @return  Returns true if the button is NOT currently depressed
     */
    bool isUP();

    /**
     * Set minimum amount of time that has to pass between two consecutive identical states for
     * it to be registered as an event. The time can be longer depending on how often tick() is called.
     * @param debounceMs debounce time in milliseconds
     */
    void setDebounceTime(unsigned long debounceMs);

    /**
     * Configure up to how many milliseconds can pass between first and second press to count it as double press
     * @param longPressMs
     */
    void setLongPressMs(unsigned long longPressMs);

    /**
     * Configure how many milliseconds the button has to be held down after LONG_DOWN/CONTINUE to emit CONTINUE event
     * @param repeatRateMs
     */
    void setContinueRateMs(unsigned long continueRateMs);

    /**
     * Configure up to how many milliseconds can pass between two UP events to emit DOUBLE_PRESS
     * @param doublePressMs
     */
    void setDoublePressMs(unsigned long doublePressMs);

    /**
     * Configure how many milliseconds had the button stay pressed for PRESS_CANCELLED to be emitted
     * @param cancelPressMs
     */
    void setCancelPressMs(unsigned long cancelPressMs);


    /**
     * @return How many milliseconds has the button been unchanged
     */
    unsigned long getStableMs();

    /**
     * @return How many times has the button been PRESSED since inception or last reset
     */
    unsigned long getPressCount();

    /**
     * Reset the PRESS counter
     */
    void resetPressCounter();

    void setEventListener(ListenerType);

    void removeEventListener();


private:
    int _pin;
    unsigned long _lastCheckMs = millis();
    unsigned long _debounceMs = 5;
    unsigned long _longDownMs = 500;
    unsigned long _continueRateMs = 250;
    unsigned long _doublePressMaxMs = 500;
    unsigned long _cancelPressMinMs = 10000;

    unsigned long _lastDown = 0;
    unsigned long _lastDownMicros = 0;
    unsigned long _downCount = 0;

    unsigned long _lastDownLong = 0;
    unsigned long _lastDownLongMicros = 0;
    unsigned long _downLongCount = 0;


    unsigned long _lastContinue = 0;
    unsigned long _lastContinueMicros = 0;
    unsigned long _continueCount = 0;

    unsigned long _lastUp = 0;
    unsigned long _lastUpMicros = 0;
    unsigned long _upCount = 0;

    unsigned long _lastPress = 0;
    unsigned long _lastPressMicros = 0;
    unsigned long _pressCount = 0;

    unsigned long _lastPressLong = 0;
    unsigned long _lastPressLongMicros = 0;
    unsigned long _pressLongCount = 0;

    unsigned long _lastPressDouble = 0;
    unsigned long _lastPressDoubleMicros = 0;
    unsigned long _pressDoubleCount = 0;

    unsigned long _lastPressCancelled = 0;
    unsigned long _lastPressCancelledMicros = 0;
    unsigned long _pressCancelledCount = 0;

    MMButtonState _buttonState;


    ListenerType _listener;

    bool _shouldUpdate() const;

    void _update();

    void _emitEvent(MMButtonEvent event);

    MMButtonState getRawState() const;
};

#endif

#pragma clang diagnostic pop