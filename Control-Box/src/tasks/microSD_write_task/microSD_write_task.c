#include "common.h"

// Global variables
const char g_outputFile[] = "fat:" STR(SD_DRIVE_NUM) ":output.txt";
char g_fatfsPrefix[] = "fat";
SDFatFS_Handle g_sdfatfsHandle;
FILE *g_outputFileStatus;

Task_Handle microSdWrite_constructTask(){
    // Declare TaskParams struct name
    Task_Params TaskParams;

    // Initialize TaskParams and set paramerters.
    Task_Params_init(&TaskParams);
    // Stack array
    TaskParams.stack = g_microSdWriteTaskStack;
    // Stack array size
    TaskParams.stackSize = MICROSD_WRITE_TASK_STACK_SIZE;
    // Stack task TI-RTOS priority
    TaskParams.priority = MICROSD_WRITE_TASK_PRIORITY;
    // arg0 and ar1 passed to exectution function for the created task. Use 0 if arg is unused.
    // you can pass variables or pointers to structs for larger data objects.
    TaskParams.arg0 = 0;
    TaskParams.arg1 = 0;

    // Construct the TI-RTOS task using the API
    Task_construct(&g_MicroSDWriteTaskStruct, microSdWrite_executeTask, &TaskParams, NULL);
    return (Task_Handle)&g_MicroSDWriteTaskStruct;
}

void microSdWrite_executeTask(UArg arg0, UArg arg1){
    (void)arg0;
    (void)arg1;
    static int i = 0;

    printf("Entering microSdWrite_executeTask()...\n");
    while(1){
        i++;
        printf("MicroSdWrite Count: %d\n", i);
        
        // Check for mounted header and proper FatFS init
        if (initSDCard() == SD_INIT_FAILED){
            Task_sleep(g_taskSleepDuration);
            continue;
        }

        // Check for output file --- will show up or be created as long as microSD card is inserted.
        if (openOutputFile() == OUTPUT_FILE_NOT_OPEN){
            // If output file does not open we should use higher delay on this task so it is not wasting resources.
            // Delay will be 5 seconds if the card is not detected so repetition of the task is slower when no card is inserted.
            Task_sleep(MS_TO_TICKS(5000));
            continue;
        }

        writeToOutputFile();
        Task_sleep(g_taskSleepDuration);
    }
}

SdInitStatus initSDCard(){
    add_device(g_fatfsPrefix,
               _MSA,
               ffcio_open,
               ffcio_close,
               ffcio_read,
               ffcio_write,
               ffcio_lseek,
               ffcio_unlink,
               ffcio_rename);

    // Reset SD chip select and set Display chip select for microSD card --- Chip select is active low
    GPIO_write(SD_SPI_CSN_PIN, 0);
    GPIO_write(DISPLAY_SPI_CSN_PIN, 1);

    // Connect header and create FatFS instance
    g_sdfatfsHandle = SDFatFS_open(CONFIG_SD, SD_DRIVE_NUM);
    if (g_sdfatfsHandle == NULL) {
        printf("microSD header not connected.\n");
        return SD_INIT_FAILED;
    }
    printf("Header ready for microSD card.\n");
    return SD_INIT_SUCCESS;
}

OutputFileStatus openOutputFile(){
    g_outputFileStatus = fopen(g_outputFile, "a");
    if (!g_outputFileStatus) {
        printf("Error opening OUTPUT.TXT\n");
        SDFatFS_close(g_sdfatfsHandle);
        return OUTPUT_FILE_NOT_OPEN;
    }
    printf("OUTPUT.TXT ready for data.\n");
    return OUTPUT_FILE_OPEN;
}

void writeToOutputFile(){
        int internalBuffHandle;

        // Disable internal buffering
        internalBuffHandle = setvbuf(g_outputFileStatus, NULL, _IONBF, 0);
        if (internalBuffHandle != 0){
            printf("Call to setvbuf failed!\n");
        }

        // TESTING: Append line of data to circular queue ///
        logData(120, 20, "12:30:00 02/10/2025");
        logData(121, 20, "12:30:01 02/10/2025");
        logData(122, 20, "12:30:02 02/10/2025");
        logData(123, 20, "12:30:03 02/10/2025");
        ////////////////////////////////////////////////////

        // Write contents from circular queue to the output file
        writeQueueToSD(g_outputFileStatus);

        // Close and unmount SD Card
        fclose(g_outputFileStatus);
        SDFatFS_close(g_sdfatfsHandle);
        printf("Data successfully written to output file.\n");
    }

void writeQueueToSD(FILE *file) {
    if (sdMemQueue.size == 0) {
        printf("Queue empty. Nothing to write.\n");
        return;
    }

    // Write data from head to tail
    int bytes_written = fwrite(&sdMemQueue.buffer[sdMemQueue.head], 1, sdMemQueue.size, file);
    fflush(file);
    rewind(file);


    if (bytes_written > 0) {
        // Remove written data from queue
        sdMemQueue.head = (sdMemQueue.head + bytes_written) % CIRCULAR_QUEUE_SIZE;
        sdMemQueue.size -= bytes_written;
    }

    sdMemQueue.buffer[sdMemQueue.head] = '\0';

    printf("Bytes written: %d B\n", bytes_written);
}

void cleanupSDCard() {
    if (g_outputFileStatus != NULL) {
        fflush(g_outputFileStatus);  // Ensure data is written
        fclose(g_outputFileStatus);  // Close the file
        g_outputFileStatus = NULL;   // Reset pointer
        printf("OUTPUT.TXT file closed for shutdown.\n");
    }

    if (g_sdfatfsHandle != NULL) {
        SDFatFS_close(g_sdfatfsHandle);  // Unmount SD card
        g_sdfatfsHandle = NULL;          // Reset pointer
        printf("SD card unmounted for shutdown.\n");
    }
}

DWORD fatfs_getFatTime(void) {
    return ((DWORD)(2025 - 1980) << 25) | ((DWORD)2 << 21) | ((DWORD)18 << 16);
}