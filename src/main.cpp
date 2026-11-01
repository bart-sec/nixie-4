// INSTRUCTION

// NORMAL MODE
// LEFT BUTTON: SHOW DATE?
// LEFT BUTTON, LONG PRESS -> TIME SET MODE
// RIGHT BUTTON: SHOW TEMP?

// TIME SET MODE (blink digit/s that is set)
// LEFT BUTTON: MOVE CURSOR/CONFIRM hour_l->hour_r->minute_l->minute_r
// RIGHT BUTTON: ADD HOUR/MIN

// ALARM SET MODE?
// DISABLE DISPLAY MODE?

#include <Arduino.h>
#include <RTClib.h>
#include <Wire.h>

RTC_DS3231 RTC;

volatile bool time_set_mode = false;
volatile int time_set_digit = 0;
volatile bool schedule_shuffle;
volatile unsigned long left_button_press_start;
volatile unsigned long right_button_press_start;
volatile bool left_button_state = LOW;
volatile bool right_button_state = LOW;

volatile int hour;
volatile int hour_l;
volatile int hour_r;
volatile int minute;
volatile int minute_l;
volatile int minute_r;

const int LEFT_BUTTON = 2;  // yellow
const int RIGHT_BUTTON = 3; // blue
const int nixie_0 = 5;      //
const int nixie_6 = 9;      //
const int nixie_8 = 7;      //
const int nixie_7 = 8;      //
const int nixie_9 = 6;      //
const int nixie_5 = 10;     //
const int nixie_4 = 11;     //
const int nixie_3 = 0;      //
const int nixie_2 = 1;      //
const int nixie_1 = 4;      //

const int POWER_HOURS_LEFT = 17;
const int POWER_HOURS_RIGHT = 16;
const int POWER_MINUTES_LEFT = 15;
const int POWER_MINUTES_RIGHT = 14;

const bool test = false;

void shuffle();
void update_current_time();
void display_current_time();
int cap_digit(int, int, int);
void blink(int, int);
void display(int, int, int, int);
void left_button_press();
void right_button_press();
int map(int);
void off_all();
void on_number(int, int);

void setup() {
  Wire.begin();
  RTC.begin();
  // RTC.adjust(DateTime((__DATE__), (__TIME__))); //will reset time set to rtc

  pinMode(LEFT_BUTTON, INPUT_PULLUP);
  pinMode(RIGHT_BUTTON, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(LEFT_BUTTON), left_button_press,
                  CHANGE);
  attachInterrupt(digitalPinToInterrupt(RIGHT_BUTTON), right_button_press,
                  CHANGE);

  pinMode(nixie_0, OUTPUT);
  pinMode(nixie_1, OUTPUT);
  pinMode(nixie_2, OUTPUT);
  pinMode(nixie_3, OUTPUT);
  pinMode(nixie_4, OUTPUT);
  pinMode(nixie_5, OUTPUT);
  pinMode(nixie_6, OUTPUT);
  pinMode(nixie_7, OUTPUT);
  pinMode(nixie_8, OUTPUT);
  pinMode(nixie_9, OUTPUT);
  pinMode(POWER_HOURS_RIGHT, OUTPUT);
  pinMode(POWER_HOURS_LEFT, OUTPUT);
  pinMode(POWER_MINUTES_RIGHT, OUTPUT);
  pinMode(POWER_MINUTES_LEFT, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
}

void loop() {
  update_current_time();

  if (time_set_mode) {
    blink(time_set_digit, 50);
    return;
  }

  if (left_button_state && !left_button_press_start) {
    left_button_press_start = millis();
    return;
  }

  if (left_button_press_start) {
    if (millis() - left_button_press_start > 1000) {
      time_set_mode = true;
      time_set_digit = 1;
    }
  }

  shuffle();
  display_current_time();
}

void update_current_time() {
  if (time_set_mode) {
    RTC.adjust(DateTime(2026, 11, 1, hour_l * 10 + hour_r,
                        minute_l * 10 + minute_r, 0));
    return;
  }

  DateTime now = RTC.now();

  int current_minute = now.minute();
  if (current_minute > minute)
    schedule_shuffle = true;

  minute = now.minute();
  minute_r = minute % 10;
  minute_l = minute / 10;

  hour = now.hour();
  hour_r = hour % 10;
  hour_l = hour / 10;
}

void display_current_time() {
  on_number(hour_l, POWER_HOURS_LEFT);
  on_number(hour_r, POWER_HOURS_RIGHT);
  on_number(minute_l, POWER_MINUTES_LEFT);
  on_number(minute_r, POWER_MINUTES_RIGHT);
}

void blink(int digit, int interval) {
  for (size_t i = 0; i < interval; i++) {
    display_current_time();
    if (!time_set_mode || digit != time_set_digit)
      return;
  }

  for (size_t i = 0; i < interval; i++) {
    switch (digit) {
    case 1:
      display(-1, hour_r, minute_l, minute_r);
      break;
    case 2:
      display(hour_l, -1, minute_l, minute_r);
      break;
    case 3:
      display(hour_l, hour_r, -1, minute_r);
      break;
    case 4:
      display(hour_l, hour_r, minute_l, -1);
      break;
    default:
      display(-1, -1, -1, -1);
    }

    if (!time_set_mode || digit != time_set_digit)
      return;
  }
}

void shuffle() {
  if (!schedule_shuffle)
    return;

  for (size_t i = 10; i > 0; i--) {
    display(i, i, i, i);
    delay(10);
  }

  for (size_t i = 10; i > 0; i--) {

    display(max(i, hour_l), max(i, hour_r), max(i, minute_l), minute_r);
    delay(10);
  }

  for (size_t i = 10; i > 0; i--) {

    display(max(i, hour_l), max(i, hour_r), minute_l, minute_r);
    delay(10);
  }

  for (size_t i = 10; i > 0; i--) {

    display(max(i, hour_l), hour_r, minute_l, minute_r);
    delay(10);
  }

  schedule_shuffle = false;
  return;
}

void display(int number1, int number2, int number3, int number4) {
  if (number1 < 0 && number2 < 0 && number3 < 0 && number4 < 0) {
    off_all();
    delay(4);
    return;
  }
  if (number1 >= 0)
    on_number(number1, POWER_HOURS_LEFT);
  if (number2 >= 0)
    on_number(number2, POWER_HOURS_RIGHT);
  if (number3 >= 0)
    on_number(number3, POWER_MINUTES_LEFT);
  if (number4 >= 0)
    on_number(number4, POWER_MINUTES_RIGHT);
}

void on_number(int digit, int row) {
  off_all();
  int actual = map(digit);
  digitalWrite(actual, HIGH);
  digitalWrite(row, HIGH);
  delay(1);
}

void off_all() {
  digitalWrite(nixie_0, LOW);
  digitalWrite(nixie_1, LOW);
  digitalWrite(nixie_2, LOW);
  digitalWrite(nixie_3, LOW);
  digitalWrite(nixie_4, LOW);
  digitalWrite(nixie_5, LOW);
  digitalWrite(nixie_6, LOW);
  digitalWrite(nixie_7, LOW);
  digitalWrite(nixie_8, LOW);
  digitalWrite(nixie_9, LOW);
  digitalWrite(POWER_HOURS_RIGHT, LOW);
  digitalWrite(POWER_HOURS_LEFT, LOW);
  digitalWrite(POWER_MINUTES_RIGHT, LOW);
  digitalWrite(POWER_MINUTES_LEFT, LOW);
  delay(1);
}

int map(int digit) {
  switch (digit) {
  case 0:
    return nixie_0;
  case 1:
    return nixie_1;
  case 2:
    return nixie_2;
  case 3:
    return nixie_3;
  case 4:
    return nixie_4;
  case 5:
    return nixie_5;
  case 6:
    return nixie_6;
  case 7:
    return nixie_7;
  case 8:
    return nixie_8;
  case 9:
    return nixie_9;

  default:
    return -1;
  }
}

void left_button_press() {
  left_button_state = digitalRead(LEFT_BUTTON);

  if (time_set_mode && left_button_state) {
    if (time_set_digit >= 4) {
      time_set_mode = false;
      time_set_digit = 0;
    } else {
      time_set_digit++;
    }
  }

  if (!left_button_state) {
    left_button_press_start = 0;
  }
}

void right_button_press() {
  right_button_state = digitalRead(RIGHT_BUTTON);
  if (time_set_mode && right_button_state) {
    switch (time_set_digit) {
    case 1:
      hour_l++;
      break;
    case 2:
      hour_r++;
      break;
    case 3:
      minute_l++;
      break;
    case 4:
      minute_r++;
      break;
    }
    hour_l = cap_digit(hour_l, 1, 2);
    hour_r = cap_digit(hour_r, 0, hour_l == 2 ? 3 : 9);
    minute_l = cap_digit(minute_l, 0, 5);
    minute_r = cap_digit(minute_r, 0, 9);
  }

  if (!right_button_state) {
    right_button_press_start = 0;
  }
}

int cap_digit(int digit, int min, int max) {
  if (digit > max)
    return min;
  return digit;
}
