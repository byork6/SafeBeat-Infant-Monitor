#pragma once

#include <stdint.h>

////////// DOC STRING TEMPLATE //////////
/**
* @brief Short description goes here.
*
* Long description goes here (optional).
*
* @param a First param (optional).
* @param b Second param (optional).
* @return What is returned goes here (optional).
*/
/////////////////////////////////////////

/**
* @brief Example code provided by the empty.c project template from the SDK.
*/
void exampleCode(void);

/**
* @brief Test code that toggles a GPIO pin every 1 second.
*
* The function sets a single GPIO pin to output and toggles it
* with a one second delay. This is used for testing the functionality of
* a digital output pin only and should not be used in the final project. 
* The input parameter is chosen based off of the gpioPinConfigs
* list inside ti_drivers_config.c.
*
* @param pin_config_index The index of the GPIO pin to be tested. Valid inputs = 5-30
*/
void testGPIO(uint32_t pin_config_index);

/**
* @brief Sets startup state for GPIO pins.
*
* TODO: Add long description once function is complete.
*/
void configGPIO(void);

/**
* @brief Initializes SPI during startup.
*
* TODO: Add long description once function is complete.
*/
void configSPI(void);
