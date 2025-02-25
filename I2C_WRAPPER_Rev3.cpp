#ifndef I2C_WRAPPER_REV3_CPP
#define I2C_WRAPPER_REV3_CPP

extern "C" {
	#include "stm32f4xx_hal.h"
}
#include "LT8491_reg.hpp"

/*
Notes on Operation:
1. Make sure to define i2c_handle device_address in private section.
*/

// Register addresses
#define CTRL_UPDATE_TELEM_ADDR  0x26 
#define CTRL_CHRG_EN_ADDR       0x23  
#define STAT_CHARGER_ADDR       0x12  
#define STAT_SYSTEM_ADDR        0x13
#define STAT_BOOT_CRC_ADDR      0x1C
#define STAT_CFG_CRC_ADDR       0x1E
#define CTRL_WRT_TO_BOOT_ADDR   0x20
#define CTRL_EE_WRT_EN_ADDR     0x21
#define CTRL_HALT_STARTUP_ADDR  0x22
#define CTRL_RESTART_CHIP_ADDR  0x24
#define CTRL_RESET_FLAG_ADDR    0x25

// Bit masks and commands
//#define CHRG_EN                 0x01 //Defined else where
//#define CHRG_DIS                0x00 //Defined else where
#define CHRG_LOGIC_ON           0x01   
#define CHRG_FAULT              0x80   
#define TELEM_ACTIVE            0x40   
#define CHRG_STAGE_MASK         0x1C   
#define GT_C10                  0x02   
#define CHRG_LOGIC_ON_MASK      0x01  
#define CRC_ERR_BOOT_MASK       0x08  
#define CRC_ERR_FACTORY_MASK    0x10  
#define BOOT_SUCCESS_MASK       0x20  
#define SYSTEM_BUSY_MASK        0x03  
#define BUSY_RDY_MASK           0x01  
#define WRITE_SUCCESS_MASK      0x02  
#define WRITE_FAIL_MASK         0x04  
#define RESTART_CHIP_CMD        0x99
#define CLEAR_RESET_FLAG        0x00
#define COPY_CONFIG_TO_BOOT     0x30
#define RESTORE_FACTORY_DEFAULTS 0x57
#define ENABLE_EEPROM_WRITE     0xCC
#define UPDATE_TELEMETRY_CMD    0xAA

// Charging stage definitions
#define STAGE_0                 0x00    
#define STAGE_1                 0x04    
#define STAGE_2                 0x08   
#define STAGE_3                 0x0C   
#define DONE_CHARGING           0x10    

// Telemetry data storage -- page 24
/*
typedef struct {
    float VBAT;  // Battery voltage
    float VIN;   // Input voltage measured from FBIN pin
    float VINR;  // Input voltage measured from VINR pin
    float IIN;   // Input current
    float PIN;   // Input power
    float TBAT;  // Battery temperature
    float IOUT;  // Output current
    float POUT;  // Output power
    float EFF;   // Charger Efficiency
} TELEM;
*/


class I2C_Wrapper {
public:

    I2C_Wrapper(I2C_HandleTypeDef* hi2c, uint8_t device_address = LT8491_I2C_ADDR_0)  //init
        : i2c_handle(hi2c), device_address(device_address) {
        // Validate I2C handle
        if (hi2c == nullptr) {
            // Handle error - in production code you might set an error flag
        }
    }


    // Basic Building Blocks
    // Set the size to 1 for single byte writing

    HAL_StatusTypeDef i2c_read(uint8_t reg, uint8_t* data, uint16_t size) {
        return HAL_I2C_Mem_Read(i2c_handle, device_address << 1, reg, I2C_MEMADD_SIZE_8BIT, data, size, timeout);
    }

    HAL_StatusTypeDef i2c_write(uint8_t reg, uint8_t data, uint16_t size) {
        return HAL_I2C_Mem_Write(i2c_handle, device_address << 1, reg, I2C_MEMADD_SIZE_8BIT, &data, size, timeout);
    }

    // ======= Section 1 Telemetry =======

    HAL_StatusTypeDef forceTelemetryUpdate() {
        return i2c_write( CTRL_UPDATE_TELEM_ADDR, UPDATE_TELEMETRY_CMD, 1);
    }
    
    bool isTelemetryUpdateInProgress() {
        uint8_t status = 0;
        if (i2c_read( CTRL_UPDATE_TELEM_ADDR, &status, 1) == HAL_OK) {
            return (status & BUSY_RDY_MASK) == BUSY_RDY_MASK;
        }
        return false;
    }

    // ======= Section 2 Charging Functionalities ======= 
    
    HAL_StatusTypeDef enableCharging() {
        return i2c_write( CTRL_CHRG_EN_ADDR, CHRG_EN, 1);
    }

    HAL_StatusTypeDef disableCharging() {
        return i2c_write( CTRL_CHRG_EN_ADDR, CHRG_DIS, 1);
    }

    HAL_StatusTypeDef readChargerStatus(uint8_t* status) {
        return i2c_read( STAT_CHARGER_ADDR, status, 1);
    }

    // status checks
    bool isChargerEnabled() {
        uint8_t data_buffer;
        if (i2c_read( STAT_CHARGER_ADDR, &data_buffer, 1) == HAL_OK) {
            return (data_buffer & CHRG_LOGIC_ON) == CHRG_LOGIC_ON;
        }
        return false;
    }

    bool isChargeFault() {
        uint8_t data_buffer;
        if (i2c_read( STAT_CHARGER_ADDR, &data_buffer, 1) == HAL_OK) {
            return (data_buffer & CHRG_FAULT) == CHRG_FAULT;
        }
        return false;
    }

    bool isTelemActive() {
        uint8_t data_buffer;
        if (i2c_read( STAT_CHARGER_ADDR, &data_buffer, 1) == HAL_OK) {
            return (data_buffer & TELEM_ACTIVE) == TELEM_ACTIVE;
        }
        return false;
    }

    uint8_t getChargingStage() {
        uint8_t data_buffer;
        if (i2c_read( STAT_CHARGER_ADDR, &data_buffer, 1) == HAL_OK) {
            return (data_buffer & CHRG_STAGE_MASK);
        }
        return 0xFF; // Return invalid value to indicate error
    }

    bool isCurrentAboveC10() {
        uint8_t data_buffer;
        if (i2c_read( STAT_CHARGER_ADDR, &data_buffer, 1) == HAL_OK) {
            return (data_buffer & GT_C10) == GT_C10;
        }
        return false;
    }

    // to convert charging stage to human-readable string
    const char* getChargingStageString() {
        uint8_t stage = getChargingStage();
        switch (stage) {
            case STAGE_0:
                return "Stage 0";
            case STAGE_1:
                return "Stage 1";
            case STAGE_2:
                return "Stage 2";
            case STAGE_3:
                return "Stage 3";
            case DONE_CHARGING:
                return "Done Charging";
            default:
                return "Unknown Stage";
        }
    }

    // ======= Section 3 CRC  ======= 

    bool isBootCRCError() {
        uint8_t status_system;
        if (i2c_read( STAT_SYSTEM_ADDR, &status_system, 1) == HAL_OK) {
            return (status_system & CRC_ERR_BOOT_MASK) == CRC_ERR_BOOT_MASK;
        }
        return true; 
    }

    bool isFactoryCRCError() {
        uint8_t status_system;
        if (i2c_read( STAT_SYSTEM_ADDR, &status_system, 1) == HAL_OK) {
            return (status_system & CRC_ERR_FACTORY_MASK) == CRC_ERR_FACTORY_MASK;
        }
        return true; 
    }

    bool isStartupSuccessful() {
        uint8_t status_system;
        if (i2c_read( STAT_SYSTEM_ADDR, &status_system, 1) == HAL_OK) {
            return (status_system & BOOT_SUCCESS_MASK) == BOOT_SUCCESS_MASK;
        }
        return false;
    }

    HAL_StatusTypeDef readBootCRC(uint16_t* crc_value) {
        uint8_t crc_bytes[2];
        HAL_StatusTypeDef status;
        
        status = i2c_read( STAT_BOOT_CRC_ADDR, crc_bytes, 2);
        if (status != HAL_OK) {
            return status;
        }
        
        *crc_value = (uint16_t)crc_bytes[0] | ((uint16_t)crc_bytes[1] << 8);
        return HAL_OK;
    }

    HAL_StatusTypeDef readConfigCRC(uint16_t* crc_value) {
        uint8_t crc_bytes[2];
        HAL_StatusTypeDef status;
        
        status = i2c_read( STAT_CFG_CRC_ADDR, crc_bytes, 2);
        if (status != HAL_OK) {
            return status;
        }
        
        *crc_value = (uint16_t)crc_bytes[0] | ((uint16_t)crc_bytes[1] << 8);
        return HAL_OK;
    }

    HAL_StatusTypeDef haltStartup() {
        return i2c_write( CTRL_HALT_STARTUP_ADDR, 0x5A, 1);
    }

    // ======= Section 4 EPROM  ======= 

    HAL_StatusTypeDef enableEepromWrite() {
        return i2c_write( CTRL_EE_WRT_EN_ADDR, ENABLE_EEPROM_WRITE, 1);
    }

    HAL_StatusTypeDef disableEepromWrite() {
        return i2c_write( CTRL_EE_WRT_EN_ADDR, 0x00, 1);
    }

    // Copy configuration registers to EEPROM boot region
    HAL_StatusTypeDef copyConfigToEeprom() {
        uint8_t charger_status, system_status;
        
        // Verify charging logic is off
        if (i2c_read( STAT_CHARGER_ADDR, &charger_status, 1) != HAL_OK) {
            return HAL_ERROR;
        }
        if ((charger_status & CHRG_LOGIC_ON) != 0) {
            return HAL_ERROR; 
        }
        
        // Verify system is not busy
        if (i2c_read( STAT_SYSTEM_ADDR, &system_status, 1) != HAL_OK) {
            return HAL_ERROR;
        }
        if ((system_status & SYSTEM_BUSY_MASK) != 0) {
            return HAL_ERROR; 
        }
        
        // Enable EEPROM write capability
        if (enableEepromWrite() != HAL_OK) {
            return HAL_ERROR;
        }
        
        // Initiate copy operation
        return i2c_write( CTRL_WRT_TO_BOOT_ADDR, COPY_CONFIG_TO_BOOT, 1);
    }

    // Restore factory defaults to EEPROM boot region
    HAL_StatusTypeDef restoreFactoryDefaults() {
        uint8_t charger_status, system_status;
        
        // Verify charging logic is off
        if (i2c_read( STAT_CHARGER_ADDR, &charger_status, 1) != HAL_OK) {
            return HAL_ERROR;
        }
        if ((charger_status & CHRG_LOGIC_ON) != 0) {
            return HAL_ERROR; 
        }
        
        // Verify system is not busy
        if (i2c_read( STAT_SYSTEM_ADDR, &system_status, 1) != HAL_OK) {
            return HAL_ERROR;
        }
        if ((system_status & SYSTEM_BUSY_MASK) != 0) {
            return HAL_ERROR; 
        }
        
        // Enable EEPROM write capability
        if (enableEepromWrite() != HAL_OK) {
            return HAL_ERROR;
        }
        
        // Initiate restore factory defaults operation
        return i2c_write( CTRL_WRT_TO_BOOT_ADDR, RESTORE_FACTORY_DEFAULTS, 1);
    }

    // Read EEPROM boot region 
    HAL_StatusTypeDef readEepromBootRegion(uint8_t* data, uint8_t length) {
        if (length > 0x36) { 
            return HAL_ERROR; 
        }
        
        return i2c_read( 0x88, data, length);
    }

    // Check EEPROM write operation status
    HAL_StatusTypeDef checkEepromWriteStatus(bool* busy, bool* success, bool* fail) {
        uint8_t status;
        HAL_StatusTypeDef result = i2c_read( CTRL_WRT_TO_BOOT_ADDR, &status, 1);
        
        if (result == HAL_OK) {
            if (busy) *busy = (status & BUSY_RDY_MASK) != 0;
            if (success) *success = (status & WRITE_SUCCESS_MASK) != 0;
            if (fail) *fail = (status & WRITE_FAIL_MASK) != 0;
        }
        
        return result;
    }

    // Wait for EEPROM write operation to complete with timeout
    HAL_StatusTypeDef waitForEepromWriteComplete(uint32_t timeout, bool* success) {
        uint32_t start_time = HAL_GetTick();
        bool busy = true;
        bool write_success = false;
        bool write_fail = false;
        
        while (busy && (HAL_GetTick() - start_time < timeout)) {
            if (checkEepromWriteStatus(&busy, &write_success, &write_fail) != HAL_OK) {
                return HAL_ERROR;
            }
            
            if (write_fail) {
                if (success) *success = false;
                return HAL_OK; // Operation completed with failure
            }
            
            if (!busy) {
                if (success) *success = write_success;
                return HAL_OK; // Operation completed
            }
            
            HAL_Delay(10); // Small delay before checking again
        }
        
        return HAL_TIMEOUT; // Timeout waiting for operation to complete
    }

    // Check if system is busy with EEPROM operations
    bool isSystemBusyWithEeprom() {
        uint8_t system_status;
        if (i2c_read( STAT_SYSTEM_ADDR, &system_status, 1) == HAL_OK) {
            return (system_status & SYSTEM_BUSY_MASK) == 0x02; // 10b indicates EEPROM write
        }
        return true; // Assume busy if read fails
    }

    // Returns EEPROM operation status as string
    const char* getEepromStatusString() {
        bool busy = false;
        bool success = false;
        bool fail = false;
        
        if (checkEepromWriteStatus(&busy, &success, &fail) != HAL_OK) {
            return "Status Read Error";
        }
        
        if (busy) {
            return "EEPROM Write In Progress";
        } else if (success) {
            return "EEPROM Write Successful";
        } else if (fail) {
            return "EEPROM Write Failed";
        } else {
            return "No EEPROM Operation";
        }
    }

    // ======= Section 5 Restart  ======= 

    HAL_StatusTypeDef restartChip(uint32_t timeout) {
        HAL_StatusTypeDef status;
        
        // Step 1: Disable charging - required before restart
        status = i2c_write( CTRL_CHRG_EN_ADDR, 0x00, 1);
        if (status != HAL_OK) {
            return status;
        }
        
        // Step 2: Wait for CHRG_LOGIC_ON bit to clear
        uint32_t start_time = HAL_GetTick();
        uint8_t charger_status;
        
        do {
            status = i2c_read( STAT_CHARGER_ADDR, &charger_status, 1);
            if (status != HAL_OK) {
                return status;
            }
            
            // Check if CHRG_LOGIC_ON bit is clear
            if ((charger_status & CHRG_LOGIC_ON_MASK) == 0) {
                break;
            }
            
            // Add a small delay to avoid hammering the I2C bus
            HAL_Delay(10);
            
        } while (HAL_GetTick() - start_time < timeout);
        
        // Check if we timed out waiting for CHRG_LOGIC_ON to clear
        if ((charger_status & CHRG_LOGIC_ON_MASK) != 0) {
            return HAL_TIMEOUT;
        }
        
        // Step 3: Issue restart command
        return i2c_write( CTRL_RESTART_CHIP_ADDR, RESTART_CHIP_CMD, 1);
    }

    HAL_StatusTypeDef clearResetFlag() {
        return i2c_write( CTRL_RESET_FLAG_ADDR, CLEAR_RESET_FLAG, 1);
    }


    HAL_StatusTypeDef checkResetOccurred(bool* has_reset) {
        uint8_t reset_flag;
        HAL_StatusTypeDef status = i2c_read( CTRL_RESET_FLAG_ADDR, &reset_flag, 1);
        
        if (status == HAL_OK && has_reset != NULL) {
            *has_reset = (reset_flag & 0x01) == 0x01;
        }
        
        return status;
    }

    HAL_StatusTypeDef performVerifiedReset(uint32_t restart_timeout, uint32_t reset_wait_ms) {
        HAL_StatusTypeDef status;
        
        // Clear reset flag first (if it was set)
        status = clearResetFlag();
        if (status != HAL_OK) {
            return status;
        }
        
        // Restart the chip
        status = restartChip(restart_timeout);
        if (status != HAL_OK) {
            return status;
        }
        
        // Wait for chip to complete reset
        HAL_Delay(reset_wait_ms);
        
        // Verify reset occurred by checking if reset flag is set
        bool reset_occurred = false;
        status = checkResetOccurred(&reset_occurred);
        if (status != HAL_OK) {
            return status;
        }
        
        // Return error if reset didn't occur
        if (!reset_occurred) {
            return HAL_ERROR;
        }
        
        return HAL_OK;
    }

    private:
        I2C_HandleTypeDef* i2c_handle; // HAL I2C handle
        uint8_t device_address;        // Device I2C address (7-bit NOT shifted)
        uint32_t timeout = 10000;
    };

#endif
