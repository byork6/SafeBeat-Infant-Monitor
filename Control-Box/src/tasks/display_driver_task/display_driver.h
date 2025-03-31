#pragma once

// Define task parameters
#define DISPLAY_DRIVER_TASK_STACK_SIZE   (DISPLAY_DRIVER_STACK_SIZE)
#define DISPLAY_DRIVER_TASK_PRIORITY     (DISPLAY_DRIVER_PRIORITY)
Task_Struct g_DisplayDriverTaskStruct;
Task_Handle g_displayDriverTaskHandle;
uint8_t g_displayDriverTaskStack[DISPLAY_DRIVER_TASK_STACK_SIZE];


/**
 * @brief Creates a TI-RTOS task for the display driver.
 *
 * This function initializes a TI-RTOS task using the Task_construct API. 
 * It sets up the stack, priority, and task parameters needed to execute 
 * the display driver task. The task is statically allocated, making it 
 * suitable for real-time embedded systems where dynamic memory use is discouraged.
 *
 * @return Handle to the constructed display driver task.
 */
Task_Handle displayDriver_constructTask(void);

/**
 * @brief Executes the display driver logic inside a TI-RTOS task.
 *
 * This function contains the main loop for the display driver. It handles 
 * SPI initialization, periodic status updates, and rendering heart and 
 * respiration rate values to the display. It also validates the SPI handle 
 * and attempts to recover from failures if the display SPI is not available.
 *
 * The task runs indefinitely, periodically performing display operations.
 * TI-RTOS `Task_sleep` is used to delay execution between updates.
 *
 * @param arg0 Unused task argument (can be set to 0).
 * @param arg1 Unused task argument (can be set to 0).
 *
 * @note Example behavior:
 * - Initializes and checks the display SPI handle.
 * - Calls renderDisplay with mock or real data.
 * - Sleeps between iterations to allow other tasks to run.
 *
 * @return None
 */
void displayDriver_executeTask(UArg arg0, UArg arg1);

void ClearScreen(void);

void MakeScreen_MatrixOrbital(uint8_t DotSize);

/**
 * @brief Renders heart rate and respiration rate values to the display.
 *
 * This function is responsible for formatting and displaying biometric data 
 * such as heart rate (HR) and respiration rate (RR). It will eventually be 
 * replaced with FT813 draw-text commands for full graphics support.
 *
 * @param heartRate Current heart rate value in beats per minute (BPM).
 * @param respirationRate Current respiration rate value in breaths per minute (BRPM).
 *
 * @note Currently uses printf for debug output. Graphics commands to be implemented later.
 *
 * @return None
 */
void renderDisplay(int heartRate, int respirationRate);
