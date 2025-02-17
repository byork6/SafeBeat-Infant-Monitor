#include "../../common/common.h"

CircularQueue memQueue = { .head = 0, .tail = 0, .size = 0 };
const char g_outputFile[] = "fat:" STR(SD_DRIVE_NUM) ":output.txt";
char g_fatfsPrefix[] = "fat";
SDFatFS_Handle g_sdfatfsHandle;
FILE *g_outputFileStatus;

Task_Handle microSDWrite_constructTask(){
    // Declare TaskParams struct name
    Task_Params TaskParams;

    // Initialize TaskParams and set paramerters.
    Task_Params_init(&TaskParams);
    // Stack array
    TaskParams.stack = g_microSDWriteTaskStack;
    // Stack array size
    TaskParams.stackSize = MICROSD_WRITE_TASK_STACK_SIZE;
    // Stack task TI-RTOS priority
    TaskParams.priority = MICROSD_WRITE_TASK_PRIORITY;
    // arg0 and ar1 passed to exectution function for the created task. Use 0 if arg is unused.
    // you can pass variables or pointers to structs for larger data objects.
    TaskParams.arg0 = 0;
    TaskParams.arg1 = 0;

    // Construct the TI-RTOS task using the API
    Task_construct(&g_MicroSDWriteTaskStruct, microSDWrite_executeTask, &TaskParams, NULL);
    return (Task_Handle)&g_MicroSDWriteTaskStruct;
}

void microSDWrite_executeTask(UArg arg0, UArg arg1){
    (void)arg0;
    (void)arg1;
    int i = 0;

    printf("Entering microSDWrite_executeTask()...\n");
    printf("MicroSDWrite Initialized.\n");
    while(1){
        i++;
        printf("MicroSDWrite Count: %d\n", i);
        
        // Check for mounted header and proper FatFS init
        if (initSDCard() == SD_INIT_FAILED){
            Task_sleep(g_taskSleepDuration);
            continue;
        }

        // Check for output file --- will show up or be created as long as microSD card is inserted.
        if (openOutputFile() == OUTPUT_FILE_NOT_OPEN){
            Task_sleep(g_taskSleepDuration);
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

    // Reset SPI chip select for microSD card
    GPIO_write(11, 0);

    // Connect header and create FatFS instance
    g_sdfatfsHandle = SDFatFS_open(CONFIG_SD_0, SD_DRIVE_NUM);
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

void appendToQueue(const char *data) {
    int len = strlen(data);
    if (memQueue.size + len >= CIRCULAR_QUEUE_SIZE) {
        printf("Queue full! Data loss possible.\n");
        return;
    }

    for (int i = 0; i < len; i++) {
        memQueue.buffer[memQueue.tail] = data[i];
        memQueue.tail = (memQueue.tail + 1) % CIRCULAR_QUEUE_SIZE;  // Wrap around
    }
    memQueue.size += len;
    memQueue.buffer[memQueue.tail] = '\0';
}

void writeQueueToSD(FILE *file) {
    if (memQueue.size == 0) {
        printf("Queue empty. Nothing to write.\n");
        return;
    }

    // Write data from head to tail
    int bytes_written = fwrite(&memQueue.buffer[memQueue.head], 1, memQueue.size, file);
    fflush(file);
    rewind(file);


    if (bytes_written > 0) {
        // Remove written data from queue
        memQueue.head = (memQueue.head + bytes_written) % CIRCULAR_QUEUE_SIZE;
        memQueue.size -= bytes_written;
    }

    memQueue.buffer[memQueue.head] = '\0';

    printf("Bytes written: %d B\n", bytes_written);
}

void logData(int heartRate, int respiratoryRate, const char* timestamp) {
    char logEntry[128] = {0};  // Temporary buffer for formatted string

    snprintf(logEntry, sizeof(logEntry), "Heart Rate: %d, Respiratory Rate: %d, Timestamp: %s\n",
            heartRate, respiratoryRate, timestamp);

    appendToQueue(logEntry);  // Append formatted string to queue
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
