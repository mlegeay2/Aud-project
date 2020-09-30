#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"
#include "../../lib/ES8388.h"

#include "AudioDsp.h"

extern "C" {
  void app_main(void);
}

void app_main(void)
{
  // initialize Audio Codec
  ES8388 es8388;
  es8388.init();
  
  // start audio DSP
  AudioDsp audioDsp(48000,16);
  audioDsp.start();

  // infinite loop playing a little melody
  int melody[15] = {80, 77, 80, 72, 72, 0, 0, 80, 77, 80, 73, 75, 0, 0};

  // infinite loop
  while(1) {
    for(int i=0; i< 15 ; i++){  

      audioDsp.setFreq(audioDsp.MidiToFreq(melody [i]));
      audioDsp.trigger();
      vTaskDelay(600 / portTICK_PERIOD_MS);
    }
  }


}




