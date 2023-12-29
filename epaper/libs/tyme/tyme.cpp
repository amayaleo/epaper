#include <chrono>
#include <thread>
#include <tyme/tyme.h>


namespace TYME{
    void delay(int x){
        std::this_thread::sleep_for(std::chrono::milliseconds(x));  
    }
    
}