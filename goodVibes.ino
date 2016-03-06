#include <IRLib.h>
#include "remote.h"

IRsend My_Sender;
// AC pins
short audioPin = 0; 
short lightPin = 1;

// DC Pins
// irPin = 3 for timing purposes or something. Can't be changed.

double colorChangeLevel = 0.2;
short offBrightness = 500;
short onBrightness = 400;
short sampleWindow = 50;
short colorWaitTime = 500;
long lastChangeTime = millis();

bool lightOn = false;
 
void setup()
{
  randomSeed(analogRead(0));
  Serial.begin(9600);
  pressButton(offBtn);
}
 
void loop() {
  handleLightPower();
  if (lightOn) {
    handleColorChange();
  }
}

void pressButton(int button) {
  My_Sender.send(NEC, buttons[button], 32);
}

void handleColorChange() {
  float audioLevel = SampleAudio(audioPin);
  Serial.println(audioLevel);
  if (audioLevel >= colorChangeLevel &&
      millis() - lastChangeTime >= colorWaitTime) {
    short rand = 0;
    rand = random(1, numButtons-1);
    pressButton(rand);
    lastChangeTime = millis();
  }
}

void handleLightPower() {
  double lightLevel = SampleLight(lightPin);
  Serial.println(lightLevel);
  if (lightLevel > offBrightness ) {
    pressButton(offBtn);
    lightOn = false;
  } else if (lightLevel < onBrightness) {
    pressButton(onBtn);
    lightOn = true;
  }
}

int SampleLight(short lightPin) {
  return 1023 - analogRead(lightPin);
}

float SampleAudio(short audioPin) {
  unsigned long startMillis= millis();  // Start of sample window
  unsigned int peakToPeak = 0;   // peak-to-peak level
  unsigned int sample = 0;
 
  unsigned int signalMax = 0;
  unsigned int signalMin = 1024;
 
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
  double volts = (peakToPeak * 3.3) / 1024;  // convert to volts
 
  return volts;
}

