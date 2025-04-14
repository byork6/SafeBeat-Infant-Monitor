#include "../../common/common.h"

Task_Handle testGpio_constructTask(uint32_t pinNumber, uint32_t taskPriority, Task_Struct *taskStruct, uint8_t *taskStack){
    // Declare TaskParams struct name
    Task_Params TaskParams;

    // Initialize TaskParams and set paramerters.
    Task_Params_init(&TaskParams);
    // Stack array
    TaskParams.stack = taskStack;
    // Stack array size
    TaskParams.stackSize = TEST_GPIO_TASK_STACK_SIZE;
    // Stack task TI-RTOS priority
    TaskParams.priority = taskPriority;
    // arg0 and ar1 passed to exectution function for the created task. Use 0 if arg is unused.
    // you can pass variables or pointers to structs for larger data objects.
    TaskParams.arg0 = pinNumber;
    TaskParams.arg1 = 0;    // 0 used if arg is unused.

    // Construct the TI-RTOS task using the API
    Task_construct(taskStruct, testGpio_executeTask, &TaskParams, NULL);
    return (Task_Handle)taskStruct;
}

void testGpio_executeTask(UArg arg0, UArg arg1){
    (void)arg1;     // suppresses warnings for unused arg
    int i  = 0;

    GPIO_setConfig(CONFIG_AUDIO_PWM, GPIO_SET_OUT_AND_DRIVE_LOW);
    GPIO_write(CONFIG_AUDIO_PWM, DRIVE_GPIO_HIGH);
    printf("Buzzer ON (start)\n");

    Task_sleep(MS_TO_TICKS(3000));  // Buzzer on for 3 seconds

    GPIO_write(CONFIG_AUDIO_PWM, DRIVE_GPIO_LOW);
    printf("Buzzer OFF (after 3 sec)\n");


    GPIO_setConfig(arg0, GPIO_SET_OUT_AND_DRIVE_LOW);
    
    printf("Entering testGpio_executeTask()...\n");
    while (1){
        i++;
        printf("testGpio Count: %d\n", i);
        GPIO_toggle(arg0);
        /*if (g_heartRate < LOW_HEART_RATE_THRESHOLD_BPM || g_heartRate > HIGH_HEART_RATE_THRESHOLD_BPM) {
            printf("Heart rate out of range! Buzzer ON\n");

            GPIO_write(CONFIG_AUDIO_PWM, DRIVE_GPIO_HIGH);
            Task_sleep(MS_TO_TICKS(3000));  // Buzzer ON for 3 seconds

            GPIO_write(CONFIG_AUDIO_PWM, DRIVE_GPIO_LOW);
            printf("Buzzer OFF\n");
        }*/
        Task_sleep(g_taskSleepDuration);
    }
}
