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
* @brief Creates and initializes all tasks and other RTOS resources required for the application.
*
* This function is responsible for creating all the tasks necessary 
* for the application, initializing associated semaphores, and 
* configuring interrupts. It sets up the power task and any other 
* tasks that toggle GPIOs or perform other specific functionalities.
* Each task is created with its respective stack and priority.
* 
* @note This function assumes that all task stacks and structures 
*       are properly defined globally and available for use.
*/
void constructAllResources();
