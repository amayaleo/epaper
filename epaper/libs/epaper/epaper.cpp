
//#include <iostream>
#include <string>
#include <cstring>
#include <epaper/epaper.h>
#include <tyme/tyme.h>



namespace EPAPER{

const uint8_t register_data_mid[] = { 0x00, 0x0e, 0x19, 0x02, 0x0f, 0x89 };	// 4.2"
const uint8_t register_data_sm[] = { 0x00, 0x0e, 0x19, 0x02, 0xcf, 0x8d };	// other sizes



EPD_Driver::EPD_Driver(uint32_t eScreen_EPD, pins_t board)
: spi_ptr{std::make_unique<SPI::Spi_t>()}
{
	pin_cfg_epaper = board;
	
	// Type
	pdi_cp = (uint16_t) eScreen_EPD;
	pdi_size = (uint16_t) (eScreen_EPD >> 8);

	uint16_t screenSizeV = 0;
	uint16_t screenSizeH = 0;
	uint16_t screenDiagonal = 0;
	uint16_t refreshTime = 0;
	
    switch (pdi_size)
    {
        case 0x15: // 1.54"

            screenSizeV = 152; // vertical = wide size
            screenSizeH = 152; // horizontal = small size
            screenDiagonal = 154;
            refreshTime = 16;
            break;

        case 0x21: // 2.13"

            screenSizeV = 212; // vertical = wide size
            screenSizeH = 104; // horizontal = small size
            screenDiagonal = 213;
            refreshTime = 15;
            break;

        case 0x26: // 2.66"

            screenSizeV = 296; // vertical = wide size
            screenSizeH = 152; // horizontal = small size
            screenDiagonal = 266;
            refreshTime = 15;
            break;

        case 0x27: // 2.71"

            screenSizeV = 264; // vertical = wide size
            screenSizeH = 176; // horizontal = small size
            screenDiagonal = 271;
            refreshTime = 19;
            break;

        case 0x28: // 2.87"

            screenSizeV = 296; // vertical = wide size
            screenSizeH = 128; // horizontal = small size
            screenDiagonal = 287;
            refreshTime = 14;
            break;

        case 0x37: // 3.70"

            screenSizeV = 416; // vertical = wide size
            screenSizeH = 240; // horizontal = small size
            screenDiagonal = 370;
            refreshTime = 15; // ?
            break;

        case 0x41: // 4.17"

            screenSizeV = 300; // vertical = wide size
            screenSizeH = 400; // horizontal = small size
            screenDiagonal = 417;
            refreshTime = 19;
            break;

        case 0x43: // 4.37"

            screenSizeV = 480; // vertical = wide size
            screenSizeH = 176; // horizontal = small size
            screenDiagonal = 437;
            refreshTime = 21;
            break;

        default:

            break;
    }

    // Force conversion for two unit16_t multiplication into uint32_t.
    // Actually for 1 colour; BWR requires 2 pages.
    image_data_size = (uint32_t) screenSizeV * (uint32_t) (screenSizeH / 8);

	// Config registers according to screen size
	memcpy(register_data, register_data_sm, sizeof(register_data_sm));
}



// int digitalRead(int x){
    // 
// return gpio_get_fd_to_value(x);
// }



// CoG initialization function
//		Implements Tcon (COG) power-on and temperature input to COG
//		- INPUT:
//			- none but requires global variables on SPI pinout and config register data
void EPD_Driver::COG_initial()
{	
	pinMode( pin_cfg_epaper.panelBusy, INPUT );     //All Pins 0	
	pinMode( pin_cfg_epaper.panelDC, OUTPUT );
	digitalWrite(pin_cfg_epaper.panelDC, HIGH);
	pinMode( pin_cfg_epaper.panelReset, OUTPUT );
	digitalWrite(pin_cfg_epaper.panelReset, HIGH);
	pinMode( pin_cfg_epaper.panelCS, OUTPUT );
	digitalWrite(pin_cfg_epaper.panelCS, HIGH);


	if (pin_cfg_epaper.panelON_EXT2 != 0xff)
	{
		pinMode( pin_cfg_epaper.panelON_EXT2, OUTPUT );//NOT_CONNECTED
		pinMode( pin_cfg_epaper.panelSPI43_EXT2, OUTPUT );//NOT_CONNECTED
		digitalWrite( pin_cfg_epaper.panelON_EXT2, HIGH );    //PANEL_ON# = 1 //NOT_CONNECTED
		digitalWrite( pin_cfg_epaper.panelSPI43_EXT2, LOW );//NOT_CONNECTED
	}

	// pinMode(spi_basic.flashCS, OUTPUT);
    // digitalWrite(spi_basic.flashCS, HIGH);
	
	// SPI init
	// #ifndef SPI_CLOCK_MAX
		 #define SPI_CLOCK_MAX 16000000
	// #endif

	#if defined(ENERGIA)
	{
		//spi.begin();
		//spi.setDataMode(SPI_MODE0);
		//spi.setClockDivider(SPI_CLOCK_DIV32);
		        // SPI.setClockDivider(16000000 / min(16000000, 4000000));
		//spi.setBitOrder(MSBFIRST);
	}
	#else
	{
		SPISettings settingScreen;
        
		settingScreen = {8000000, MSBFIRST, SPI_MODE0};
		spi.begin();
		spi.beginTransaction(settingScreen);
	}
	#endif

	digitalWrite( pin_cfg_epaper.panelON_EXT2, HIGH );    //PANEL_ON# = 1
	digitalWrite( pin_cfg_epaper.panelSPI43_EXT2, LOW );
	TYME::delay( 5 );

	// Power On COG driver sequence
	reset(1, 5, 10, 5, 1);

	softReset();
	sendIndexData( 0xe5, &register_data[2], 1 );  //Input Temperature: 25C
	sendIndexData( 0xe0, &register_data[3], 1 );  //Active Temperature
	sendIndexData( 0x00, &register_data[4], 2 );  //PSR
}

void EPD_Driver::sendIndexData( uint8_t index, const uint8_t *data, uint32_t len )
    {	
    	digitalWrite( pin_cfg_epaper.panelDC, LOW );      //DC Low
    	digitalWrite( pin_cfg_epaper.panelCS, LOW );      //CS Low
    	//spi.transfer(index);
        spi_ptr->Transfer2bytes(index);
    	digitalWrite( pin_cfg_epaper.panelCS, HIGH );     //CS High
    	digitalWrite( pin_cfg_epaper.panelDC, HIGH );     //DC High
    	digitalWrite( pin_cfg_epaper.panelCS, LOW );      //CS Low
    	for ( uint32_t i = 0; i < len; i++ )
    	{
    		//spi.transfer(data[ i ]);
            spi_ptr->Transfer2bytes(data[ i ]);
    	}

    	digitalWrite( pin_cfg_epaper.panelCS, HIGH );     //CS High
    }



    void EPD_Driver::softReset()
    {
    	sendIndexData( 0x00, &register_data[1], 1 );	//Soft-reset, will reset to run the internal LUT for global update
    	while( digitalRead( pin_cfg_epaper.panelBusy ) != std::stoi(HIGH) );
    }


    void EPD_Driver::reset(uint32_t ms1, uint32_t ms2, uint32_t ms3, uint32_t ms4, uint32_t ms5)
    {
    	// note: group delays into one array
    	TYME::delay(ms1);
        digitalWrite(pin_cfg_epaper.panelReset, HIGH); // RES# = 1
        TYME::delay(ms2);
        digitalWrite(pin_cfg_epaper.panelReset, LOW);
        TYME::delay(ms3);
        digitalWrite(pin_cfg_epaper.panelReset, HIGH);
        TYME::delay(ms4);
        digitalWrite(pin_cfg_epaper.panelCS, HIGH); // CS# = 1
        TYME::delay(ms5);
    }


    void EPD_Driver::DCDC_powerOn()
    {
    	sendIndexData( 0x04, &register_data[0], 1 );  //Power on
    	while( digitalRead( pin_cfg_epaper.panelBusy ) != std::stoi(HIGH) );
    }


    // EPD Screen refresh function
    //		- INPUT:
    //			- none but requires global variables on SPI pinout and config register data
    void EPD_Driver::displayRefresh()
    {
    	sendIndexData( 0x12, &register_data[0], 1 );	//Display Refresh
    	while( digitalRead( pin_cfg_epaper.panelBusy ) != std::stoi(HIGH) );
    }

    // Global Update function
    //		Implements global update functionality on either small/mid EPD
    //		- INPUT:
    //			- two image data (either BW and 0x00 or BW and BWR types)
    void EPD_Driver::globalUpdate(const uint8_t * data1s, const uint8_t * data2s)
    {
    	// send first frame
        sendIndexData(0x10, data1s, image_data_size); // First frame

    	// send second frame
    	sendIndexData(0x13, data2s, image_data_size); // Second frame	

    	DCDC_powerOn();
    	displayRefresh();
    	// digitalWrite(spi_basic.panelCS, HIGH);
    }


    // CoG shutdown function
    //		Shuts down the CoG and DC/DC circuit after all update functions
    //		- INPUT:
    //			- none but requires global variables on SPI pinout and config register data
    void EPD_Driver::COG_powerOff()
    {
    	sendIndexData( 0x02, &register_data[0], 0 );  //Turn off DC/DC    
    	//while ( digitalRead( spi_basic.panelBusy ) != std::stoi( HIGH)  ); 
        
       
        while ( digitalRead( pin_cfg_epaper.panelBusy ) != std::stoi(HIGH));
    	digitalWrite( pin_cfg_epaper.panelDC, LOW );
    	digitalWrite( pin_cfg_epaper.panelCS, LOW );
    	digitalWrite( pin_cfg_epaper.panelBusy, LOW );
    	TYME::delay( 150 );
    	digitalWrite( pin_cfg_epaper.panelReset, LOW );
    }

}