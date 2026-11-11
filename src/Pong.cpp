
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
const int joystickSW = 27;
//Joc
int scorejuc = 0 , scoreRobot = 0;
static bool jocActiv = true;
int ultimulGol = 0;
bool butonEliberat = false;
//jucator

int jx =0,jy=32,jr = 8,vjy;
//AI
//ay1 este centrul pentru intervalul predictiei al mingii
int ax=127,ay = 32 ,ar = 8,ay1;

//Minge;
//x,y - coord
//1 - Este folosit pentru predictia mingii
//2 - pozitia predicted
//
float bx = 64,by = 32;
int br = 3;
int RandomNum = random(2,8);
int v = 2;
float vx = -1,vy = -1;
float vx1 , vy1,by1,bx1;
float bx2,by2;

//Unghiurile -Radiani -Grade
int unghiR = 0;
int unghiG = 0;

void setupPong()
{
  pinMode(D2,INPUT  );
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

//Ideea este ca pentru fiecare coliziune se va calcula un nou set de coordonate(calculezi proiectia unei mingi imaginare),in primul rand
//vedem dac mingea se indreapta spre AI, ct1 reprezinta valoarea pentru proiecte,calculam y-ul pe care mingea il va avea cand va atinge
//peretele din dreapta,transformam bx1+br in int deoarece valoarea este declarata initial intr-un float si poate sa sara uneori de 127
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
    ay1 = constrain((int)(by2 - ar / 2.0f),
                    0, 64 - ar);
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

    jocActiv = true;

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
    // Coliziune cu platforma jucatorului
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
    // Coliziune cu tavanul
    if (by - br <= 0)
    {
        by = br;
        vy = -vy;
        PredictMinge();
        GandireAI();
    }

    // Coliziune cu podeaua
    if (by + br >= 63)
    {
        by = 63 - br;
        vy = -vy;
        PredictMinge();
        GandireAI();
    }

    // Coliziuni cu Ai-ul
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
}
//Conditii de castig
if(bx + br < 0)
{
    scoreRobot++;
    ultimulGol = 2;
    jocActiv = false;
    butonEliberat = false;
    return;
}

if(bx - br > 127)
{
    scorejuc++;
    ultimulGol = 1;
    jocActiv = false;
    butonEliberat = false;
    return;
}
}

void MiscareAI()
{
  if(ay < ay1)
  {
    if(ay + ar < 64)
    ay++;
  }
  else if(ay > ay1)
  {
    if(ay - br > 0)
    ay--;
  }
}

void PongJoc()
{
  if(jocActiv == false)
  {
    if(ultimulGol == 1)
      ScoreJ();
    else if(ultimulGol == 2)
      ScoreAI();

    return;
  }
  else
  {
    display.clearDisplay();
    MiscarePlatforme();
    MiscareMinge();
    Coliziuni();

    if(jocActiv == false)
      return;

    MiscareAI();
    Desen();
  }
}
