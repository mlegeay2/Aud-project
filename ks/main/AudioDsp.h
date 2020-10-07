#ifndef __AUDIODSP_H
#define __AUDIODSP_H

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "../../lib/KS.h"

class AudioDsp
{
public:
  AudioDsp(int SR, int BS);
  bool start();
  void stop();
  void setFreq(int midi_fondamentale, int tonalite);
  void trigger();
  float MidiToFreq(int midi);
private:
  void audioTask();
  static void audioTaskHandler(void* arg);
  
  int fSampleRate, fBufferSize, fNumOutputs;
  TaskHandle_t fHandle;
  bool fRunning;
  
  KS ks1;
  KS ks2;
  KS ks3;
  KS ks4;
  KS ks5;
  KS ks6;

};

#endif