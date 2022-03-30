#define x       A0    // analog pin connected to X intput (bell)
#define y       A1    // analog pin connected to Y intput (bell)
#define tiltsw  8     // digital pin connected to tilt switch input (tip)
#define btn     _     // digital pin connected to button input (udder)
#define start   _     // digital pin connected to button input (start)
#define seg7_0  _     // __ pin connected to 7-segment controller output
#define led1    _     // digital pin connected to LED output (bell_LED)
#define led2    _     // digital pin connected to LED output (tip_LED)
#define led3    _     // digital pin connected to LED output (udder_LED)


enum action {
  tip,
  ring,
  pull
};
byte rann;
bool gameOver;
byte score;
unsigned int interval;
bool continu;

void setup() {                      // Begins when power switch is set to ON and provides voltage to Vcc pin
  pinMode(tiltsw, INPUT);
  pinMode(x, INPUT);
  pinMode(y, INPUT);
  pinMode(btn, INPUT);
  pinMode(start, INPUT);
  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
  pinMode(led3, OUTPUT);
  pinMode(seg7_0, OUTPUT);
  score = 0;                        // User's score in game
  interval = 3000;                  // Time limit to accomplish task
  randomSeed(micros());             // Sets random seed based on time until Start button pressed
  continu = false;                  // A continue flag to keep game process going
}

void loop() {
  startval = digitalRead(start);          // Check Start button
  if(startval && !continu){               // Check beginning of game
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
    if((score % 4) == 0))                 // Check if it's time to speed up
      interval = interval-100;            // Decreasing time interval
  }
}

void checkSuccess(int timelimit, enum actiontype){      // Check objective sequence
  bool success;
  for(int count=0; count<timelimit; count++){           // Iterating through time limit
    success = checkInput(actiontype);                   // Setting based on input
    if(success){                                        // Check if user performed action
      continu = true;                                   // Proceed with game
      score++;                                          // Increment user's score
      return;
    }
    delay(1);                                           // Part of iterating through time limit
  }
  fail();                                               // Have not completed in time so run end-game
}

bool checkInput(enum actiontype){                       // Check input signal(s)
  if(actionype == tip){                                 // Check if user needs to tip the cow
    bool tilt = digitalRead(tiltsw);                    // Reading tilt switch input
    if(!tilt){                                          // Check read
      return true;
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
    bool btnpress = digitalRead(btn);                   // Reading "pull" button input
    if(btnpress){                                       // Check read
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
