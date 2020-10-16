#include <stdio.h>
#include <iostream>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"
#include "../../lib/ES8388.h"
#include "../../lib/ButtonHandler.h"
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

  ButtonHandler buttonHandler(1,8);

  // infinite loop playing a little melody
  int melody[16] = {64,64,64,64,60,60,60,60,67,67,67,67,62,62,62,62};
  int tonalites[16] = {1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0};


  // infinite loop
  while(1) {



    for(int i=0; i< 16 ; i++){

      buttonHandler.tick();
      float tempo = (buttonHandler.getValue()*10+60);
      std::cout << "Tempo = " << int(tempo) << "\n";

      audioDsp.setFreq(melody [i], tonalites[i]);
      audioDsp.trigger();
      //Reglage du tempo
      vTaskDelay((30000/tempo) / portTICK_PERIOD_MS);
    }
  }


}
