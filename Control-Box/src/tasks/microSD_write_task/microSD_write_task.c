#include "../../common/common.h"

/////// TEST CODE ///////
// Mock memory section containing data
const char mock_memory_queue[]
    __attribute__((aligned(4))) = "***********************************************************************\n"
                                  "Heart Rate: xxx, Respiratory Rate: xx, Timestamp: xx:xx xx/xx/xx\n"
                                  "Heart Rate: xxx, Respiratory Rate: xx, Timestamp: xx:xx xx/xx/xx\n"
                                  "Heart Rate: xxx, Respiratory Rate: xx, Timestamp: xx:xx xx/xx/xx\n"
                                  "Heart Rate: xxx, Respiratory Rate: xx, Timestamp: xx:xx xx/xx/xx\n"
                                  "Heart Rate: xxx, Respiratory Rate: xx, Timestamp: xx:xx xx/xx/xx\n"
                                  "Heart Rate: xxx, Respiratory Rate: xx, Timestamp: xx:xx xx/xx/xx\n"
                                  "***********************************************************************\n";

/////////////////////////

static unsigned char g_sdBuff[SD_BUFF_SIZE] __attribute__((aligned(4)));
const char g_outputFile[] = "fat:" STR(SD_DRIVE_NUM) ":output.txt";
char g_fatfsPrefix[] = "fat";

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
    TaskParams.arg0 = (UArg)mock_memory_queue;
    TaskParams.arg1 = 0;

    // Construct the TI-RTOS task using the API
    Task_construct(&g_MicroSDWriteTaskStruct, microSDWrite_executeTask, &TaskParams, NULL);
    return (Task_Handle)&g_MicroSDWriteTaskStruct;
}

void microSDWrite_executeTask(UArg arg0, UArg arg1){
    (void)arg1;
    printStr("Entering microSDWrite_executeTask()");
    int i = 0;

    printStr("microSDWrite Initialized...");
    while(1){
        i++;
        printVar("microSDWrite Count: ", &i, 'd');
        handleFileOperations(arg0);
        Task_sleep(g_taskSleepDuration);
    }
}

bool createOutputFile(){
    printStr("Output file does not exist. Creating it...");

    FILE *file = fopen(g_outputFile, "w");
    if (!file) {
        printStr("Error: Failed to create output file.");
        return false;
    }
    fclose(file);
    printStr("Output file created successfully.");
    return true;
}

void exportQueueToOutputFile(FILE *file, UArg queue_data){
    // const char ** is a ptr to an array of string ptrs
    const char **queue = (const char **)queue_data;
    int i = 0;
    char buffer[1024];
    int buffer_pos = 0;

    if (!file) {
        printStr("Error: Failed to open output file in append mode.");
        return;
    }

    while (queue[i] != NULL) {
        int len = snprintf(&buffer[buffer_pos], sizeof(buffer) - buffer_pos, "%s\n", queue[i]);
        if (len < 0 || buffer_pos + len >= sizeof(buffer)) {
            // Flush the buffer if full
            fwrite(buffer, 1, buffer_pos, file);
            buffer_pos = 0;
            // Retry the current string
            continue;
        }
        buffer_pos += len;
        queue[i] = NULL;
        i++;
    }

    // Write remaining data in the buffer
    if (buffer_pos > 0) {
        fwrite(buffer, 1, buffer_pos, file);
    }

    fflush(file);
    fclose(file);
    printStr("All data from mock_memory_queue written to output file.");
}

void handleFileOperations(UArg queue_data) {
    SDFatFS_Handle sdfatfsHandle;
    FILE *dst;
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
    sdfatfsHandle = SDFatFS_open(CONFIG_SD_0, SD_DRIVE_NUM);
    if (sdfatfsHandle == NULL) {
        printStr("Drive 0 not detected.");
        Task_yield();
    }
    else{
        printStr("Drive 0 mounted successfully.");
    }

    // Open output file
    dst = fopen(g_outputFile, "a");
    if (!dst) {
        printStr("Error opening output file");
        SDFatFS_close(sdfatfsHandle);
        Task_yield();
    }
    else{
        // Disable internal buffering
        internalBuffHandle = setvbuf(dst, NULL, _IONBF, 0);
        if (internalBuffHandle != 0){
            printStr("Call to setvbuf failed!");
        }

        // Write contents from buffer to the output file
        // exportQueueToOutputFile(dst, (UArg)queue_data);
        fwrite(mock_memory_queue, 1, strlen(mock_memory_queue), dst);
        fflush(dst);
        rewind(dst);

        // Unmount SD Card
        fclose(dst);
        SDFatFS_close(sdfatfsHandle);
        printStr("Data successfully written to output file.");
        return;
    }
}