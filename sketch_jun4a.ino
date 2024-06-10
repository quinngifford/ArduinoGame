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

int level = 0;
int points = 0;
int c = 0;
int guess[] = {0,0,0};
int color[] = {0,0,0};
int increment = 1;
int secs;

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
  attachInterrupt(digitalPinToInterrupt(pinL), interruptL, FALLING);
  attachInterrupt(digitalPinToInterrupt(pinR), interruptR, FALLING);
  attachInterrupt(digitalPinToInterrupt(pinS), interruptS, CHANGE);
  timer.start(20000, AsyncDelay::MILLIS);
  beep.start(2000, AsyncDelay::MILLIS);
  randomColors();
  generateMIDI();
}

void loop() {
  if(timer.isExpired()){
    Serial.println();Serial.print("Level ");Serial.print(level+1);Serial.print("/");Serial.print(8);Serial.println(" complete! ");
    level++;
    if(level > 7){
      Serial.print("FINAL SCORE: "); Serial.println(points);Serial.println();
      level = 0;
      points = 0;
    }
    int accuracy = abs(color[0]-guess[0])+abs(color[1]-guess[1])+abs(color[2]-guess[2]);
    points+= accuracy;
    Serial.print("Color: ");Serial.print(color[0]);Serial.print(", ");Serial.print(color[1]);Serial.print(", ");Serial.print(color[2]);Serial.println(", ");
    Serial.print("Guess: ");Serial.print(guess[0]);Serial.print(", ");Serial.print(guess[1]);Serial.print(", ");Serial.print(guess[2]);Serial.println(", ");
    Serial.print("Error: "); Serial.print(accuracy); Serial.print(" Total Error: "); Serial.println(points);Serial.println();
    secs = (20-(level*2))*1000;
    timer.start(secs, AsyncDelay::MILLIS);
    Serial.print("Level ");
    Serial.println(level);
    guess[0] = 0;guess[1] = 0;guess[2] = 0;
    randomColors();
  }
  else
  {
    if(beep.isExpired()){
      float beepDelay = (0-timer.getDuration())*0.1;
      beep.start(beepDelay,AsyncDelay::MILLIS);
      CircuitPlayground.playTone(midi[60], 50);
    }
    if(stateS){
      if(increment == 1){
        increment = 10;
      }
      else{
        increment = 1;
      }
      Serial.print("Increment: ");
      Serial.println(increment);
      stateS = false;
    }
    if(stateL){
      c++;
      if(c>2){
        c=0;
      }
      String cola;
      switch(c){
        case 0:
          cola = "Red: ";
          break;
        case 1:
          cola = "Green: ";
          break;
        case 2:
          cola = "Blue: ";
          break;
      }
      Serial.print(cola);
      Serial.println(guess[c]);
      stateL = false;
    }
    else if(stateR){
      if(guess[c]+increment<=255){
        guess[c] += increment;
      }
      else{
        guess[c]=255;
      }
      String cola;
      switch(c){
        case 0:
          cola = "Red: ";
          break;
        case 1:
          cola = "Green: ";
          break;
        case 2:
          cola = "Blue: ";
          break;
      }
      Serial.print(cola);
      Serial.println(guess[c]);
      stateR = false;
    }
  }
}
void randomColors(){
  color[0] = random(255);
  color[1] = random(255);
  color[2] = random(255);
  for (int i=0; i < 10; i++){
      CircuitPlayground.setPixelColor(i, color[0],color[1],color[2]);
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