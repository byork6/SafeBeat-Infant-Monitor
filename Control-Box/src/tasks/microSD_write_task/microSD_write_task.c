#include "../../common.h"
#include "microSD_write_task.h"
#include <unistd.h>
#include "third_party/fatfs/ff.h"
#include <stdio.h>


// File paths
// #define STR_(n) #n
// #define STR(n)  STR_(n)
// const char outputfile[] = "fat:" STR(DRIVE_NUM) ":output.txt";   // Have to use microSD with this line
// File path in the same directory as the .c file for testing
const char outputFile[] = "C:\\DevProjects\\CCSTheia\\workspace_safebeat_infant_monitor\\Control-Box\\src\\tasks\\microSD_write_task\\microSD_write_taskoutput.txt";                             

// Mock memory section containing data
const char *mock_memory_queue[] = {
    "Data packet 1",
    "Data packet 2",
    "Data packet 3",
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

void microSDWrite_executeTask(UArg arg0, UArg arg1){
    (void)arg1;
    printStr("Entering microSDWrite_executeTask()");

    const char **queue = (const char **)arg0; // Cast arg0 to the mock memory queue
    FILE *file;
    int queue_ptr = 0;
    int i = 0;

    printStr("microSDWrite Initialized...");
    while (1){
        // Block used for testing
        printVar("microSDWrite Count: ", &i, 'd');
        if (i == 0){
            Task_sleep(500);
        }
        i++;

        /////// PROJECT CODE ///////
        file = fopen(outputFile, "r");
        if (!file) {
            printStr("Output file does not exist. Creating it...");
            
            // Create the file in write mode
            file = fopen(outputFile, "w");
            if (!file) {
                printStr("Error: Failed to create output file.");
                Task_yield();
                continue;
            }
            fclose(file); // Close the file after creation
            printStr("Output file created successfully.");
        } else {
            fclose(file); // Close the file if it already exists
        }

        // Now open the file in append mode to write data
        file = fopen(outputFile, "a");
        if (!file) {
            printStr("Error: Failed to open output file in append mode.");
            Task_yield();
            continue;
        }

        // Write data from the queue to the file
        while (queue[queue_ptr] != NULL) {
            fprintf(file, "%s\n", queue[queue_ptr]); // Append data to the file
            queue[queue_ptr] = NULL;                // Mark this queue entry as processed
            queue_ptr++;
        }


        fflush(file);   // Ensure all data is written to the file
        fclose(file);   // Close the file to release resources
        queue_ptr = 0;  // Reset queue pointer

        printStr("All data from mock_memory_queue written to output file.");

        // Task_sleep(1000);
        Task_yield();
    }
}
