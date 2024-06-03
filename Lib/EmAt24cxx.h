#ifndef EMAT24CXX_H_
#define EMAT24CXX_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <i2c.h>
#include <gpio.h>
#include <stdbool.h>
#include <stdint.h>

typedef struct
{
	I2C_HandleTypeDef* hi2c;
	GPIO_TypeDef* 	   wpGpio;
	uint16_t 		   wpPin;
	uint8_t 		   sizeKBit;
	uint8_t  		   deviceAddress;

	void(*iwdgReset)(void);
}EmAt24cxx_t;

bool EmAt24cxx__isReady(const EmAt24cxx_t* self);
bool EmAt24cxx__write  (const EmAt24cxx_t* self, uint16_t address, const void *data, uint16_t len);
bool EmAt24cxx__read   (const EmAt24cxx_t* self, uint16_t address, void *data, uint16_t len);
bool EmAt24cxx__erase  (const EmAt24cxx_t* self, uint16_t address, uint8_t data, uint16_t len);

#ifdef __cplusplus
}
#endif

#endif /* EMAT24CXX_H_ */
