#pragma once

#define NUM_OUT_PINS 6

#define PIN_BUTTON 2
#define PINS_CLOCKWISE {3, 0, 1, 10, 9, 8}

// list of patterns
// #define NUM_PATTERNS 1

void offPattern();
void randomBlinkPattern();
void defaultPattern();
void spinPattern();

typedef void (*PatternFunction)();

PatternFunction patternList[] = {
    &spinPattern,
    &defaultPattern,
    &randomBlinkPattern,
    &offPattern,
};

// #define NUM_PATTERNS (sizeof(patternList) / sizeof(PatternFunction))

#define NUM_PATTERNS 4
