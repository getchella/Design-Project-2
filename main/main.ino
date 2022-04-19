//#include <Adafruit_GFX.h>
//#include <Adafruit_SSD1306.h>
#include <Wire.h>
#include <SPI.h>
#include <TMRpcm.h>
#include <U8x8lib.h>

U8X8_SSD1306_128X64_NONAME_SW_I2C u8x8(/* clock=*/ SCL, /* data=*/ SDA, /* reset=*/ U8X8_PIN_NONE);

#define x A0       // analog pin connected to X intput (bell)
#define y A1       // analog pin connected to Y intput (bell)
#define tiltsw 9   // digital pin connected to tilt switch input (tip)
#define udder 8    // digital pin connected to button input (udder)
#define start 7    // digital pin connected to button input (start)
#define speaker 3  // digital PWM pin to output audio from SD card
#define SD_CLK 13  // SPI clock
#define SD_MISO 12 // SPI MISO
#define SD_MOSI 11 // SPI MOSI
#define SD_CS 4    // SPI chip select
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
TMRpcm tmrpcm;

//Adafruit_SSD1306 display(oled_width, oled_height, &Wire, -1);

void setup()
{ // Begins when power switch is set to ON and provides voltage to Vcc pin
//  display.begin(SSD1306_SWITCHCAPVCC, oled_addr);
  u8x8.begin();
  u8x8.setFont(u8x8_font_px437wyse700a_2x2_r);

  pinMode(tiltsw, INPUT);
  pinMode(x, INPUT);
  pinMode(y, INPUT);
  pinMode(udder, INPUT);
  pinMode(start, INPUT);
  pinMode(speaker, OUTPUT);
  tmrpcm.speakerPin = 3;
  SD.begin(SD_CS);

  score = 0;            // User's score in game
  interval = 8000;      // Time limit to accomplish task
  continu = false;      // A continue flag to keep game process going

  u8x8.clearDisplay();
  u8x8.setCursor(36, 0);
  u8x8.print("PRESS");
  u8x8.setCursor(36, 20);
  u8x8.print("START");
  u8x8.display();

  tmrpcm.play("got_milk.wav");
  
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
//      displayAction(tip);
      delay(1000);
      // TODO: output to speaker
    }
    else if (rann == 2) // Checking and assigning enum action
    {
      action = ring; // Assigning enum action
//      displayAction(ring);
      delay(1000);
      // TODO: output to speaker
    }
    else // Checking and assigning enum action
    {
      action = pull; // Assigning enum action
//      displayAction(pull);
      delay(1000);
      // TODO: output to speaker
    }

    checkSuccess(interval, action); // Time to see if user performs action

    if ((score % 4) == 0)        // Check if it's time to speed up
      interval = interval - 100; // Decreasing time interval
    if (score == 10)
      fail();
    displayScore();              // Update LCD with current score
  }
}

void checkSuccess(int timelimit, actions actiontype) // Check objective sequence
{
  responses inputresponse;
  for (int count = 0; count < timelimit; count++)
  {
    inputresponse = checkInput(actiontype); // Setting based on input
    if (inputresponse == correct)                     // Check if user performed action
    {
      continu = true; // Proceed with game
      score++;        // Increment user's score
      return;
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
  
/*
if (inputresponse == incorrect) {
    display.print("incorrect");
  } else if (inputresponse == nothing) {
    display.print("nothing");
  } else {
    display.print("correct");
  }
  */
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
  for (int i = 0; i < 2000; i++) // 2 seconds to correct tilt
  {
    if (!digitalRead(tiltsw))
    {
      return false; // tilt switch is not activated
    }
    else
    {
      // TODO: output message to LCD that tilt switch is activated
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

void startup() // Startup sequence at beginning of game
{
  randomSeed(micros()); // Sets random seed based on time until Start button pressed
  // TODO count 3, 2, 1
  // TODO set 7-seg to "go"
  continu = true; // Proceeding with the game
  displayScore();
}

void fail() // Ending sequence to finish game
{
  continu = false;            // Cannot proceed with the game
  for (int i = 0; i < 5; i++) // Blink final score
  {
    
    u8x8.clearDisplay();
    u8x8.setCursor(16, 0);
    u8x8.print("GAME OVER");
    if (score < 10)
    {
      u8x8.setCursor(54, 20);
    }
    else
    {
      u8x8.setCursor(49, 20);
    }
    u8x8.print(score);
    u8x8.display();
    delay(500);
    u8x8.clearDisplay();
    delay(500);
  }
  
  score = 0; // Reset score for next game

  u8x8.clearDisplay();
}

void displayScore()
{
  
  u8x8.clearDisplay();
  u8x8.setCursor(32, 0);
  u8x8.print("SCORE:");
  if (score < 10)
  {
    u8x8.setCursor(54, 20);
  }
  else
  {
    u8x8.setCursor(49, 20);
  }
  u8x8.print(score);
  u8x8.display();
}

void displayAction(actions actiontype)
{
  u8x8.clearDisplay();
  if (actiontype == tip)
  {
    u8x8.setCursor(20, 40);
    u8x8.print("Tip It!!");
  }
  else if (actiontype == ring)
  {
    u8x8.setCursor(16, 40);
    u8x8.print("Ring It!!");
  }
  else if (actiontype == pull)
  {
    u8x8.setCursor(16, 40);
    u8x8.print("Pull It!!");
  }
  u8x8.display();
}
