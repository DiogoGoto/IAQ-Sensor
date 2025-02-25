/**
 * @file LT8491.cpp
 * @author Diogo Goto (diogo.goto@ucalgary.com), Lara Musa (), Justice Vermeylen ()
 * @brief File containing the implementation of the LT8491 driver
 * @version 0.1
 * @date 2025-02-24
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#include "LT8491.hpp"


LT8491_DRIVER::LT8491_DRIVER(TELEM_CONFIG telem_configs, I2C_Wrapper& i2c_pointer)
:telem_configs(telem_configs), i2c(i2c_pointer){
    isInitialized = true;

    //Check if booted
    uint8_t data_buffer = 0;
    i2c.i2c_read(STAT_SYSTEM_ADDR, &data_buffer, 1);
    if(data_buffer && SYSTEM_INITIALIZED) {
        isInitialized = false;
        return;
    }

#ifdef RUN_CONFIG //FIXME: implement this once we move to the MPPT board

    en_cfg_write(); //Enable Config Write
    //Write Configs
    if(i2c.i2c_write( CFG_RSENSE1_ADDR, telem_configs.RSENSE1, 2)) {isInitialized = false;}
    if(i2c.i2c_write( CFG_RIMON_OUT_ADDR, telem_configs.RIMON_OUT, 2)) {isInitialized = false;}
    if(i2c.i2c_write( CFG_RSENSE2_ADDR, telem_configs.RSENSE2, 2)) {isInitialized = false;}
    if(i2c.i2c_write( CFG_RDACO_ADDR, telem_configs.RDACO, 2)) {isInitialized = false;}
    if(i2c.i2c_write( CFG_RFBOUT1_ADDR, telem_configs.RFBOUT1, 2)) {isInitialized = false;}
    if(i2c.i2c_write( CFG_RFBOUT2_ADDR, telem_configs.RFBOUT2, 2)) {isInitialized = false;}
    if(i2c.i2c_write( CFG_RDACI_ADDR, telem_configs.RDACI, 2)) {isInitialized = false;}
    if(i2c.i2c_write( CFG_RFBIN2_ADDR, telem_configs.RFBIN2, 2)) {isInitialized = false;}
    if(i2c.i2c_write( CFG_RFBIN1_ADDR, telem_configs.RFBIN1, 2)) {isInitialized = false;}
    
    // more configs ...

    dis_cfg_write(); //Disable Config Write
#endif
}

uint8_t LT8491_DRIVER::get_telemetry(TELEM* data){
    uint8_t status = 0;
    uint8_t data_buffer[18];
    status = i2c.i2c_read( STAT_CHARGER_ADDR, data_buffer, 1); //FIXME change to the correct function once the I2C driver is implemented
    if(!(data_buffer[0] && TELEM_ACTIVE)){
    if(!check_control_write()) {en_cfg_write();} 
        i2c.i2c_write( CTRL_UPDATE_TELEM_ADDR, FORCE_TELEM_UPDATE, 1); //FIXME change to the correct function once the I2C driver is implemented
        HAL_Delay(100);
    }

        
        status = i2c.i2c_read( TELE_TBAT_ADDR, data_buffer, 18); // FIXME: change to the correct function once the I2C driver is implemented
                                                                               // Double check the LT8491 does auto increment the address when reading
        //Read Data
        uint16_t holder;
        holder = (data_buffer[0] << 8) | data_buffer[1];
        data->TBAT = (float) holder;
        holder = (data_buffer[2] << 8) | data_buffer[3];
        data->POUT = (float) holder;
        holder = (data_buffer[4] << 8) | data_buffer[5];
        data->PIN = (float) holder;
        holder = (data_buffer[6] << 8) | data_buffer[7];
        data->EFF = (float) holder;
        holder = (data_buffer[8] << 8) | data_buffer[9];
        data->IOUT = (float) holder;
        holder = (data_buffer[10] << 8) | data_buffer[11];
        data->IIN = (float) holder;
        holder = (data_buffer[12] << 8) | data_buffer[13];
        data->VBAT = (float) holder;
        holder = (data_buffer[14] << 8) | data_buffer[15];
        data->VIN = (float) holder;
        holder = (data_buffer[16] << 8) | data_buffer[17];
        data->VINR = (float) holder;

        //Process Data
        data->TBAT = data->TBAT / 10;
        data->POUT = data->POUT / 100;
        data->PIN = data->PIN / 100;
        data->EFF = data->EFF / 100;
        data->IOUT = data->IOUT / 1000;
        data->IIN = data->IIN / 1000;
        data->VBAT = data->VBAT / 100;
        data->VIN = data->VIN / 100;
        data->VINR = data->VINR / 100;

    return status;
}

bool LT8491_DRIVER::get_charge_logic(void){
    uint8_t data_buffer;
    i2c.i2c_read( STAT_CHARGER_ADDR, &data_buffer, 1);
    return data_buffer & CHRG_LOGIC_ON;
}

bool LT8491_DRIVER::check_control_write(void){
    //STAT_CHARGER→CHRG_LOGIC_ON=0,
    //CTRL_EE_WRT_EN≠0xCC
    //STAT_SYSTEM→SYSTEM_BUSY=00b
    uint8_t data_buffer;
    i2c.i2c_read( CTRL_EE_WRT_EN_ADDR, &data_buffer, 1);
    bool ee_wrt_en = data_buffer == EEPROM_WRITE_EN;
    i2c.i2c_read( STAT_SYSTEM_ADDR, &data_buffer, 1);
    bool system_idle = data_buffer == SYSTEM_BUSY_IDLE;

    bool charger_on = get_charge_logic();

    return !charger_on && !ee_wrt_en && system_idle;
}

// FIXME: Double check if fuction is programmed correctly
uint8_t LT8491_DRIVER::en_cfg_write(void){
    uint8_t status = 0;
    uint8_t data_buffer;
    i2c.i2c_read( CTRL_EE_WRT_EN_ADDR, &data_buffer, 1);
    if(data_buffer != EEPROM_WRITE_EN){
        i2c.i2c_write( CTRL_EE_WRT_EN_ADDR, EEPROM_WRITE_EN, 1);
        HAL_Delay(100);
        i2c.i2c_read( CTRL_EE_WRT_EN_ADDR, &data_buffer, 1);
        if(data_buffer != EEPROM_WRITE_EN){
            status = 1;
        }
    }
    return status;
}
// FIXME: Double check if fuction is programmed correctly
uint8_t LT8491_DRIVER::dis_cfg_write(void){
    uint8_t status = 0;
    uint8_t data_buffer;
    i2c.i2c_read( CTRL_EE_WRT_EN_ADDR, &data_buffer, 1);
    if(data_buffer != EEPROM_WRITE_DIS){
        i2c.i2c_write( CTRL_EE_WRT_EN_ADDR, EEPROM_WRITE_DIS, 1);
        HAL_Delay(100);
        i2c.i2c_read( CTRL_EE_WRT_EN_ADDR, &data_buffer, 1);
        if(data_buffer != EEPROM_WRITE_DIS){
            status = 1;
        }
    }
    return status;
}

bool LT8491_DRIVER::get_isInitialized(void){
    return isInitialized;
}
