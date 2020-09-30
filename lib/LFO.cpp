#include <cmath>

#include "LFO.h"

#define SINE_TABLE_SIZE 16384

LFO::LFO(int SR) : 
sineTable(SINE_TABLE_SIZE),
cPhasor(SR),
mPhasor(SR),
cFreq(440.0),
modIndex(100.0),
gain(1.0),
samplingRate(SR){}

void LFO::setCFreq(float f){
  cFreq = f;
}

void LFO::setMFreq(float f){
  mPhasor.setFrequency(f);
}

void LFO::setIndex(float i){
  modIndex = i;
}
    
void LFO::setGain(float g){
  gain = g;
}
    
float LFO::tick(){
  int mIndex = mPhasor.tick()*SINE_TABLE_SIZE;
  float modulator = sineTable.tick(mIndex);
  cPhasor.setFrequency((echo.tick(sawtooth.tick()*2 - 1)*0.5) + modulator*modIndex);
  int cIndex = cPhasor.tick()*SINE_TABLE_SIZE;
  return sineTable.tick(cIndex)*gain;
}
