#include <Adafruit_NeoPixel.h>
#include <Wire.h>
#include "Adafruit_MPR121.h"
#define NUMPIXELS      12
const int ledPin =  4; 
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, ledPin);

     // the number of the LED pin

// You can have up to 4 on one i2c bus but one is enough for testing!
Adafruit_MPR121 cap = Adafruit_MPR121();

// Keeps track of the last pins touched
// so we know when buttons are 'released'
uint16_t lasttouched = 0;
uint16_t currtouched = 0;

//---------------------------------

#ifdef __AVR__
  #include <avr/power.h>
#endif
//#include <AnalogTouch.h>
const int timerPin = 2;     // the number of the pushbutton pin
//const int ledPin =  4;      // the number of the LED pin
const int touchPin = 7;
const int speakerPin = 9;
const int speakerGroundPin = 8;

// How many NeoPixels are attached to the Arduino?
#define NUMPIXELS      12

//For AnalogTouch
#define pinAnalog A0
#define offset 2
#if offset > 6
#error "Too big offset value"
#endif

#include "pitches.h"

//Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, ledPin);

// notes in the melody:
int melodyEnd[] = {
  NOTE_C4, NOTE_G3, NOTE_G3, NOTE_A3, NOTE_G3, 0, NOTE_B3, NOTE_C4
};

// note durations: 4 = quarter note, 8 = eighth note, etc.:
int noteDurations[] = {
  4, 8, 8, 4, 4, 4, 4, 4
};

int pushButtonValue = 0;
int numberOfTurns = 1;
int numberOfDurationPerTurnInMinutes = 1;
int modi = 0;
int durationOfButtonPressed = 0;
int numberOfTimesPressed = 0;
bool pressedLow = false;
bool pressedHigh = false;
int numberOfMinutes = 0;
int numberOfPeople = 0;
bool activated = false;
int counter = 0;
int counterMinutes = 0;
int currentRound = 0;
int isEnd = false;
int endCounter = 0;

void setup() {
  while (!Serial);        // needed to keep leonardo/micro from starting too fast!
  Serial.begin(9600);
  Serial.println("Adafruit MPR121 Capacitive Touch sensor test"); 
  
  // Default address is 0x5A, if tied to 3.3V its 0x5B
  // If tied to SDA its 0x5C and if SCL then 0x5D
  if (!cap.begin(0x5A)) {
    Serial.println("MPR121 not found, check wiring?");
    while (1);
  }
  Serial.println("MPR121 found!");
  
  //--------------
  
  
  // iterate over the notes of the melody:
  //MELODY LOW, hardware specific
  Serial.begin(9600);
  pinMode(ledPin, OUTPUT);
  pinMode(speakerPin, OUTPUT);
  // initialize the pushbutton pin as an input:
  pinMode(timerPin, INPUT);
  digitalWrite(timerPin, HIGH);//activate pull up
  pinMode(speakerGroundPin, OUTPUT);
  digitalWrite(speakerGroundPin, LOW);
  pixels.begin(); 
  
  pixels.setBrightness(3);
  updateColor(0, 255, 0, 0);
}

bool readTouch(){
  bool touched = false;
    // Get the currently touched pads
  currtouched = cap.touched();
  
  for (uint8_t i=0; i<12; i++) {
    // it if *is* touched and *wasnt* touched before, alert!
    if ((currtouched & _BV(i)) && !(lasttouched & _BV(i)) ) {
      Serial.print(i); Serial.println(" touched");
      //pixels.setPixelColor(1, pixels.Color(255,0,0));
      //pixels.show();
      touched = true;
      return touched;
    }
    // if it *was* touched and now *isnt*, alert!
    if (!(currtouched & _BV(i)) && (lasttouched & _BV(i)) ) {
      Serial.print(i); Serial.println(" released");
      //pixels.setPixelColor(1, pixels.Color(0,255, 0));
      //pixels.show();
      touched = false;
      return touched;
    }
  }
}

void updateColor(int r, int g, int b, int value){
  for(int i=0;i<value;i++){
    // pixels.Color takes RGB values, from 0,0,0 up to 255,255,255
    pixels.setPixelColor(i, pixels.Color(r,g,b)); // Moderately bright green color.
  }
  pixels.show(); 
}

void startScrum(int numberOfMinutes, int numberOfPeople, int counter) {
  delay(50);
  updateColor(0,0,0,12);
  updateColor(25,222,255,(numberOfMinutes - counter));
}

void playMelody(){
  for (int thisNote = 0; thisNote < 8; thisNote++) {
      int noteDuration = 1000 / noteDurations[thisNote];
      tone(9, melodyEnd[thisNote], noteDuration);
  
      // to distinguish the notes, set a minimum time between them.
      // the note's duration + 30% seems to work well:
      int pauseBetweenNotes = noteDuration * 1.30;
      delay(pauseBetweenNotes);
      // stop the tone playing:
      noTone(9);
    }
}

void playMelody2(){
  for (int thisNote = 0; thisNote < 8; thisNote++) {
      int noteDuration = 100 / noteDurations[thisNote];
      tone(9, melodyEnd[thisNote], noteDuration);
  
      // to distinguish the notes, set a minimum time between them.
      // the note's duration + 30% seems to work well:
      int pauseBetweenNotes = noteDuration * 2.30;
      delay(pauseBetweenNotes);
      // stop the tone playing:
      noTone(2);
    }
}

void loop() {
  pushButtonValue = digitalRead(timerPin);

  if (pushButtonValue == LOW) {
    durationOfButtonPressed += 1;
    if (durationOfButtonPressed == 100) {
      modi = (modi + 1) % 3;
      numberOfTimesPressed = 1;
    }
  } else {
    durationOfButtonPressed = 0;
  }
  
  if (pushButtonValue == LOW && !pressedHigh) {
    pressedLow = true;
  }
  if (pushButtonValue == HIGH && pressedLow) {
    pressedHigh = true;
  }
  if (pressedHigh && pressedHigh) {
      numberOfTimesPressed = (numberOfTimesPressed + 1) % 13;
      
      pressedHigh = false;
      pressedLow = false;
  }

  switch(modi) {
    case 0:
      counterMinutes = 0;
      numberOfPeople = 0;
      numberOfMinutes = 0;
      
      updateColor(0,0,0,12);
      updateColor(255,0,0,numberOfTimesPressed);
      numberOfMinutes = numberOfTimesPressed;
      break;
    case 1:
      updateColor(0,0,0,12);      
      updateColor(255,222,0,numberOfTimesPressed);
      numberOfPeople = numberOfTimesPressed;
      break;
    case 2:
      updateColor(0,0,0,12);      
      updateColor(25,222,220,12);
      
      break;
  }
  if (currentRound == numberOfPeople) {
    isEnd = true;
  }
  if(readTouch() && modi == 2 && currentRound < numberOfPeople){
    counter++;
    if (counter == 13) {
      counter = 0;
      counterMinutes++;      
    }
    if (numberOfMinutes == counterMinutes) {
      playMelody();
      counterMinutes = 0;
      currentRound++;
    }
    updateColor(0,0,0,12);      
    updateColor(0,0,222,counter);

    delay(500);    
  } else if (endCounter == 0 && isEnd && modi == 2 && currentRound == numberOfPeople) {
    playMelody2();
    endCounter++;
    modi = 0;
    currentRound = 0;
    numberOfMinutes = 0;
    
    numberOfPeople = 0;
    activated = false;
    counter = 0;
    counterMinutes = 0;
    currentRound = 0;
    
    isEnd = false;
    updateColor(0,0,0,12);
    updateColor(10,10,10,12);
    pressedHigh = false;
    pressedLow = false;
  }
}
