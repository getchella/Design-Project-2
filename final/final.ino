//#include <Adafruit_GFX.h>
//#include <Adafruit_SSD1306.h>
#include <Wire.h>
#include <SPI.h>
//#include <TMRpcm.h>
#include "pitches.h"
#include <U8x8lib.h>

U8X8_SSD1306_128X64_NONAME_SW_I2C display(/* clock=*/ SCL, /* data=*/ SDA, /* reset=*/ U8X8_PIN_NONE);

#define x A0       // analog pin connected to X intput (bell)
#define y A1       // analog pin connected to Y intput (bell)
#define tiltsw 9   // digital pin connected to tilt switch input (tip)
#define udder 8    // digital pin connected to button input (udder)
#define start 7    // digital pin connected to button input (start)
#define speaker 3  // digital PWM pin to output audio from SD card
#define SD_CLK 13  // SPI clock
#define SD_MISO 12 // SPI MISO
#define SD_MOSI 11 // SPI MOSI
#define SD_CS 10   // SPI chip select
#define oled_addr 0x3C
#define oled_width 128
#define oled_height 64

typedef enum actions
{
  tip,
  ring,
  pull
};
typedef enum responses
{
  correct,
  incorrect,
  nothing
};

actions action;
byte rann;
bool gameOver;
byte score;
unsigned int interval;
bool continu;
int tiltCount; // counter used to measure duration of tilt
//TMRpcm tmrpcm;

//Adafruit_SSD1306 display(oled_width, oled_height, &Wire, -1);

void setup()
{ // Begins when power switch is set to ON and provides voltage to Vcc pin
  //display.begin(SSD1306_SWITCHCAPVCC, oled_addr);
  display.begin();
  display.setFont(u8x8_font_px437wyse700a_2x2_r);

  pinMode(tiltsw, INPUT);
  pinMode(x, INPUT);
  pinMode(y, INPUT);
  pinMode(udder, INPUT);
  pinMode(start, INPUT);
  pinMode(speaker, OUTPUT);

  score = 0;       // User's score in game
  interval = 8000; // Time limit to accomplish task
  continu = false; // A continue flag to keep game process going

  display.clearDisplay();
  display.setCursor(36, 0);
  display.print("PRESS");
  display.setCursor(36, 20);
  display.print("START");
  display.display();
}

void loop()
{
  if (!digitalRead(start) && !continu) // Check beginning of game
  {
    startup();
  }
  if (continu) // Check player is still in the game
  {
    rann = random(1, 4); // Randomly picking an action
    if (rann == 1)       // Checking and assigning enum action
    {
      action = tip; // Assigning enum action
      //displayAction(tip);
      setTone(NOTE_B1, 1000);
      delay(500);
    }
    else if (rann == 2) // Checking and assigning enum action
    {
      action = ring; // Assigning enum action
      //displayAction(ring);
      setTone(NOTE_F4, 600);
      delay(300);
      setTone(NOTE_F4, 600);
    }
    else // Checking and assigning enum action
    {
      action = pull; // Assigning enum action
      //displayAction(pull);
      setTone(NOTE_C7, 250);
      delay(250);
      setTone(NOTE_C7, 250);
      delay(250);
      setTone(NOTE_C7, 250);
      delay(250);
    }

    checkSuccess(interval, action); // Time to see if user performs action

    if ((score % 4) == 0)        // Check if it's time to speed up
      interval = interval - 100; // Decreasing time interval
    if (score == 5)
      win();
    displayScore(); // Update LCD with current score
  }
}

void checkSuccess(int timelimit, actions actiontype){      // Check objective sequence
  tiltCount = 0;    // sets tilt count to 0

  for(int count=0; count<timelimit; count++){// Iter                   // Setting based on input
    responses response = checkInput(actiontype);
    if(response == correct){                                        // Check if user performed action
      continu = true;                                   // Proceed with game
      score++;  // Increment user's score
      return;
    }
    else if (response == incorrect) {
      break;
    }
    delay(1);
  }
  fail();   // not in time
}

responses checkInput(actions actiontype){                            // Check input signal(s)
  int xval = analogRead(x);                                     // Reading joysticks for whole checkInput
  int yval = analogRead(y); 

  if(actiontype == tip){                                        // Tilt                            
    if(digitalRead(tiltsw)){                                    // Check if they tilt for 50
      tiltCount++;                                              
      if (tiltCount == 500) {                                  
        return correct;
      }
    } else if(!digitalRead(udder)) {                             // If any other action return false
      return incorrect;
    } else if(xval > 600 || xval < 400 || yval > 600 || yval < 400){
      return incorrect;
    } else {
      return nothing;
    }

  } else if(actiontype == ring){                                // Ring
    if(xval > 600 || xval < 400 || yval > 600 || yval < 400){   // Check if joystick is moved far enough
      return correct;
    } else if(digitalRead(tiltsw)) {                              
      tiltCount++;                                              // If any other action return false
      if (tiltCount == 500) {
        return incorrect;
      }
    } else if(!digitalRead(udder)) {
      return incorrect;
    } else {
      return nothing;                                             
    }

  } else if(actiontype == pull){                                // Udder
    if(!digitalRead(udder)){                                    // Check if udder is yanked
      return correct;
    } else if(digitalRead(tiltsw)) {                              // If any other action return false
      tiltCount++;
      if (tiltCount == 500) {
        return incorrect;
      }
    } else if(xval > 600 || xval < 400 || yval > 600 || yval < 400) {
      return incorrect;
    } else {
      return nothing;
    }
  }

}

/*
void checkSuccess(int timelimit, actions actiontype) // Check objective sequence
{
  responses inputresponse;
  for (int count = 0; count < timelimit; count++)
  {
    inputresponse = checkInput(actiontype); // Setting based on input
    if (inputresponse == correct)           // Check if user performed action
    {
      continu = true; // Proceed with game
      score++;        // Increment user's score
      if (score == 100)
      {
        win();
      }
      else
      {
        return;
      }
    }
    else if (inputresponse == incorrect) // Check if user performed wrong action
    {
      break;
    }
    else if (inputresponse == nothing) // Check if user hasn't done anything
    {
      continue;
    }
    delay(1); // Part of iterating through time limit
  }
  fail(); // Have not completed in time so run end-game
}

responses checkInput(actions actiontype) // Check input signal(s)
{
  if (actiontype == tip) // Tilt action
  {
    if (checkTip()) // Check our tilt first
    {
      return correct;
    }
    else if (checkUdder()) // Check other input actions
    {
      return incorrect;
    }
    else if (checkRing()) // Check other input actions
    {
      return incorrect;
    }
    else // Nothing was done
    {
      return nothing;
    }
  }
  else if (actiontype == ring) // Ring action
  {
    if (checkRing()) // Check our ring first
    {
      return correct;
    }
    else if (checkTip()) // Check other input actions
    {
      return incorrect;
    }
    else if (checkUdder()) // Check other input actions
    {
      return incorrect;
    }
    else // Nothing was done
    {
      return nothing;
    }
  }
  else if (actiontype == pull) // Pull action
  {
    if (checkUdder()) // Check our pull first
    {
      return correct;
    }
    else if (checkTip()) // Check other input actions
    {
      return incorrect;
    }
    else if (checkRing()) // Check other input actions
    {
      return incorrect;
    }
    else // Nothing was done
    {
      return nothing;
    }
  }
}


bool checkUdder()
{
  return !digitalRead(udder); // Check if udder is yanked
}

bool checkTip()
{
  if (!digitalRead(tiltsw)) {
    tiltCount++;
    if (tiltCount == 50) {
      return true;
    }
  }
  return false;
  
  
  for (int i = 0; i < 2000; i++) // 2 seconds to correct tilt
  {
    if (!digitalRead(tiltsw))
    {
      return false; // tilt switch is not activated
    }
    else
    {
      display.clearDisplay();
      display.setCursor(10, 20);
      display.print("You're tilted");
      display.display();
      delay(1000);
    }
  }
  return true; // tilt was activated for long enough to be considered an input
  
}

bool checkRing()
{
  int xval = analogRead(x);                                 // Reading joystick x value
  int yval = analogRead(y);                                 // Reading joystick y value
  if (xval > 600 || xval < 400 || yval > 600 || yval < 400) // Check if moved far enough
  {
    return true; // Joystick was moved far enough
  }
  else
  {
    return false; // Joystick was NOT moved far enough
  }
}
*/

void startup() // Startup sequence at beginning of game
{
  randomSeed(micros()); // Sets random seed based on time until Start button pressed
  for (int i = 3; i >= 0; i--) {
        display.clearDisplay();
        if (i == 0)
        {
          display.setCursor(30, 20);
          display.print("GO!");
        }
        else
        {
          display.setCursor(30, 40);
          display.print(i);
        }
        display.display();
        delay(750);
      }
    continu = true; // Proceeding with the game
  displayScore();
}


void fail() // Ending sequence to finish game
{
  continu = false;            // Cannot proceed with the game
  for (int i = 0; i < 4; i++) // Blink final score
  {
    display.clearDisplay();
    display.setCursor(16, 0);
    display.print("GAME");
    display.setCursor(16, 16);
    display.print("OVER");
    if (score < 10)
    {
      display.setCursor(54, 20);
    }
    else
    {
      display.setCursor(49, 20);
    }
    display.print(score);
    display.display();
    delay(500);
    display.clearDisplay();
    delay(500);
  }

  score = 0; // Reset score for next game

  display.clearDisplay();
}

void win() // Ending sequence to finish game
{
  continu = false;            // Cannot proceed with the game
  for (int i = 0; i < 5; i++) // Blink final score
  {
    display.clearDisplay();
    display.setCursor(16, 0);
    display.print("YOU WIN");
    if (score < 10)
    {
      display.setCursor(54, 20);
    }
    else
    {
      display.setCursor(49, 20);
    }
    display.print(score);
    display.display();
    delay(500);
    display.clearDisplay();
    delay(500);
  }

  score = 0; // Reset score for next game

  display.clearDisplay();
}

void displayScore()
{
  display.clearDisplay();
  display.setCursor(32, 0);
  display.print("SCORE:");
  if (score < 10)
  {
    display.setCursor(54, 20);
  }
  else
  {
    display.setCursor(49, 20);
  }
  display.print(score);
  display.display();
  delay(1000);
}

void displayAction(actions actiontype)
{
  display.clearDisplay();
  if (actiontype == tip)
  {
    display.setCursor(25, 20);
    display.print("Tip It!!");
  }
  else if (actiontype == ring)
  {
    display.setCursor(20, 20);
    display.print("Ring It!!");
  }
  else if (actiontype == pull)
  {
    display.setCursor(20, 20);
    display.print("Pull It!!");
  }
  display.display();
}

void setTone(int note, int duration)
{
  tone(speaker, note, duration);
  delay(duration);
  noTone(speaker);
}
