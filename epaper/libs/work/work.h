#pragma once


namespace  WORK{

    struct Work_t{
        //Gpio_t()=default;
        virtual ~Work_t() = default;
        virtual void init() = 0;
    };
    
}