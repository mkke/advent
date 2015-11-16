// Fire2012 by Mark Kriegsman, July 2012
// as part of "Five Elements" shown here: http://youtu.be/knWiGsmgycY
//
// This basic one-dimensional 'fire' simulation works roughly as follows:
// There's a underlying array of 'heat' cells, that model the temperature
// at each point along the line.  Every cycle through the simulation,
// four steps are performed:
//  1) All cells cool down a little bit, losing heat to the air
//  2) The heat from each cell drifts 'up' and diffuses a little
//  3) Sometimes randomly new 'sparks' of heat are added at the bottom
//  4) The heat from each cell is rendered as a color into the leds array
//     The heat-to-color mapping uses a black-body radiation approximation.
//
// Temperature is in arbitrary units from 0 (cold black) to 255 (white hot).
//
// This simulation scales it self a bit depending on NUM_LEDS; it should look
// "OK" on anywhere from 20 to 100 LEDs without too much tweaking.
//
// I recommend running this simulation at anywhere from 30-100 frames per second,
// meaning an interframe delay of about 10-35 milliseconds.
//
//
// There are two main parameters you can play with to control the look and
// feel of your fire: COOLING (used in step 1 above), and SPARKING (used
// in step 3 above).
//
// COOLING: How much does the air cool as it rises?
// Less cooling = taller flames.  More cooling = shorter flames.
// Default 55, suggested range 20-100
#define COOLING  70
 
// SPARKING: What chance (out of 255) is there that a new spark will be lit?
// Higher chance = more roaring fire.  Lower chance = more flickery fire.
// Default 120, suggested range 50-200.
#define SPARKING 80
 
#define LED_COLUMNS 12
#define FIRE_LEDS 16
#define LEVEL_LEDS (FIRE_LEDS/2)
// Array of temperature readings at each simulation cell
byte heat[LED_COLUMNS][FIRE_LEDS];
CRGBPalette16 firePalette;
CRGBPalette16 firePaletteEm;
void patternFireReset() {
  firePalette = CRGBPalette16( CRGB::Black, CRGB::Red, CRGB::Yellow, CRGB::White);
  firePaletteEm = CRGBPalette16( CRGB::Black, CRGB::Blue, CRGB::Aqua,  CRGB::White);
  for (int c = 0; c < LED_COLUMNS; c++) {
    for (int i = 0; i < FIRE_LEDS; i++) {
      heat[c][i] = 0;
    }
  }
}
 
void patternFire() {
  random16_add_entropy( random());

  for (int c = 0; c < LED_COLUMNS; c++) {
    updateColumnFire(heat[c], c);
  }

  FastLED.show();
  delayLed(16);
}
 
void updateColumnFire(byte* heat, int ledColumn) {
  // Step 1.  Cool down every cell a little
    for( int i = 0; i < FIRE_LEDS; i++) {
      heat[i] = qsub8( heat[i],  random8(0, ((COOLING * 10) / FIRE_LEDS) + 2));
    }
 
    // Step 2.  Heat from each cell drifts 'up' and diffuses a little
    for( int k= FIRE_LEDS - 3; k > 0; k--) {
      heat[k] = (heat[k - 1] + heat[k - 2] + heat[k - 2] ) / 3;
    }
   
    // Step 3.  Randomly ignite new 'sparks' of heat near the bottom
    if (random8() < SPARKING + (isMotionDetected() ? 50 : 0)) {
      int y = random8(7);
      heat[y] = qadd8( heat[y], random8(160,255) );
    }
 
    // Step 4.  Map from heat cells to LED colors
    int lowerLedIdx = ledColumn * LEVEL_LEDS + LED_COLUMNS * LEVEL_LEDS;
    for (int j = 0; j < LEVEL_LEDS; j++) {
      leds[LEVEL_LEDS - j - 1 + lowerLedIdx] = FireHeatColor2(heat[j], isDayColumn(ledColumn));
    }

    int upperLedIdx = ledColumn * LEVEL_LEDS;
    for (int j = 0; j < LEVEL_LEDS; j++) {
      leds[LEVEL_LEDS - j - 1 + upperLedIdx] = FireHeatColor2(heat[j + LEVEL_LEDS], isDayColumn(ledColumn));
    }
}
 
boolean isDayColumn(int ledColumn) {
  int dayLedStart = day2led[(day - 1) % LED_COLUMNS];
  return (ledColumn == dayLedStart) || (ledColumn == dayLedStart + LED_COLUMNS * LEVEL_LEDS);
}
 
CRGB FireHeatColor2(int heat, boolean emphasize) {
  // Scale the heat value from 0-255 down to 0-240
  // for best results with color palettes.
  byte colorindex = scale8( heat, 240);
  return ColorFromPalette(emphasize ? firePaletteEm : firePalette, colorindex);
}
 
// CRGB HeatColor( uint8_t temperature)
// [to be included in the forthcoming FastLED v2.1]
//
// Approximates a 'black body radiation' spectrum for
// a given 'heat' level.  This is useful for animations of 'fire'.
// Heat is specified as an arbitrary scale from 0 (cool) to 255 (hot).
// This is NOT a chromatically correct 'black body radiation'
// spectrum, but it's surprisingly close, and it's extremely fast and small.
//
// On AVR/Arduino, this typically takes around 70 bytes of program memory,
// versus 768 bytes for a full 256-entry RGB lookup table.
 
CRGB FireHeatColor( uint8_t temperature) {
  CRGB heatcolor;
 
  // Scale 'heat' down from 0-255 to 0-191,
  // which can then be easily divided into three
  // equal 'thirds' of 64 units each.
  uint8_t t192 = scale8_video( temperature, 192);
 
  // calculate a value that ramps up from
  // zero to 255 in each 'third' of the scale.
  uint8_t heatramp = t192 & 0x3F; // 0..63
  heatramp <<= 2; // scale up to 0..252
 
  // now figure out which third of the spectrum we're in:
  if( t192 & 0x80) {
    // we're in the hottest third
    heatcolor.r = 255; // full red
    heatcolor.g = 255; // full green
    heatcolor.b = heatramp; // ramp up blue
   
  } else if( t192 & 0x40 ) {
    // we're in the middle third
    heatcolor.r = 255; // full red
    heatcolor.g = heatramp; // ramp up green
    heatcolor.b = 0; // no blue
   
  } else {
    // we're in the coolest third
    heatcolor.r = heatramp; // ramp up red
    heatcolor.g = 0; // no green
    heatcolor.b = 0; // no blue
  }
 
  return heatcolor;
}
