#include <MD_Parola.h>
#include <MD_MAX72xx.h>
#include <SPI.h>
#include <math.h>

#define CLK 13  
#define DIN 11 
#define LOAD 10   
#define P1DOWN A4
#define P1UP A5
#define P2DOWN 1
#define P2UP 0



class Rectangle {
public:
  Rectangle(uint8_t width, uint8_t length, int8_t x, int8_t y, int8_t ySpeed, int8_t xSpeed) 
  : width(width), length(length)
  , x(x), y(y)
  , xSpeed(xSpeed), ySpeed(ySpeed) {}
  uint8_t width = {0};
  uint8_t length = {0};
  int8_t x = {0};
  int8_t y = {0};
  int8_t ySpeed = {0};
  int8_t xSpeed = {0};
};


// SPI hardware interface
MD_MAX72XX mx = MD_MAX72XX(MD_MAX72XX::PAROLA_HW, LOAD, 1);

class GameUtil {
public:
  static void show(Rectangle rect) {
    const int mask = pow(2, rect.width) - 1;
    for (int i = 0; i < rect.length; i++) {
      mx.setRow(0, rect.y + i, (mask << rect.x | mx.getRow(0, rect.y + i)));
    }
  }

  static void initializeGame();
};

// matrix dimensions 
const uint8_t matrixHeight  = 8;
const uint8_t matrixWidth   = 8;

// initial positions
const uint8_t firstRacketStartingY = matrixHeight / 2 - 1;
const uint8_t secondRacketStartingY = matrixHeight / 2 - 1;
const uint8_t firstRacketStartingX = 0;
const uint8_t secondRacketStartingX = matrixWidth - 1;
uint8_t ballStartingX = matrixWidth / 2;
const uint8_t ballStartingY = matrixHeight / 2;
uint8_t ballStartingHorVel  = 1;
const uint8_t ballStartingVerVel  = 1;
const uint8_t racketStartingHorVel  = 0;
const uint8_t racketStartingVerVel  = 0;

// rackets' dimension
const uint8_t racketLength  = 3;
const uint8_t racketWidth   = 1;

// ball's dimension
const uint8_t ballWidth = 1;
const uint8_t ballLength = 1;

// player 1 buttons
uint8_t buttonStateP1Down = 0;
uint8_t buttonStateP1Up = 0;

// player 2 buttons
uint8_t buttonStateP2Down = 0;
uint8_t buttonStateP2Up = 0;


// first player's racket object
Rectangle racket1 = Rectangle(racketWidth, racketLength, firstRacketStartingX, firstRacketStartingY, racketStartingVerVel, racketStartingHorVel); // player 1 racket
// second player's racket object
Rectangle racket2 = Rectangle(racketWidth, racketLength, secondRacketStartingX, secondRacketStartingY, racketStartingVerVel, racketStartingHorVel); // player 2 racket
// ball's object
Rectangle ball = Rectangle(ballWidth, ballLength, ballStartingX, ballStartingY, ballStartingVerVel, ballStartingHorVel); // ball

  
static void GameUtil::initializeGame() {
  // first player's racket object
  racket1 = Rectangle(racketWidth, racketLength, firstRacketStartingX, firstRacketStartingY, racketStartingVerVel, racketStartingHorVel); // player 1 racket
  // second player's racket object
  racket2 = Rectangle(racketWidth, racketLength, secondRacketStartingX, secondRacketStartingY, racketStartingVerVel, racketStartingHorVel); // player 2 racket
  // ball's object
  ball = Rectangle(ballWidth, ballLength, ballStartingX, ballStartingY, ballStartingVerVel, ballStartingHorVel); // ball
}

void moveRackets() {
  if (buttonStateP1Down == LOW && racket1.y < matrixHeight - racketLength) {
    racket1.y++;
  } else if (buttonStateP1Up == LOW && racket1.y > 0) {
    racket1.y--;
  }
  if (buttonStateP2Down == LOW && racket2.y < matrixHeight - racketLength) {
    racket2.y++;
  } else if (buttonStateP2Up == LOW && racket2.y > 0) {
    racket2.y--;
  }
}

void moveBall() {
  ball.x += ball.xSpeed;
  ball.y += ball.ySpeed;

  if (ball.y >= matrixHeight) {
    ball.ySpeed *= -1;
    ball.y = matrixHeight - 1;
  } else if (ball.y < 0) {
    ball.ySpeed *= -1;
    ball.y = 0;
  }
  
  if (ball.x == matrixWidth - 1 && (ball.y >= racket2.y && ball.y < racket2.y + racket2.length)) {
    ball.xSpeed *= -1;
    ball.x = matrixWidth - 1;
  } else if (ball.x == 0 && (ball.y >= racket1.y && ball.y < racket1.y + racket1.length)) {
    ball.xSpeed *= -1;
    ball.x = 0;
  }
  
  if (ball.x == matrixWidth || ball.x == -1) {
    GameUtil::initializeGame();
  } 
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(P1DOWN, INPUT);
  pinMode(P1UP, INPUT);
  pinMode(P2DOWN, INPUT);
  pinMode(P2UP, INPUT);
  mx.begin();
  mx.clear(0);
  char data;
  Serial.print('1');
  do {
    data = Serial.read();
    if (data == '1') {
      Serial.print('2');
      break;
    } else if (data == '2') {
      ball.xSpeed *= -1;
      ball.x -= 1;
      ballStartingHorVel = -1;
      ballStartingX = matrixWidth / 2 - 1;
      break;
    }
  } while (true);
}

void handle_input() {
  buttonStateP2Up = HIGH;
  buttonStateP2Down = HIGH;
  if (Serial.available()) 
  {
    char data = Serial.read();
    if (data == 'u')
      buttonStateP2Up = LOW;
    else if (data == 'd')
     buttonStateP2Down = LOW;
  }
}

void loop() {

  delay(200);
  mx.clear(0);
  handle_input();
  if ((buttonStateP1Down = digitalRead(P1DOWN)) == LOW)
    Serial.print('d');
  if ((buttonStateP1Up = digitalRead(P1UP)) == LOW)
    Serial.print('u');

  moveRackets();
  moveBall();
  GameUtil::show(ball);
  GameUtil::show(racket1);
  GameUtil::show(racket2);
}