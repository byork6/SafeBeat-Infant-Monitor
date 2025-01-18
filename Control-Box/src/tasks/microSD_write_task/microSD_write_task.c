#include "../../common.h"
#include "microSD_write_task.h"

// File paths
// #define STR_(n) #n
// #define STR(n)  STR_(n)
// const char outputfile[] = "fat:" STR(DRIVE_NUM) ":output.txt";   // Have to use microSD with this line
// File path in the same directory as the .c file for testing
const char outputFile[] = "C:\\devProjects\\CCSTheia\\SafeBeat-Infant-Monitor\\Control-Box\\src\\tasks\\microSD_write_task\\microSD_write_taskoutput.txt";                             

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

    printStr("microSDWrite Initialized...");
    while (1){
        /// Block used for testing ///
        printVar("microSDWrite Count: ", &i, 'd');
        if (i == 0){
            Task_sleep(500);
        }
        i++;
        /////////////////////////////

        /////// PROJECT CODE ///////
        FILE *file = fopen(outputFile, "a");
        if(!file){
            if (!create_output_file()) {
                Task_sleep(100);
                continue;
            }
            file = fopen(outputFile, "a");
            if (!file) {
                printStr("Error: Failed to open output file after creation.");
                Task_sleep(100);
                continue;
            }
        }
        
        export_queue_to_output_file(file, arg0);

        // Task sleeps for limited time until data re-populates the queue
        Task_sleep(1000);
    }
}

bool create_output_file(){
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

void export_queue_to_output_file(FILE *file, UArg queue_data){
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