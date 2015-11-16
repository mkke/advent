#define NUM_LEDS 192

/* day  !  led * 8
 * ---------------
 *   1  !   2
 *   2  !   9
 *   3  !  11
 *   4  !   5
 *   5  !   7
 *   6  !   1
 *   7  !   3
 *   8  !  10
 *   9  !   8
 *  10  !   0
 *  11  !   4
 *  12  !   6
 *  13  !  17
 *  14  !  19
 *  15  !  12
 *  16  !  22
 *  17  !  16
 *  18  !  20
 *  19  !  13
 *  20  !  15
 *  21  !  18
 *  22  !  21
 *  23  !  23
 *  24  !  14
 */

byte day2led[] = { 2, 9, 11, 5, 7, 1, 3, 10, 8, 0, 4, 6, 17, 19, 12, 22, 16, 20, 13, 15, 18, 21, 23, 14 };

CRGB leds[NUM_LEDS];

void setupLeds() {
  FastLED.addLeds<LPD8806,A4,A5,BRG>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  fill_solid(leds, NUM_LEDS, CRGB(0,0,0));
  FastLED.setBrightness(brightness);
  FastLED.show();
  
  for (int p = FIRST_PATTERN; p < LAST_PATTERN; p++) {
    resetPattern(p);
  }
}

uint64_t nextLedTicks = 0;
void delayLed(uint64_t msec) {
  nextLedTicks = ticks + msec * ticksPerSecond / 1000;
}

int oldLedMode = -1;
int oldLedPattern = -1;
void loopLeds() {
  if (ticks < nextLedTicks) {
    return;
  }

  if (oldLedMode != mode) {
    oldLedMode = mode;
    switch (mode) {
      case MODE_OFF:
        fill_solid(leds, NUM_LEDS, CRGB(0,0,0));
        FastLED.show();
        break;
      case MODE_ON:
        oldLedPattern = -1;
        break;
    }
  }

  if (oldLedMode == MODE_ON) {
    if (oldLedPattern != pattern) {
      oldLedPattern = pattern;
      resetPattern(oldLedPattern);
      fill_solid(leds, NUM_LEDS, CRGB(0,0,0));
    }
  
    runPattern(oldLedPattern);
  }
}

void resetPattern(int pattern) {
    switch (pattern) {
      case PATTERN_SPARKLE:
        patternSparkleReset();
        break;
      case PATTERN_RAINBOW:
        patternRainbowReset();
        break;
      case PATTERN_FIRE:
        patternFireReset();
        break;
      case PATTERN_NOISE:
        patternNoiseReset();
        break;
    }
}

void runPattern(int pattern) {
    switch (pattern) {
      case PATTERN_SPARKLE:
        patternSparkle();
        break;
      case PATTERN_RAINBOW:
        patternRainbow();
        break;
      case PATTERN_FIRE:
        patternFire();
        break;
      case PATTERN_NOISE:
        patternNoise();
        break;
    }
}


