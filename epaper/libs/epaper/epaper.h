#pragma once
#include <iostream>
#include <work/work.h>
#include <gpio/gpio.h>
#include <spi/spi.h>



namespace  EPAPER{

//struct SPI::Spi_t;

#define INPUT DIR_IN
#define OUTPUT DIR_OUT
#define HIGH "1"
#define LOW "0"
#define MSBFIRST 0
#define SPI_MODE0 0
#define ENERGIA
#define SPI_CLOCK_DIV32 (uint16_t)8000000
#define NOT_CONNECTED (uint8_t)0xff

/// * Monochrome screens and default colour screens
    #define eScreen_EPD_154 (uint32_t)0x1509 ///< reference xE2154CSxxx
#define eScreen_EPD_213 (uint32_t)0x2100 ///< reference xE2213CSxxx//SELECT
    #define eScreen_EPD_266 (uint32_t)0x2600 ///< reference xE2266CSxxx
    #define eScreen_EPD_271 (uint32_t)0x2700 ///< reference xE2271CSxxx
    #define eScreen_EPD_287 (uint32_t)0x2800 ///< reference xE2287CSxxx
    #define eScreen_EPD_370 (uint32_t)0x3700 ///< reference xE2370CSxxx
    #define eScreen_EPD_417 (uint32_t)0x4100 ///< reference xE2417CSxxx
    #define eScreen_EPD_437 (uint32_t)0x430C ///< reference xE2437CSxxx


struct pins_t
{
    // uint8_t panelSCL;
    ///< EXT3 pin 1 Black -> +3.3V
    ///< EXT3 pin 2 Brown -> SPI SCK
    uint8_t panelBusy; ///< EXT3 pin 3 Red
    uint8_t panelDC; ///< EXT3 pin 4 Orange
    uint8_t panelReset; ///< EXT3 pin 5 Yellow
    ///< EXT3 pin 6 Green -> SPI MISO
    ///< EXT3 pin 7 Blue -> SPI MOSI
    // uint8_t panelSDA;
    uint8_t panelCS;
    uint8_t panelON_EXT2;
    uint8_t panelSPI43_EXT2; ///< BS
    uint8_t flashCS;
};

typedef struct spiSettings
{
  int vel ;//8000000;
  int m_MSBFIRST;
  int m_SPI_MODE0;
}
SPISettings;


struct Spi
{
    Spi()=default;
    ~Spi()=default;

    void transfer(uint8_t index){}
    void begin(){}
    void beginTransaction(SPISettings& settingScreen){}

    void setDataMode(int l_SPI_MODE0){}
    void setClockDivider(int l_SPI_CLOCK_DIV32){}
    //void setClockDivider(16000000 / min(16000000, 4000000));
    void setBitOrder(int l_MSBFIRST){}
};


struct EPD_Driver: public GPIO::Gpio_t //,  SPI::Spi_t
{
    private:
	    // Data structures
        //SPI spi_basic = {0};
	    pins_t pin_cfg_epaper { 0 };
	
  public:
	EPD_Driver(uint32_t eScreen_EPD, pins_t board);
    
	// CoG Functions

	///
  /// @brief COG initialization
  /// @note Implements Tcon (COG) power-on and temperature input to COG
  ///
  void COG_initial();

	///
  /// @brief COG shutdown
  /// @note Turns-off DC/DC circuit
  ///
  void COG_powerOff();

	///
  /// @brief Global update function
  /// @param data1s first image data frame
  /// @param data2s second image data frame
  /// @note Automatically switches between small and medium-sized displays
  ///
	void globalUpdate(const uint8_t *data1s, const uint8_t *data2s);

  protected:

	// COG Utilities

  ///
  /// @brief SPI transfer protocol for index and data transmit to COG
  /// @param index target register address
  /// @param data pointer to image or data array
  /// @param len length or size of data array
  ///
	void sendIndexData( uint8_t index, const uint8_t *data, uint32_t len ); 

  ///
  /// @brief Soft-reset of CoG driver
  /// @note 
  ///
	void softReset();

  ///
  /// @brief EPD screen refresh command
  /// @note
  ///
	void displayRefresh();

  ///
  /// @brief CoG driver power-on hard reset
  /// @param ms1 up to ms5; millisecond delays
  /// @note Implemented after VCC/VDD Turn-on
  ///
	void reset(uint32_t ms1, uint32_t ms2, uint32_t ms3, uint32_t ms4, uint32_t ms5);
	
  ///
  /// @brief DC-DC power-on command
  /// @note Implemented after image data are uploaded to CoG
  /// @note Specific to small-sized EPDs only
  ///
  void DCDC_powerOn();
	
	// Internal global variables
	const char* pdi_brd;
	uint16_t pdi_size;
	uint16_t pdi_cp;
	uint32_t image_data_size;
	uint8_t register_data[6] = {};

  private:
  Spi spi;
  std::unique_ptr<SPI::Spi_t>spi_ptr{};
};




const pins_t boardRaspberryPi_EXT =
{
    .panelBusy = 27, 
    .panelDC = 18, 
    .panelReset = 17,
    .panelCS = 4,
    .panelON_EXT2 = NOT_CONNECTED,
    .panelSPI43_EXT2 = NOT_CONNECTED,
    .flashCS = 0
};
    
}