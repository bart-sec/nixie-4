// INSTRUCTION
// NORMAL MODE
// LEFT BUTTON: SHOW DATE?
// RIGHT BUTTON: SHOW TEMP?

// TIME SET MODE (blink digit/s that is set)
// LEFT BUTTON: MOVE CURSOR/CONFIRM hh=>mm=>ss
// RIGHT BUTTON: ADD HOUR/MIN/SEC

// ALARM SET MODE?
// DISABLE DISPLAY MODE?

#include <Arduino.h>
#include <RTClib.h>
#include <Wire.h>

RTC_DS3231 RTC;

volatile bool time_set_mode = true;
volatile int time_set_digit = 0;
volatile bool schedule_shuffle;
volatile unsigned long left_button_press_start;
volatile unsigned long right_button_press_start;
volatile bool left_button_state = LOW;
volatile bool right_button_state = LOW;

int hour;
int hour_l;
int hour_r;
int minute;
int minute_l;
int minute_r;
// int second;
// int second_l;
// int second_r;

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
void blink(int, int);
void display(int, int, int, int);
void left_button_press();
void right_button_press();
int map(int);
void off(int digit);
void off_all();
void on_number(int, int);
void on_number_raw(int, int);

void setup() {
  Wire.begin();
  RTC.begin();
  RTC.adjust(DateTime((__DATE__), (__TIME__)));

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
}

void loop() {

  update_current_time();

  // if (test) {
  //   for (size_t i = 0; i < 10; i++) {
  //     for (size_t j = 0; j < 10; j++) {
  //       for (size_t k = 0; k < 10; k++) {
  //         for (size_t l = 0; l < 10; l++) {
  //           for (size_t o = 0; o < 2; o++) {
  //             on_number(i, POWER_HOURS_LEFT);
  //             on_number(j, POWER_HOURS_RIGHT);
  //             on_number(k, POWER_MINUTES_LEFT);
  //             on_number(l, POWER_MINUTES_RIGHT);
  //           }
  //         }
  //       }
  //     }
  //   }
  //   return;
  // }

  if (time_set_mode) {
    blink(time_set_digit, 50);
    return;
  }

  if (left_button_state && !left_button_press_start) {
    left_button_press_start = millis();
    return;
  }

  if (time_set_mode) {
  }

  if (left_button_press_start) {
    if (millis() - left_button_press_start > 2000) {
      time_set_mode = true;
      time_set_digit = 1;
    }
  }

  shuffle();
  display_current_time();
}

void update_current_time() {
  DateTime now = RTC.now();

  // second = now.second();
  // int second_r = second % 10;
  // int second_l = second / 10;
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

// int map_row(int digit) {
//   switch (digit) {
//   case 1:
//     return POWER_HOURS_LEFT;
//   }
// }
void blink(int digit, int interval) {
  for (size_t i = 0; i < interval; i++) {
    display_current_time();
    // if (!time_set_mode)
    //   return;
  }

  for (size_t i = 0; i < interval; i++) {
    // off(2);
    // off_all();
    display(-1, hour_r, minute_l, minute_r);
    delay(4);
    // if (!time_set_mode)
    //   return;
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
  // for (size_t i = 10; i > 0; i--) {
  //
  //   display(max(i, hour_l), max(i, hour_r), max(i, minute_l), max(i,
  //   minute_r)); delay(20);
  // }
  schedule_shuffle = false;
  return;
}

void display(int number1, int number2, int number3, int number4) {
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

// not used
void on_number_raw(int nixie, int row) {
  off_all();
  digitalWrite(nixie, HIGH);
  digitalWrite(row, HIGH);
}

void off(int digit) {

  if (digit == 1)
    digitalWrite(POWER_HOURS_LEFT, LOW);
  if (digit == 2)
    digitalWrite(POWER_HOURS_RIGHT, LOW);
  if (digit == 3)
    digitalWrite(POWER_MINUTES_LEFT, LOW);
  if (digit == 4)
    digitalWrite(POWER_MINUTES_RIGHT, LOW);

  delayMicroseconds(420); // to prevent "ghost" effect to other tube
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
  delayMicroseconds(420); // to prevent "ghost" effect to other tube
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
  if (time_set_mode && time_set_digit >= 4) {
    time_set_mode = false;
    time_set_digit = 0;
  }
  if (time_set_mode) {
    time_set_digit++;
  }

  if (!left_button_state) {
    left_button_press_start = 0;
  }
}

void right_button_press() {
  right_button_state = digitalRead(RIGHT_BUTTON);
  if (time_set_mode) {
    time_set_mode = false;
  }
  if (!right_button_state) {
    right_button_press_start = 0;
  }
}
