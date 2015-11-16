/*
 * Adventskalender - 2015 by <ms@mallorn.de>
 */

#include <errno.h>
#include <util/atomic.h>
//#include <NewRemoteReceiver.h>
#include <EEPROM.h>
#include <FastLED.h>

#define PIN_RADIO_RX 2
#define PIN_MOTION_DETECTOR 4
#define PIN_CLOCK_INTERRUPT_PWM 9
#define PIN_CLOCK_INTERRUPT_INT 3

#define INT_RADIO_RX 0
#define INT_CLOCK 1
 
#define REMOTE_ADDRESS 17612030
 
#define ticksPerSecond 979  // 979 for most PWM pins

/* clock values, see tab 'clock' */
uint64_t time = 0; // <-- this is the time copied in loop() from clockInterruptTime, so code can rely on it not to change within a loop
uint64_t ticks = 0;
uint32_t day_count = 0; // days since epoch start
uint64_t midnight_today = 0; // seconds from epoch start to today 0:00 UTC
uint32_t time_today = 0; // seconds since today 0:00 UTC
char time_h = 0;
char time_m = 0;
char time_s = 0;
int dayOfWeek = -1; // sunday is 0

#define MODE_ON  1
#define MODE_OFF 0

#define FIRST_DAY 1
#define LAST_DAY 24

#define FIRST_PATTERN 1
#define PATTERN_SPARKLE 1
#define PATTERN_RAINBOW 2
#define PATTERN_FIRE 3
#define PATTERN_NOISE 4
#define LAST_PATTERN 4

#define EEPROM_MODE    0
#define EEPROM_DAY     1
#define EEPROM_PATTERN 2
#define EEPROM_BRIGHTNESS 3
#define EEPROM_AUTO_OFF_DELAY 4
#define EEPROM_STAY_ON_START 5
#define EEPROM_STAY_ON_END 6
int mode;
int day;
int pattern;
int brightness;
int autoOffDelay; // in minutes
int stayOnStart; // hour UTC
int stayOnEnd; // hour UTC

uint64_t mode_on_ticks = 0;
void setMode(int newMode) {
  mode = newMode;
  //EEPROM.write(EEPROM_MODE, newMode);
  if (newMode == MODE_ON) {
    mode_on_ticks = ticks;
  }
}

int64_t getModeOnSeconds() {
  if (mode == MODE_ON) {
    return (ticks - mode_on_ticks) / ticksPerSecond;
  } else {
    return 0;
  }
}

void setDay(int newDay) {
  day = newDay;
  EEPROM.write(EEPROM_DAY, newDay);
}

void setPattern(int newPattern) {
  pattern = newPattern;
  EEPROM.write(EEPROM_PATTERN, newPattern);
}

void setAutoOffDelay(int newAutoOffDelay) {
  autoOffDelay = newAutoOffDelay;
  EEPROM.write(EEPROM_AUTO_OFF_DELAY, newAutoOffDelay);
}

void setStayOnStart(int newStayOnStart) {
  stayOnStart = newStayOnStart;
  EEPROM.write(EEPROM_STAY_ON_START, newStayOnStart);
}

void setStayOnEnd(int newStayOnEnd) {
  stayOnEnd = newStayOnEnd;
  EEPROM.write(EEPROM_STAY_ON_END, newStayOnEnd);
}

void setBrightness(int newBrightness) {
  brightness = newBrightness;
  EEPROM.write(EEPROM_BRIGHTNESS, newBrightness);
  FastLED.setBrightness(brightness);
}

void setup() {
  Serial.begin(9600);
  Serial.println("* Adventskalender 0.9");
  
  setupClockInterrupt();

  brightness = EEPROM.read(EEPROM_BRIGHTNESS);
  setupLeds();
  
  pinMode(PIN_MOTION_DETECTOR, INPUT);
 
//  NewRemoteReceiver::init(INT_RADIO_RX, PIN_RADIO_RX, processCode);
  
  mode = MODE_ON /* EEPROM.read(EEPROM_MODE) */;
  if (mode != MODE_OFF && mode != MODE_ON) {
    setMode(MODE_OFF);
  }
  day = EEPROM.read(EEPROM_DAY);
  if (day < FIRST_DAY || day > LAST_DAY) {
    setDay(1);
  }
  pattern = EEPROM.read(EEPROM_PATTERN);
  if (pattern < FIRST_PATTERN || pattern > LAST_PATTERN) {
    setPattern(FIRST_PATTERN);
  }
  autoOffDelay = EEPROM.read(EEPROM_AUTO_OFF_DELAY);
  if (autoOffDelay < 1 || autoOffDelay > 254) {
    setAutoOffDelay(5);
  }
  stayOnStart = EEPROM.read(EEPROM_STAY_ON_START);
  if (stayOnStart > 23) {
    setStayOnStart(7);
  }
  stayOnEnd = EEPROM.read(EEPROM_STAY_ON_END);
  if (stayOnEnd > 23) {
    setStayOnEnd(17);
  }
}

void loop() {
  loopClock();
  loopSerial();
  loopMotion();
  loopLeds();
}

// Callback function is called only when a valid code is received.
/*
void processCode(NewRemoteCode receivedCode) {
  if (receivedCode.address == REMOTE_ADDRESS) {
    if (receivedCode.groupBit) {
      if (receivedCode.switchType == NewRemoteCode::off) {
        setMode(MODE_OFF);
      }
    } else {
      switch (receivedCode.unit) {
        case 0:
          setMode(receivedCode.switchType == NewRemoteCode::on ? MODE_ON : MODE_OFF);
          break;
        case 1: {
          int newDay = day + (receivedCode.switchType == NewRemoteCode::on ? 1 : -1);
          if (newDay < FIRST_DAY) {
            newDay = LAST_DAY;
          } else if (newDay > LAST_DAY) {
            newDay = FIRST_DAY;
          }
          setDay(newDay);
          }
          break;
        case 2: {
          int newPattern = pattern + (receivedCode.switchType == NewRemoteCode::on ? 1 : -1);
          if (newPattern < FIRST_PATTERN) {
            newPattern = LAST_PATTERN;
          } else if (newPattern > LAST_PATTERN) {
            newPattern = FIRST_PATTERN;
          }
          setPattern(newPattern);
          }
          break;
      }
    }
  }
}
*/
