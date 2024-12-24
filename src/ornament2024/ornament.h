#pragma once

#define NUM_OUT_PINS 6

#define PIN_BUTTON 2
#define PINS_CLOCKWISE {3, 0, 1, 10, 9, 8}

// list of patterns
// #define NUM_PATTERNS 1

void offPattern();
void randomBlinkPattern();
void snowPattern();
void defaultPattern();
void spinPattern();
void spinReversePattern();
void debugPattern();
void sineSpinPattern();
void decayPattern();
void vSpinPattern();
void clockPattern();
void solidFullBrightnessPattern();
void halfBrightnessPattern();
void quarterBrightnessPattern();

typedef void (*PatternFunction)();

PatternFunction patternList[] = {
    // &debugPattern,
    // &sineSpinPattern,
    &snowPattern, // this is a good one
    &decayPattern, // and this is good too
    &clockPattern,
    &vSpinPattern,
    &spinPattern,
    &spinReversePattern,
    // &defaultPattern,
    &randomBlinkPattern,
    &solidFullBrightnessPattern,
    &halfBrightnessPattern,
    &quarterBrightnessPattern,
    // &offPattern,
};

// wasn't sure if this was breaking something
#define NUM_PATTERNS (sizeof(patternList) / sizeof(PatternFunction))

// #define NUM_PATTERNS 5
