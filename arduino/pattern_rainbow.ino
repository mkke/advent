int rainbowHue;
void patternRainbowReset() {
  rainbowHue = 0;
}

void patternRainbow() {
  rainbowHue++;
  if (rainbowHue > 255) {
    rainbowHue = 0;
  }
  fill_rainbow(leds, NUM_LEDS, rainbowHue);
  FastLED.show();
//  Serial.print("flow: led = ");
//  Serial.println(flowPos);
  delayLed(16);
}


