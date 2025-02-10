#include "../../common/common.h"

#define SD_QUEUE_SIZE 1024  // Adjust size based on your needs

typedef struct {
    char buffer[SD_QUEUE_SIZE];
    int head;  // Points to the start of valid data
    int tail;  // Points to the next available space
    int size;  // Current size of valid data
} CircularQueue;

CircularQueue memQueue = { .head = 0, .tail = 0, .size = 0 };

const char g_outputFile[] = "fat:" STR(SD_DRIVE_NUM) ":output.txt";
char g_fatfsPrefix[] = "fat";
SDFatFS_Handle g_sdfatfsHandle;
FILE *g_dst;

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

    printStr("Entering microSDWrite_executeTask()");
    int i = 0;

    printStr("microSDWrite Initialized...");
    while(1){
        i++;
        printVar("microSDWrite Count: ", &i, 'd');
        handleFileOperations();
        Task_sleep(g_taskSleepDuration);
    }
}

void handleFileOperations() {
    int internalBuffHandle;

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

    // Mount and register the SD Card
    g_sdfatfsHandle = SDFatFS_open(CONFIG_SD_0, SD_DRIVE_NUM);
    if (g_sdfatfsHandle == NULL) {
        printStr("Drive 0 not detected.");
        Task_yield();
    }
    else{
        printStr("Drive 0 mounted successfully.");
    }

    // Open output file
    g_dst = fopen(g_outputFile, "a");
    if (!g_dst) {
        printStr("Error opening output file");
        SDFatFS_close(g_sdfatfsHandle);
        Task_yield();
    }
    else{
        // Disable internal buffering
        internalBuffHandle = setvbuf(g_dst, NULL, _IONBF, 0);
        if (internalBuffHandle != 0){
            printStr("Call to setvbuf failed!");
        }

        // TESTING: Append line of data to circular queue
        // logData(120, 20, "12:30:00 02/10/2025");
        // logData(121, 20, "12:30:01 02/10/2025");
        // logData(122, 20, "12:30:02 02/10/2025");
        // logData(123, 20, "12:30:03 02/10/2025");

        // Write contents from circular queue to the output file
        writeQueueToSD(g_dst);

        // Close and unmount SD Card
        fclose(g_dst);
        SDFatFS_close(g_sdfatfsHandle);
        printStr("Data successfully written to output file.");
        return;
    }
}

void appendToQueue(const char *data) {
    int len = strlen(data);
    if (memQueue.size + len >= SD_QUEUE_SIZE) {
        printStr("Queue full! Data loss possible.");
        return;
    }

    for (int i = 0; i < len; i++) {
        memQueue.buffer[memQueue.tail] = data[i];
        memQueue.tail = (memQueue.tail + 1) % SD_QUEUE_SIZE;  // Wrap around
    }
    memQueue.size += len;
    memQueue.buffer[memQueue.tail] = '\0';
}

void writeQueueToSD(FILE *file) {
    if (memQueue.size == 0) {
        printStr("Queue empty. Nothing to write.");
        return;
    }

    // Write data from head to tail
    int bytes_written = fwrite(&memQueue.buffer[memQueue.head], 1, memQueue.size, file);
    fflush(file);
    rewind(file);


    if (bytes_written > 0) {
        // Remove written data from queue
        memQueue.head = (memQueue.head + bytes_written) % SD_QUEUE_SIZE;
        memQueue.size -= bytes_written;
    }

    memQueue.buffer[memQueue.head] = '\0';

    printVar("bytes_written: ", &bytes_written, 'd');
}

void logData(int heartRate, int respiratoryRate, const char* timestamp) {
    char logEntry[128] = {0};  // Temporary buffer for formatted string

    snprintf(logEntry, sizeof(logEntry), "Heart Rate: %d, Respiratory Rate: %d, Timestamp: %s\n",
            heartRate, respiratoryRate, timestamp);

    appendToQueue(logEntry);  // Append formatted string to queue
}

void cleanupSDCard() {
    if (g_dst != NULL) {
        fflush(g_dst);  // Ensure data is written
        fclose(g_dst);  // Close the file
        g_dst = NULL;   // Reset pointer
        printStr("output.txt file closed for shutdown.");
    }

    if (g_sdfatfsHandle != NULL) {
        SDFatFS_close(g_sdfatfsHandle);  // Unmount SD card
        g_sdfatfsHandle = NULL;          // Reset pointer
        printStr("SD card unmounted for shutdown.");
    }
}
