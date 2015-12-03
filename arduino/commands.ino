void loopSerial() {
  if (Serial.available() > 0) {
    char line[255] = {0};
    int lineSize = Serial.readBytesUntil('\n', (char*) &line, sizeof(line) - 1);
    if (lineSize > 0) {
      processCommand((char*) &line);
    }
  }
}

void processCommand(char* line) {
  // line length is guaranteed to be > 0
  switch (line[0]) {
  case 'I':
    Serial.print("I");
    Serial.println(REMOTE_ADDRESS);
    break;
  case 'M':
    if (line[1] == '0') {
      setMode(MODE_OFF);
    } else if (line[1] == '1') {
      setMode(MODE_ON);
    }
    if (mode == MODE_OFF) {
      Serial.println("M0");
    } else {
      Serial.println("M1");
    }
    break;
  case 'D': {
    if (line[1] != '?') {
      int newDay;
      int count = sscanf((const char*)&line[1], "%d", &newDay);
      if (count >= 1 && newDay >= FIRST_DAY && newDay <= LAST_DAY) {
        setDay(newDay);
      }
    }
    char dayinfo_s[60] = "";
    sprintf(dayinfo_s, "D%d", day);
    Serial.println(dayinfo_s);
    }
    break;
  case 'L':
    Serial.println("L1=\"Sparkle\",2=\"Rainbow\",3=\"Fire\",4=\"Noise\"");
    break;
  case 'P': {
    if (line[1] != '?') {
      int newPattern;
      int count = sscanf((const char*)&line[1], "%d", &newPattern);
      if (count >= 1 && newPattern >= FIRST_PATTERN && newPattern <= LAST_PATTERN) {
        setPattern(newPattern);
      }
    }
    char patterninfo_s[60] = "";
    sprintf(patterninfo_s, "P%d", pattern);
    Serial.println(patterninfo_s);
    }
    break;
  case 'B': {
    if (line[1] != '?') {
      int newBrightness;
      int count = sscanf((const char*)&line[1], "%d", &newBrightness);
      if (count >= 1 && newBrightness >= 1 && newBrightness <= 255) {
        setBrightness(newBrightness);
      }
    }
    char info_s[60] = "";
    sprintf(info_s, "B%d", brightness);
    Serial.println(info_s);
    }
    break;
  case 'A': {
    if (line[1] != '?') {
      int newAutoOffDelay;
      int count = sscanf((const char*)&line[1], "%d", &newAutoOffDelay);
      if (count >= 1 && newAutoOffDelay >= 1 && newAutoOffDelay < 255) {
        setAutoOffDelay(newAutoOffDelay);
      }
    }
    char info_s[60] = "";
    sprintf(info_s, "A%d", autoOffDelay);
    Serial.println(info_s);
    }
    break;
  case 'S': {
    if (line[1] != '?') {
      int newStayOnStart;
      int count = sscanf((const char*)&line[1], "%d", &newStayOnStart);
      if (count >= 1 && newStayOnStart >= 0 && newStayOnStart <= 23) {
        setStayOnStart(newStayOnStart);
      }
    }
    char info_s[60] = "";
    sprintf(info_s, "S%d", stayOnStart);
    Serial.println(info_s);
    }
    break;
  case 'E': {
    if (line[1] != '?') {
      int newStayOnEnd;
      int count = sscanf((const char*)&line[1], "%d", &newStayOnEnd);
      if (count >= 1 && newStayOnEnd >= 0 && newStayOnEnd <= 23) {
        setStayOnEnd(newStayOnEnd);
      }
    }
    char info_s[60] = "";
    sprintf(info_s, "E%d", stayOnEnd);
    Serial.println(info_s);
    }
    break;
  case 'T': 
    { // current time
      if (line[1] != '?') {
        // we'd like to use 64-bit times, but AVR libc doesn't implement them in scanf/printf
        uint32_t newTime;
        int count = sscanf((const char*)&line[1], "%lu", &newTime);
        if (count >= 1) {
          setTime(newTime);
        }
      }

      char timeinfo_s[60] = "";
      sprintf(timeinfo_s, "T%lu (%02hhd:%02hhd:%02hhd)", (uint32_t) time, time_h, time_m, time_s);
      Serial.println(timeinfo_s);
    }
    break;
  default:
    Serial.println("ERROR");
  }
}

