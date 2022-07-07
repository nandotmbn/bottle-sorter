#include <Arduino.h>
#include <Servo.h>
#include <LiquidCrystal_I2C.h>
#include <Adafruit_Thermal.h>
#include <SoftwareSerial.h>

// PINOUT DEFINING
#define CAPACITIVE 34
#define INDUCTIVE 35
#define IR_DETECTOR 5
#define IR_SMALL 27
#define IR_MEDIUM 14
#define IR_BIG 12
#define BUTTON_START 4
#define BUTTON_END 18
#define BUTTON_RESET 6
#define SERVO 13
#define TX_PIN 33
#define RX_PIN 32

// SERVED PINOUT
// SCL 22
// SDA 21

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
const int lcdColumns = 20;
const int lcdRows = 4;

Servo Valve;
LiquidCrystal_I2C lcd(0x27, lcdColumns, lcdRows);
SoftwareSerial mySerial(RX_PIN, TX_PIN); // Declare SoftwareSerial obj first
Adafruit_Thermal printer(&mySerial);

int pos = 0;

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

void startListener()
{
  if (digitalRead(BUTTON_START) == LOW)
  {
    delay(20);
    while (digitalRead(BUTTON_START) == LOW)
      ;
    Serial.println("START");
    isRunning = true;
  }
}

bool resetListener()
{
  if (digitalRead(BUTTON_RESET) == LOW)
  {
    delay(20);
    while (digitalRead(BUTTON_RESET) == LOW)
      ;
    return true;
  }
  return false;
}

bool endListener()
{
  if (digitalRead(BUTTON_END) == LOW)
  {
    delay(20);
    while (digitalRead(BUTTON_END) == LOW)
      ;
    isRunning = false;
    return true;
  }

  return false;
}

bool bottleDetector()
{
  if (digitalRead(IR_DETECTOR) == LOW)
    return true;
  return false;
}

bool garbageSorter()
{
  if (digitalRead(CAPACITIVE) && digitalRead(INDUCTIVE))
    return true;
  return false;
}

int bottleSorter()
{
  if (!digitalRead(IR_SMALL))
    return 1;
  else if (!digitalRead(IR_MEDIUM))
    return 2;
  else if (!digitalRead(IR_BIG))
    return 3;
}

void lcdState()
{
  lcd.setCursor(0, 0);
  lcd.print("Anda Mengumpulkan :");
  lcd.setCursor(0, 1);
  lcd.print(String(userSmallBottle) + " botol kecil.");
  lcd.setCursor(0, 2);
  lcd.print(String(userMediumBottle) + " botol sedang.");
  lcd.setCursor(0, 3);
  lcd.print(String(userBigBottle) + " botol besar.");
}

void printExample(int point)
{

  printer.begin();

  // Font options
  printer.setFont('A');

  // Test more styles
  printer.boldOn();
  printer.setSize('S'); // Set type size, accepts 'S', 'M', 'L'
  printer.justify('C');
  printer.println(F(" "));
  printer.justify('C');
  printer.setSize('S'); // Set type size, accepts 'S', 'M', 'L'
  printer.println(F("Anda"));
  printer.justify('C');
  printer.setSize('S'); // Set type size, accepts 'S', 'M', 'L'
  printer.println(F("Dapat"));
  printer.justify('C');
  printer.setSize('L'); // Set type size, accepts 'S', 'M', 'L'
  printer.println(String(point));
  printer.justify('C');
  printer.setSize('M');
  printer.println(F("Poin"));
  printer.println(F("\r\n"));
  printer.println(F("\r\n"));
  printer.println(F("===="));

  printer.wake();       // MUST wake() before printing again, even if reset
  printer.setDefault(); // Restore printer to defaults
}

void setup()
{
  Serial.begin(9600);
  initPin();
  Valve.attach(SERVO);
  Valve.write(0);
  lcd.init();
  lcd.backlight();

  mySerial.begin(9600);
}

void loop()
{
  startListener();
  if (endListener()) // Apakah perhitungan per user akan dihilangkan
  {
    lcd.clear();
    float point = userSmallBottle / minSmallBottle + userMediumBottle / minMediumBottle + userBigBottle / minBigBottle;

    lcd.setCursor(0, 0);
    lcd.print("Dengan ini anda ");
    lcd.setCursor(0, 1);
    lcd.print("mendapatkan : ");
    lcd.setCursor(0, 2);
    lcd.print(String((int)point) + " poin");

    delay(2000);
    lcd.clear();
    printExample((int)point);


    userSmallBottle = 0;
    userMediumBottle = 0;
    userBigBottle = 0;
  }
  lcdState();
  if (!isRunning) // Apakah sudah ditekan tombol start?
    return;       // Jika tidak, cek lagi.

  if (!bottleDetector()) // Apakah botolnya ada?
    return;              // Jika tidak, cek lagi.

  delay(1000);

  if (!garbageSorter()) // Apakah botolnya plastik?
    return;             // Jika tidak, cek lagi.

  for (pos = 0; pos <= 180; pos += 1)
  {
    Valve.write(pos);
    delay(5);
  }

  delay(5000);

  for (pos = 180; pos >= 0; pos -= 1)
  { // goes from 180 degrees to 0 degrees
    Valve.write(pos);
    delay(5);
  }

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

  if (resetListener())
  {
    userSmallBottle = 0;
    userMediumBottle = 0;
    userBigBottle = 0;
    smallBottle = 0;
    mediumBottle = 0;
    bigBottle = 0;
  }
}