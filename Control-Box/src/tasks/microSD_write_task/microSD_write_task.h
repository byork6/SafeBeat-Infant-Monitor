#pragma once

// Define task parameters
#define MICROSD_WRITE_TASK_STACK_SIZE   1024
#define MICROSD_WRITE_TASK_PRIORITY     1
Task_Struct g_MicroSDWriteTaskStruct;
uint8_t g_microSDWriteTaskStack[MICROSD_WRITE_TASK_STACK_SIZE];

// Define microSD drive number and buffer size
#define DRIVE_NUM 0
#define SD_BUFF_SIZE 1024

/**
 * @brief Creates a TI-RTOS task for microSD write operations.
 *
 * This function initializes a TI-RTOS task using the Task_construct API. It sets up 
 * the stack, priority, and parameters required for the RTOS task execution.
 * The task is statically allocated, making it suitable for real-time systems
 * where dynamic memory allocation is not desirable.
 *
 * @return None
 */
void microSDWrite_createTask();

/**
 * @brief Executes the microSD write task by processing and writing data to the SD card.
 *
 * This function contains the execution logic for the microSD write task. It processes
 * data from a mock memory queue, performs necessary operations, and writes the data 
 * to the microSD card. The task runs indefinitely as part of the RTOS task loop.
 * Delays are introduced using `Task_sleep` to manage timing between operations.
 *
 * @param arg0 Pointer to the mock memory queue containing the data to be written
 *             (passed from TaskParams struct inside microSDWrite_createTask).
 * @param arg1 Unused argument (can be set to 0).
 *
 * @note Example task behavior:
 *       - Processes data from `mock_memory_queue`.
 *       - Logs the current state and writes data periodically to avoid resource contention.
 *       - Delays are used to allow other tasks to execute.
 *
 * @return None
 */
void microSDWrite_executeTask(UArg arg0, UArg arg1);

void create_output_file();

void export_queue_to_output_file (UArg queue_data);