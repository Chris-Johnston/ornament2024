// ornament 2024 code

#define PIN_OUTPUT_COUNT 6
#define PIN_BUTTON 2

#undef MILLIS_USE_TIMERB0

int pins[] = {
  3, 0, 1, 10, 9, 8
};

#define CORRECTED_MILLIS (millis() / 8)
#define CORRECTED_DELAY(ms) (delay(ms))

// 8       3
// 9 tree  0
// 10      1

unsigned long lastDebounceTime = 0;
int pattern = 0;

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <avr/pgmspace.h>
#include <pins_arduino.h>
#include "ornament.h"

void setupIsr();

void clear();

// the setup function runs once when you press reset or power the board
void setup() {
  
  for (int i = 0; i < PIN_OUTPUT_COUNT; i++)
  {
    pinMode(pins[i], OUTPUT);
  }
  pinMode(PIN_BUTTON, INPUT_PULLUP);

  setupIsr();

  // look into using the VREF features so that I can see how drained the battery is
}

int lastButtonState = 0;
int buttonState = 0;

void handleDebounce()
{
  // this shit doesn't work and I should just set up the interrupt instead
  bool reading = !digitalRead(PIN_BUTTON);
  if (reading != lastButtonState)
  {
    lastDebounceTime = CORRECTED_MILLIS;
  }
  if ((CORRECTED_MILLIS - lastDebounceTime) > 50)
  {
    if (reading != buttonState)
    {
      buttonState = reading;
      if (reading)
      {
        pattern = (pattern + 1) % NUM_PATTERNS;
        clear();
      }
    }
  }
  lastButtonState = reading;
}

uint8_t pwmTimers[PIN_OUTPUT_COUNT];
bool pwmTimersEnabled[PIN_OUTPUT_COUNT];
int pwmTimer = 0;

// digital_pin_to_bit_mask

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
  // uint8_t portaValue = 0;
  // if bit set, indicates that the value
  // should overwrite whatever's in the register
  /// uint8_t controlMask = 0;
  uint8_t portASet = 0, portAClear = 0;
  
  for (int i = 0; i < PIN_OUTPUT_COUNT; i++) {
    // preserve whatevers already in the register
    if (pwmTimersEnabled[i] == false) continue;
    // this was still showing up as on because of <=
    // if (pwmTimers[i] == 0) continue;

    if (pwmTimer < pwmTimers[i]) {
      portASet |= digital_pin_to_bit_mask[pins[i]];
    }
    else {
      portAClear |= digital_pin_to_bit_mask[pins[i]];
    }
  }

  // PORTA.OUT = (PORTA.OUT & ~controlMask) | (portaValue & controlMask);
  PORTA.OUTSET = portASet;
  PORTA.OUTCLR = portAClear;

  TCB0.INTFLAGS = 0; // clear interrupt
}

#define TCB0_CTRLA_ENABLE 1
#define TCB0_CTRLA_CLKSEL_1 0 // CLK_PER
#define TCB0_CTRLA_CLKSEL_half 1 // CLK_PER / 2
#define TCB0_CTRLB_ASYNC 0b01000000
#define TCB0_CTRLB_PERIODIC_INTERRUPT_MODE 0b00

void setupIsr() {
  // TCA0.CTRLA = 0;
  TCA0.SINGLE.CTRLA = 0b0001;
  // TCA0.SINGLE.PER = 0xff;
  // takeOverTCA0();
  // takeOverTCA1();
  // takeOverTCD0();
  // use TCB0 because TCA0 is in use 
  TCB0.CCMP = 0xff;
  // TCB0_CTRLB_ASYNC
  TCB0.CTRLB = TCB0_CTRLB_ASYNC | TCB0_CTRLB_PERIODIC_INTERRUPT_MODE;
  // set prescaler in TCB0.CRTLA
  TCB0.INTCTRL = 1 ; // no capture interrupt
  TCB0.CTRLA = TCB0_CTRLA_CLKSEL_half | TCB0_CTRLA_ENABLE;
  
}

// anything beyond this is too dang bright
#define MAX_PWM_BRIGHTNESS 50


void showPattern()
{
  (*patternList[pattern])();
}

void loop()
{
  handleDebounce();
  showPattern();

  // init

  // for (int i = 0; i < PIN_OUTPUT_COUNT; i++) {
  //   // pwmTimers[i] = i * ((MAX_PWM_BRIGHTNESS)/PIN_OUTPUT_COUNT);
  //   pwmTimers[i] = (i % 2 == 1) ? 255 : 128;
  //   pwmTimersEnabled[i] = true;
  // }

  return;

  while (true) {
    #ifdef LINEAR
    for (int i = 0; i < PIN_OUTPUT_COUNT; i++) {
      pwmTimers[i] = (pwmTimers[i] + 5) % MAX_PWM_BRIGHTNESS;
      // delay(1);
    }
    #endif

    for (int i = 0; i < PIN_OUTPUT_COUNT; i++) {
      // millis seems to update at the same interval as TCA, interesting
      pwmTimers[i] = (MAX_PWM_BRIGHTNESS / 2) + (MAX_PWM_BRIGHTNESS / 2) * sin(( i * 2 * PI / PIN_OUTPUT_COUNT));
    }

    delay(100);

  }


  // int state = 0;
  // while (true) {
  //   for (pwm_duty = 0; pwm_duty < 175; pwm_duty += 25) {
  //     // I think something about this is firing an interrupt
  //     // but this takes forever
  //     // and so as a result the 1ms delay takes maybe 250 ms
  //     // _delay_ms(1);
  //     // delay(1);
  //     state = ~state;
  //     digitalWrite(0, state);
  //   }
  // }
    // for (int i = 0; i< 6; i++) {
    // digitalWrite(pins[i], HIGH);
    // delay(50);
    // digitalWrite(pins[i], LOW);
    // delay(50);
  // }

  return;
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
  // nothing
  delay(5);
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
  clear();
  // millis is off by 8
  int on = (CORRECTED_MILLIS / 1000) % PIN_OUTPUT_COUNT;
  digitalWrite(pins[on], HIGH);
}