#pragma once

// Inclusions used only by microSD_write_task.c
#include <file.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include <third_party/fatfs/ffcio.h>

// Define task parameters
#define MICROSD_WRITE_TASK_STACK_SIZE   (MICROSD_WRITE_STACK_SIZE)
#define MICROSD_WRITE_TASK_PRIORITY     (MICROSD_WRITE_PRIORITY)
Task_Struct g_MicroSDWriteTaskStruct;
Task_Handle g_microSDWriteTaskHandle;
uint8_t g_microSDWriteTaskStack[MICROSD_WRITE_TASK_STACK_SIZE];

// Define microSD drive number and buffer size
#define SD_DRIVE_NUM 0
#ifndef SD_BUFF_SIZE
    #define SD_BUFF_SIZE 2048
#endif
#define STR_(n) #n
#define STR(n) STR_(n)

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
Task_Handle microSDWrite_constructTask();

/**
 * @brief Executes the microSD write task by processing and writing data to the SD card.
 *
 * This function contains the execution logic for the microSD write task. It processes
 * data from a mock memory queue, performs necessary operations, and writes the data 
 * to the microSD card. The task runs indefinitely as part of the RTOS task loop.
 * Delays are introduced using `Task_sleep` to manage timing between operations.
 *
 * @param arg0 Pointer to the mock memory queue containing the data to be written
 *             (passed from TaskParams struct inside microSDWrite_constructTask).
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

/**
 * @brief Creates the output file if it does not exist.
 *
 * This function attempts to create the output file in write mode. 
 * If the file already exists, it does nothing. Otherwise, it ensures 
 * that the file is properly created for subsequent writing operations.
 *
 * @return None
 */
bool createOutputFile();

/**
 * @brief Exports data from a memory queue to the output file.
 *
 * This function takes a file pointer and queue data as inputs. 
 * It processes the queue, writing each string to the specified file 
 * in append mode. The file pointer must already point to an open file.
 *
 * @param file Pointer to the output file opened in append mode.
 * @param queue_data Pointer to the mock memory queue containing the data.
 *
 * @return None
 */
void exportQueueToOutputFile(FILE *file, UArg queue_data);

void handleFileOperations(UArg queue_data);
