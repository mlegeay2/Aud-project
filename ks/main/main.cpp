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
  int melody[16] = {72, 72, 72, 72, 81, 81, 81, 81, 76, 76, 76, 76, 79, 79, 79, 79};
  int tonalites[16] = {0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0};
  //int gain_premiere_corde = {};

  // infinite loop
  while(1) {
    for(int i=0; i< 16 ; i++){  

      audioDsp.setFreq(melody [i], tonalites[i]);
      audioDsp.trigger();
      vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
  }


}




