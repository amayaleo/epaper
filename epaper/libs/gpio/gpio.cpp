extern "C"{
    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    #include <errno.h>
    #include <unistd.h>
    #include <fcntl.h>
    #include <poll.h>
}

#include <chrono>
#include <thread>
#include <string_view>
#include <string>

#include <gpio/gpio.h>


namespace GPIO{


    int Gpio_t::file_open_and_write_value(const std::string_view fname, const std::string_view wdata)
    {
        //@params : verifica que exista el pin . Si no existe ,retorna -1 .
        const int fd = open(fname.data(), O_WRONLY | O_NONBLOCK);        
        if (fd < 0){
            printf("Could not open file %s...%d\r\n", fname.data(), fd);
            return -1;
       }
        write(fd, wdata.data(), strlen(wdata.data()));        
        close(fd);
        return 0;
    }


    int Gpio_t::gpio_export(const int gpio_num)
    {
        //char gpio_str[4];
       // sprintf(gpio_str, "%d", gpio_num);
        return file_open_and_write_value(SYSFS_GPIO_PATH SYSFS_GPIO_EXPORT_FN,std::to_string(gpio_num) /*gpio_str*/);
    }


    // GPIO UNEXPORT
    int Gpio_t::gpio_unexport(const int gpio_num)
    {        
        return file_open_and_write_value(SYSFS_GPIO_PATH SYSFS_GPIO_UNEXPORT_FN, std::to_string(gpio_num) /*gpio_str*/);
    }

    int Gpio_t::gpio_set_direction(const int gpio_num, const std::string_view dir)
    {                                
        const std::string path_str ="/sys/class/gpio/gpio" + std::to_string(gpio_num) + "/direction";
        return file_open_and_write_value(path_str.c_str(), dir.data());
    }

    int Gpio_t::gpio_set_value(const int gpio_num, const std::string_view value)
    {
        std::string path_str = "/sys/class/gpio/gpio" + std::to_string(gpio_num) + "/value";                
        return file_open_and_write_value(path_str.c_str(), value.data());
    }

    int Gpio_t::gpio_set_edge(const int gpio_num, const std::string_view edge)
    {                        
        std::string path_str = "/sys/class/gpio/gpio" + std::to_string(gpio_num) + "/edge";        
        return file_open_and_write_value(path_str.c_str(), edge.data());
    }

    int Gpio_t::gpio_get_fd_to_value(const int gpio_num)
    {
        const std::string fname="/sys/class/gpio/gpio"+ std::to_string(gpio_num) + "/value";
        const int fd = open(fname.data(), O_RDONLY | O_NONBLOCK);        
        if (fd < 0)std::cerr<<"Could not open file "<< fname.data()<<"..."<< std::to_string(fd)<<"\r\n" ;
        
        return fd;
    }


    bool Gpio_t::settings(const int pin , const std::string_view str_v ,std::ifstream& fileTmp){
            const std::string filePathGpio = "/sys/class/gpio/gpio" + std::to_string(pin) + "/direction";                                
            const std::string fNameResult ="echo " + std::to_string(pin) + " > /sys/class/gpio/export";               
            fileTmp.open(filePathGpio.c_str());        
        if(!fileTmp.is_open()){
                const int result_output = std::system(fNameResult.c_str());
                if (result_output == 0) {                
                        std::cout << "Pin GPIO "+ std::to_string(pin) +" exported successfully." << "\n";                
                } else {                
                        std::cerr << "Error unexporting GPIO "+ std::to_string(pin) +"." <<"\n";                
                    return false;
                }
            }  
            fileTmp.close();  
            gpio_unexport(pin);        
            gpio_export(pin);             
            gpio_set_direction(pin,str_v.data());
        return true;
    }


    const bool Gpio_t::app(bool& flag) 
    {                
        struct pollfd fdpoll;
        int m_num_fdpoll { 1 };        
        int m_looper { 0 };
        char *buf[64];

        settings( m_gpio_in  , DIR_IN  ,filenameGpio);
        settings( m_gpio_out , DIR_OUT ,filenameGpio);
        
        gpio_set_edge (m_gpio_in,EDGE_FALLING);
        gpio_set_value(m_gpio_out,VALUE_HIGH);

          
        m_gpio_in_fd = gpio_get_fd_to_value(m_gpio_in);        
        
        if(m_state==true)
        {
        while(m_looper<READING_STEPS) {
            memset((void *)&fdpoll,0,sizeof(fdpoll));
            fdpoll.fd = m_gpio_in_fd;
            fdpoll.events = POLLPRI;
            m_res = poll(&fdpoll,m_num_fdpoll,POLL_TIMEOUT);

            if(m_res < 0) {
                #ifdef DBG_GPIO
                printf("Poll failed...%d\r\n",res);   
                #endif         
                }
            if(m_res == 0) {
                #ifdef DBG_GPIO
                    std::cout<<"\nPoll success...timed out or received button press...\r\n";
                #endif
                }
            if(fdpoll.revents & POLLPRI) {
                lseek(fdpoll.fd, 0, SEEK_SET);
                read(fdpoll.fd, buf, 64);
                #ifdef DBG_GPIO
                    std::cout<<"Standby reading msj mrf24j40...\n";
                #endif
                }
            ++m_looper;
            fflush(stdout);
            }           
        }
        else{            
            gpio_set_value(m_gpio_out,VALUE_HIGH);
            std::this_thread::sleep_for(std::chrono::milliseconds(10));            
        }    
        gpio_set_value(m_gpio_out,VALUE_LOW);           
        return false;
    }

    void Gpio_t::CloseGpios()
    {
        if(filenameGpio.is_open())filenameGpio.close();   
        close(m_gpio_in_fd);        
        gpio_set_value(m_gpio_out,VALUE_LOW);
     
        gpio_unexport(m_gpio_out);
        gpio_unexport(m_gpio_in);
        //if(fileGpioOutput.is_open())fileGpioOutput.close();
    }


    Gpio_t::~Gpio_t(){            
            CloseGpios();                                
            std::cout<<"~Gpio()\n";
        
    }




	void Gpio_t::pinMode( uint8_t pin, std::string_view st ){
        //spi_basic.panelDC=0;
        gpio_set_direction(pin , st.data());
    }
	void Gpio_t::digitalWrite(uint8_t pin, std::string_view  st){
        //spi_basic.panelDC=0;
        gpio_set_value(pin , st.data());
    }

    int Gpio_t::digitalRead(int x){    
        return gpio_get_fd_to_value(x);
    }
}