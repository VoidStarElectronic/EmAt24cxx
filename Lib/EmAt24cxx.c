/*
 * EmAt24cxx.c
 *
 *  Created on: Jun 1, 2023
 *      Author: Electronic Man
 */

#include <EmAt24cxx.h>
#include <stdlib.h>

static inline void EmAt24cxx__delay(uint32_t d)
{
	HAL_Delay(d);
}
static inline uint8_t EmAt24cxx__getPageSize(const EmAt24cxx_t* self)
{
	if(self->sizeKBit <= 2){
		return 8;
	}
	else if(self->sizeKBit <= 16){
		return 16;
	}
	else{
		return 32;
	}
}
static inline void EmAt24cxx__enableWP(const EmAt24cxx_t* self)
{
	if(self->wpGpio)
	{
		HAL_GPIO_WritePin(self->wpGpio, self->wpPin, GPIO_PIN_RESET);
	}
}
static inline void EmAt24cxx__disableWP(const EmAt24cxx_t* self)
{
	if(self->wpGpio)
	{
		HAL_GPIO_WritePin(self->wpGpio, self->wpPin, GPIO_PIN_SET);
	}
}
static inline HAL_StatusTypeDef EmAt24cxx__i2cWrite(const EmAt24cxx_t* self, uint16_t address, uint8_t *data, size_t len)
{
	uint16_t size = self->sizeKBit * 128;

	if(size <= 256)
	{
		return HAL_I2C_Mem_Write(self->hi2c, self->deviceAddress, address, I2C_MEMADD_SIZE_8BIT, data, len, 0xFF);
	}
	else if(size <= 512)
	{
		return HAL_I2C_Mem_Write(self->hi2c, self->deviceAddress | ((address & 0x0100) >> 7), (address & 0xff), I2C_MEMADD_SIZE_8BIT, data, len, 0xFF);
	}
	else if(size <= 1024)
	{
		return HAL_I2C_Mem_Write(self->hi2c, self->deviceAddress | ((address & 0x0300) >> 7), (address & 0xff), I2C_MEMADD_SIZE_8BIT, data, len, 0xFF);
	}
	else if(size <= 2048)
	{
		return HAL_I2C_Mem_Write(self->hi2c, self->deviceAddress | ((address & 0x0700) >> 7), (address & 0xff), I2C_MEMADD_SIZE_8BIT, data, len, 0xFF);
	}
	else
	{
		return HAL_I2C_Mem_Write(self->hi2c, self->deviceAddress, address, I2C_MEMADD_SIZE_16BIT, data, len, 0xFF);
	}
}
static inline HAL_StatusTypeDef EmAt24cxx__i2cRead(const EmAt24cxx_t* self, uint16_t address, uint8_t *data, size_t len)
{
	uint16_t size = self->sizeKBit * 128;

	if(size <= 256)
	{
		return HAL_I2C_Mem_Read(self->hi2c, self->deviceAddress, address, I2C_MEMADD_SIZE_8BIT, data, len, 0xFF);
	}
	else if(size <= 512)
	{
		return HAL_I2C_Mem_Read(self->hi2c, self->deviceAddress | ((address & 0x0100) >> 7), (address & 0xff), I2C_MEMADD_SIZE_8BIT, data, len, 0xFF);
	}
	else if(size <= 1024)
	{
		return HAL_I2C_Mem_Read(self->hi2c, self->deviceAddress | ((address & 0x0300) >> 7), (address & 0xff), I2C_MEMADD_SIZE_8BIT, data, len, 0xFF);
	}
	else if(size <= 2048)
	{
		return HAL_I2C_Mem_Read(self->hi2c, self->deviceAddress | ((address & 0x0700) >> 7), (address & 0xff), I2C_MEMADD_SIZE_8BIT, data, len, 0xFF);
	}
	else
	{
		return HAL_I2C_Mem_Read(self->hi2c, self->deviceAddress, address, I2C_MEMADD_SIZE_16BIT, data, len, 0xFF);
	}
}


/**
  * @brief  checks if device is ready for communication.
  *
  * @param  self  pointer to device configuration
  *
  * @retval bool status
  */
bool EmAt24cxx__isReady(const EmAt24cxx_t* self)
{
	assert(self);

	if (HAL_I2C_IsDeviceReady(self->hi2c, self->deviceAddress, 2, 0xFF) == HAL_OK)
	{
		return true;
	}
	else
	{
		return false;
	}
}

/**
  * @brief  write an amount of data
  *
  * @param  self     pointer to device configuration
  * @param  address  memory address
  * @param  data     pointer to data buffer
  * @param  len      amount of data to be write
  *
  * @retval bool status
  */
bool EmAt24cxx__write(const EmAt24cxx_t* self, uint16_t address, const void *data, uint16_t len)
{
	assert(self);
	assert(address < (self->sizeKBit*128));
	assert((len + address) < (self->sizeKBit*128));

	const uint8_t pageSize	= EmAt24cxx__getPageSize(self);
	uint8_t* dataPtr = (uint8_t*)data;
	uint16_t width;

	EmAt24cxx__disableWP(self);

	while(true)
	{
		if(self->iwdgReset)
		{
			self->iwdgReset();
		}

		width = pageSize - (address  % pageSize);

		if (width > len)
		{
			width = len;
		}

		if (EmAt24cxx__i2cWrite(self, address, dataPtr, width) == HAL_OK)
		{
			EmAt24cxx__delay(10);
			len -= width;
			dataPtr += width;
			address += width;
			if (len == 0)
			{
				EmAt24cxx__enableWP(self);
				return true;
			}
		}
		else
		{
			EmAt24cxx__enableWP(self);
			return false;
		}
	}
}

/**
  * @brief  erase memory with specific data.
  *
  * @param  self     	  pointer to device configuration
  * @param  startAddress  start memory address
  * @param  endAddress    end memory address
  * @param  data          erase memory with data
  *
  * @retval bool status
  */
bool EmAt24cxx__erase(const EmAt24cxx_t* self, uint16_t address, uint8_t data, uint16_t len)
{
	assert(self);
	assert(address < (self->sizeKBit*128));
	assert((len + address) < (self->sizeKBit*128));

	const uint16_t pageSize	= EmAt24cxx__getPageSize(self);
	uint8_t erasePageData[pageSize];
	uint16_t width;

	for(uint8_t i = 0 ; i < pageSize ; ++i)
	{
		erasePageData[i] = data;
	}

	EmAt24cxx__disableWP(self);

	while(true)
	{
		if(self->iwdgReset)
		{
			self->iwdgReset();
		}

		width = pageSize - (address  % pageSize);

		if (width > len)
		{
			width = len;
		}

		if (EmAt24cxx__i2cWrite(self, address, erasePageData, width) == HAL_OK)
		{
			EmAt24cxx__delay(10);
			len -= width;
			address += width;
			if (len == 0)
			{
				EmAt24cxx__enableWP(self);
				return true;
			}
		}
		else
		{
			EmAt24cxx__enableWP(self);
			return false;
		}
	}
}

/**
  * @brief  read an amount of data
  *
  * @param  self     pointer to device configuration
  * @param  address  memory address
  * @param  data     pointer to data buffer
  * @param  len      amount of data to be read
  *
  * @retval bool status
  */
bool EmAt24cxx__read(const EmAt24cxx_t* self, uint16_t address, void *data, uint16_t len)
{
	assert(self);
	assert(address < (self->sizeKBit*128));
	assert((len + address) < (self->sizeKBit*128));

	if(EmAt24cxx__i2cRead(self, address, (uint8_t*)data, len) == HAL_OK)
	{
		return true;
	}
	return false;
}



