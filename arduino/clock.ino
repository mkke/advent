#include <util/atomic.h>

/* see http://www.instructables.com/id/Make-an-accurate-Arduino-clock-using-only-one-wire/

We are using a PWM output (which has a base freq of 490Hz) to create a signal
and route that to the interrupt input.
*/

volatile int clockInterruptCount = 0;
volatile uint64_t clockInterruptTime = 0; // <-- this is the time changed in the interrupt handler
volatile uint64_t clockInterruptTicks = 0; // <-- this is the time changed in the interrupt handler

void clockInterruptHandler() {
  clockInterruptTicks++;
  clockInterruptCount++;
  if (clockInterruptCount >= ticksPerSecond) {
    clockInterruptTime++;
    clockInterruptCount = 0;
  }
}

void setupClockInterrupt() {
  pinMode(PIN_CLOCK_INTERRUPT_PWM, OUTPUT);
  pinMode(PIN_CLOCK_INTERRUPT_INT, INPUT);
  attachInterrupt(INT_CLOCK, clockInterruptHandler, CHANGE);
  analogWrite(PIN_CLOCK_INTERRUPT_PWM, 127);
}

boolean timeValid = false;
boolean inline isTimeValid() {
  // time is valid after the first time setting it
  return timeValid;
}

void deriveTimeComponents() {
  long t = time_today = time % 86400;
  midnight_today = time - t;
  day_count = time / 86400;
  
  time_h = t / 3600;
  t -= time_h * 3600;
  time_m = (int) t / 60;
  time_s = t - time_m * 60;
  dayOfWeek = (day_count + 4) % 7; // 0 = Sunday
}

uint64_t lastTime = 0;
void loopClock() {
  // time is updated in interrupt, so might change while we derive values;
  // so make a copy
  ATOMIC_BLOCK( ATOMIC_RESTORESTATE ){
    time = clockInterruptTime;
    ticks = clockInterruptTicks;
  }
  
  if (time != lastTime) {
    lastTime = time;
    if (isTimeValid()) {
      deriveTimeComponents();
    }
    
    // if we have no absolute time, switch to MODE_OFF after <autoOffDelay> minutes
    if (!isTimeValid()) {
      if (mode == MODE_ON && getModeOnSeconds() > autoOffDelay * 60) {
        setMode(MODE_OFF);
      }
    } else {
      // we have absolute time, so respect stayOnStart/End
      if (time_h < stayOnStart || time_h >= stayOnEnd || stayOnEnd < stayOnStart || /* weekend */ dayOfWeek == 0 || dayOfWeek == 6) {
        if (mode == MODE_ON && getModeOnSeconds() > autoOffDelay * 60) {
          setMode(MODE_OFF);
        }
      }
    }
  }
}

void setTime(long newTime) {
  ATOMIC_BLOCK( ATOMIC_RESTORESTATE ){
    clockInterruptCount = 0;
    clockInterruptTime = time = newTime;
  }
  timeValid = true;
  deriveTimeComponents();
}
