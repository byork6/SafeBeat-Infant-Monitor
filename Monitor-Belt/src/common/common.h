#pragma once

// Built-in library inclusions
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

/////// SDK HEADER CONTENT ///////
// General purpose TI Drivesrs
#include <ti/drivers/Power.h>
#include <ti/drivers/GPIO.h>
#include <ti/drivers/SPI.h>
#include <ti/drivers/SDFatFS.h>
#include <ti/drivers/Temperature.h>
#include <ti/drivers/rf/RF.h>

// TI-RTOS7 BIOS execution
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Semaphore.h>
#include <ti/sysbios/knl/Mailbox.h>

// TI driver and sysbios configuration - files generated by "main.sysconfig" & must be included.
#include "ti_drivers_config.h"
#include "ti_sysbios_config.h"
#include "ti_radio_config.h"

/////// CUSTOM HEADER CONTENT ///////
// CUSTOM MACROS
// Task Priorities
// The max priority that can be used is currently 6 --- A priority of 0 is reserved for the idleLoop() and should not be used here.
// This can be changed in main.sysconfig -> POSIX Settings -> Other Dependencies -> Task -> # of task priorities.
// The number of task priorities setting in the .sysconfig includes 0, therefore if the set value is 7, then the range of usable priorities is 0 to 6.
#define POWER_SHUTDOWN_PRIORITY     1
#define MICROSD_WRITE_PRIORITY      2
#define TEST_GPIO_PRIORITY          3
#define RED_LIGHT_BLINK_PRIORITY    3       // Used for debugging
#define GREEN_LIGHT_BLINK_PRIORITY  3       // Used for debugging
#define AFE_READ_PRIORITY           4
#define UART_BRIDGE_PRIORITY        5
#define TEMP_MONITORING_PRIORITY    6
// Task stack sizes in bytes --- NOTE: Must be a multiple of 8 bytes to maintain stack pointer alignment
#define POWER_SHUTDOWN_STACK_SIZE   512
#define MICROSD_WRITE_STACK_SIZE    1024
#define TEST_GPIO_STACK_SIZE        1024
#define AFE_READ_STACK_SIZE         1024
#define UART_BRIDGE_STACK_SIZE      1024
#define TEMP_MONITORING_STACK_SIZE  1024
// GPIO
#define DRIVE_GPIO_HIGH (1)
#define DRIVE_GPIO_LOW (0)
#define GPIO_SET_OUT_AND_DRIVE_LOW (GPIO_CFG_OUT_STD | GPIO_CFG_OUT_LOW)
#define GPIO_SET_OUT_AND_DRIVE_HIGH (GPIO_CFG_OUT_STD | GPIO_CFG_OUT_HIGH)
// Clock_tickPeriod = 10 us --- i.e. 25,000 Ticks = 250 ms --- The macros below convert common time units into ticks to use in delay routines.
#define SECONDS_TO_TICKS(seconds) ((seconds) * 100000)                      
#define MS_TO_TICKS(milliseconds) ((milliseconds) * 100)
#define US_TO_TICKS(microseconds) ((microseconds) * 10)
// Default task sleep duration in ticks
#define DEFAULT_TASK_SLEEP_DURATION (MS_TO_TICKS(250))
#define TEMP_MONITORING_TASK_SLEEP_DURATION (MS_TO_TICKS(250))
// Temperature monitoring
#define HIGH_TEMP_THRESHOLD_CELSIUS 35
#define CRITICAL_TEMP_THRESHOLD_CELSIUS 40
#define HIGH_TEMP_TASK_SLEEP_DURATION (MS_TO_TICKS(1000))
#define CRITICAL_TEMP_TASK_SLEEP_DURATION (MS_TO_TICKS(5000))

// GLOBAL VARIABLES
extern int g_taskSleepDuration;

// CUSTOM INCLUSIONS
#include "../config/config_functions.h"
#include "../tasks/test_gpio_task/test_gpio_task.h"
#include "../tasks/power_shutdown_task/power_shutdown_task.h"
#include "../tasks/temperature_monitoring_task/temperature_monitoring_task.h"
#include "../tasks/uart_bridge_task/uart_bridge_task.h"
#include "../tasks/uart_bridge_task/RFQueue.h"
#include "../tasks/afe_read_task/afe_read_task.h"


// --- TYPE DEFINITIONS --- //
typedef struct {
    uint8_t heartRate;
    uint8_t respRate;
} VitalPacket;

extern Mailbox_Struct g_vitalMailboxStruct;
extern Mailbox_Handle g_vitalMailboxHandle;
extern uint8_t g_vitalMailboxBuffer[sizeof(VitalPacket) * 4];


// CUSTOM FUNCTION PROTOTYPES
////////// DOC STRING TEMPLATE //////////
/**
* @brief - Short description goes here.
*
* Long description goes here (optional).
*
* @param a - First param (optional).
* @param b - Second param (optional).
* @return  - What is returned goes here (optional).
*/
/////////////////////////////////////////

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
void createAllResources();

/**
* @brief - Test code that toggles a GPIO pin every 1 second.
*
* The function sets a single GPIO pin to output and toggles it
* with a one second delay. This is used for testing the functionality of
* a digital output pin only and should not be used in the final project. 
* The input parameter is chosen based off of the gpioPinConfigs
* list inside ti_drivers_config.c.
*
* @param pin_config_index - The index of the GPIO pin to be tested. Valid inputs = 5-30
*/
void testGpio(uint32_t pin_config_index);

void createVitalMailbox(void);
