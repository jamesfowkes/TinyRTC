#include <stdbool.h>

#include "messaging.h"

bool s_msgReceived = false;

bool setRTC(uint8_t yy, uint8_t mm, uint8_t dd, uint8_t hh, uint8_t mm, uint8_t ss)
{
    return true;
}

bool printRTC(void)
{
   
    return false;
}


void main(void)
{
    while(1)
    {
        if (s_msgReceived)
        {
            handleMessage(getNextMessage());
        }
    }
}