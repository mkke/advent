struct spark {
  int led;
  int hue;
  int saturation;
  int brightness;
  int speed;
};

#define NUM_SPARKS 16
#define NUM_DAY_SPARKS 2
struct spark sparks[NUM_SPARKS];
int nextSpark = 0;

void patternSparkleReset() {
  for (int i = 0; i < NUM_SPARKS; i++) {
    sparks[i].led = -1;
  }
}

uint64_t nextSparkTicks = 0;
void delaySpark(uint64_t msec) {
  nextSparkTicks = ticks + msec * ticksPerSecond / 1000;
}

void patternSparkle() {
  // update active sparks
  for (int i = 0; i < NUM_SPARKS; i++) {
    int led = sparks[i].led;
    if (led >= 0) {
      if (sparks[i].speed > 0) {
        sparks[i].brightness += sparks[i].speed;
        if (sparks[i].brightness > 255) {
          sparks[i].brightness = 255;
          sparks[i].speed = -sparks[i].speed * 0.8;
        }
      } else {
        sparks[i].brightness += sparks[i].speed;
        if (sparks[i].brightness < 0) {
          sparks[i].brightness = 0;
          sparks[i].led = -1;
        } else if (sparks[i].speed > -8){
          sparks[i].speed >>=1;
        }
      }
      leds[led] = CHSV(sparks[i].hue, sparks[i].saturation, sparks[i].brightness);
    } else {
      // create a new spark, either now, or later
      if (nextSparkTicks < ticks) {
        random16_add_entropy(random());
        
        // the first sparks are for the day; only if the led is already taken, other days are considered
        if (i < NUM_DAY_SPARKS) {
          int ledStart = day2led[day - 1] * 8;
          sparks[i].led = random8(ledStart, ledStart + 8);
        } else {
          sparks[i].led = random8(NUM_LEDS);
        }
        
        while (true) {
          boolean duplicate = false;
          for (int j = 0; j < NUM_LEDS; j++) {
            if (j != i && sparks[j].led == sparks[i].led) {
              duplicate = true;
              break;
            }
          }
          if (duplicate) {
            sparks[i].led = random8(NUM_LEDS);
          } else {
            break;
          }
        }
        sparks[i].hue = random8();
        sparks[i].saturation = random8(64,255);
        sparks[i].brightness = 0;
        sparks[i].speed = random8(20, 64);
        
        // setup next spark
        delaySpark(random16(500 / NUM_SPARKS, 1500 / NUM_SPARKS));
      }
    }
  }
  
  
  
  FastLED.show();
  delayLed(16);
}


