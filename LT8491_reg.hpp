/**
 * @file LT8491_reg.hpp
 * @author Diogo Goto (diogo.goto@ucalgary.com), Lara Musa (), Justice Vermeylen ()
 * @brief File that contains addresses and useful values for the LT8491
 * @version 0.1
 * @date 2025-02-24
 * 
 * @copyright Copyright (c) 2025
 * 
*/


#ifndef LT8491_REG_HPP
#define LT8491_REG_HPP

#define LT8491_I2C_ADDR_0 0x29


// Telemetry 
//==============================================
// Data
#define TELE_TBAT_ADDR 0x00
#define TELE_POUT_ADDR 0x02
#define TELE_PIN_ADDR 0x04
#define TELE_EFF_ADDR 0x06
#define TELE_IOUT_ADDR 0x08
#define TELE_IIN_ADDR 0x0A
#define TELE_VBAT_ADDR 0xC
#define TELE_VIN_ADDR 0xE
#define TELE_VINR_ADDR 0x10

// Configurations 
#define CFG_RSENSE1_ADDR 0x28
#define CFG_RIMON_OUT_ADDR 0x2A
#define CFG_RSENSE2_ADDR 0x2C
#define CFG_RDACO_ADDR 0x2E
#define CFG_RFBOUT1_ADDR 0x30
#define CFG_RFBOUT2_ADDR 0x32
#define CFG_RDACI_ADDR 0x34
#define CFG_RFBIN2_ADDR 0x36
#define CFG_RFBIN1_ADDR 0x38

//==============================================

// Status
//==============================================
#define STAT_CHARGER_ADDR 0x12
    #define TELEM_ACTIVE 0x40
    #define CHARGING 0x02
    #define CHRG_LOGIC_ON 0x01
#define STAT_SYSTEM_ADDR 0x13
    #define SYSTEM_INITIALIZED 0x20
    #define SYSTEM_BUSY_IDLE 0x00
    #define SYSTEM_BUSY_CRC 0x01
    #define SYSTEM_BUSY_EEPROM 0x02

// Faults
#define STAT_CHRG_FAULTS_ADDR 0x19
//==============================================

// General Configs
//==============================================
#define CTRL_WRT_TO_BOO_ADDR 0x20
#define CTRL_EE_WRT_EN_ADDR 0x21
    #define EEPROM_WRITE_EN 0xCC
    #define EEPROM_WRITE_DIS 0x00
    #define EEPROM_FACTORY_RESET 0x57
#define CTRL_CHRG_EN_ADDR 0x23
    #define CHRG_EN 0x1
    #define CHRG_DIS 0x0
#define CTRL_UPDATE_TELEM_ADDR 0x26
    #define FORCE_TELEM_UPDATE 0xAA 
//==============================================

// Other Configs
//==============================================
// Charging stages transition thresholds registers
#define CFG_VS3_25C_ADDR 0x3B
#define CFG_UV_S0_ADDR 0x3C
#define CFG_S0_UV_ADDR 0x3D
#define CFG_S0_S1_ADDR 0x3E
#define CFG_S1_S0_ADDR 0x3F

// Auto restart registers
//==============================================
#endif //LT8491_REG_HPP