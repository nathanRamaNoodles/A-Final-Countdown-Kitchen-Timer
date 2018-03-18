#include <Time.h>
#include <TimeLib.h>
/*
   I am a free software; you can redistribute me and/or modify
   me, but you must include the people who participated in making me.
   Creator: Nathan Ramanathan(but my friends call me Nathan^2)
                                     // Special thanks to the Bald Engineer for the FadeWithoutDelay Code
  // See baldengineer.com/fading-led-analogwrite-millis-example.html for more information
*/
//-----------------------------------------------------------Do Not connect a pin to D12 because it is in use as a dummy pin---------------------------------------------------------//
int dummyPin = 12;//I had D33 originally and it created a bug on Arduino Boards, who have less than 33 I/O pins.  So I decided to sacrifice a REAL Pin.
int PiezoPin = 6;//This pin is connected to the piezo buzzer
/*
  Measuring AC Current Using ACS712
*/
const int sensorIn = A0;
int mVperAmp = 1870; // use 100 for 20A Module and 1870 for 30A Module
double Voltage = 0;
double VRMS = 0;
double AmpsRMS = 0;
char previousStr = ' ';
float result;
bool smartSwitch = true;
bool SerialCount = true;
bool SerialCount1 = false;
bool SerialCount2 = false;
double pausedInterval = 0;
int readValue;             //value read from the sensor
int maxValue = 0;          // store max value here
int minValue = 1024;          // store min value here
unsigned long pMillis = 0;
unsigned long serialMillis = 0;
int CurrentCount = 0;
double inital = 0;
bool Simulate = false;
//bool getInital = true;
bool recorded = false;
bool lastPressed = false;
#define relay 8     //#define is an easier way to identify, in anyone's code, all pins the Arduino will meddle with.
#define led 11
int speakerPin;
//Global variables
int hr = 1; //1 hour
int mn = 30; //30 minutes
int sec = 0; //0seconds      //Therefore, 1 hr and 30 minutes is maxLoad.  //Use maxLoad for the maximum amount of time for the switch to be open(For safety purposes).
double maxLoad = hr * (60 * 60 * 1E3)/*hours*/ + mn * (60 * 1E3)/*minutes*/ + sec * (1E3)/*seconds*/;
double h = 0, m = 0, s = 0, a = 0, b = 0, c = 0, BeepTime = .25E3;
bool e = false, d = false, f = false, fadeOrNot = false, viewer = false, pause = true, start = false, shortTime = false, lightOrNot = true, buzzOrNot = true; //If you music off as default, simply change buzzOrNot to false
String where = ""; String split = "";
int str = 0, count = 0, buzzerCount = 0;
double interval = 0;
double newInterval = 0;
////////////////////////////////////////////////////////////////////////////////////////////////////////
// define directions for LED fadeWithoutDelay


// constants for min and max PWM
const int minPWM = 0;
const int maxPWM = 255;
// State Variable for Fade Direction
bool q = true;
bool toggle = false;
// Global Fade Value
// but be bigger than byte and signed, for rollover
int fadeValue = 0;
// How smooth to fade?
byte fadeIncrement = 5;
// millis() timing Variable, just for fading
unsigned long previousFadeMillis;
// How fast to increment?
int fadeInterval = 45;
//////////////////////////////////////////////// The FINAL COUNTDOWN song
#include "pitches.h"
unsigned long songMillis = 0;
int thisNote = -1;
// notes in the melody:
//int melody[] = {
//  NOTE_E4, NOTE_F4, NOTE_FS4, NOTE_G4, NOTE_A4, NOTE_B4, NOTE_C5
//};


int melody[] = {//The FINAL COUNTDOWN song
  NOTE_B4, NOTE_A4, NOTE_B4, NOTE_E4, 0, NOTE_C5, NOTE_B4, NOTE_C5, NOTE_B4, NOTE_A4, 0, NOTE_C5, NOTE_B4, NOTE_C5, NOTE_E4, NOTE_FS4, 0, NOTE_A4, NOTE_G4, NOTE_A4, NOTE_G4, NOTE_FS4, NOTE_A4,   NOTE_G4, 0, NOTE_B4, NOTE_A4, NOTE_B4, NOTE_E4, 0, NOTE_C5, NOTE_B4, NOTE_C5, NOTE_B4, NOTE_A4, 0, NOTE_B4, NOTE_A4, NOTE_B4, NOTE_E4, NOTE_FS4, 0, NOTE_A4, NOTE_G4, NOTE_A4, NOTE_G4, NOTE_FS4, NOTE_A4, NOTE_G4
};


// note durations: 4 = quarter note, 8 = eighth note, etc.:
int noteDurations[] = {
  16, 16, 4, 4, 2, 16, 16, 8, 8, 4, 2, 16, 16, 4, 4, 4, 4, 16, 16, 8, 8, 8, 8,  4, 8, 16, 16, 4, 4, 2, 16, 16, 8, 8, 4, 2, 16, 16, 4, 4, 4, 6, 16, 16, 8, 8, 8, 8, 4
};


const int lengthOfSong = 23 + 26; //For some reason the millis() doesn't like calculating array size.
///////////////////////////


////////////////////////////////////////For the Millis() function to meddle with
unsigned long psMillis = 0;
unsigned long shortTimeMillis = 0;
unsigned long placeHolder1 = 0;
unsigned long placeHolder2 = 0;
////////////////////////////////////////////////////////////////////////////////////////////////////////
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);//Depends on which Arduino you choose, since we are taking advantage of wireless programming and Bluetooth communications.  For example, the Hc-05 and the Arduino are both running at a 115200 baud rate.
  speakerPin = PiezoPin;
  pinMode(speakerPin, OUTPUT);  //Telling the Arduino which pins are output
  pinMode(led, OUTPUT);
  pinMode(relay, OUTPUT);
  digitalWrite(relay, HIGH);
  delay(100);   //Turns on the AC switch(relay)
  ///////////////////////////////////////////////////////////Introduction(Runs only once)
  Serial.println(F("Welcome to Nathan^2's Universal AC Switch.  How long do you want to turn on this machine?  For example 4 hours, 30 minutes, and 2 seconds is '4h 30m 2s'. Order doesn't matter."));
  Serial.println(F("But, if you typed 4h 2m 3s, but you actually meant 1m 20s, just type what you meant '0h 1m 20s'. It's really that simple!"));
  Serial.println(F("Type 'v' to view to see your timer.   1.5 hours is the max(Safety Hazard)!"));
  Serial.println(F("DID you know there is a Light!  To switch it on or off, type 'l'."));
  Serial.println(F("Type 'b' to turn the alarm off or on.\n"));
}


void loop() {
  // put your main code here, to run repeatedly:
  unsigned long currentMillis = millis();
  if (Serial.available() > 0) {
    str = Serial.read();
    if (isAlpha(str)) {
      if (!start)
        switch (str) {
          case 'h':              //hours
            h = where.toFloat();           //Don't modify variable h, m, or s because we want to show the user what they typed in!
            interval -= a;
            a = (h * 60 * 60 * 1E3);
            where = "";
            d = true;
            interval += a;
            psMillis = millis();
            viewer = true;
            break;
          case 'm':
            //minutes
            m = where.toFloat();
            interval -= b;
            b = (m * 60 * 1E3);
            where = "";
            e = true;
            interval += b;
            psMillis = millis();
            viewer = true;
            break;
          case 's':
            //seconds
            s = where.toFloat();
            interval -= c;
            c = (s * 1E3);
            where = "";
            f = true;
            interval += c;
            psMillis = millis();
            viewer = true;
            break;
          case 'v':
            if ((interval <= maxLoad)) {
              Serial.print("You entered a time of ");
              readTime(d, e, f, h, m, s);
              Serial.println(split);
              Serial.print("\t");
              Serial.println("Type 'y' for yes and we will start!");
              viewer = false;
            }
            else {                     //Safety precaution.  To change just change the maxLoad at the Beginning all the code(Line 17-19).
              Serial.println("Too long! Go under " + String(hr) + " hours " + String(mn) + " minutes " + String(sec) + " seconds.");
            }
            break;
          case 'y':
            if ((interval <= maxLoad) && (interval > 0) && !start) { /// Starts the machine
              viewer = false;
              fadeInterval = 4;
              Serial.println("Starting now!");
              Serial.println(F("To pause/resume:'p'/'o'\t reset:'r'\t end timer: 'e'\t Time remaining: 'w'."));
              if (buzzOrNot) {
                shortTime = true;
                BeepTime = 1E3;
                buzzerCount = 4;
              }
              if (lightOrNot) {
                digitalWrite(led, HIGH);
              }
              digitalWrite(relay, HIGH);
              fadeOrNot = false;
              newInterval = interval;
              psMillis = millis();
              start = true;
              placeHolder1 = millis();
            }
            break;
        }
      switch (str) {
        case 'p':
          if (start) {
            toggle = !toggle;
            if (!toggle) {
              if (!pause && !(Smart(false)) || !digitalRead(relay)) {
                digitalWrite(relay, HIGH);
                pause = true;
                fadeOrNot = false;
                newInterval = newInterval - (placeHolder2 - placeHolder1);
                psMillis = currentMillis;
                placeHolder1 = currentMillis;
                Serial.println("Resuming");
                SerialCount = true;
              }
              else if (!(AmpsRMS < (inital + 0.0030))) {
                Serial.println("It's already running. :)");
              }
            }
            else {
              if (pause) {
                digitalWrite(relay, LOW);
                pause = false;  //Beleive it or not, but we need to set pause to false, inorder to pause. :0
                fadeOrNot = true;
                placeHolder2 = currentMillis;
                pausedInterval = (newInterval - (placeHolder2 - placeHolder1));
                if (!smartSwitch) {
                  Serial.print("Paused.  Time Remaining: ");
                  readDuring(pausedInterval);
                }
                SerialCount1 = false;
                SerialCount2 = false;
              }
              else if (AmpsRMS < (inital + 0.0030)) {
                Serial.println("It's already paused!");
              }
            }
          }
          break;
        case 'a':
          inital = AmpsRMS;
          Serial.println("Definition of Off changed");
          break;
        case 'i':
          smartSwitch = !smartSwitch;
          if (smartSwitch) {
            Serial.println("Smart-Switch: on");
          }
          else {
            Serial.println("Smart-Switch: off");
          }
          break;
        case 'r':                          //reset the timer
          if (start) {
            Serial.print("Resetting timer back to ");
            readTime(d, e, f, h, m, s);
            Serial.print(split);
            newInterval = interval;
            if (pause) {
              Serial.println(", and resuming.");
            }
            else
            {
              Serial.println(", and still paused");
            }
            SerialCount1 = false;
            SerialCount2 = false;
            psMillis = millis();
            placeHolder2 = millis();
            placeHolder1 = millis();
            thisNote = -1;
          }
          break;
        case 'e':                          //End the Timer
          if (start) {
            Serial.println("Ending Timer ");
            pause = true;
            newInterval = 0;
            SerialCount1 = false;
            SerialCount2 = false;
            thisNote = lengthOfSong + 1;
          }
          else {
            digitalWrite(relay, !digitalRead(relay));
          }
          break;
        case 'w':                          //End the Timer
          if (start && pause) {
            Serial.print("\t\t\t Time left: ");
            double pausedInterval = (newInterval - (millis() - placeHolder1));
            readDuring(pausedInterval);
          }
          break;
        case 'l':                                             //Light settings
          lightOrNot = true;
          if (count == 0) {
            digitalWrite(led, LOW);
            lightOrNot = false;
            Serial.println("\t\tNightLight: Off");
            fadeOrNot = true;
          }
          else if (count == 1 && !start) {
            digitalWrite(led, HIGH);
            Serial.println("\t\tNightLight: On");
            fadeOrNot = true;
          }
          else if (count == 2 && !start) {
            fadeOrNot = false;
            Serial.println("\t\tNightLight: Fading");
          }
          else if (start && pause) {
            if (count == 1) {
              fadeOrNot = false;
              Serial.println("\t\tNightLight: Resume fading");
              count++;
            }
          }
          else if (start && !pause) {
            if (count == 1) {
              count++;
              fadeOrNot = true;
              Serial.println("\t\tNightLight: Blinking");
            }
          }
          if (count == 2) {
            count = -1;
          }
          count++;
          break;
        case 'b':                                        //Alarm settings
          Serial.print("\t\tBuzzer: ");
          if (buzzOrNot) {
            buzzOrNot = false;
            shortTime = false;
            speakerPin = dummyPin;
            Serial.println("Off");
          }
          else {
            buzzOrNot = true;
            Serial.println("On");
            speakerPin = PiezoPin;
            shortTime = true;
            BeepTime = 75;
          }
          break;
      }
    }
    else if (isDigit(str)) {
      where += char(str);    //Add numbers, not words, in a string that will be converted later with toFloat().
    }
    if (str == 'o') {
      lastPressed = true;
    }
    else {
      lastPressed = false;
    }
    previousStr = str;
  }
  else {                          //This else statement will probably have more instances than the if statement, so it is perfect for code that needs to be updated every millisecond!
    if (start) {
      if (currentMillis - serialMillis >= 1000) {
        if (Smart(false) && !lastPressed) {
          if (SerialCount) {
            fadeOrNot = true;
            placeHolder2 = currentMillis;
            pausedInterval = (newInterval - (placeHolder2 - placeHolder1));
            Serial.print("Paused.  Time Remaining: ");
            readDuring(pausedInterval);
            SerialCount = false;
            SerialCount1 = true;
            pause = false;
          }
        }
        else {
          if (SerialCount1) {
            pause = true;
            SerialCount = true;
            SerialCount1 = false;
            SerialCount2 = true;
          }
          if (SerialCount2) {
            fadeOrNot = false;
            newInterval = newInterval - (placeHolder2 - placeHolder1);
            psMillis = currentMillis;
            placeHolder1 = currentMillis;
            Serial.println("Resuming");
            SerialCount2 = false;
          }
          serialMillis = currentMillis;
        }
        if (lastPressed && digitalRead(relay) && pause) {
          lastPressed = false;
        }
      }
    }
    else {
      if (currentMillis < 1000) {
        inital = AmpsRMS;
      }
    }
    if (CurrentCount <= 100)
      getVPP(currentMillis);
    else {
      result = ((maxValue - minValue) * 5.0) / 1024.0;
      Voltage = result;
      VRMS = (Voltage / 2.0) * 0.707;
      AmpsRMS = (VRMS * 1000) / mVperAmp;
      maxValue = 0;          // store max value here
      minValue = 1024;          // store min value here
      CurrentCount = 0;
    }
    if (fadeOrNot == false) {
      doTheFade(currentMillis);  //fading the led(... 'cause it looks cool)
    }
    if (!pause && lightOrNot) {
      if (currentMillis - psMillis >= 500) {   //to indicate the timer is paused


        digitalWrite(led, !digitalRead(led));  //a fast way to turn the led off and on.
        psMillis = currentMillis;
      }
    }
    if (shortTime) {
      if (currentMillis - shortTimeMillis >= BeepTime) {
        if (buzzerCount > 5) {
          shortTime = false;
          buzzerCount = 0;
        }
        else {
          digitalWrite(speakerPin, !digitalRead(speakerPin));
          shortTimeMillis = currentMillis;
          buzzerCount++;
        }
      }
    }
    if ((viewer)) {
      if (currentMillis - psMillis >= 3E3) {         //After inputing the timer, the user might not know what to do next. So a hint will pop up like an annoying ad after 3 seconds.
        Serial.println("\t\tHint:  Type 'v' to view your timer.");
        psMillis = currentMillis;
        viewer = false;
      }
    }
    if (start && pause) {    //This, if() statement, activates when 'y' is typed into serial.
      //Is it time yet?
      if (interval >= 18.5E3) {
        if (currentMillis - psMillis >= (newInterval - 18.5E3)) { //This will turn off the machine when statement is true.  18.5E3 seconds is the time for the music to play.
          MelodyWithoutDelay(currentMillis);//ends timer with THE FINAL COUNTDOWN song
        }
      }
      else {
        if (currentMillis - psMillis >= (newInterval)) {
          thisNote = lengthOfSong;
          MelodyWithoutDelay(currentMillis);//Ends Timer with beeps


        }
      }
    }
  }
}


void doTheFade(unsigned long thisMillis) {                                   // Special thanks to the Bald Engineer for the FadeWithoutDelay Code
  // See baldengineer.com/fading-led-analogwrite-millis-example.html for more information
  // Created by James Lewis
  // is it time to update yet?
  // if not, nothing happens
  if (lightOrNot) {
    if (thisMillis - previousFadeMillis >= fadeInterval) {
      // yup, it's time!
      if (q) {
        fadeValue += fadeIncrement;
        if (fadeValue >= maxPWM) {
          // At max, limit and change direction
          fadeValue = maxPWM;
          q = false;
        }
      } else {
        //if we aren't going up, we're going down
        fadeValue -= fadeIncrement;
        if (fadeValue <= minPWM) {
          // At min, limit and change direction
          fadeValue = minPWM;
          q = true;
        }
      }
      // Only need to update when it changes


      analogWrite(led, fadeValue);


      // reset millis for the next iteration (fade timer only)
      previousFadeMillis = thisMillis;
    }
  }
}


void readTime(bool a_a, bool b_b, bool c_c, int hh, int mm, int ss) {
  split = "";
  if (a_a) {
    split += (hh + String(" hours"));
  }
  if (a_a && b_b) {
    split += ((", and ") + String(mm) + String(" minutes"));
  }
  else if (b_b) {
    split += (mm + String(" minutes"));
  }
  if ((b_b && c_c) || (c_c && a_a)) {
    split += ((", and ") + String(ss) + String(" seconds"));
  }
  else if (c_c) {
    split += (ss + String(" seconds."));
  }
  if (!(a_a) && !(b_b) && !(c_c)) {
    split = ("apparently nothing.");
  }
}
void readDuring(double Get) {
  int ha = hour(Get / (1E3));
  int ma = minute(Get / (1E3));
  int sa = second(Get / (1E3));
  bool aTrue = false; bool bTrue = false; bool cTrue = false;
  if (ha != 0)
    aTrue = true;
  if (ma != 0)
    bTrue = true;
  if (sa != 0)
    cTrue = true;
  readTime(aTrue, bTrue, cTrue, ha, ma, sa);
  Serial.println(split);
}


//
void MelodyWithoutDelay(double currentMillisillis) {
  int noteDuration = 1700 / noteDurations[thisNote];
  int pauseBetweenNotes = noteDuration * 1.2;
  if ((currentMillisillis - songMillis <= pauseBetweenNotes / 4)) {
    digitalWrite(led, LOW);
  }
  else if ((currentMillisillis - songMillis <= pauseBetweenNotes)) {
    tone(speakerPin, melody[thisNote], noteDuration);
    if (lightOrNot)
      digitalWrite(led, HIGH);
  }
  else {
    fadeOrNot = true;
    if (thisNote >= lengthOfSong) {
      thisNote = -2;
      psMillis = currentMillisillis;
      digitalWrite(relay, LOW);
      Serial.println("Time's up!");
      fadeInterval = 45;
      fadeOrNot = false;
      start = false;
      shortTime = true;
      BeepTime = 1E3;
    }
    thisNote++;
    noTone(speakerPin);
    songMillis = currentMillisillis;
  }
}

void getVPP(long start_time)
{
  if ((start_time - pMillis) > 1) //sample for 1 Sec
  {
    readValue = analogRead(sensorIn);
    // see if you have a new maxValue
    if (readValue > maxValue)
    {
      /*record the maximum sensor value*/
      maxValue = readValue;
    }
    if (readValue < minValue)
    {
      /*record the maximum sensor value*/
      minValue = readValue;
    }
    pMillis = start_time;
    CurrentCount++;
  }
  // Subtract min from max
}

bool Smart(bool b) {
  if (!smartSwitch) {
    return b;
  }
  else {
    return (AmpsRMS < (inital + 0.0030));
  }
}

