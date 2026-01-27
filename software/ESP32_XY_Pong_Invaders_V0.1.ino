// ESP32 Vectorgame Console V0.1 beta by Ingmar Bihlo (ingmarsretro) jan.2026
// the esp-32 controller generates analoge voltage for x- and y- axis to be routed to an old crt oszilloscope
// in XY-Mode. The scope beam drawing all objects sequencial in vectors (you know VECTREX ?).
// to simulate the "blanking" of the beam e.g. while moving from one object to the next the speed of the beam is higher
// so you (allmost) dont see it.
// this code was written for a demonstration at the fh-day
// but i will it go on with it ...  
#include <Arduino.h>

/* =================== IOs input and output ports =================== */
#define DAC_X 25   //dac out for x axis
#define DAC_Y 26   //dac out for y axix
#define POT_L 34   //adc in for poti left
#define POT_R 35   //adc in for poti right
#define BTN_FIRE 32  //digital in for fire/start button
#define SOUND_PIN 27 //pwm out for buzzer/speaker
#define SOUND_CH  0

/* =================== voltages for field size and objektpos =================== */
#define SCREEN_MIN 20
#define SCREEN_MAX 235
#define FIELD_LEFT   30
#define FIELD_RIGHT  225
#define FIELD_BOTTOM 30
#define FIELD_TOP    220
#define PADDLE_X_L 40
#define PADDLE_X_R 215
#define PADDLE_H   35

/* =================== game modes enum =================== */
enum GameMode {
  MODE_SELECT,
  MODE_PONG,
  MODE_INVADERS
};

GameMode currentMode = MODE_SELECT;

/* =================== PONG Variablen =================== */
float ballX = 128, ballY = 128;
float ballVX = 1.2, ballVY = 0.9;
int scoreL = 0;
int scoreR = 0;
bool pongGameOver = false;
unsigned long pongGameOverTime = 0;
#define PONG_WIN_SCORE 5

/* =================== INVADERS Variablen =================== */
#define X_MIN 20
#define X_MAX 235
#define Y_MIN 30
#define Y_MAX 220
#define PLAYER_Y 50
#define INV_STEP_DOWN 10
#define INV_COUNT 5

float playerX = 128;
bool shotActive = false;
float shotX, shotY;
bool invAlive[INV_COUNT];
float invX[INV_COUNT];
float invY[INV_COUNT];
float invDir = 1;
bool gameOver = false;
unsigned long gameOverTime = 0;
unsigned long lastInvaderSound = 0;
int invaderSoundStep = 0;
const int invaderTones[4] = { 220, 175, 196, 165 };

/* =================== MENU Variablen =================== */
int menuSelection = 0; // 0=PONG, 1=INVADERS
unsigned long lastButtonPress = 0;

/* =================== dac =================== */
inline void setXY(uint8_t x, uint8_t y) {
  dacWrite(DAC_X, x);
  dacWrite(DAC_Y, y);
}

inline void jumpTo(uint8_t x, uint8_t y) {
  setXY(x, y);
  delayMicroseconds(2);
}

/* =================== sound =================== */
void tonausgabe(int freq, int dur_ms) {
  ledcWriteTone(SOUND_PIN, freq);
  delay(dur_ms);
  ledcWriteTone(SOUND_PIN, 0);
}

/* =================== draw lines =================== */
void drawLine(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1) {
  const int steps = 24;
  for (int i = 0; i <= steps; i++) {
    float t = (float)i / steps;
    setXY(
      x0 + t * (x1 - x0),
      y0 + t * (y1 - y0)
    );
    delayMicroseconds(10);
  }
}

void drawRect(uint8_t x, uint8_t y, uint8_t w, uint8_t h) {
  drawLine(x, y, x + w, y);
  drawLine(x + w, y, x + w, y + h);
  drawLine(x + w, y + h, x, y + h);
  drawLine(x, y + h, x, y);
}

/* =================== numbers - digits =================== */
void drawDigit(uint8_t d, uint8_t x, uint8_t y, uint8_t s) {
  static const uint8_t seg[10][7] = {
    {1,1,1,1,1,1,0},{0,1,1,0,0,0,0},{1,1,0,1,1,0,1},
    {1,1,1,1,0,0,1},{0,1,1,0,0,1,1},{1,0,1,1,0,1,1},
    {1,0,1,1,1,1,1},{1,1,1,0,0,0,0},{1,1,1,1,1,1,1},
    {1,1,1,1,0,1,1}
  };

  uint8_t w = s, h = s * 2;

  if (seg[d][0]) drawLine(x, y+h, x+w, y+h);
  if (seg[d][1]) drawLine(x+w, y+h, x+w, y+h/2);
  if (seg[d][2]) drawLine(x+w, y+h/2, x+w, y);
  if (seg[d][3]) drawLine(x, y, x+w, y);
  if (seg[d][4]) drawLine(x, y+h/2, x, y);
  if (seg[d][5]) drawLine(x, y+h, x, y+h/2);
  if (seg[d][6]) drawLine(x, y+h/2, x+w, y+h/2);
}

/* =================== text characters for "FH KAERNTEN" =================== */
void drawCharF(int x,int y,int s){
  drawLine(x,y,x,y+3*s);
  drawLine(x,y+3*s,x+2*s,y+3*s);
  drawLine(x,y+2*s,x+2*s,y+2*s);
}
void drawCharH(int x,int y,int s){
  drawLine(x,y,x,y+3*s);
  drawLine(x+2*s,y,x+2*s,y+3*s);
  drawLine(x,y+2*s,x+2*s,y+2*s);
}
void drawCharK(int x,int y,int s){
  drawLine(x,y,x,y+3*s);
  drawLine(x,y+2*s,x+2*s,y+3*s);
  drawLine(x,y+2*s,x+2*s,y);
}
void drawCharA(int x,int y,int s){
  drawLine(x,y,x+s,y+3*s);
  drawLine(x+s,y+3*s,x+2*s,y);
  drawLine(x+s/2,y+2*s,x+3*s/2,y+2*s);
}
void drawCharE(int x,int y,int s){
  drawLine(x,y,x,y+3*s);
  drawLine(x,y+3*s,x+2*s,y+3*s);
  drawLine(x,y+2*s,x+2*s,y+2*s);
  drawLine(x,y,x+2*s,y);
}
void drawCharR(int x,int y,int s){
  drawLine(x,y,x,y+3*s);
  drawLine(x,y+3*s,x+2*s,y+3*s);
  drawLine(x+2*s,y+3*s,x+2*s,y+2*s);
  drawLine(x,y+2*s,x+2*s,y+2*s);
  drawLine(x,y+2*s,x+2*s,y);
}
void drawCharN(int x,int y,int s){
  drawLine(x,y,x,y+3*s);
  drawLine(x,y+3*s,x+2*s,y);
  drawLine(x+2*s,y,x+2*s,y+3*s);
}
void drawCharT(int x,int y,int s){
  drawLine(x,y+3*s,x+2*s,y+3*s);
  drawLine(x+s,y,x+s,y+3*s);
}
void drawCharP(int x,int y,int s){
  drawLine(x,y,x,y+3*s);
  drawLine(x,y+3*s,x+2*s,y+3*s);
  drawLine(x+2*s,y+3*s,x+2*s,y+2*s);
  drawLine(x,y+2*s,x+2*s,y+2*s);
}
void drawCharO(int x,int y,int s){
  drawLine(x,y,x,y+3*s);
  drawLine(x,y+3*s,x+2*s,y+3*s);
  drawLine(x+2*s,y+3*s,x+2*s,y);
  drawLine(x,y,x+2*s,y);
}
void drawCharG(int x,int y,int s){
  drawLine(x+2*s,y,x+2*s,y+3*s);
  drawLine(x,y+3*s,x+2*s,y+3*s);
  drawLine(x,y+2*s,x+2*s,y+2*s);
  drawLine(x,y,x+2*s,y);
}
void drawCharI(int x,int y,int s){
  drawLine(x,y,x,y+3*s);
}
void drawCharV(int x,int y,int s){
  drawLine(x,y+3*s,x+s,y);
  drawLine(x+s,y,x+2*s,y+3*s);
}
void drawCharD(int x,int y,int s){
  drawLine(x,y,x,y+3*s);
  drawLine(x,y+3*s,x+2*s,y+2*s);
  drawLine(x+2*s,y+2*s,x+2*s,y+s);
  drawLine(x+2*s,y+s,x,y);
}
void drawCharS(int x,int y,int s){
  drawLine(x,y,x+2*s,y);
  drawLine(x,y,x,y+s);
  drawLine(x,y+s,x+2*s,y+s);
  drawLine(x+2*s,y+s,x+2*s,y+2*s);
  drawLine(x,y+2*s,x+2*s,y+2*s);
  drawLine(x,y+2*s,x,y+3*s);
  drawLine(x,y+3*s,x+2*s,y+3*s);
}

void drawTextFHKaernten() {
  int x = 50, y = 35, s = 4;
  drawCharF(x,y,s); x+=14;
  drawCharH(x,y,s); x+=22;
  drawCharK(x,y,s); x+=14;
  drawCharA(x,y,s); x+=14;
  drawCharE(x,y,s); x+=14;
  drawCharR(x,y,s); x+=14;
  drawCharN(x,y,s); x+=14;
  drawCharT(x,y,s); x+=14;
  drawCharE(x,y,s); x+=14;
  drawCharN(x,y,s);
}

/* =================== menue =================== */
void drawMenuPong(int y) {
  int x = 70, s = 5;
  drawCharP(x,y,s); x+=14;
  drawCharO(x,y,s); x+=14;
  drawCharN(x,y,s); x+=14;
  drawCharG(x,y,s);
}

void drawMenuInvader(int y) {
  int x = 50, s = 5;
  drawCharI(x,y,s); x+=10;
  drawCharN(x,y,s); x+=14;
  drawCharV(x,y,s); x+=14;
  drawCharA(x,y,s); x+=14;
  drawCharD(x,y,s); x+=14;
  drawCharE(x,y,s); x+=14;
  drawCharR(x,y,s);
}

void drawMenu() {
  jumpTo(0,0);
  
  drawTextFHKaernten();
  
  // PONG Option
  drawMenuPong(140);
  if (menuSelection == 0) {
    drawRect(60, 135, 100, 25); // frame for pong
  }
  
  // INVADER Option
  drawMenuInvader(100);
  if (menuSelection == 1) {
    drawRect(40, 95, 140, 25); // frame for invader
  }
}

void updateMenu() {
  // read poti for selection
  int potVal = analogRead(POT_L);
  menuSelection = (potVal > 2047) ? 1 : 0;
  
  // read button press for execution 
  if (digitalRead(BTN_FIRE) == LOW && millis() - lastButtonPress > 300) {
    lastButtonPress = millis();
    tonausgabe(1000, 100);
    
    if (menuSelection == 0) {
      currentMode = MODE_PONG;
      // PONG initialisieren
      ballX = 128; ballY = 128;
      ballVX = 1.2; ballVY = 0.9;
      scoreL = 0; scoreR = 0;
      pongGameOver = false;
      } 
    else {
      currentMode = MODE_INVADERS;
      // INVADERS initialisieren
      gameOver = false;
      shotActive = false;
      for(int i=0;i<INV_COUNT;i++){
        invAlive[i]=true;
        invX[i]=60+i*30;
        invY[i]=180;
      }
    }
  }
}

/* =================== PONG game =================== */
void updatePong(int pL, int pR) {
  if (pongGameOver) {
    // Zurück zum Menü nach 5 Sekunden
    if (millis() - pongGameOverTime > 5000) {
      currentMode = MODE_SELECT;
      pongGameOver = false;
      menuSelection = 0;
    }
    return;
  }

  ballX += ballVX;
  ballY += ballVY;

  if (ballY < FIELD_BOTTOM || ballY > FIELD_TOP) {
    ballVY *= -1;
    tonausgabe(900, 35);
  }

  if (ballX < PADDLE_X_L+3 && ballY > pL && ballY < pL+PADDLE_H) {
    ballVX *= -1;
    tonausgabe(1200, 50);
  }

  if (ballX > PADDLE_X_R-3 && ballY > pR && ballY < pR+PADDLE_H) {
    ballVX *= -1;
    tonausgabe(1200, 50);
  }

  if (ballX < FIELD_LEFT) {
    scoreR++; 
    tonausgabe(500, 120);
    if (scoreR >= PONG_WIN_SCORE) {
      pongGameOver = true;
      pongGameOverTime = millis();
      tonausgabe(300, 500);
    }
    ballX = 128; ballY = 128;
  }

  if (ballX > FIELD_RIGHT) {
    scoreL++; 
    tonausgabe(500, 120);
    if (scoreL >= PONG_WIN_SCORE) {
      pongGameOver = true;
      pongGameOverTime = millis();
      tonausgabe(300, 500);
    }
    ballX = 128; ballY = 128;
  }
}

void drawPong(int pL, int pR) {
  jumpTo(0,0);

  drawRect(FIELD_LEFT,FIELD_BOTTOM,FIELD_RIGHT-FIELD_LEFT,FIELD_TOP-FIELD_BOTTOM);

  drawLine(PADDLE_X_L,pL,PADDLE_X_L,pL+PADDLE_H);
  drawLine(PADDLE_X_R,pR,PADDLE_X_R,pR+PADDLE_H);

  drawRect(ballX,ballY,4,4);

  drawDigit(scoreL%10,90,200,6);
  drawDigit(scoreR%10,150,200,6);

  drawTextFHKaernten();

  // Game Over display
  if (pongGameOver) {
    drawLine(70,120,180,120);
    drawLine(70,110,180,110);
    drawLine(70,130,180,130);
  }
}

/* =================== INVADERS game =================== */
int aliveInvaders() {
  int c=0;
  for(int i=0;i<INV_COUNT;i++)
    if(invAlive[i]) c++;
  return c;
}

void updateInvaderSound() {
  if (gameOver) return;
  int alive = aliveInvaders();
  int interval = 500 - (INV_COUNT - alive) * 80;
  interval = constrain(interval, 80, 500);
  if (millis() - lastInvaderSound > interval) {
    lastInvaderSound = millis();
    ledcWriteTone(SOUND_PIN, invaderTones[invaderSoundStep]);
    delay(20);
    ledcWriteTone(SOUND_PIN, 0);
    invaderSoundStep = (invaderSoundStep + 1) % 4;
  }
}

void updateInvaders() {
  if (gameOver) {
    // Zurück zum Menü nach 5 Sekunden
    if (millis() - gameOverTime > 5000) {
      currentMode = MODE_SELECT;
      gameOver = false;
      menuSelection = 1;
    }
    return;
  }

  playerX = map(analogRead(POT_L),0,4095,X_MIN+10,X_MAX-10);

  if (!shotActive && digitalRead(BTN_FIRE)==LOW){
    shotActive=true;
    shotX=playerX;
    shotY=PLAYER_Y+8;
    tonausgabe(1500,20);
  }

  if (shotActive){
    shotY+=2;
    if (shotY>Y_MAX) shotActive=false;

    for(int i=0;i<INV_COUNT;i++){
      if(invAlive[i] &&
         abs(shotX-invX[i])<6 &&
         abs(shotY-invY[i])<4){
        invAlive[i]=false;
        shotActive=false;
        tonausgabe(400,80);
        
        // Prüfen ob alle Invader zerstört
        if (aliveInvaders() == 0) {
          gameOver = true;
          gameOverTime = millis();
          tonausgabe(800, 100);
          delay(50);
          tonausgabe(1000, 100);
          delay(50);
          tonausgabe(1200, 200);
        }
      }
    }
  }

  bool hitBorder=false;
  int alive = aliveInvaders();
  float speed = 0.6 + (INV_COUNT - alive) * 0.35;

  for(int i=0;i<INV_COUNT;i++){
    if(!invAlive[i]) continue;
    invX[i]+= (invDir>0?speed:-speed);
    if(invX[i]<X_MIN+10 || invX[i]>X_MAX-10)
      hitBorder=true;
  }

  if(hitBorder){
    invDir=-invDir;
    for(int i=0;i<INV_COUNT;i++){
      invY[i]-=INV_STEP_DOWN;
      if(invAlive[i] && invY[i]<=PLAYER_Y+5){
        gameOver=true;
        gameOverTime = millis();
        tonausgabe(120,400);
      }
    }
  }
}

void drawInvaders() {
  jumpTo(0,0);

  // Player (Kanone)
  drawLine(playerX-8,PLAYER_Y,playerX+8,PLAYER_Y);
  drawLine(playerX+8,PLAYER_Y,playerX,PLAYER_Y+10);
  drawLine(playerX,PLAYER_Y+10,playerX-8,PLAYER_Y);

  // Schuss
  if(shotActive)
    drawLine(shotX, shotY, shotX, shotY+6);

  // Invaders
  for(int i=0;i<INV_COUNT;i++)
    if(invAlive[i])
      drawRect(invX[i]-6,invY[i]-4,12,8);

  drawTextFHKaernten();

  // Game Over
  if(gameOver){
    drawLine(90,120,170,120);
    drawLine(90,110,170,110);
  }
}

/* =================== setup =================== */
void setup() {
  analogReadResolution(12);
  pinMode(BTN_FIRE, INPUT_PULLUP);
  ledcAttach(SOUND_PIN, 2000, 8);
  
  // Startton
  tonausgabe(800, 100);
  delay(50);
  tonausgabe(1000, 100);
}

/* =================== mainloop =================== */
void loop() {
  switch(currentMode) {
    case MODE_SELECT:
      updateMenu();
      drawMenu();
      break;
      
    case MODE_PONG:
      {
        int pL = map(analogRead(POT_L),0,4095,FIELD_BOTTOM,FIELD_TOP-PADDLE_H);
        int pR = map(analogRead(POT_R),0,4095,FIELD_BOTTOM,FIELD_TOP-PADDLE_H);
        updatePong(pL, pR);
        drawPong(pL, pR);
      }
      break;
      
    case MODE_INVADERS:
      updateInvaders();
      updateInvaderSound();
      drawInvaders();
      break;
  }
}
