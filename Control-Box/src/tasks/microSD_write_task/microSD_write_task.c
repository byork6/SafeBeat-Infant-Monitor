#include "../../common.h"
#include "microSD_write_task.h"

// File paths
// #define STR_(n) #n
// #define STR(n)  STR_(n)
// const char outputfile[] = "fat:" STR(DRIVE_NUM) ":output.txt";   // Have to use microSD with this line
// File path in the same directory as the .c file for testing
const char outputFile[] = "C:\\devProjects\\CCSTheia\\SafeBeat-Infant-Monitor\\Control-Box\\src\\tasks\\microSD_write_task\\output_data.txt";                             

// Mock memory section containing data
const char *mock_memory_queue[] = {
    "Heart Rate: xxx, Respiratory Rate: xx, Timestamp: xx:xx xx/xx/xx",
    "Heart Rate: xxx, Respiratory Rate: xx, Timestamp: xx:xx xx/xx/xx",
    "Heart Rate: xxx, Respiratory Rate: xx, Timestamp: xx:xx xx/xx/xx",
    NULL // Indicates end of data
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

// TODO: Data writes very slow - see if it can be optimized to write faster?
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
        create_output_file();

        export_queue_to_output_file(arg0);

        printStr("All data from mock_memory_queue written to output file.");

        // Task_sleep(1000);
        Task_yield();
    }
}

void create_output_file(){
    FILE *file;

    file = fopen(outputFile, "r");
    if (!file) {
        printStr("Output file does not exist. Creating it...");
        
        // Create the file in write mode
        file = fopen(outputFile, "w");
        if (!file) {
            printStr("Error: Failed to create output file.");
            Task_yield();
        }
        fclose(file); // Close the file after creation
        printStr("Output file created successfully.");
    } 
    else {
        fclose(file); // Close the file if it already exists
    }
}

void export_queue_to_output_file(UArg queue_data){
    // const char ** is a ptr to an array of string ptrs
    const char **queue = (const char **)queue_data;
    int i = 0;
    FILE *file;

    file = fopen(outputFile, "a");
    if (!file) {
        printStr("Error: Failed to open output file in append mode.");
        Task_yield();
    }

    while (queue[i] != NULL) {
        fprintf(file, "%s\n", queue[i]);
        queue[i] = NULL;
        i++;
    }

    fflush(file);
    fclose(file);
}