/**************************************************************************
  StringLand is a 1D Neopixel Game with an extra "sideways" dimension. These extra dimensions are not all visible at once.
  
  Written by John Melki-Wegner aka "Expensive Notes" with an Arduino UNO clone, 4 buttons and a 120 NeoPixel string
  
  Aim: To score as much as possible before your island is invaded by the nasty invaders (red pixels). 
  Island: Your Island is the yellow pixels in the middle of string land. It is surrounded by blue sea.
  You: You are the Green Pixel which you can move up and down the visible string via buttons 
  Score: Each time you touch a Nasty (red pixel) you score a point and the red pixels are banished to the end of the string world. The white pixel is your score.
  Dimensions: To change dimension to a different 1D string press the other two buttons. Your current dimension is shown by the black pixel on your island.

  From Jaycar:
  Neopixels and Uno Board
  LED Strip UNO
         5V  5V
        DIN  (any PWM pin) such as pin 6 :: Using Pin 9 as it is PWM
        GND  GND

 **************************************************************************/

// ------------------ NeoPixels -------------------------------------------
#include <Adafruit_NeoPixel.h>

//Use pin 6 for Neopixel control
#define LED_PIN 6
#define LED_COUNT 120

Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

//Rainbow Colours - not necessary for the game but used at game end.
uint32_t rainbowCol[188] =  {5505183, 5505187, 5505192 , 5177521 , 5111990 , 5046458 , 4980927 , 4587720 , 4456652 , 4325585 ,
                             3932374 , 3801306 , 3604703 , 3014888 , 2818284 , 2621681 , 2359541 , 2162938 , 1638655 , 1048831 , 786687  ,
                             262399  , 255 , 1279  , 4351  , 5631  , 6655  , 7679  , 9983  , 11007 , 12031 , 13311 , 16383 , 17407 , 18687 , 21759 , 23039 ,
                             24063 , 25087 , 27391 , 28415 , 29439 , 30719 , 32767 , 34815 , 36095 , 39167 , 40447 , 41471 , 42495 , 44799 ,
                             45823 , 46847 , 48127 , 50175 , 51199 , 55551 , 56575 , 57855 , 58879 , 59903 , 62207 , 63231 , 64255 , 65535 , 65526 ,
                             65522 , 65518 , 65505 , 65496 , 65492 , 65483 , 65479 , 65475 , 65471 , 65467 , 65458 , 65454 , 65450 , 65437 ,
                             65432 , 65424 , 65415 , 65411 , 65407 , 65403 , 65394 , 65390 , 65386 , 65382 , 65369 , 65364 , 65360 , 65356 ,
                             65343 , 65339 , 65335 , 65326 , 65322 , 65318 , 65305 , 65301 , 65296 , 65292 , 65288 , 65280 , 589568  , 851712  ,
                             1441536 , 1703680 , 1965824 , 2817792 , 3079936 , 3407616 , 3669760 , 4194048 , 4456192 , 4783872 , 5046016 ,
                             5897984 , 6160128 , 6422272 , 7274240 , 7536384 , 7864064 , 8126208 , 8650496 , 8912640 , 9240320 , 9502464 ,
                             10092288  , 10616576  , 10878720  , 11730688  , 11992832  , 12320512  , 12582656  , 13106944  , 13369088  , 13696768  ,
                             13958912  , 14548736  , 14810880  , 15924992  , 16187136  , 16449280  , 16776960  , 16775680  , 16773632  ,
                             16772608  , 16771328  , 16770304  , 16768256  , 16766976  , 16765952  , 16762624  , 16760576  , 16759552  , 16757248  ,
                             16756224  , 16755200  , 16753920  , 16752896  , 16750848  , 16749568  , 16748544  , 16745216  , 16744192  ,
                             16742144  , 16739840  , 16738816  , 16737792  , 16736512  , 16734464  , 16733440  , 16732160  , 16731136  ,
                             16727808  , 16726784  , 16725760  , 16724736  , 16721408  , 16720384  , 16719104  , 16717056  , 16716032  ,
                             16714752  , 16711680
                            };


// --------------------------- Game Butttons --------------------------

//Yellow and Blue Buttons Mapped to Arduino Uno
#define numSwitches 4
//Pins used by GamePad Shield buttons. The first 4 are the Yellow and Blue buttons
//You can use any other digital pins
int switchPins[numSwitches] = {2, 3, 4, 5};
//Button Values - Will be High or Low. To store Pin values
int pinVal[numSwitches] = {1, 1, 1, 1};

// ------------------------- Game Variables -------------------
//Size of String Land
#define xStart 50
#define xEnd 100
int playerX = 75; //Players position
int playerXmove = 0; //How the player moves
//There are 10 nasty invaders. They are placed at the ends of each string
int nastyXStart [5]  = {xStart, xStart, xStart, xStart, xStart}; 
int nastyXEnd [5]  = {xEnd, xEnd, xEnd, xEnd, xEnd};
//Which string dimension are you currently on
int dimension = 2;
//Timer for game over before restart
int gameOver = 0;
//middle of String Land
int middle;
//Score!
int score = 0;

//*************************************************************************

void setup() {
  // ---------- NeoPixels -----------
  strip.begin(); //always needed
  strip.show(); // 0 data => off.
  strip.setBrightness(50); // ~20% (max = 255)

  // --------------- GamePad -----------
  //Set digital Pins
  for (int i = 0; i < numSwitches; i++) {
    pinMode(switchPins[i], INPUT_PULLUP);
  }
  pinMode(6, OUTPUT);

  //----------- General -------------
  Serial.begin(9600);
  middle = int((xEnd + xStart) / 2);
}


void loop() {
  //game play
  readGameShield();
  movePlayer();
  moveNastys();
  playerHitsNasty();
  
  //Draw stuff
  clearAllPixels();
  showIslandAndDimension();
  drawPlayer();
  showNastys();
  isGameLost();
  showScore();
  strip.show();
  
  //Delay decreases as the score increases making the game faster and harder
  int dTime = 100 - score;
  if (dTime < 1) dTime =1;
  delay(dTime);
}

//*****************************************************************************


// ---------------- Movement ---------------
void movePlayer() {

  //adjust movement of player
  playerXmove = 0;
  //These are the two blue buttons
  if (pinVal[3] == LOW) playerXmove = 1;
  if (pinVal[1] == LOW) playerXmove = -1;
  //move player
  playerX = playerX + playerXmove;
  //Don't go past the end of String Land minus one
  if (playerX > xEnd - 1) playerX = xEnd - 1;
  if (playerX < xStart + 1) playerX = xStart + 1;

  //Switch Dimensions
  if (pinVal[0] == LOW) dimension++;
  if (dimension > 4) dimension = 4;
  if (pinVal[2] == LOW) dimension--;
  if (dimension < 0) dimension = 0;
}

void moveNastys() {
  //move Nasties in all 5 dimensions
  for (int i = 0; i < 5; i++) {
    if (random(10) == 0) nastyXStart[i] = nastyXStart[i] + random(2);
    if (random(10) == 0) nastyXEnd[i] = nastyXEnd[i] - random(2);
  }
}

void playerHitsNasty() {
  //If a Nasty is hit banish it to the ends of stringland
  if (playerX >= nastyXEnd[dimension]) {
    nastyXEnd[dimension] = xEnd;
    score++;
  }
  if (playerX <= nastyXStart[dimension]) {
    nastyXStart[dimension] = xStart;
    score++;
  }
}

void isGameLost() {
  //Check every Nasty to see if it invaded your island
  for (int i = 0; i < 5 ; i++) {
    if (nastyXStart[i] >= middle - 2 || nastyXEnd[i]<= middle + 2) {
      //timer for red pixels of shame
      gameOver = 1000;
      //Nice colours for fun
      rainbowSpread();
      //reset variables ready to replay
      playerX = 75;
      playerXmove = 0;
      for (int j = 0; j < 5; j++) {
        nastyXStart [j]  = xStart;
        nastyXEnd [j]  = xEnd;
      }
      dimension = 2;
      score = 0;
    }
  }
}

// ---------------- Game pad ----------------

void readGameShield() {
  //Read button states
  for (int i = 0; i < numSwitches; i++) {
    pinVal[i] = digitalRead(switchPins[i]);
  }
}

// ---------------- NeoPixels ---------------

void drawPlayer() {
  strip.setPixelColor(playerX, 0, 255, 0);
}

void showScore() {
  strip.setPixelColor(score, 200,200,200);
}

void showNastys() {
  strip.setPixelColor(nastyXStart[dimension], 255, 0, 0);
  strip.setPixelColor(nastyXEnd[dimension], 255, 0, 0);
}

void showIslandAndDimension() {
  for (int i = middle - 2 ; i < middle + 3; i++) {
    strip.setPixelColor(i, 40, 40, 0);
  }
  strip.setPixelColor(middle + dimension - 2, 0, 0, 0);
}

void clearAllPixels() {
  for (int i = 0; i < 120; i++) {
    if (i > xStart && i < xEnd ) {
      if (gameOver > 0) {
        //red pixels of shame
        strip.setPixelColor(i, 255, 0, 0);
        gameOver--;
      }
      else {
        //sea colour
        strip.setPixelColor(i, 0, 0, 8);
      }
    }
    //bad lands outside String Land are black 
    else strip.setPixelColor(i, 0, 0, 0);
  }
}

//fun colours for the end of the game
void rainbowSpread() {
  for (int j = 0; j < 128; j++) {
    for (int i = 0; i < 55; i++) {
      strip.setPixelColor(60 + i, rainbowCol[j + i]);
      strip.setPixelColor(60 - i, rainbowCol[j + i]);
    }
    delay(10);
    strip.show();
  }
}
