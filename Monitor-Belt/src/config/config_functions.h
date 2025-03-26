#pragma once

////////// DOC STRING TEMPLATE //////////
/**
* @brief Short description goes here.
*
* Long description goes here (optional).
*
* @param a - First param (optional).
* @param b - Second param (optional).
* @return  - What is returned goes here (optional).
*/
/////////////////////////////////////////


/**
* @brief contains all TI initialization call. Must be called before configBOARD().
*
* Any TI drivers called inside initBOARD() must have the corresponding inclusion under "TI Driver Header Files" in common.h
* and must be selected in the main.syscfg GUI. For example, if GPIO_init() is called from the TI driver library then 
* "GPIO" must have a green check by it in the .syscfg GUI and "#include <ti/drivers/GPIO.h>"" must be in common.h
*/
void initBOARD(void);

/**
* @brief contains all custom board configurations for things like GPIO, SPI, etc.
*
* NOTE: Add long description once function is complete.
*/
void configBOARD(void);

/**
* @brief Sets startup state for GPIO pins.
*
* NOTE: Add long description once function is complete.
*/
void configGPIO(void);

/**
* @brief Initializes SPI during startup.
*
* NOTE: Add long description once function is complete.
*/
void configSPI(void);
