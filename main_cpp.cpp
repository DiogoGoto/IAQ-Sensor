/*
 * main_cpp.cpp
 *
 *  Created on: Nov 20, 2024
 *      Author: diogo
 */

#include "main_cpp.hpp"
#include "runInterface.hpp"
#include "LT8491.hpp"

TELEM_CONFIG telem_configs;
TELEM sensor_data;

void sendSerialUSB(TELEM sensor_data);


void run_main(){
	// void Setup(void){
	//=============================================================
	telem_configs.RSENSE1	 = 200;
	telem_configs.RIMON_OUT	 = 0000; // TBD
	telem_configs.RSENSE2	 = 760;
	telem_configs.RDACO		 = 13500;
	telem_configs.RFBOUT1 	 = 4870;
	telem_configs.RFBOUT2	 = 2000;
	telem_configs.RDACI	 	 = 622;
	telem_configs.RFBIN2 	 = 188;
	telem_configs.RFBIN1 	 = 984;

	I2C_Wrapper i2c_wrapper(&hi2c1);
	LT8491_DRIVER lt8491_driver(telem_configs, i2c_wrapper);

	while(!lt8491_driver.get_isInitialized()){
		// if it fails to initializes CRY
		// Implement handles to factory reset and rewrite EEPROM
		HAL_Delay(100);
		LT8491_DRIVER lt8491_driver(telem_configs, i2c_wrapper);
	}

    //=============================================================
    //}
	while(1){
		// void loop(void){
		//=========================================================


		if(!lt8491_driver.get_telemetry(&sensor_data)){
			sendSerialUSB(sensor_data);
		}

		HAL_Delay(100);
		//=========================================================
		//}
	}


}


void sendSerialUSB(TELEM sensor_data) {
	char message[128];
    snprintf(message, sizeof(message), "TBAT:%.4f,POUT:%.4f,PIN:%.4f,EFF:%.4f,IOUT:%.4f,IIN:%.4f,VBAT:%.4f,VIN:%.4f,VINR:%.4f \n",
    		sensor_data.TBAT, sensor_data.POUT, sensor_data.PIN,
            sensor_data.EFF, sensor_data.IOUT, sensor_data.IIN,
            sensor_data.VBAT, sensor_data.VIN, sensor_data.VINR);

    HAL_UART_Transmit(&huart3, (uint8_t *)message, strlen(message), HAL_MAX_DELAY);
}
