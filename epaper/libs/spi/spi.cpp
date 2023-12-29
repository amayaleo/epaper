#include <spi/spi.h>
//#include <app/config.h>

#define SPI_DEVICE  "/dev/spidev0.0"

#define CMD_WRITE 0x2
#define CMD_READ 0x3

#define READ 0b011 //Read data from memory array beginning at selected address
#define WRITE 0b010 //Write data to memory array beginning at selected address
#define WRDI 0b100//Reset the write enable latch (disable write operations)
#define WREN 0b110//Set the write enable latch (enable write operations)
#define RDSR 0b101//Read STATUS
#define WRSR 0b001//Write STATUS register

namespace SPI {
  void Spi_t::msj_fail(void){
        #ifdef DBG
          printf("Could not open the Spi device...\r\n");
      #endif
      return;
  }


  void Spi_t::settings_spi(){
      spi->tx_buf = (unsigned long)m_tx_buffer;
      spi->rx_buf = (unsigned long)m_rx_buffer;
      spi->bits_per_word = 0;
      spi->speed_hz = m_spi_speed;
      spi->delay_usecs = 1;
      spi->len = 3;          
          
        m_tx_buffer[0] = 0x00;
        m_tx_buffer[1] = 0x00;
        m_tx_buffer[2] = 0x00;
        m_tx_buffer[3] = 0x00;
        m_rx_buffer[0] = 0xFF;
        m_rx_buffer[1] = 0xFF;
        m_rx_buffer[2] = 0xFF;
        m_rx_buffer[3] = 0xff;
    return;
  }


  void Spi_t::init(){
  fs = open(SPI_DEVICE, O_RDWR);
  if(fs < 0) {
      msj_fail();
      exit(EXIT_FAILURE);
    }
    ret = ioctl(fs, SPI_IOC_RD_MODE, &scratch32);
  if(ret != 0) {
        msj_fail();
        if(fs)close(fs);
        exit(EXIT_FAILURE);
    }
    scratch32 |= SPI_MODE_0;
    ret = ioctl(fs, SPI_IOC_WR_MODE, &scratch32);   //SPI_IOC_WR_MODE32
  if(ret != 0) {
      msj_fail();
      close(fs);
      exit(EXIT_FAILURE);
    }
    ret = ioctl(fs, SPI_IOC_RD_MAX_SPEED_HZ, &scratch32);
  if(ret != 0) {
      close(fs);
      exit(EXIT_FAILURE);
    }
      scratch32 = m_spi_speed;
      ret = ioctl(fs, SPI_IOC_WR_MAX_SPEED_HZ, &scratch32);

      if(ret != 0) {
          msj_fail();
          close(fs);
          exit(EXIT_FAILURE);
      }
      return;
  }


const uint8_t Spi_t::Transfer2bytes(const uint16_t cmd){
    spi->len = sizeof(cmd);
    m_rx_buffer[0]=m_rx_buffer[1]=0xff;
    m_rx_buffer[2]=m_rx_buffer[3]=0x00;
    memcpy(m_tx_buffer, &cmd, sizeof(cmd));
    ret = ioctl(fs, SPI_IOC_MESSAGE(1), spi.get());
    if((cmd>>8&0xff)==0x00){}
      //if(ret != 0) return rx_buffer[1];  
  return m_rx_buffer[1];
  }


  const uint8_t Spi_t::Transfer3bytes(const uint32_t cmd){
    spi->len = 3;
    m_rx_buffer[0]=m_rx_buffer[1]=m_rx_buffer[2]==0xff;
    m_rx_buffer[3]=0x00;
    memcpy(m_tx_buffer, &cmd, sizeof(cmd));
    ret = ioctl(fs, SPI_IOC_MESSAGE(1), spi.get());
      if((cmd>>16&0xff)==0x00) {}
        
        //if(ret != 0) return rx_buffer[2];       
    return m_rx_buffer[2];
    }


    void Spi_t::spi_close(){
        if(fs)close(fs);
      return;
    }




    Spi_t::Spi_t()
    : m_spi_speed ( SPI_SPEED )
    , m_tx_buffer  { 0x00 }
    , m_rx_buffer  { 0x00 }
    , spi       { std::make_unique<struct spi_ioc_transfer >() } 
    {
          #ifdef DBG
              std::cout<<"Spi()\n";
          #endif
          settings_spi();   
          init(); 
        return;
    }

      Spi_t::~Spi_t(){
      spi_close();
      #ifdef DBG
          std::cout<<"~Spi()\n";
      #endif
      exit(EXIT_SUCCESS);
    }

}//end namespace SPI_H
