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
Task_Handle g_microSdWriteTaskHandle;
uint8_t g_microSdWriteTaskStack[MICROSD_WRITE_TASK_STACK_SIZE];

// Define microSD drive number and buffer size
#define SD_DRIVE_NUM 0
#define SD_BUFF_SIZE 2048
#define STR_(n) #n
#define STR(n) STR_(n)

// Type definitions
typedef enum {
    SD_INIT_FAILED = 0,
    SD_INIT_SUCCESS = 1
} SdInitStatus;

typedef enum {
    OUTPUT_FILE_NOT_OPEN = 0,
    OUTPUT_FILE_OPEN = 1
} OutputFileStatus;

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
Task_Handle microSdWrite_constructTask();

/**
 * @brief Executes the microSD write task by processing and writing data to the SD card.
 *
 * This function contains the execution logic for the microSD write task. It processes
 * data from a mock memory queue, performs necessary operations, and writes the data 
 * to the microSD card. The task runs indefinitely as part of the RTOS task loop.
 * Delays are introduced using `Task_sleep` to manage timing between operations.
 *
 * @param arg0 Pointer to the mock memory queue containing the data to be written
 *             (passed from TaskParams struct inside microSdWrite_constructTask).
 * @param arg1 Unused argument (can be set to 0).
 *
 * @note Example task behavior:
 *       - Processes data from `mock_memory_queue`.
 *       - Logs the current state and writes data periodically to avoid resource contention.
 *       - Delays are used to allow other tasks to execute.
 *
 * @return None
 */
void microSdWrite_executeTask(UArg arg0, UArg arg1);


/**
 * @brief Initializes the SD card and mounts the file system.
 *
 * This function configures the SD card interface, registers the file system 
 * operations, and attempts to mount the SD card. It first resets the SPI 
 * chip select for the SD card, then attempts to open the file system using 
 * `SDFatFS_open()`. If the SD card is detected successfully, the function 
 * returns `SD_INIT_SUCCESS`. Otherwise, it returns `SD_INIT_FAILED`.
 *
 * @return SdInitStatus 
 *         - `SD_INIT_SUCCESS` if the SD card is successfully mounted.
 *         - `SD_INIT_FAILED` if the SD card is not detected.
 */
SdInitStatus initSDCard();

/**
 * @brief Opens the output file on the SD card for writing.
 *
 * This function attempts to open the file specified by `g_outputFile` in append mode.
 * If the file cannot be opened, the SD card is closed, and an error status is returned.
 *
 * @return OUTPUT_FILE_OPEN if the file was successfully opened.
 * @return OUTPUT_FILE_NOT_OPEN if the file could not be opened.
 */
OutputFileStatus openOutputFile();

/**
 * @brief Writes data from the circular queue to the output file.
 *
 * This function disables internal buffering, appends data to the circular queue,
 * writes queued data to the SD card, and then closes the file and unmounts the SD card.
 */
void writeToOutputFile();

/**
 * @brief Writes the queued data to the SD card file.
 *
 * This function writes data from the circular queue into the specified file.
 * If the queue is empty, a message is printed, and nothing is written.
 *
 * @param file Pointer to the open file where data should be written.
 */
void writeQueueToSD(FILE *file);

/**
 * @brief Closes the output file and unmounts the SD card.
 *
 * Ensures all pending data is written to the file, closes it, and unmounts the SD card.
 * Also resets the associated file and SD handle pointers.
 */
void cleanupSDCard();

/**
 * @brief Placeholder to prevent warning for undefined symbol
 */
DWORD fatfs_getFatTime(void);
