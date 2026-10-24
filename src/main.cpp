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
#include <Wire.h>
#include <RTClib.h>

RTC_DS3231 RTC;

volatile bool timeSetMode;
volatile unsigned long leftBtnPressStart;
volatile bool leftBtnState = LOW;
volatile bool rightBtnState = LOW;

int hour;
int minute;
int second;

const int leftButton = 2;  // yellow
const int rightButton = 3; // blue
const int nixie_0 = 5;     //
const int nixie_6 = 9;     //
const int nixie_8 = 7;     //
const int nixie_7 = 8;     //
const int nixie_9 = 6;     //
const int nixie_5 = 10;    //
const int nixie_4 = 11;    //
const int nixie_3 = 0;     //
const int nixie_2 = 1;     //
const int nixie_1 = 4;     //

const int power_hours_l = 17;
const int power_hours_r = 16;

const int power_minutes_l = 15;
const int power_minutes_r = 14;

const bool test = false;

void displayCurrentTime();
void blink(int,int,int,int);
void display(int,int,int,int);
void leftButtonPress();
void rightButtonPress();
int map(int);
void off_all();
void on_number(int, int);
void on_number_raw(int, int);

void setup()
{
  Wire.begin();
  RTC.begin();
  RTC.adjust(DateTime((__DATE__), (__TIME__)));

  pinMode(leftButton, INPUT_PULLUP);
  pinMode(rightButton, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(leftButton), leftButtonPress, CHANGE);
  attachInterrupt(digitalPinToInterrupt(rightButton), rightButtonPress, CHANGE);

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
  pinMode(power_hours_r, OUTPUT);
  pinMode(power_hours_l, OUTPUT);
  pinMode(power_minutes_r, OUTPUT);
  pinMode(power_minutes_l, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop()
{

  if (test)
  {
    for (size_t i = 0; i < 10; i++)
    {
      for (size_t j = 0; j < 10; j++)
      {
        for (size_t k = 0; k < 10; k++)
        {
          for (size_t l = 0; l < 10; l++)
          {
            for (size_t o = 0; o < 2; o++)
            {
              on_number(i, power_hours_l);
              delay(3);

              on_number(j, power_hours_r);
              delay(3);

              on_number(k, power_minutes_l);
              delay(3);

              on_number(l, power_minutes_r);
              delay(3);
            }
          }
        }
      }
    }
    return;
  }

  if (timeSetMode)
  {
    if (rightBtnState)
    {
      timeSetMode = false;
    }
    blink(0,0,0,0);
    return;
  }

  // if (leftBtnState && rightBtnState)
  // {
  //   display(5,5,5,5);
  //   return;
  // }
  // else 
  if (leftBtnState && !leftBtnPressStart)
  {
    leftBtnPressStart = millis();

    // display(1,2,3,4);
    return;
  }
  // else if (rightBtnState)
  // {
  //   display(4,3,2,1);
  //   return;
  // }

  if (leftBtnPressStart)
  {
    if (millis() - leftBtnPressStart > 2000)
    {
      timeSetMode = true;
    }
  }

  displayCurrentTime();
}

void displayCurrentTime()
{
  DateTime now = RTC.now();

  // second = now.second();

  // int second_r = second % 10;
  // int second_l = second / 10;

  minute = now.minute();

  int minute_r = minute % 10;
  int minute_l = minute / 10;

  hour = now.hour();

  int hour_r = hour % 10;
  int hour_l = hour / 10;

  on_number(hour_l, power_hours_l);
  delay(1);
  on_number(hour_r, power_hours_r);
  delay(1);
  on_number(minute_l, power_minutes_l);
  delay(1);
  on_number(minute_r, power_minutes_r);
  delay(1);
}

void blink(int number1, int number2, int number3, int number4)
{
  for (size_t i = 0; i < 100; i++)
  {
    display(number1, number2, number3, number4);
    if(!timeSetMode)
      return;
  }
  for (size_t i = 0; i < 100; i++)
  {
    off_all();
    delay(4);
    if(!timeSetMode)
      return;
  }
}

void display(int number1, int number2, int number3, int number4)
{
  on_number(number1, power_hours_l);
  delay(1);
  on_number(number2, power_hours_r);
  delay(1);
  on_number(number3, power_minutes_l);
  delay(1);
  on_number(number4, power_minutes_r);
  delay(1);
}

void on_number(int digit, int row)
{
  off_all();
  int actual = map(digit);
  digitalWrite(actual, HIGH);
  digitalWrite(row, HIGH);
}

void on_number_raw(int nixie, int row)
{
  off_all();
  digitalWrite(nixie, HIGH);
  digitalWrite(row, HIGH);
}

void off_all()
{
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
  digitalWrite(power_hours_r, LOW);
  digitalWrite(power_hours_l, LOW);
  digitalWrite(power_minutes_r, LOW);
  digitalWrite(power_minutes_l, LOW);
  delayMicroseconds(420); // to prevent "ghost" effect to other tube
}

int map(int digit)
{
  switch (digit)
  {
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

void leftButtonPress()
{
  leftBtnState = digitalRead(leftButton);
  if (!leftBtnState)
  {
    leftBtnPressStart = 0;
  }
}

void rightButtonPress()
{
  rightBtnState = digitalRead(rightButton);
  timeSetMode=false;
}