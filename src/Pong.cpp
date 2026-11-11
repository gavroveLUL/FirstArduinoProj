
#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Wire.h>
#include <Pong.h>
#include <math.h>
#define LUNGIME_ECRAN 128
#define LATIME_ECRAN 64

extern Adafruit_SSD1306 display;

const static int D2 = 2;
const int joystickX = 34;
const int joystickY = 35;
const int joystickSW = 25;
//Game
int scorejuc = 0 , scoreRobot = 0;
static bool JocActive = true;
int ultimulGol = 0;
bool butonEliberat = false;
static int TimpTrecut = 0;
static int TimpTrecut2 = -100;
static bool ButonApasat = false;
extern bool jocActiv;
//Player
int jx =0,jy=32,jr = 8,vjy;
//AI
//Ay1 is the center coord for the ball prediction
int ax=127,ay = 32 ,ar = 8,ay1;
int Dificultate = 1;
bool DificultateSetata = false;
int MarjaEroare = 0;
int Random2 = random(0,MarjaEroare);
//Ball;
//x,y - coord
//1 - Used for predicting the position
//2 - Predicted position
//
float bx = 64,by = 32;
int br = 3;
int RandomNum = random(2,8);
int v = 2;
float vx = -1,vy = -1;
float vx1 , vy1,by1,bx1;
float bx2,by2;

//Angles in radians and degrees
int unghiR = 0;
int unghiG = 0;

void setupPong()
{
  pinMode(D2,INPUT);
  pinMode(joystickSW, INPUT_PULLUP);
}

void Desen()
{
    display.clearDisplay();
    display.fillRect(jx,jy,1,jr,WHITE);
    display.fillRect(ax,ay,1,ar,WHITE);
    display.fillCircle(bx,by,br,WHITE);

    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(53,0);
    display.print(scorejuc);
    display.print("-");
    display.print(scoreRobot);

    display.display();
}

void MiscarePlatforme()
{
  if(analogRead(joystickY) > 2000)
  {
    if(jy - jr>=0)
    jy--;
    else
    jy = 0 + jr;
  }
  else if(analogRead(joystickY) < 1500)
  {
    if(jy + jr <= 63 )
    jy++;
    else
    jy = 63 - jr;
  }
}

void MiscareMinge()
{
bx +=vx;
by += vy;
}

//"Simulating" the ball's trajectory to predict it's coords
void PredictMinge()
{
  if(vx > 0)
  {
    int i = 500;
    vy1 = vy;
    vx1 = vx;
    bx1 = bx;
    by1 = by;
    while(bx1 + br < ax && i!=0)
    {
      bx1 += vx1;
      by1 += vy1;
      if(by1-br <= 0)
      {
        by1 = br;
        vy1 =-vy1;
      }
      if(by1 + br >= 63)
      {
        by1 = 63 - br;
        vy1 = -vy1;
      }
      i--;
    }
    bx2 = bx1;
    by2 = by1;
  }
}

void GandireAI()
{
    ay1 = constrain((int)(by2 - ar  / 2.0f) + MarjaEroare,
                    0, 64 - ar);
}
void CheckButton2()
{
    static int ultimaCitire = HIGH;
    static int stareStabila = HIGH;

    ButonApasat = false;

    int citire = digitalRead(D2);

    if (citire != ultimaCitire) {
        TimpTrecut2 = millis();
    }

    if (millis() - TimpTrecut2 >= 30) {
        if (citire != stareStabila) {
            stareStabila = citire;

            if (stareStabila == HIGH) {
                ButonApasat = true;
            }
        }
    }

    ultimaCitire = citire;
}

void ResetPong()
{
    bx = 64;
    by = 32;

    jx = 0;
    jy = 32;
    jr = 8;

    ax = 127;
    ay = 32;
    ar = 8;
    ay1 = 32;

    vx = random(0,2) == 0 ? -1 : 1;
    vy = random(0,2) == 0 ? -1 : 1;

    JocActive = true;

    if(vx > 0)
    {
        PredictMinge();
        GandireAI();
    }
}

void VerificareRestart()
{

    if(digitalRead(D2) == HIGH)
    {
        ResetPong();
    }
}

void ScoreJ()
{
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE);

    display.setCursor(20,10);
    display.println("Ai dat gol!");

    display.setCursor(50,30);
    display.print(scorejuc);
    display.print("-");
    display.println(scoreRobot);

    display.setCursor(5,50);
    display.println("Apasa pt restart");

    display.display();

    VerificareRestart();
}

void ScoreAI()
{
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE);

    display.setCursor(15,10);
    display.println("Ti-ai luat gol!");

    display.setCursor(50,30);
    display.print(scorejuc);
    display.print("-");
    display.println(scoreRobot);

    display.setCursor(5,50);
    display.println("Apasa pt restart");

    display.display();

    VerificareRestart();
}

void Coliziuni()
{
//Coll with player's platform
   if (vx < 0 &&
    bx - br <= jx + 1 &&
    bx + br >= jx &&
    by + br >= jy &&
    by - br <= jy + jr)
{
    float centruPlatforma = jy + jr / 2.0f;

    float diferenta = (by - centruPlatforma) / (jr / 2.0f);

    diferenta = constrain(diferenta, -1.0f, 1.0f);

    float unghiR = diferenta * PI / 3.0f;

    vx = v * cos(unghiR);
    vy = v * sin(unghiR);

    bx = jx + 1 + br;
    PredictMinge();
    GandireAI();

}
    // Coll with the top floor
    if (by - br <= 0)
    {
        by = br;
        vy = -vy;
        PredictMinge();
        GandireAI();
    }

    // Coll with the bottom floor
    if (by + br >= 63)
    {
        by = 63 - br;
        vy = -vy;
        PredictMinge();
        GandireAI();
    }

    // Coll with AI
   if (vx > 0 &&
    bx + br >= ax &&
    bx - br <= ax + 1 &&
    by + br >= ay &&
    by - br <= ay + ar)
{
    float centruPlatforma = ay + ar / 2.0f;

    float diferenta = (by - centruPlatforma) / (ar / 2.0f);

    diferenta = constrain(diferenta, -1.0f, 1.0f);

    float unghiR = diferenta * PI / 3.0f;

    vx = -(v * cos(unghiR));
    vy = v * sin(unghiR);

    bx = ax - br;

    Random2 = random(0,MarjaEroare); 
}
//Winning conditions
if(bx + br < 0)
{
    scoreRobot++;
    ultimulGol = 2;
    JocActive = false;
    butonEliberat = false;
    return;
}

if(bx - br > 127)
{
    scorejuc++;
    ultimulGol = 1;
    JocActive = false;
    butonEliberat = false;
    return;
}
}

void MiscareAI()
{
  if(ay < ay1)
  {
    if(ay + ar + Random2< 64)
    ay++;
  }
  else if(ay > ay1)
  {
    if(ay - br - Random2> 0)
    ay--;
  }
}
void AlegereDificultate()
{
  
       switch(Dificultate)
       {
        case 1:
        display.setCursor(20,10);
        display.setTextSize(1);
        display.println("Dificultate: Usor");
        break;
        case 2:
        display.setCursor(20,10);
        display.setTextSize(1);
        display.println("Dificultate: Mediu");
        break;
        case 3:
        display.setCursor(20,10);
        display.setTextSize(1);
        display.println("Dificultate: Greu");
        break;
       }
  display.display();
}
void Meniu( )
{
  if(millis() - TimpTrecut > 300)
  {
    int x = analogRead(joystickX);

    if (x > 3000 && Dificultate < 3) {
        Dificultate++;
        TimpTrecut = millis();
    }
    else if (x < 1500 && Dificultate > 1) {
        Dificultate--;
        TimpTrecut = millis();
    }
  }
}
static void IesireJoc()
{
   bx = 64;
    by = 32;

    jx = 0;
    jy = 32;
    jr = 8;

    ax = 127;
    ay = 32;
    ar = 8;
    ay1 = 32;

    vx = random(0,2) == 0 ? -1 : 1;
    vy = random(0,2) == 0 ? -1 : 1;

    jocActiv = false;
}
void PongJoc()
{
    if (!DificultateSetata) {
        CheckButton2();
        Meniu();

        display.clearDisplay();
        AlegereDificultate();

        if (ButonApasat) {
            switch (Dificultate) {
                case 1:
                    MarjaEroare = 10;
                    break;

                case 2:
                    MarjaEroare = 5;
                    break;

                case 3:
                    MarjaEroare = 1;
                    break;
            }

            Random2 = random(0, MarjaEroare);
            DificultateSetata = true;
        }

        return;
    }

    if (!JocActive) {
        if (ultimulGol == 1) {
            ScoreJ();
        }
        else if (ultimulGol == 2) {
            ScoreAI();
        }

        return;
    }

    display.clearDisplay();

    MiscarePlatforme();
    MiscareMinge();
    Coliziuni();

    if (!JocActive) {
        return;
    }

    MiscareAI();
    Desen();
    if(digitalRead(joystickSW ) == LOW)
    IesireJoc();
}
