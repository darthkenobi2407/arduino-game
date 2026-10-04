#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// ============================================================
// OLED
// ============================================================

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// ============================================================
// BUTTONS
// ============================================================

#define BTN_UP     2
#define BTN_DOWN   3
#define BTN_LEFT   4
#define BTN_RIGHT  5

// ============================================================
// GAME STATES
// ============================================================

enum GameState {
  MENU,
  FLAPPY,
  CROSSY
};

GameState gameState = MENU;

int menuSelection = 0;

// ============================================================
// BUTTON HELPERS
// ============================================================

bool pressed(int pin) {
  return digitalRead(pin) == LOW;
}

bool buttonPressed(int pin) {
  static bool lastState[6] = {HIGH, HIGH, HIGH, HIGH, HIGH, HIGH};

  bool current = digitalRead(pin);
  bool result = false;

  if (current == LOW && lastState[pin] == HIGH) {
    result = true;
  }

  lastState[pin] = current;

  return result;
}

// ============================================================
// FLAPPY BIRD VARIABLES
// ============================================================

float birdY;
float birdVelocity;

const int birdX = 25;
const int birdSize = 5;

float pipeX;
int pipeGapY;

const int pipeWidth = 12;
const int pipeGap = 23;

int flappyScore;
bool flappyGameOver;

// ============================================================
// CROSSY ROAD VARIABLES
// ============================================================

int playerX;
int playerY;

const int playerSize = 5;

int roadOffset = 0;
int crossyScore;

bool crossyGameOver;

// Cars
struct Car {
  int x;
  int y;
  int speed;
  int width;
};

Car cars[8];

// ============================================================
// SETUP
// ============================================================

void setup() {

  pinMode(BTN_UP, INPUT_PULLUP);
  pinMode(BTN_DOWN, INPUT_PULLUP);
  pinMode(BTN_LEFT, INPUT_PULLUP);
  pinMode(BTN_RIGHT, INPUT_PULLUP);

  randomSeed(analogRead(A0));

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    while (true);
  }

  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);

  showSplash();

  delay(1000);
}

// ============================================================
// MAIN LOOP
// ============================================================

void loop() {

  if (gameState == MENU) {
    menuLoop();
  }

  else if (gameState == FLAPPY) {
    flappyLoop();
  }

  else if (gameState == CROSSY) {
    crossyLoop();
  }
}

// ============================================================
// SPLASH SCREEN
// ============================================================

void showSplash() {

  display.clearDisplay();

  display.setTextSize(2);
  display.setCursor(18, 15);
  display.println("NANO");

  display.setCursor(10, 35);
  display.println("ARCADE");

  display.display();

  delay(1200);
}

// ============================================================
// MENU
// ============================================================

void menuLoop() {

  if (buttonPressed(BTN_UP)) {
    menuSelection--;

    if (menuSelection < 0)
      menuSelection = 1;
  }

  if (buttonPressed(BTN_DOWN)) {
    menuSelection++;

    if (menuSelection > 1)
      menuSelection = 0;
  }

  if (buttonPressed(BTN_LEFT) ||
      buttonPressed(BTN_RIGHT)) {

    if (menuSelection == 0) {
      startFlappy();
    }

    else {
      startCrossy();
    }
  }

  drawMenu();
}

// ============================================================
// DRAW MENU
// ============================================================

void drawMenu() {

  display.clearDisplay();

  display.setTextSize(2);
  display.setCursor(25, 2);
  display.println("ARCADE");

  display.setTextSize(1);

  // Flappy
  if (menuSelection == 0) {
    display.fillRect(8, 22, 112, 15, SSD1306_WHITE);

    display.setTextColor(SSD1306_BLACK);
    display.setCursor(22, 26);
    display.println("FLAPPY");

    display.setTextColor(SSD1306_WHITE);
  }

  else {
    display.drawRect(8, 22, 112, 15, SSD1306_WHITE);

    display.setCursor(22, 26);
    display.println("FLAPPY");
  }

  // Crossy
  if (menuSelection == 1) {
    display.fillRect(8, 41, 112, 15, SSD1306_WHITE);

    display.setTextColor(SSD1306_BLACK);
    display.setCursor(22, 45);
    display.println("CROSSY");

    display.setTextColor(SSD1306_WHITE);
  }

  else {
    display.drawRect(8, 41, 112, 15, SSD1306_WHITE);

    display.setCursor(22, 45);
    display.println("CROSSY");
  }

  display.display();

  delay(30);
}

// ============================================================
// FLAPPY START
// ============================================================

void startFlappy() {

  gameState = FLAPPY;

  birdY = 32;
  birdVelocity = 0;

  pipeX = 128;

  pipeGapY = random(15, 45);

  flappyScore = 0;

  flappyGameOver = false;
}

// ============================================================
// FLAPPY LOOP
// ============================================================

void flappyLoop() {

  if (flappyGameOver) {

    drawFlappy();

    display.setTextSize(1);
    display.setCursor(25, 28);
    display.println("GAME OVER");

    display.setCursor(18, 40);
    display.println("D4 = RETRY");

    display.setCursor(18, 51);
    display.println("D5 = MENU");

    display.display();

    if (buttonPressed(BTN_LEFT)) {
      startFlappy();
    }

    if (buttonPressed(BTN_RIGHT)) {
      gameState = MENU;
    }

    delay(80);

    return;
  }

  // Flap
  if (buttonPressed(BTN_LEFT)) {
    birdVelocity = -2.8;
  }

  // Gravity
  birdVelocity += 0.18;

  birdY += birdVelocity;

  // Pipe movement
  pipeX -= 1.4;

  // New pipe
  if (pipeX < -pipeWidth) {

    pipeX = 128;

    pipeGapY = random(14, 45);

    flappyScore++;
  }

  // Collision with top/bottom
  if (birdY < 0 ||
      birdY + birdSize > 63) {

    flappyGameOver = true;
  }

  // Pipe collision
  if (birdX + birdSize > pipeX &&
      birdX < pipeX + pipeWidth) {

    if (birdY < pipeGapY - pipeGap / 2 ||
        birdY + birdSize > pipeGapY + pipeGap / 2) {

      flappyGameOver = true;
    }
  }

  // Draw
  drawFlappy();

  delay(20);
}

// ============================================================
// DRAW FLAPPY
// ============================================================

void drawFlappy() {

  display.clearDisplay();

  // Score
  display.setTextSize(1);
  display.setCursor(2, 2);
  display.print("S:");
  display.print(flappyScore);

  // Bird
  display.fillRect(
    birdX,
    (int)birdY,
    birdSize,
    birdSize,
    SSD1306_WHITE
  );

  // Bird eye
  display.drawPixel(
    birdX + 3,
    (int)birdY + 1,
    SSD1306_BLACK
  );

  // Pipe top
  display.fillRect(
    pipeX,
    0,
    pipeWidth,
    pipeGapY - pipeGap / 2,
    SSD1306_WHITE
  );

  // Pipe bottom
  display.fillRect(
    pipeX,
    pipeGapY + pipeGap / 2,
    pipeWidth,
    64 - (pipeGapY + pipeGap / 2),
    SSD1306_WHITE
  );

  display.display();
}

// ============================================================
// CROSSY START
// ============================================================

void startCrossy() {

  gameState = CROSSY;

  playerX = 62;
  playerY = 56;

  crossyScore = 0;

  crossyGameOver = false;

  roadOffset = 0;

  // Create cars
  for (int i = 0; i < 8; i++) {

    cars[i].x = random(-20, 128);
    cars[i].y = 12 + (i / 2) * 10;

    cars[i].speed = (i % 2 == 0) ? 1 : -1;

    cars[i].width = random(8, 15);
  }
}

// ============================================================
// CROSSY LOOP
// ============================================================

void crossyLoop() {

  if (crossyGameOver) {

    drawCrossy();

    display.setTextSize(1);

    display.setCursor(25, 27);
    display.println("GAME OVER");

    display.setCursor(18, 40);
    display.println("D4 = RETRY");

    display.setCursor(18, 51);
    display.println("D5 = MENU");

    display.display();

    if (buttonPressed(BTN_LEFT)) {
      startCrossy();
    }

    if (buttonPressed(BTN_RIGHT)) {
      gameState = MENU;
    }

    delay(80);

    return;
  }

  // ==========================================================
  // PLAYER MOVEMENT
  // ==========================================================

  if (buttonPressed(BTN_UP)) {

    playerY -= 6;

    if (playerY < 5)
      playerY = 5;

    crossyScore++;
  }

  if (buttonPressed(BTN_DOWN)) {

    playerY += 6;

    if (playerY > 57)
      playerY = 57;
  }

  if (buttonPressed(BTN_LEFT)) {

    playerX -= 6;

    if (playerX < 0)
      playerX = 0;
  }

  if (buttonPressed(BTN_RIGHT)) {

    playerX += 6;

    if (playerX > 123)
      playerX = 123;
  }

  // ==========================================================
  // MOVE CARS
  // ==========================================================

  for (int i = 0; i < 8; i++) {

    cars[i].x += cars[i].speed;

    // Wrap around
    if (cars[i].speed > 0 &&
        cars[i].x > 128) {

      cars[i].x = -cars[i].width;
    }

    if (cars[i].speed < 0 &&
        cars[i].x < -cars[i].width) {

      cars[i].x = 128;
    }

    // Collision
    if (playerX + playerSize > cars[i].x &&
        playerX < cars[i].x + cars[i].width &&
        playerY + playerSize > cars[i].y &&
        playerY < cars[i].y + 6) {

      crossyGameOver = true;
    }
  }

  // Reached top
  if (playerY <= 5) {

    crossyScore++;

    playerY = 57;

    // Increase difficulty
    for (int i = 0; i < 8; i++) {

      if (cars[i].speed > 0)
        cars[i].speed++;

      else
        cars[i].speed--;
    }
  }

  drawCrossy();

  delay(40);
}

// ============================================================
// DRAW CROSSY ROAD
// ============================================================

void drawCrossy() {

  display.clearDisplay();

  // Score
  display.setTextSize(1);
  display.setCursor(2, 2);
  display.print("S:");
  display.print(crossyScore);

  // Grass
  display.fillRect(
    0,
    0,
    128,
    10,
    SSD1306_WHITE
  );

  display.fillRect(
    0,
    54,
    128,
    10,
    SSD1306_WHITE
  );

  // Road
  display.fillRect(
    0,
    10,
    128,
    44,
    SSD1306_BLACK
  );

  // Road lane markings
  for (int y = 15; y < 54; y += 10) {

    for (int x = 0; x < 128; x += 16) {

      display.drawLine(
        x,
        y,
        x + 7,
        y,
        SSD1306_WHITE
      );
    }
  }

  // Cars
  for (int i = 0; i < 8; i++) {

    display.fillRect(
      cars[i].x,
      cars[i].y,
      cars[i].width,
      6,
      SSD1306_WHITE
    );

    // Wheels
    display.drawPixel(
      cars[i].x + 2,
      cars[i].y + 5,
      SSD1306_BLACK
    );

    display.drawPixel(
      cars[i].x + cars[i].width - 3,
      cars[i].y + 5,
      SSD1306_BLACK
    );
  }

  // Player
  display.fillRect(
    playerX,
    playerY,
    playerSize,
    playerSize,
    SSD1306_WHITE
  );

  // Player eyes
  display.drawPixel(
    playerX + 1,
    playerY + 1,
    SSD1306_BLACK
  );

  display.drawPixel(
    playerX + 3,
    playerY + 1,
    SSD1306_BLACK
  );

  display.display();
}