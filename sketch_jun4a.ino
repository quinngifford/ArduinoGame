#include <Adafruit_CircuitPlayground.h>
#include <AsyncDelay.h>

const int pinL = 4;
const int pinR = 5;
const int pinS = 7;
volatile bool stateL = false;
volatile bool stateR = false;
volatile bool stateS = false;
AsyncDelay timer;
AsyncDelay beep;
float midi[127];
int A_four = 440;

int totalmillis = 0;
int level = 0;
int points = 0;
int c = 0;
int guess[] = {0,0,0};
int color[] = {0,0,0};
int increment = 1;

void interruptL(){
  stateL = true;
}
void interruptR(){
  stateR = true;
}
void interruptS(){
  stateS = true;
}


void setup() {
  CircuitPlayground.begin();
  Serial.begin(9600);
  attachInterrupt(digitalPinToInterrupt(pinL), interruptL, RISING);
  attachInterrupt(digitalPinToInterrupt(pinR), interruptR, RISING);
  attachInterrupt(digitalPinToInterrupt(pinS), interruptS, CHANGE);
  timer.start(20000, AsyncDelay::MILLIS);
  beep.start(2000, AsyncDelay::MILLIS);
  randomColors();
}

void loop() {
  if(timer.isExpired()){
    Serial.println("Level complete");
    level++;
    if(level > 7){
      level = 0;
    }
    points += abs(color[0]-guess[0])+abs(color[1]-guess[1])+abs(color[2]-guess[2]);
    guess[0] = 0;guess[1] = 0;guess[2] = 0;color[0] = 0;color[1] = 0;color[2] = 0;
    randomColors();
    int secs = (20-(level*2))*1000;
    totalmillis+=secs;
    timer.start(secs, AsyncDelay::MILLIS);
    Serial.println("Next Level");
    c = 0;
  }
  else
  {
    if(beep.isExpired()){
      float beepDelay = (millis()-totalmillis)*0.1;
      beep.start(beepDelay,AsyncDelay::MILLIS);
      CircuitPlayground.playTone(midi[60], 100);
    }
    if(stateS){
      if(increment == 1){
        increment = 10;
      }
      else{
        increment = 1;
      }
      stateS = false;
    }
    if(stateL&!stateR){
      guess[c] -= increment;
      stateL = false;
    }
    else if(stateR&!stateL){
      guess[c] += increment;
      stateR = false;
    }
    else if(stateR&stateL){
      c++;
      if(c>2){
        c=0;
      }
      stateR = false;
      stateL = false;
    }

  }
}
void randomColors(){
  int r1 = random(255);
  int r2 = random(255);
  int r3 = random(255);
  for (int i=0; i < 10; i++){
      CircuitPlayground.setPixelColor(i, r1,r2,r3);
  }
}

void generateMIDI()
{
  for (int x = 0; x < 127; ++x)
  {
    midi[x] = (A_four / 32.0) * pow(2.0, ((x - 9.0) / 12.0));
    Serial.println(midi[x]);
  }
}