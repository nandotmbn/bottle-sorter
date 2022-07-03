#include <Arduino.h>

// PINOUT DEFINING
#define CAPACITIVE 34
#define INDUCTIVE 35
#define IR_DETECTOR 5
#define IR_SMALL 16
#define IR_MEDIUM 19
#define IR_BIG 21
#define BUTTON_START 3
#define BUTTON_END 5

// STATE DECLARATION
bool isRunning = false;
bool isSortingNow = false;

// CUMULATIVE COUNTER
int smallBottle = 0;
int mediumBottle = 0;
int bigBottle = 0;

// USER STATE COUNTER
int userSmallBottle = 0;
int userMediumBottle = 0;
int userBigBottle = 0;

// CONSTANT VARS
const int minSmallBottle = 10;
const int minMediumBottle = 5;
const int minBigBottle = 3;

void initPin()
{
  pinMode(CAPACITIVE, INPUT);
  pinMode(INDUCTIVE, INPUT);
  pinMode(IR_DETECTOR, INPUT);
  pinMode(IR_SMALL, INPUT);
  pinMode(IR_MEDIUM, INPUT);
  pinMode(IR_BIG, INPUT);
  pinMode(BUTTON_START, INPUT);
  pinMode(BUTTON_END, INPUT);
}

bool startListener()
{
  if (digitalRead(BUTTON_START) == LOW)
  {
    delay(20);
    while (digitalRead(BUTTON_START) == LOW)
      ;
    return true;
  }
  return false;
}

bool bottleDetector()
{
  if (digitalRead(IR_DETECTOR) == HIGH)
    return true;
  return false;
}

bool garbageSorter()
{
  if (!digitalRead(CAPACITIVE) && !digitalRead(INDUCTIVE))
    return true;
  return false;
}

bool bottleSorter()
{
  if (digitalRead(IR_SMALL))
    return 1;
  else if (digitalRead(IR_MEDIUM))
    return 2;
  else if (digitalRead(IR_BIG))
    return 3;
}

void setup()
{
  Serial.begin(9600);
  initPin();
}

void loop()
{
  if (!startListener()) // Apakah sudah ditekan tombol start?
    return;             // Jika tidak, cek lagi.

  if (!bottleDetector()) // Apakah botolnya ada?
    return;              // Jika tidak, cek lagi.

  if (!garbageSorter()) // Apakah botolnya plastik?
  {
    /*
      ...
      Code handler untuk "Bukan Botol Plastik"
      ...
    */
    return; // Jika tidak, cek lagi.
  }

  /*
    ...
    Code block untuk mekanisme pembukaan katup
    ...
  */

  isSortingNow = true;
  while (isSortingNow)
  {
    switch (bottleSorter())
    {
    case 1: // Botol Kecil
      userSmallBottle++;
      smallBottle++;
      isSortingNow = false;
      break;
    case 2: // Botol Sedang
      userMediumBottle++;
      mediumBottle++;
      isSortingNow = false;
      break;
    case 3: // Botol Besar
      userBigBottle++;
      bigBottle++;
      isSortingNow = false;
      break;

    default:
      break;
    }
  }

    
}