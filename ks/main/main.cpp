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

  ButtonHandler buttonHandler(4,20);

  // infinite loop playing a little melody
  int melody[16] = {72, 76, 72, 76, 72, 81, 76, 72, 81, 76, 72, 81, 76, 72, 81, 76};
  int tonalites[16] = {0, 1, 0, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1};
  //int gain_premiere_corde = {};

  // infinite loop
  while(1) {
    buttonHandler.tick();
    
    float pulse = buttonHandler.getValue()*100 + 80;
    std::cout << "Carrier Freq: " << pulse << "\n";
    
    
    for(int i=0; i< 16 ; i++){  

      audioDsp.setFreq(melody [i], tonalites[i]);
      audioDsp.trigger();
      //Reglage du tempo
      vTaskDelay(pulse / portTICK_PERIOD_MS);
    }
  }


}




