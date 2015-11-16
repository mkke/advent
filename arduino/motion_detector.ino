boolean isMotionDetected() {
  boolean sens = (digitalRead(PIN_MOTION_DETECTOR) == HIGH);
  return sens;
}

uint64_t nextMotionTicks = 0;
void delayMotion(uint64_t msec) {
  nextMotionTicks = ticks + msec * ticksPerSecond / 1000;
}

void loopMotion() {
  if (ticks < nextMotionTicks) {
    return;
  }

  if (isMotionDetected() && mode == MODE_OFF) {
    setMode(MODE_ON);
  }
  
  delayMotion(100);
}

