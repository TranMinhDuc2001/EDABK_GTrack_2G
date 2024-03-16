#include <stdint.h>
#include "BMA253.h"
#include "stm32f0xx_hal.h"
#include "stm32f0xx_hal_i2c.h"
#include "main.h"


void configureNoMotionInterrupt(I2C_HandleTypeDef *hi2c) {
    uint8_t data;

    // Select no-motion mode
	// Gia tri da cau hinh pham vi gia toc 2g
    data = 0x03;
    HAL_I2C_Mem_Write(hi2c, BMA253_ADDRESS, 0x0F, I2C_MEMADD_SIZE_8BIT, &data, 1, HAL_MAX_DELAY);

    // Set 1kHz bandwidth
    data = 0x0F;
    HAL_I2C_Mem_Write(hi2c, BMA253_ADDRESS, 0x10, I2C_MEMADD_SIZE_8BIT, &data, 1, HAL_MAX_DELAY);

    // Set normal mode
    data = 0x56;
    HAL_I2C_Mem_Write(hi2c, BMA253_ADDRESS, 0x11, I2C_MEMADD_SIZE_8BIT, &data, 1, HAL_MAX_DELAY);


	 data = 0x01;// ngắt nhảy từ 0 lên 1
    HAL_I2C_Mem_Write(hi2c, BMA253_ADDRESS, 0x20, I2C_MEMADD_SIZE_8BIT, &data, 1, HAL_MAX_DELAY);

    // Set no-motion threshold
    //data = 0x10; // Example threshold, needs adjustment based on sensitivity required
		data = 0x19;// 70 in dec-4B  50dec-32
    HAL_I2C_Mem_Write(hi2c, BMA253_ADDRESS, 0x29, I2C_MEMADD_SIZE_8BIT, &data, 1, HAL_MAX_DELAY);

    // Set no-motion duration
    //data = 0x0A; // Example duration, adjust based on the required stationary time
		data = 0xFF;
    HAL_I2C_Mem_Write(hi2c, BMA253_ADDRESS, 0x27, I2C_MEMADD_SIZE_8BIT, &data, 1, HAL_MAX_DELAY);
		data = 0x08; // Map no-motion interrupt to INT1
		HAL_I2C_Mem_Write(hi2c, BMA253_ADDRESS, 0x19, I2C_MEMADD_SIZE_8BIT, &data, 1, HAL_MAX_DELAY);
    // Enable no-motion interrupt
	 // Enable axes for no-motion detection
    data = 0x07; // Enable x, y, and z axes
    HAL_I2C_Mem_Write(hi2c, BMA253_ADDRESS, 0x18, I2C_MEMADD_SIZE_8BIT, &data, 1, HAL_MAX_DELAY);


}

