
#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Wire.h>
#include<Fp.h>
#define LUNGIME_ECRAN 128
#define LATIME_ECRAN 64

extern Adafruit_SSD1306 display;

// Button
const static int D2 = 2;

static int randomNum = random(10,40);
static int randomNum2 = random(10,40);
static int joysticSW = 25;
extern bool jocActiv;
//Bird
static int px = 20,py = 30;
static int raza = 4;
//Animations
const uint8_t pasare1[] PROGMEM = {
  0b00000000,
  0b00111100,
  0b01111110,
  0b11110111,
  0b11111111,
  0b01111110,
  0b00101000,
  0b01000100
};

const uint8_t pasare2[] PROGMEM = {
  0b00000000,
  0b00000000,
  0b00111100,
  0b01110110,
  0b11111111,
  0b01111110,
  0b00111100,
  0b00011000
};

const uint8_t pasare3[] PROGMEM = {
  0b01000100,
  0b00101000,
  0b01111110,
  0b11110111,
  0b11111111,
  0b01111110,
  0b00111100,
  0b00000000
};

static int frame = 0;
static int UFrame;

//pipe 1 and 2
static int tx = 100,tw = 5;
static int tx2 =150 ,tw2 = 5;

static int uInput = LOW;
static int MilisTrecute = 0;
static int StareButon = HIGH;
static int UltimulButon = HIGH;
static int score = 0;


//bools
bool start = false;
bool GameOver = false;
static bool butonApasat = false;

void InitFlappyBird()
{
    px = 20;
    py = 30;
    tx = 100;
    tx2 = 150;

    score = 0;
    start = false;
    GameOver = false;
}

void StartJoc()
{
    display.clearDisplay();
    display.setCursor(0, 20);
    display.setTextSize(1);
    display.println("Apasa pentru");
    display.println("a incepe!");

    display.display();

    if (digitalRead(D2) == HIGH) {
       px = 20;
       py= 32; 
       score = 0;
      start = true;
    }
}
void DeseneazaPasare()
{
    if (frame == 0)
        display.drawBitmap(px - 4, py - 4, pasare1, 8, 8, WHITE);

    else if (frame == 1)
        display.drawBitmap(px - 4, py - 4, pasare2, 8, 8, WHITE);

    else
        display.drawBitmap(px - 4, py - 4, pasare3, 8, 8, WHITE);
}
void Animatie()
{
  if(millis() - UFrame>= 150)
  {
    UFrame = millis();
    if(frame == 2)
    frame = 0;
   else
   frame++;
  }
}
void Gameover()
{
      display.clearDisplay();
      display.setCursor(0,0);
    display.print("GAME OVER!");
    display.setCursor(0,30);
    display.println("Doresti sa continui?");
    display.display();

    if (digitalRead(D2) == HIGH) {
        GameOver = false;
        start=false;
        tx = 100;
        tx2 = 150;
        randomNum = random(10,40);
        randomNum2 = random(10,40);
        return;
    }
}
void DesenareHarta()
{
  display.drawRect(0,0,128,64,WHITE);
  display.fillRect(tx,0,tw,randomNum,WHITE);
  display.fillRect(tx,randomNum + 20,tw,128,WHITE);
display.fillRect(tx2,0,tw2,randomNum2,WHITE);
  display.fillRect(tx2,randomNum2 + 20,tw2,128,WHITE);  
  display.setCursor(5,5);
  display.println(score);
  display.display();
}
void CheckButton()
{
uInput = digitalRead(D2);

if(uInput != UltimulButon)
{MilisTrecute = millis();}

if(millis() - MilisTrecute > 30)
{
  if(uInput != StareButon)
{
  if(StareButon == LOW)
  py -=10;
  StareButon = uInput;
  butonApasat = true;
}
}
  UltimulButon = uInput;

return;

}
void MecanicaJoc()
{
tx--;
tx2 --;
py += 1;
}
void Coliziune()
{
  if(py - raza < 0 || py + raza > 64)
    GameOver = true;
  if((px + 2 >= tx && px-2 <= tx+tw))
  {
    if(py-raza >= randomNum + 20  || py + raza <= randomNum || py-raza <= 0 || py+raza >= 64)
    GameOver = true;
  }
  if((px + raza >= tx2 && px-raza <= tx2 + tw2))
  {
    if(py-raza >= randomNum2 + 20  || py + raza <= randomNum2)
    GameOver = true;
  }
  return;
}
static void IesireJoc()
{
     GameOver = false;
        start=false;
        tx = 100;
        tx2 = 150;
        randomNum = random(10,40);
        randomNum2 = random(10,40);
        jocActiv = false;
}
void FPJoc() {
  display.clearDisplay();
if(start == false)
StartJoc();
else
{
  if(GameOver == true)
  { 
Gameover();
  }
  else
  {
Animatie();
display.clearDisplay();
DeseneazaPasare();
DesenareHarta();
MecanicaJoc();
Coliziune();
CheckButton();
if(tx < 5)
{
  tx = 100;
  randomNum = random(10,40);
  score++;
}
if(tx2 < 5)
{
  tx2  = tx+50;
  randomNum2 = random(10,40);
  score++;
}
if(digitalRead(joysticSW) == LOW)
IesireJoc();

delay(20);
}}
}
