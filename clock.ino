#include <Wire.h>
#include <ds3231.h>
#include <FastLED.h>

#define TIMESET_PIN 2
#define HOUR_PIN 3
#define MIN_PIN 4
#define SETTIME_LED_PIN 7
#define LED_PIN 11
#define NUM_LEDS 60

#define BASE_COLOR CRGB(0, 0, 0)
#define SEC_COLOR CRGB(51, 255, 153)
#define MIN_COLOR CRGB(255, 0, 255)
#define HOUR_COLOR CRGB(40, 40, 255)

struct ts t;

int setTimeDown = 0;
int hourDown = 0;
int minDown = 0;

int settingTime = 0;

CRGB leds[NUM_LEDS];

void setup() {
  Serial.begin(9600);
  Wire.begin();
  DS3231_init(DS3231_INTCN);
  //reset();
  FastLED.addLeds<NEOPIXEL, LED_PIN>(leds, NUM_LEDS);

  pinMode(TIMESET_PIN, INPUT);
  pinMode(HOUR_PIN, INPUT);
  pinMode(MIN_PIN, INPUT);
  pinMode(SETTIME_LED_PIN, OUTPUT);

  for(int i = 0; i < NUM_LEDS; i++) {
    leds[i] = BASE_COLOR;
  }
}
 
void loop() {
  DS3231_get(&t);

  if (!setTimeDown && digitalRead(TIMESET_PIN)) {
    setTimeDown = 1;
    settingTime = !settingTime;
  } else if (setTimeDown && !digitalRead(TIMESET_PIN)) {
    setTimeDown = 0;
  }

  if (settingTime) {
    setTime();
  } else {
    delay(1000);
  }
  digitalWrite(SETTIME_LED_PIN, settingTime);
  
  int standardHour = t.hour;
  if (standardHour == 0) {
    standardHour = 12;
  }
  if (standardHour > 12) {
    standardHour = standardHour - 12;
  }

  int hourLED = map(standardHour, 1, 12, NUM_LEDS/12, NUM_LEDS-1);
  int minLED = map(t.min, 0, 59, 0, NUM_LEDS-1);
  int secLED = map(t.sec, 0, 59, 0, NUM_LEDS-1);
  int minHigh = max(hourLED, minLED) != hourLED;

  /*
  Serial.print("hour: ");
  Serial.print(hourLED);
  Serial.print("\tmin: ");
  Serial.print(minLED);
  Serial.print("\t");
  printTime();
  */

  if (minHigh) {
    for (int i = 0; i <= minLED; i++) {
      leds[i] = MIN_COLOR;
    }
    for (int i = minLED+1; i < NUM_LEDS; i++) {
      leds[i] = BASE_COLOR;
    }
  }

  for (int i = 0; i <= hourLED; i++) {
    leds[i] = HOUR_COLOR;
  }
  
  if (!minHigh) {
    for (int i = 0; i <= minLED; i++) {
      leds[i] = MIN_COLOR;
    }
    for (int i = hourLED+1; i < NUM_LEDS; i++) {
      leds[i] = BASE_COLOR;
    }
  }

  leds[secLED] = SEC_COLOR;
  
  FastLED.show();
}

void setTime() {
  if (!hourDown && digitalRead(HOUR_PIN)) {
    hourDown = 1;
    int newHour = t.hour + 1;
    if (newHour > 23) {
      newHour = 0;
    }
    t.hour = newHour;
  } else if (!digitalRead(HOUR_PIN)) {
    hourDown = 0;
  }

  if (!minDown && digitalRead(MIN_PIN)) {
    minDown = 1;
    int newMin = t.min + 1;
    if (newMin >= 60) {
      newMin = 0;
    }
    t.min = newMin;
  } else if (!digitalRead(MIN_PIN)) {
    minDown = 0;
  }

  printTime();

  DS3231_set(t);
}

void reset() {
  t.hour=0; 
  t.min=0;
  t.sec=0;
  DS3231_set(t);
}

void printTime() {
  Serial.print("Hour : ");
  Serial.print(t.hour);
  Serial.print(":");
  Serial.print(t.min);
  Serial.print(".");
  Serial.println(t.sec);
}
