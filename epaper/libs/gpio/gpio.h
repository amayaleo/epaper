#pragma once
#include <fstream>
#include <iostream>
#include <string_view>
//#include <work/work.h>

// namespace WORK{
// struct Work_t;
// }

namespace  GPIO{

    // typedef struct epaperSettingsPins
    // {
        // int BS {22};     //gpio22
        // int BUSY {27};   //gpio27
        // int RST_N {17};  //gpio17
        // int DC  {18}; //gpio18
        // int CSB {0} ; //gpio n/open
        // int SCL {3}; //gpio3 //pin 5
        // int SDA {2}; //gpio2 //pin 3
    // }configEpaperPin_t;
    

#define IN_INTERRUPT    23      //GPIO INTERRUPT 
#define OUT_INTERRUPT   12    //GPIO LED DBG
#define READING_STEPS   2     //10 originalmente

#define SYSFS_GPIO_PATH             "/sys/class/gpio"
#define SYSFS_GPIO_EXPORT_FN        "/export"
#define SYSFS_GPIO_UNEXPORT_FN      "/unexport"
#define SYSFS_GPIO_VALUE            "/value"
#define SYSFS_GPIO_DIRECTION        "/direction"
#define SYSFS_GPIO_EDGE             "/edge"

#define DIR_IN                      "in"
#define DIR_OUT                     "out"

#define VALUE_HIGH                  "1"
#define VALUE_LOW                   "0"

#define EDGE_RISING                 "rising"
#define EDGE_FALLING                "falling"

#define POLL_TIMEOUT        10*1000

#define DBG_GPIO_PRINT(x) std::cout<<"Step :"<<( x )<<"\n"


    struct Gpio_t //: public WORK::Work_t
    {
            Gpio_t()=default;
            // Gpio_t(bool& st)
            // : m_state   { st }{
                // std::cout <<"Gpio_t::Gpio_t()\n";  
            // }
            
            ~Gpio_t();            
        
            int         file_open_and_write_value(const std::string_view, const std::string_view);
            int         gpio_export(const int);
            int         gpio_unexport(const int);
            int         gpio_set_direction(const int , const std::string_view );
            int         gpio_set_value(const int , const std::string_view);
            int         gpio_set_edge(const int , const std::string_view);
            int         gpio_get_fd_to_value(const int );            
            bool        settings(const int , const std::string_view , std::ifstream& );
            void        CloseGpios(void);
            const bool  app(bool&) ;
            void        pinMode( uint8_t , std::string_view  );
            void        digitalWrite( uint8_t , std::string_view  );
             int        digitalRead(int);
        private :
            static inline int static_file_open_and_write_value{0};
            bool        m_state{false};
            int         m_gpio_in_fd{};
            int         m_res{};
            const unsigned int m_gpio_out{OUT_INTERRUPT};
            const int   m_gpio_in{IN_INTERRUPT};
            std::ifstream filenameGpio;
    };
    
}