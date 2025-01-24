#include "../../common/common.h"

/////// TEST CODE ///////
const char outputFile[] = "C:\\DevProjects\\CCSTheia\\SafeBeat-Infant-Monitor\\Control-Box\\src\\tasks\\microSD_write_task\\microSD_write_taskoutput.txt";                             

// Mock memory section containing data
const char *mock_memory_queue[] = {
    "Heart Rate: xxx, Respiratory Rate: xx, Timestamp: xx:xx xx/xx/xx",
    "Heart Rate: xxx, Respiratory Rate: xx, Timestamp: xx:xx xx/xx/xx",
    "Heart Rate: xxx, Respiratory Rate: xx, Timestamp: xx:xx xx/xx/xx",
    "Heart Rate: xxx, Respiratory Rate: xx, Timestamp: xx:xx xx/xx/xx",
    "Heart Rate: xxx, Respiratory Rate: xx, Timestamp: xx:xx xx/xx/xx",
    "Heart Rate: xxx, Respiratory Rate: xx, Timestamp: xx:xx xx/xx/xx",
    "Heart Rate: xxx, Respiratory Rate: xx, Timestamp: xx:xx xx/xx/xx",
    "Heart Rate: xxx, Respiratory Rate: xx, Timestamp: xx:xx xx/xx/xx",
    "Heart Rate: xxx, Respiratory Rate: xx, Timestamp: xx:xx xx/xx/xx",
    "Heart Rate: xxx, Respiratory Rate: xx, Timestamp: xx:xx xx/xx/xx",
    "Heart Rate: xxx, Respiratory Rate: xx, Timestamp: xx:xx xx/xx/xx",
    "Heart Rate: xxx, Respiratory Rate: xx, Timestamp: xx:xx xx/xx/xx",
    NULL
};
/////////////////////////

static unsigned char sd_buff[SD_BUFF_SIZE] __attribute__((aligned(4)));
const char outputfile[] = "fat:" STR(SD_DRIVE_NUM) ":output.txt";

void microSDWrite_createTask(){
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
}

void microSDWrite_executeTask(UArg arg0, UArg arg1){
    (void)arg1;
    printStr("Entering microSDWrite_executeTask()");
    int i = 0;

    ///////// Attempt 2 ////////
    printStr("microSDWrite Initialized...");
    while(1){
        i++;
        printVar("microSDWrite Count: ", &i, 'd');
        handleFileOperations(arg0);
        // 500,000 Ticks = 5 s
        Task_sleep(500000);
    }
    ////////////////////////////

    ////////// Attempt 1 /////////
    // printStr("microSDWrite Initialized...");
    // while (1){
    //     /// Block used for testing ///
    //     printVar("microSDWrite Count: ", &i, 'd');
    //     if (i == 0){
    //         Task_sleep(500);
    //     }
    //     i++;
    //     /////////////////////////////

    //     /////// PROJECT CODE ///////
    //     FILE *file = fopen(outputFile, "a");
    //     if(!file){
    //         if (!createOutputFile()) {
    //             Task_sleep(100);
    //             continue;
    //         }
    //         file = fopen(outputFile, "a");
    //         if (!file) {
    //             printStr("Error: Failed to open output file after creation.");
    //             Task_sleep(100);
    //             continue;
    //         }
    //     }
        
    //     exportQueueToOutputFile(file, arg0);

    //     // Task sleeps for limited time until data re-populates the queue
    //     Task_sleep(1000);
    // }
    ///////////////////////////////////
}

bool createOutputFile(){
    printStr("Output file does not exist. Creating it...");

    FILE *file = fopen(outputFile, "w");
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

/* Handle file copy and operations */
void handleFileOperations(UArg queue_data) {
    SDFatFS_Handle sdfatfsHandle;
    FILE *dst;

    /* Mount and register the SD Card */
    sdfatfsHandle = SDFatFS_open(CONFIG_SD_0, SD_DRIVE_NUM);
    if (!sdfatfsHandle) {
        printStr("microSD card not detected.");
        Task_yield();
    }

    /* Open output file */
    dst = fopen(outputfile, "a");
    if (!dst) {
        printStr("Error opening output file");
        SDFatFS_close(sdfatfsHandle);
        Task_yield();
    }

    // Write contents from buffer to the output file
    exportQueueToOutputFile(dst, (UArg)queue_data);

    /* Unmount SD Card */
    SDFatFS_close(sdfatfsHandle);

    printStr("Data successfully written to output file.");

    return;
}