#ifndef MELODY_H
#define MELODY_H

#include "pins.h"

#include "Arduino.h"
#include "pitches.h"

extern int melody[];
extern int durations[];
void playMelody();

#endif // MELODY_H