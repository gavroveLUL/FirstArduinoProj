
#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Wire.h>
#include <Fp.h>
#include <Pong.h>
#define LUNGIME_ECRAN 128
#define LATIME_ECRAN 64

Adafruit_SSD1306 display(LUNGIME_ECRAN, LATIME_ECRAN, &Wire, -1);

// Button
const int D2 = 2;
const int joystickX = 34;
const int joystickY = 35;
const int joystickSW = 25;
static int i = 0;
long TimpTrecut = 0;
long UltimulTimpTrecut = 0;
bool jocActiv = false;
uint8_t joystick[4];


void setup() {
    Serial.begin(9600);
      pinMode(joystickSW, INPUT_PULLUP); 
    pinMode(D2, INPUT_PULLDOWN);

    if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
        if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3D)) {
            Serial.println("OLED not found!");
            for (;;);
        }
    }

    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    InitFlappyBird();
    setupPong();
    millis();
}
void MeniuActiv()
{display.setCursor(40,0);
  display.setTextSize(1);
 display.print("Meniu");
 display.setCursor(10,20);
 display.setTextSize(2);
 switch (i)
 {
 case 0:
display.println("FlappyBird");
  break;
 case 1:
 display.println("Pong");
 break;
 default:
  break;
 }
}
void VariabilaI()
{
  TimpTrecut = millis();
  if(millis() - TimpTrecut > UltimulTimpTrecut)
  {
    if(analogRead(joystickX)  > 3000 && i < 1)
{    i++;
        UltimulTimpTrecut = TimpTrecut;

}    else if(analogRead(joystickX ) < 1500 && i > 0)
{    i--;
    UltimulTimpTrecut = TimpTrecut;
}     
  }
}
void VerificaApasare()
{
  if(digitalRead(D2) == HIGH)
   jocActiv = true;

}
void loop() {
  if(jocActiv == true)
  {
if(i == 0)
FPJoc();
else
PongJoc();
  }
  else
    {VariabilaI();
    display.setCursor(1,1);
    display.clearDisplay();
    MeniuActiv();
    VerificaApasare();
    display.display();
delay(100);}
}
