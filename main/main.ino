#define x       A0    // analog pin connected to X intput (bell)
#define y       A1    // analog pin connected to Y intput (bell)
#define tiltsw  9     // digital pin connected to tilt switch input (tip)
#define udder   8     // digital pin connected to button input (udder)
#define start   7     // digital pin connected to button input (start)
#define speaker 3     // digital PWM pin to output audio from SD card
#define SD_CLK  13    // SPI clock
#define SD_MISO 12    // SPI MISO
#define SD_MOSI 11    // SPI MOSI
#define SD_CS   4     // SPI chip select


enum ternary {
  tip,
  ring,
  pull
};

ternary action;
byte rann;
bool gameOver;
byte score;
unsigned int interval;
bool continu;
int tiltCount;    // counter used to measure duration of tilt

void setup() {                      // Begins when power switch is set to ON and provides voltage to Vcc pin
  pinMode(tiltsw, INPUT);
  pinMode(x, INPUT);
  pinMode(y, INPUT);
  pinMode(udder, INPUT);
  pinMode(start, INPUT);
  pinMode(speaker, OUTPUT);
  
  score = 0;                        // User's score in game
  interval = 3000;                  // Time limit to accomplish task
  randomSeed(micros());             // Sets random seed based on time until Start button pressed
  continu = false;                  // A continue flag to keep game process going
}

void loop() {
  if(digitalRead(start) && !continu){               // Check beginning of game
    startup();
  }
  if(continu){                            // Check player is still in the game
    rann = random(1, 4);                  // Randomly picking an action
    if(rann == 1){                        // Checking and assigning enum action
      action = tip;                       // Assigning enum action
    } else if(rann==2){                   // ''
      action = ring;                      // ''
    } else {                              // ''
      action = pull;                      // ''
    }
    checkSuccess(interval, action);       // Time to see if user performs action
    if((score % 4) == 0)                  // Check if it's time to speed up
      interval = interval-100;            // Decreasing time interval
  }
}

void checkSuccess(int timelimit, ternary actiontype){      // Check objective sequence
  tiltCount = 0;    // sets tilt count to 0
  
  for(int count=0; count<timelimit; count++){           // Iter                   // Setting based on input
    if(checkInput(actiontype)){                                        // Check if user performed action
      continu = true;                                   // Proceed with game
      score++;
      
      // Increment user's score
      return;
    }
    delay(1);                                           // Part of iterating through time limit
  }
  fail();                                               // Have not completed in time so run end-game
}

/*
bool checkInputNew(enum actiontype) {
  bool tilt = digitalRead(tiltsw);
  int xval = analogRead(x);                                   // Read X-axis of joystick input
  int yval = analogRead(y);
  bool pull = digitalRead(udder);

  if(digitalRead(tiltsw)){                                          // Check read
    count++;
    if (count == 50) {
      return true;
    }
  }
  
}
*/

bool checkInput(ternary actiontype){                       // Check input signal(s)
  if(actiontype == tip){                                 // Check if user needs to tip the cow
    if(digitalRead(tiltsw)){                                          // Check read
      tiltCount++;
      if (tiltCount == 50) {
        return true;
      }
    } else {
      return false;
    }
  } else if(actiontype == ring){                                // Check if user needs to ring the bell
    int xval = analogRead(x);                                   // Read X-axis of joystick input
    int yval = analogRead(y);                                   // Read Y-axis of joystick input
    if(xval > 600 || xval < 400 || yval > 600 || yval < 400){   // Check if joystick is moved far enough
      return true;
    } else {
      return false;
    }
  } else if(actiontype == pull){                        // Check if user needs to pull the udder
    if(!digitalRead(udder)){                             // Check read
      return true;
    } else {
      return false;
    }
  }
}

void startup(){                             // Startup sequence at beginning of game
  //TODO count 3, 2, 1 and blink LEDs or smth
  //TODO set 7-seg to "go"
  continu = true;                           // Proceeding with the game
}

void fail(){                                // Ending sequence to finish game
  continu = false;                          // Cannot proceed with the game
  //TODO blink final score
  //TODO set 7 seg to 00 or dash dash or smth
  score = 0;                                // Reset score for next game
}
