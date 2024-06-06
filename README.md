# EmAt24cxx
 Stm32 At24Cxx eeprom driver

Example 1 :
* simple

```
#include "main.h"
#include "gpio.h"
#include "i2c.h"

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

    while(1);
}
```

Example 2 :
* watchdog enabled

```
#include "main.h"
#include "gpio.h"
#include "i2c.h"
#include "iwdg.h"

#include "EmAt24cxx.h"

void IWDG__reset(void)
{
    //TODO : reset watchdog
    HAL_IWDG_Refresh(&hiwdg);
}

static const EmAt24cxx_t at24c32 =
{
    .hi2c = &hi2c1,
    .sizeKBit = 32,
    .deviceAddress = 0xA0,
    .iwdgReset = IWDG__reset,
};

int main(void)
{
    HAL_Init();
    SystemClock_Config();
    MX_GPIO_Init();
    MX_I2C1_Init();
    MX_IWDG_Init();


    const char wbuf[] = "this is Electronic Man";
    char rbuf[sizeof(wbuf)] = {0};

    if(EmAt24cxx__isReady(&at24c32))
    {
        EmAt24cxx__write(&at24c32, 100, wbuf, sizeof(wbuf));
        EmAt24cxx__read(&at24c32, 100, rbuf, sizeof(rbuf));
    }

    while(1)
    {
        IWDG__reset();
    }
}
```

Example 3 :
* watchdog enabled
* wp pin used

```
#include "main.h"
#include "gpio.h"
#include "i2c.h"
#include "iwdg.h"

#include "EmAt24cxx.h"

void IWDG__reset(void)
{
    //TODO : reset watchdog
    HAL_IWDG_Refresh(&hiwdg);
}

static const EmAt24cxx_t at24c32 =
{
    .hi2c = &hi2c1,
    .sizeKBit = 32,
    .deviceAddress = 0xA0,
    .iwdgReset = IWDG__reset,
    .wpGpio = WP_GPIO_Port,
    .wpPin = WP_Pin,
};

int main(void)
{
    HAL_Init();
    SystemClock_Config();
    MX_GPIO_Init();
    MX_I2C1_Init();
    MX_IWDG_Init();


    const char wbuf[] = "this is Electronic Man";
    char rbuf[sizeof(wbuf)] = {0};

    if(EmAt24cxx__isReady(&at24c32))
    {
        EmAt24cxx__write(&at24c32, 100, wbuf, sizeof(wbuf));
        EmAt24cxx__read(&at24c32, 100, rbuf, sizeof(rbuf));
    }

    while(1)
    {
        IWDG__reset();
    }
}
```

Example 4 :
* use 3 eeprom devices

```
#include "main.h"
#include "gpio.h"
#include "i2c.h"

#include "EmAt24cxx.h"

static const EmAt24cxx_t at24c02 =
{
    .hi2c = &hi2c2,
    .sizeKBit = 2,
    .deviceAddress = 0xA0,
};
static const EmAt24cxx_t at24c32 =
{
    .hi2c = &hi2c1,
    .sizeKBit = 32,
    .deviceAddress = 0xA1,
};
static const EmAt24cxx_t at24c64 =
{
    .hi2c = &hi2c1,
    .sizeKBit = 64,
    .deviceAddress = 0xA2,
    .wpGpio = WP_GPIO_Port,
    .wpPin = WP_Pin,
};

int main(void)
{
    HAL_Init();
    SystemClock_Config();
    MX_GPIO_Init();
    MX_I2C1_Init();
    MX_I2C2_Init();
    
    const char wbuf[] = "this is Electronic Man";
    char rbuf[sizeof(wbuf)] = {0};

    if(EmAt24cxx__isReady(&at24c02))
    {
        EmAt24cxx__write(&at24c02, 100, wbuf, sizeof(wbuf));
        EmAt24cxx__read(&at24c02, 100, rbuf, sizeof(rbuf));
    }
   
    if(EmAt24cxx__isReady(&at24c32))
    {
        EmAt24cxx__write(&at24c32, 100, wbuf, sizeof(wbuf));
        EmAt24cxx__read(&at24c32, 100, rbuf, sizeof(rbuf));
    }
   
    if(EmAt24cxx__isReady(&at24c64))
    {
        EmAt24cxx__write(&at24c64, 100, wbuf, sizeof(wbuf));
        EmAt24cxx__read(&at24c64, 100, rbuf, sizeof(rbuf));
    }

    while(1)
    {

    }
}
```