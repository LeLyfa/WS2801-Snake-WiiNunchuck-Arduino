#include <Wiichuck.h>
#include <SPI.h>
#include <Adafruit_WS2801.h>

Adafruit_WS2801 strip = Adafruit_WS2801(152, 3, 2);

Wiichuck wii;

int frame[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 37, 38, 56, 57, 75, 76, 94, 95, 113, 114, 132, 133, 134, 135, 136, 137, 138, 139, 140, 141, 142, 143, 144, 145, 146, 147, 148, 149, 150, 151};

int snakelength = 1;
int x = 4;
int y = 3;
int snakex[] = {x, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
int snakey[] = {y, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
int view = 4;

int applex = 4;
int appley = 6;

int powerx = 2;
int powery = 11;

int dly = 5;
int times = 20;


static int cA(int row/*Y*/, int col/*X*/) {
  int reversed = 0;

  if (col % 2 == 0) {
    reversed = 1;
  }

  int base = (col) * 19;
  if (reversed) {
    base += 19 - 1;
  }
  int final = reversed == 1 ? base - row : base + row;
  return final;
}

void setup() {
  strip.begin();
  strip.show();

  wii.init(0, 0);
  wii.calibrate();

  delay(1000);

  Serial.begin(9600);
}
 
void loop() {
  createFrame(true);
  delay(500);

  game();
  delay(100);
}

void game() {
  fillBlack(false);
  createFrame(true);
  play();
  gameOver();
}

void gameOver() {
  fillBlack(false);
  rainbowCycle(1);
  fillBlack(true);
  delay(500);

  //SCORE?

  asm volatile (" jmp 0");
}

void play() {

  while (true) {
    for (int i = 0; i !=dly; i++) {
      input();
      if(dly == 2){
        if(times == 1){
          times= 11;
          dly=5;
        }
        times--;
      }
      delay(10);
    }
    moove();
    updateDisplay();
    checkPlayerDead();

  }
}

void input() {

  if (wii.poll()) {
    int jx = wii.joyX();
    int jy = wii.joyY();

    if (jx >= 200) {
      view = 2;
    } else if (jx < 70) {
      view = 3;
    } else if (jy > 200) {
      view = 1;
    } else if (jy < 100) {
      view = 0;
    }
  }
}

void moove() {

  if (snakelength > 1) {
    snakex[1] = x;
    snakey[1] = y;
  }

  if (view == 0/*rechts*/) {
    x = x + 1;
  } else if (view == 1/*links*/) {
    x = x - 1;
  } else if (view == 2/*hoch*/) {
    y = y + 1;
  } else if (view == 3/*runter*/) {
    y = y - 1;
  }

  snakex[0] = x;
  snakey[0] = y;

  for (int i = snakelength; i > 1; i--) {
    snakex[i - 1] = snakex[i - 2];
    snakey[i - 1] = snakey[i - 2];
    Serial.println("in loop");
  }

  //Serial.println(x);
  //Serial.println(y);

}

void playerEatsApple() {
  if (x == applex && y == appley) {
    applex = random(1, 6);
    appley = random(1, 17);
    snakelength++;
  }

  if (x == powerx && y == powery) {
    powerx = random(1, 6);
    powery = random(1, 17);
    dly = 2;
  }
}

void checkPlayerDead() {
  if (y == 0 || y == 18 || x == 0 || x == 7) {
    gameOver();
  }

  for (int i = 2; i != sizeof(snakex) / sizeof(int) - 1; i++) {
    if (x == snakex[i] && y == snakey[i]) {
      gameOver();
    }
  }
}


void updateDisplay() {
  fillBlack(false);
  createFrame(false);

  for (int i = 0; i != snakelength; i++) {
    strip.setPixelColor(cA(snakey[i], snakex[i]), Color(0, 0, 100));
  }
  playerEatsApple();
  strip.setPixelColor(cA(appley, applex), Color(0, 100, 0));
  strip.setPixelColor(cA(powery, powerx), Color(100, 60, 120));
  strip.show();

}

void createFrame(boolean show) {
  for (int i = 0; i != sizeof(frame) / sizeof(int); i++) {
    strip.setPixelColor(frame[i], Color(100, 0, 0));
  }
  if (show) {
    strip.show();
  }
}

void fillBlack(boolean show) {
  for (int i = 0; i != strip.numPixels(); i++) {
    strip.setPixelColor(i, Color(0, 0, 0));
  }
  if (show) {
    strip.show();
  }
}

uint32_t Color(byte r, byte g, byte b) {
  uint32_t c;
  c = r;
  c <<= 8;
  c |= g;
  c <<= 8;
  c |= b;
  return c;
}

uint32_t Wheel(byte WheelPos)
{
  if (WheelPos < 85) {
    return Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  } else if (WheelPos < 170) {
    WheelPos -= 85;
    return Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } else {
    WheelPos -= 170;
    return Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
}

void rainbowCycle(uint8_t wait) {
  int i, j;

  for (j = 0; j < 256 * 0.5; j++) {   // 5 cycles of all 25 colors in the wheel
    for (i = 0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel( ((i * 256 / strip.numPixels()) + j) % 256) );
    }
    strip.show();   // write all the pixels out
    delay(wait);
  }
}
