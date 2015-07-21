/*
 * C Library Includes
 */

#include <stdbool.h>
#include <stdint.h>

/*
 * Application Includes
 */

#include "msggetter.h"
#include "messaging.h"

bool setRTC(uint8_t yy, uint8_t mmm, uint8_t dd, uint8_t hh, uint8_t mm, uint8_t ss)
{
	(void)yy;
	(void)mmm;
	(void)dd;
	(void)hh;
	(void)mm;
	(void)ss;
    return true;
}

bool printRTC(void)
{
   
    return false;
}

int main(void)
{
    while(1)
    {
        if (updateMessages())
        {
            handleMessage(getNextMessage());
        }
    }
    return -1;
}