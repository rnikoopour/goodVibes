#include <IRLib.h>
#include "remote.h"

//#define DEBUGLIGHT
//#define DEBUGAUDIO
//#define DEBUGSENSITIVITY

IRsend My_Sender;
// AC pins
const char audioPin = 0; 
const char lightPin = 1;

// DC Pins
// const char irPin = 3 required for timing
const char sensitivityBtn = 13;

const char numSensitivities = 3;
const float colorChangeLevel[] = {0.25, 0.5, 1.0};

const short offBrightness = 500;
const short onBrightness = 400;
const short colorWaitTime = 1000;

const char sampleWindow = 50;
const char numAudioSamples = 20;

unsigned long lastChangeTime = millis();
float audioSamples[numAudioSamples] = {};
int sensitivityBtnState = HIGH;
int sensitivity = 0;
bool sensitivityPressed = false;
bool lightOn = false;
 
void setup()
{
  Serial.begin(9600);
  randomSeed(analogRead(0));
  pressButton(&My_Sender, offBtn);
  for (char i = 0; i < numAudioSamples; i++) {
    audioSamples[i] = -1.0;
  }
  pinMode(sensitivityBtn, INPUT);
}
 
void loop() {
  adjustSensitivity();
  if (millis() < lastChangeTime) {
    // Handles millis() overflow (approx every 50 days)
    lastChangeTime = 0;
  }
  
  handleLightPower();
  if (lightOn) {
    handleColorChange();
  }
}

void adjustSensitivity() {
  sensitivityBtnState = digitalRead(sensitivityBtn);
  if (sensitivityBtnState == LOW &&
      !sensitivityPressed) {
    sensitivityPressed = true;
    sensitivity++;
    sensitivity %= numSensitivities;
    #ifdef DEBUGSENSITIVITY
    Serial.println("Button pressed ");
    Serial.print("Sensitiviy level = ");
    Serial.println(colorChangeLevel[sensitivity]);
    #endif
  } else if (sensitivityBtnState == HIGH) {
    sensitivityPressed = false;
  }
  
}

float getAudioAverage() {
  float average = 0.0;
  
  for (char i = 0; i < numAudioSamples; i++) {
    if (audioSamples[i] == -1.0) {
      return -1.0;
    } else {
      average += audioSamples[i];
    }
  }
  average /= numAudioSamples;
  return average;
}

void handleColorChange() {
  static char index = 0;
  float audioLevel = SampleAudio(audioPin);
  float audioAverage = getAudioAverage();
  float audioDelta = audioLevel - audioAverage;
  audioSamples[index] = audioLevel;
  index++;
  index %= numAudioSamples;
  #ifdef DEBUGAUDIO
  if (index == numAudioSamples/2) {
    Serial.print("AudioAve = ");
    Serial.println(audioAverage);
  }
  #endif

  if (audioAverage != -1.0 &&
      audioDelta >= colorChangeLevel[sensitivity] &&
      millis() - lastChangeTime >= colorWaitTime) {
    char rand = 0;
    rand = random(1, numButtons-1);
    pressButton(&My_Sender, rand);
    lastChangeTime = millis();
  #ifdef DEBUGAUDIO
    Serial.println();
    Serial.print("AudioLevel = ");
    Serial.println(audioLevel);
    Serial.print("AudioAve = ");
    Serial.println(audioAverage);
    Serial.print("audioDelta = ");
    Serial.println(audioDelta);
    Serial.println();
  #endif


  }
}

void handleLightPower() {
  short lightLevel = SampleLight(lightPin);
  #ifdef DEBUGLIGHT
  Serial.println(lightLevel);
  #endif
  
  if (lightLevel > offBrightness ) {
    pressButton(&My_Sender, offBtn);
    lightOn = false;
  } else if (lightLevel < onBrightness) {
    pressButton(&My_Sender, onBtn);
    lightOn = true;
  }
}

short SampleLight(short lightPin) {
  return 1023 - analogRead(lightPin);
}

float SampleAudio(short audioPin) {
  unsigned long startMillis= millis();  // Start of sample window
  unsigned int peakToPeak = 0;   // peak-to-peak level
  unsigned int sample = 0;
 
  unsigned int signalMax = 0;
  unsigned int signalMin = 1024;
  static char index = 0;
 
  // collect data for 50 mS
  while (millis() - startMillis < sampleWindow)
    {
      sample = analogRead(audioPin);
      if (sample < 1024)  // toss out spurious readings
	{
	  if (sample > signalMax)
	    {
	      signalMax = sample;  // save just the max levels
	    }
	  else if (sample < signalMin)
	    {
	      signalMin = sample;  // save just the min levels
	    }
	}
    }
  peakToPeak = signalMax - signalMin;  // max - min = peak-peak amplitude
  float volts = (peakToPeak * 3.3) / 1024;  // convert to volts

  return volts;
}
