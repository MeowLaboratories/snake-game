#include "LedControl.h"
// connection to MAX7219 (data, clk, load/cs, #of dispplay)
LedControl lc1 = LedControl(12, 11, 10, 8);
LedControl lc2 = LedControl(7, 5, 6, 8);

int fruitX, fruitY;
char dir = 'r';
int snakeLength = 3;
int maxSnakeLength = 512;
byte snakeX[512], snakeY[512];
bool flagFruit = true;

void setup() {
  int devices = 8;
  ////wake up the MAX72XX from power-saving mode
  for (int address = 0; address < devices; address++) {
    /*The MAX72XX is in power-saving mode on startup*/
    lc1.shutdown(address, false);
    lc2.shutdown(address, false);
    /* Set the brightness to a medium values */
    lc1.setIntensity(address, 2);
    lc2.setIntensity(address, 2);
    /* and clear the display */
    lc1.clearDisplay(address);
    lc2.clearDisplay(address);
  }

  for (int i = 0; i < maxSnakeLength; i++) {
    snakeX[i] = 0;
    snakeY[i] = 0;
  }

  snakeX[0] = 5;
  snakeX[1] = 4;
  snakeX[2] = 3;

  drawPixel(snakeX[0], snakeY[0], true);
  drawPixel(snakeX[1], snakeY[1], true);
  drawPixel(snakeX[2], snakeY[2], true);

  Serial.begin(9600);
  pinMode(A0, INPUT); //up
  pinMode(A1, INPUT); //right
  pinMode(A2, INPUT); //down
  pinMode(A3, INPUT); //left

  fruitX = 16, fruitY = 16;
}

void loop() {
  render();
  draw();
  delay(100);
}

void drawPixel(int x, int y, bool state)
{
  int posX = x % 8;
  int posY = y % 8;
  int addr = x / 8;

  if (addr == 0)
    addr = 3;
  else if (addr == 1)
    addr = 2;
  else if (addr == 2)
    addr = 1;
  else if (addr == 3)
    addr = 0;

  int d = y / 8;

  if (d == 0)
    addr += 12;
  else if (d == 1)
    addr += 8;
  else if (d == 2)
    addr += 4;
  else if (d == 3)
    addr += 0;

  if (addr < 8)
    lc1.setLed(addr, posY, posX, state); //void setLed(int addr, int row, int col, boolean state)
  else if (addr >= 8)
    lc2.setLed(addr - 8, posY, posX, state); //void setLed(int addr, int row, int col, boolean state)
}

void checkButtons() {
  if (digitalRead(A0) == HIGH)
  {
    if (dir != 'd')
      dir = 'u';
  }
  else if (digitalRead(A1) == HIGH)
  {
    if (dir != 'l')
      dir = 'r';
  }
  else if (digitalRead(A2) == HIGH)
  {
    if (dir != 'u')
      dir = 'd';
  }
  else if (digitalRead(A3) == HIGH)
  {
    if (dir != 'r')
      dir = 'l';
  }
}

void drawSnake() {
  drawPixel(snakeX[snakeLength], snakeY[snakeLength], false);
  drawPixel(snakeX[0], snakeY[0], true);
}

void render() {
  if (isPartOfSnake(fruitX, fruitY)) {
    makeFruit();
    snakeLength++;
  }
  snakeItSelf();

  checkButtons();
  int tempX = snakeX[0];
  int tempY = snakeY[0];

  //Serial.println(dir);
  if (dir == 'u')
  {
    tempY--;
  }
  else if (dir == 'd')
  {
    tempY++;
  }
  else if (dir == 'l')
  {
    tempX--;
  }
  else if (dir == 'r')
  {
    tempX++;
  }

  if (tempX > 31) tempX = 0;
  if (tempY > 31) tempY = 0;
  if (tempX < 0) tempX = 31;
  if (tempY < 0) tempY = 31;

  push(tempX, tempY);
}

void draw()
{
  //  for(int i = 0; i < 8; i++)
  //  {
  //    lc1.clearDisplay(i);
  //    lc2.clearDisplay(i);
  //  }
  drawFruit();
  drawSnake();  
}

void makeFruit() {
  int x, y;
  x = random(0, 32);
  y = random(0, 32);
  while (isPartOfSnake(x, y)) {
    x = random(0, 32);
    y = random(0, 32);
  }
  fruitX = x;
  fruitY = y;
}

void drawFruit() {
  if(flagFruit)
  {
    drawPixel(fruitX, fruitY, true);
    flagFruit = false;
  }
  else
    {
      drawPixel(fruitX, fruitY, false);
      flagFruit = true;
    }
}

boolean isPartOfSnake(int x, int y) {
  for (int i = 0; i < snakeLength - 1; i++) {
    if ((x == snakeX[i]) && (y == snakeY[i])) {
      return true;
    }
  }
  return false;
}

void snakeItSelf() { //poha
  for (int i = 1; i < snakeLength; i++) {
    if ((snakeX[0] == snakeX[i]) && (snakeY[0] == snakeY[i]))
      gameOver();
  }
}

void gameOver() {
  showScore();
  snakeLength = 3;
  for (int i = 0; i < maxSnakeLength; i++) {
    snakeX[i] = 0;
    snakeY[i] = 0;
  }
  dir = 'r';
  snakeX[0] = 5;
  snakeX[1] = 4;
  snakeX[2] = 3;

  drawPixel(snakeX[0], snakeY[0], true);
  drawPixel(snakeX[1], snakeY[1], true);
  drawPixel(snakeX[2], snakeY[2], true);

  makeFruit();
}

void push(int newX, int newY)
{
  for (int i = snakeLength; i >= 1; i--)
  {
    snakeX[i] = snakeX[i - 1];
    snakeY[i] = snakeY[i - 1];
  }
  snakeX[0] = newX;
  snakeY[0] = newY;
}

void showDigit(int addr, int digit) {
  byte zero[7] = {B01110000, B10001000, B10011000, B10101000, B11001000, B10001000, B01110000};
  byte one[7] = {B01000000, B11000000, B01000000, B01000000, B01000000, B01000000, B11100000};
  byte two[7] = {B01110000, B10001000, B00001000, B00010000, B00100000, B01000000, B11111000};
  byte three[7] = {B11111000, B00010000, B00100000, B00010000, B00001000, B10001000, B01110000};
  byte four[7] = {B00010000, B00110000, B01010000, B10010000, B11111000, B00010000, B00010000};
  byte five[7] = {B11111000, B10000000, B11110000, B00001000, B00001000, B10001000, B01110000};
  byte six[7] = {B00110000, B01000000, B10000000, B11110000, B10001000, B10001000, B01110000};
  byte seven[7] = {B11111000, B10001000, B00001000, B00010000, B00100000, B00100000, B00100000};
  byte eight[7] = {B01110000, B10001000, B10001000, B01110000, B10001000, B10001000, B01110000};
  byte nine[7] = {B01110000, B10001000, B10001000, B01111000, B00001000, B00010000, B01100000};

  if (digit == 0)
  {
    for (int i = 0; i < 7; i++) {
      lc1.setRow(addr, i, zero[i]);
    }
  }
  if (digit == 1)
  {
    for (int i = 0; i < 7; i++) {
      lc1.setRow(addr, i, one[i]);
    }
  }
  if (digit == 2)
  {
    for (int i = 0; i < 7; i++) {
      lc1.setRow(addr, i, two[i]);
    }
  }
  if (digit == 3)
  {
    for (int i = 0; i < 7; i++) {
      lc1.setRow(addr, i, three[i]);
    }
  }
  if (digit == 4)
  {
    for (int i = 0; i < 7; i++) {
      lc1.setRow(addr, i, four[i]);
    }
  }
  if (digit == 5)
  {
    for (int i = 0; i < 7; i++) {
      lc1.setRow(addr, i, five[i]);
    }
  }
  if (digit == 6)
  {
    for (int i = 0; i < 7; i++) {
      lc1.setRow(addr, i, six[i]);
    }
  }
  if (digit == 7)
  {
    for (int i = 0; i < 7; i++) {
      lc1.setRow(addr, i, seven[i]);
    }
  }
  if (digit == 8)
  {
    for (int i = 0; i < 7; i++) {
      lc1.setRow(addr, i, eight[i]);
    }
  }
  if (digit == 9)
  {
    for (int i = 0; i < 7; i++) {
      lc1.setRow(addr, i, nine[i]);
    }
  }
}

void showScore() {
  for (int i = 0; i < 8; i++)
  {
    lc1.clearDisplay(i);
    lc2.clearDisplay(i);
  }
  delay(1000);
  int score = 5 * (snakeLength - 3);
  showDigit(0, score % 10);
  score /= 10;
  showDigit(1, score % 10);
  score /= 10;
  showDigit(2, score % 10);
  score /= 10;
  showDigit(3, score % 10);
  score /= 10;
  delay(3000);
  refreshScreen();
}

void debugButton()
{
  if (digitalRead(A0) == HIGH)
  {
    Serial.print("1");
  }
  else
    Serial.print("0");

  if (digitalRead(A1) == HIGH)
  {
    Serial.print("1");
  }
  else
    Serial.print("0");

  if (digitalRead(A2) == HIGH)
  {
    Serial.print("1");
  }
  else
    Serial.print("0");

  if (digitalRead(A3) == HIGH)
  {
    Serial.println("1");
  }
  else
    Serial.println("0");

  delay(200);
}

void refreshScreen()
{
  for (int i = 0; i < 8; i++)
  {
    lc1.clearDisplay(i);
    lc2.clearDisplay(i);
  }
}

