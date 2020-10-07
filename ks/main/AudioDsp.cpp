#include <stdio.h>
#include <cmath>
#include <algorithm>

#include "AudioDsp.h"
#include "driver/i2s.h"

#define MULT_S16 32767
#define DIV_S32 4.6566129e-10

AudioDsp::AudioDsp(int SR, int BS) : 
fSampleRate(SR),
fBufferSize(BS),
fNumOutputs(2),
fHandle(nullptr),
fRunning(false),
ks1 (SR),
ks2 (SR),
ks3 (SR),
ks4 (SR),
ks5 (SR),
ks6 (SR)
{
  // config i2s pin numbers
  i2s_pin_config_t pin_config;
  pin_config = {
    .bck_io_num = 5,
    .ws_io_num = 25,
    .data_out_num = 26,
    .data_in_num = 35
  };
  
  // config i2
  i2s_config_t i2s_config = {
    .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX | I2S_MODE_RX),
    .sample_rate = fSampleRate,
    .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
    .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
    .communication_format = I2S_COMM_FORMAT_STAND_I2S,
    .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1, // high interrupt priority
    .dma_buf_count = 3,
    .dma_buf_len = fBufferSize,
    .use_apll = false,
    .tx_desc_auto_clear = true,
    .fixed_mclk = false
  };
  
  // installing i2s driver
  i2s_driver_install((i2s_port_t)0, &i2s_config, 0, nullptr);
  i2s_set_pin((i2s_port_t)0, &pin_config);
  PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO0_U, FUNC_GPIO0_CLK_OUT1);
  REG_WRITE(PIN_CTRL, 0xFFFFFFF0);
}

// starts audio task
bool AudioDsp::start()
{
  if (!fRunning) {
    fRunning = true;
    return (xTaskCreatePinnedToCore(audioTaskHandler, "Audio DSP Task", 4096, (void*)this, 24, &fHandle, 0) == pdPASS);
  } 
  else {
    return true;
  }
}

// stops audio task
void AudioDsp::stop()
{
  if (fRunning) {
    fRunning = false;
    vTaskDelay(1/portTICK_PERIOD_MS);
    fHandle = nullptr;
  }
}

void AudioDsp::setFreq(int midi_fondamentale, int tonalite){

  //majeure
  if (tonalite == 0){
    ks1.setFreq(MidiToFreq(midi_fondamentale));
    ks2.setFreq(MidiToFreq(midi_fondamentale+4));
    ks3.setFreq(MidiToFreq(midi_fondamentale+7));
    ks4.setFreq(MidiToFreq(midi_fondamentale-12));
    ks5.setFreq(MidiToFreq(midi_fondamentale-8));
    ks6.setFreq(MidiToFreq(midi_fondamentale-5));
  }

  //mineur
  if (tonalite == 1){
    ks1.setFreq(MidiToFreq(midi_fondamentale));
    ks2.setFreq(MidiToFreq(midi_fondamentale+3));
    ks3.setFreq(MidiToFreq(midi_fondamentale+7));
    ks4.setFreq(MidiToFreq(midi_fondamentale-12));
    ks5.setFreq(MidiToFreq(midi_fondamentale-9));
    ks6.setFreq(MidiToFreq(midi_fondamentale-5));
  }


  
}

void AudioDsp::trigger(){
  ks1.trigger();
  ks2.trigger();
  ks3.trigger();
  ks4.trigger();
  ks5.trigger();
  ks6.trigger();
}

//Convert a midi note to a frequency
float AudioDsp::MidiToFreq(int midi){
  return 440.0*pow(2.0,(midi-69.0)/12.0);
}

void AudioDsp::audioTask()
{
  // inifinite loop
  while (fRunning) {
    int16_t samples_data_out[fNumOutputs*fBufferSize];
    
    // processing buffers
    for (int i = 0; i < fBufferSize; i++) {
      // DSP
      float currentSample = ks1.tick() + ks2.tick() + ks3.tick() + ks4.tick() + ks5.tick() + ks6.tick();
      
      // copying to output buffer
      samples_data_out[i*fNumOutputs] = currentSample*MULT_S16;
      samples_data_out[i*fNumOutputs+1] = samples_data_out[i*fNumOutputs];
    }
    // transmitting output buffer
    size_t bytes_written = 0;
    i2s_write((i2s_port_t)0, &samples_data_out, fNumOutputs*sizeof(int16_t)*fBufferSize, &bytes_written, portMAX_DELAY);
  }
  // Task has to deleted itself beforee returning
  vTaskDelete(nullptr);
}

// static cast of the audio task
void AudioDsp::audioTaskHandler(void* arg)
{
  AudioDsp* audio = static_cast<AudioDsp*>(arg);
  audio->audioTask();
}
