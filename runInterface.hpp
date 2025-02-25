/*
 * runInterface.hpp
 *
 *  Created on: Nov 20, 2024
 *      Author: diogo
 */



#ifndef INC_RUNINTERFACE_HPP_
#define INC_RUNINTERFACE_HPP_

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f4xx_hal.h" // Ensure HAL library compatibility
#include <stdio.h>
#include <string.h>

extern I2C_HandleTypeDef hi2c1;
extern UART_HandleTypeDef huart3;

/**
 * @brief Interface to run the C++ codebase in the C compiler
 */
void run_interface();

#ifdef __cplusplus
}
#endif

#endif /* INC_RUNINTERFACE_HPP_ */
