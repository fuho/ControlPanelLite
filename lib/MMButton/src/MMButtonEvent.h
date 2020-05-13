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

*/

#ifndef MMBUTTONEVENT_H
#define MMBUTTONEVENT_H


enum class MMButtonEventType {
    DOWN, DOWN_LONG, CONTINUE, UP, PRESS, PRESS_LONG, PRESS_DOUBLE, PRESS_CANCELLED
};

class MMButtonEvent {
public:
    MMButtonEventType type;
    int pin;
    unsigned long millis;
    unsigned long micros;
    unsigned long duration;
    unsigned long count;

    explicit MMButtonEvent(
        MMButtonEventType type,
        int pin,
        unsigned long millis,
        unsigned long micros,
        unsigned long duration,
        unsigned long count
    );

    char *repr();
};

#endif //MMBUTTONEVENT_H
