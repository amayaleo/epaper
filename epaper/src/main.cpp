#include <iostream>
#include <memory>
#include <epaper/epaper.h>
#define SCREEN 213
#include <graphics/demoImageData.h>
#include <tyme/tyme.h>



int main(){

    EPAPER::EPD_Driver epaper(eScreen_EPD_213, EPAPER::boardRaspberryPi_EXT);
    // EPD_Driver epdtest(eScreen_EPD_271, boardRaspberryPiPico_RP2040_EXT3);

    // Initialize CoG
    epaper.COG_initial();
    // Global Update Call
    // void EPD_Driver::globalUpdate(const uint8_t * data1s, const uint8_t * data2s)
    epaper.globalUpdate( BW_monoBuffer, BW_0x00Buffer);
    TYME::delay(100);
    epaper.globalUpdate(BWR_blackBuffer,BWR_redBuffer);
    // Turn off CoG
    epaper.COG_powerOff();

    return 0;
}