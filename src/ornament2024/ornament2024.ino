// ornament 2024 code

// This code is messy - I do not care. You have been warned.

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <avr/pgmspace.h>
#include <pins_arduino.h>
#include "ornament.h"

#define PIN_OUTPUT_COUNT 6
#define PIN_BUTTON 2

#undef MILLIS_USE_TIMERB0

// Clockwise starting from the top
int pins[] = {
  3, 0, 1, 10, 9, 8
};

// I broke the millis() timer when setting up the ISR
// and I do not care enough to fix it.
// This is fine.
// nevermind I wasn't using TIMERA0 anyways so reverted this change

#define CORRECTED_MILLIS (millis())
#define CORRECTED_DELAY(ms) (delay(ms))

// 8       3
// 9 tree  0
// 10      1

unsigned long lastDebounceTime = 0;
int pattern = 0;

void setupIsr();
void clear();

void setup() {
  
  for (int i = 0; i < PIN_OUTPUT_COUNT; i++)
  {
    pinMode(pins[i], OUTPUT);
  }
  pinMode(PIN_BUTTON, INPUT_PULLUP);

  setupIsr();

  // TODO: look into using the VREF features so that I can see how drained the battery is
}

int lastButtonState = 0;
int buttonState = 0;

// state used for software PWM
uint8_t pwmTimers[PIN_OUTPUT_COUNT];
bool pwmTimersEnabled[PIN_OUTPUT_COUNT];
int pwmTimer = 0;

void setPwm(int pin, bool pwmEnabled, int pwmRate)
{
  pwmTimersEnabled[pin] = pwmEnabled;
  if (pwmEnabled)
  {
    pwmTimers[pin] = pwmRate;
  }
  else {
    pwmTimers[pin] = 0;
  }
}

ISR(TCB0_INT_vect) {

  pwmTimer = (pwmTimer + 1) % 255;
  uint8_t portASet = 0, portAClear = 0;
  
  for (int i = 0; i < PIN_OUTPUT_COUNT; i++) {
    // preserve whatevers already in the register
    if (pwmTimersEnabled[i] == false) continue;
    
    if (pwmTimer < pwmTimers[i]) {
      portASet |= digital_pin_to_bit_mask[pins[i]];
    }
    else {
      portAClear |= digital_pin_to_bit_mask[pins[i]];
    }
  }

  PORTA.OUTSET = portASet;
  PORTA.OUTCLR = portAClear;

  TCB0.INTFLAGS = 0; // clear interrupt
}

ISR(PORTA_PORT_vect) {
  byte flags = PORTA.INTFLAGS;
  PORTA.INTFLAGS |= flags;
  
  if ((CORRECTED_MILLIS - lastDebounceTime) > 50) {

    pattern = (pattern + 1) % NUM_PATTERNS;
    clear();

    lastDebounceTime = CORRECTED_MILLIS;
  }
}

#define TCB0_CTRLA_ENABLE 1
#define TCB0_CTRLA_CLKSEL_1 0 // CLK_PER
#define TCB0_CTRLA_CLKSEL_half 1 // CLK_PER / 2
#define TCB0_CTRLB_ASYNC 0b01000000
#define TCB0_CTRLB_PERIODIC_INTERRUPT_MODE 0b00

void setupIsr() {
  TCB0.CCMP = 0xff;
  TCB0.CTRLB = TCB0_CTRLB_ASYNC | TCB0_CTRLB_PERIODIC_INTERRUPT_MODE;
  TCB0.INTCTRL = 1 ; // no capture interrupt
  TCB0.CTRLA = TCB0_CTRLA_CLKSEL_half | TCB0_CTRLA_ENABLE;
  
  // pin 2 interrupt
  // bit 7 invert I/O
  // pull up enabled (bit 3)
  // bits 2:0 - sense falling edge
  // PORTA_PIN2CTRL = 0b1000 | 0x3;

  // pin 2 is actually pa6
  PORTA.INTFLAGS = (1 << 6);// 0b100;
  PORTA.PIN6CTRL = 0b10000000 | 0b1000 | 0b11;
  VPORTA.INTFLAGS = (1 << 6);
  // PORTA.INTCTRL = 1;
  // PORTA.PIN2CTRL = 0b1011;
}

// this was necessary when I didn't use the right value of resistor
// but has been fixed now, should be fine to use the full range of resistor
// without burning anyone's eyes
// additionally this prevents LEDs being on way too bright if the board browns out or
// gets stuck.

// anything beyond this is too dang bright
// #define MAX_PWM_BRIGHTNESS 50

void showPattern()
{
  (*patternList[pattern])();
}

void loop()
{
  showPattern();
}

void clear()
{
  for (int i = 0; i < PIN_OUTPUT_COUNT; i++) {
    setPwm(i, false, 0);
  }
  PORTA.OUT = 0;
}

void offPattern()
{
  // nothing - no point in doing this
  clear();
  delay(100);
}

void randomBlinkPattern() {
  clear();
  setPwm(rand() % PIN_OUTPUT_COUNT, true, rand() % 255);
  delay(50);
}

void defaultPattern() {
  for (int i = 0; i < PIN_OUTPUT_COUNT; i++) {
    setPwm(i, true, (i % 2 == 1) ? 255 : 128);
  }
  delay(25);
}

void spinPattern() {

  delay(25);
  return;
  clear();
  int on = (CORRECTED_MILLIS / 1000) % PIN_OUTPUT_COUNT;
  digitalWrite(pins[on], HIGH);
}

void debugPattern() {
  bool state = (CORRECTED_MILLIS / 1000) % 2;
  digitalWrite(pins[0], state);

  // digitalWrite(pins[1], test);
  // digitalWrite(pins[2], test);
}