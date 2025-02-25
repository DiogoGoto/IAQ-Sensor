/**
 * @file LT8491.hpp
 * @author Diogo Goto (diogo.goto@ucalgary.com), Lara Musa (), Justice Vermeylen ()
 * @brief File containing the class for the LT8491 driver and other useful structures
 * @version 0.1
 * @date 2025-02-24
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#ifndef LT8491_HPP
#define LT8491_HPP

#include "LT8491_reg.hpp"
#include "I2C_WRAPPER_Rev3.cpp"

//#include "i2c_driver.hpp" Implement this later
extern "C" {
    #include "stm32f4xx_hal.h"
}

#define ERROR 1
#define SUCCESS 0
/**
 * @struct TELEMETRY_CONFIG
 * @brief Structure to hold values to configure the telemetry (address name: CFG_..._ADDR)
 * @note How to choose these values read hardware config in the data sheet (pg 61- 69)
 */
typedef struct TELEMETRY_CONFIG{
    uint16_t RSENSE1;   
    uint16_t RIMON_OUT; 
    uint16_t RSENSE2;   
    uint16_t RDACO;     // RDACO1 + RDACO2
    uint16_t RFBOUT1;   
    uint16_t RFBOUT2;   
    uint16_t RDACI;     // RDACI1 + RDACI2
    uint16_t RFBIN2;    
    uint16_t RFBIN1;    
} TELEM_CONFIG;

/**
 * @struct TELEMETRY_OUTPUT
 * @brief Holds the processed telemetry (address name: TELE_..._ADDR) more info pg 30
 * @note For details on operation pg 23
 */
typedef struct TELEMETRY_OUTPUT{
    float TBAT;      // Battery Temperature
    float POUT;      // Output Power
    float PIN;       // Input Power
    float EFF;       // Charger Efficiency
    float IOUT;      // Output Current
    float IIN;       // Input Current
    float VBAT;      // Battery Voltage
    float VIN;       // Input Voltage measured from FBIN pin
    float VINR;      // Input Voltage Measured from VINR pin
} TELEM;

/**
 * @struct STAT_FAULTS
 * @brief Holds all faults values
 * @note The the index of each fault is top -> bottom = 8th bit -> 1st bit 
 */
typedef struct STAT_FAULTS //FIXME: decide to keep it or not
{   
    uint8_t all_faults; // All faults for quick check
                        //- How To clear  
    bool TS3_EXPIRED;   // Set Auto-Restart
    bool TS2_EXPIRED;   // Set Auto-Restart 
    bool TS1_EXPIRED;   // Set Auto-Restart
    bool T20_EXPIRED;   // Set Auto-Restart
    bool BAT_DISCON;    // "Manually" turn off and on charging 
    bool HIGH_TBAT;     // "Manually" turn off and on charging
    bool LOW_TBAT;      // "Manually" turn off and on charging
    bool LOW_VBAT;      // "Manually" turn off and on charging OR Emergency shut-off
}STAT_FAULTS;

/**
 * @struct GENERAL_CONFIG
 * @brief Common configs that might need to modified various time during operation 
 * @note Worth checking the regs file for macros for some of these register
 */
typedef struct GENERAL_CONFIG{
    uint8_t CTRL_WRT_TO_BOOT;   // Can also be used to check writing status (pg: 39) 
    uint8_t CTRL_EE_WRT_EN;     // Allowed values already defined in the regs file (pg: 39)  CHECK DATA SHEET BEFORE MODIFYING THIS REG
    uint8_t CTRL_CHRG_EN;       // Allowed values already defined in the regs file
} CONFIGS;


class LT8491_DRIVER{
public:
    /**
     * @brief Construct a new LT8491_DRIVER object
     * 
     * @param telem_configs Holds the value for the current and voltage sense components needed for telemetry
     */
	LT8491_DRIVER(TELEM_CONFIG telem_configs, I2C_Wrapper& i2c_pointer);

    /**
     * @brief Reads, processes and stores the telemetry data
     * 
     * @param data Telemetry data object that holds the processed telemetry data
     * @return uint8_t 0 if successful, 1 if there was an error
     */
    uint8_t get_telemetry(TELEM* data);

    /**
     * @brief Get the faults object
     * 
     * @param faults Saves the faults in the faults object 
     * @return uint8_t returns 0 if successful, 1 if there was an error
     */
    uint8_t get_faults(STAT_FAULTS* faults);

    /**
     * @brief Check if the conditions to write to the control registers are met
     * 
     * @return true if the conditions are met, false otherwise
     */
    bool check_control_write(void);

    /**
     * @brief Get the charge logic state
     * 
     * @return true if charging logic is on, false otherwise
     */
    bool get_charge_logic(void);

    /**
     * @brief Enable writing to the control registers
     * 
     * @return uint8_t 0 if successful, 1 if there was an error
     */
    uint8_t en_cfg_write(void);

    /**
     * @brief Disable writing to the control registers
     * 
     * @return uint8_t 0 if successful, 1 if there was an error
     */
    uint8_t dis_cfg_write(void);

    /**
     * @brief Check if the chip is initialized
     * 
     * @note Don't forget to check the status of the system before using the chip
     * 
     * @return true if the system is initialized, false otherwise
     */
    bool get_isInitialized(void);
private:
    TELEM_CONFIG telem_configs;
    TELEM data;
    STAT_FAULTS faults;
    CONFIGS general_configs;
    I2C_Wrapper& i2c;


    bool isInitialized = false;

    //I2C_Wrapper& i2c; to be implemented later
};

#endif //LT8491_HPP
