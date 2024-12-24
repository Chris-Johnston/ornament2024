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
void debugPattern();

typedef void (*PatternFunction)();

PatternFunction patternList[] = {
    &debugPattern,
    &spinPattern,
    &defaultPattern,
    &randomBlinkPattern,
    &offPattern,
};

// wasn't sure if this was breaking something
// #define NUM_PATTERNS (sizeof(patternList) / sizeof(PatternFunction))

#define NUM_PATTERNS 5
