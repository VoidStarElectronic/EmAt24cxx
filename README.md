# EmAt24cxx
 Stm32 At24Cxx eeprom driver

Example 1 :
#include "main.h"
#include "EmAt24cxx.h"

static const EmAt24cxx_t at24c32 =
{
	.hi2c = &hi2c1,
	.sizeKBit = 32,
	.deviceAddress = 0xA0,
};

int main(void)
{
    HAL_Init();
    SystemClock_Config();
    MX_GPIO_Init();
    MX_I2C1_Init();
	
    const char wbuf[] = "this is Electronic Man";
    char rbuf[sizeof(wbuf)] = {0};

    if(EmAt24cxx__isReady(&at24c32))
    {
        EmAt24cxx__write(&at24c32, 100, wbuf, sizeof(wbuf));
        EmAt24cxx__read(&at24c32, 100, rbuf, sizeof(rbuf));
    }

    while(1)
    {
        
    }
}
