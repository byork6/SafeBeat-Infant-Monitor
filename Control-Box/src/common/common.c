#include "common.h"
#include "src/tasks/temperature_monitoring_task/temperature_monitoring_task.h"

// Declare global vars
char g_fatfsPrefix[] = "fat";
// Global task sleep duration in ticks -- Global variable used to change task delay dynamically for temperature_monitoring_task.
int g_taskSleepDuration = DEFAULT_TASK_SLEEP_DURATION;

void createAllResources() {
    // Create power button semaphore
    // TODO: Change Semaphore_create to Semaphore_construct for static memory allocation and move creation to power task creation task
    Semaphore_Params powerShutdownSemaphoreParams;
    Semaphore_Params_init(&powerShutdownSemaphoreParams);
    g_powerShutdownSemaphore = Semaphore_create(0, &powerShutdownSemaphoreParams, NULL);
    // Enable power button interrupts and set callback
    GPIO_enableInt(CONFIG_GPIO_PWR_BTN);
    GPIO_setCallback(CONFIG_GPIO_PWR_BTN, powerShutdownISR);

    // Create tasks for TI-RTOS7 --- Order them from lowest to highest priority.
    // Task 1 - Pri = 1
    g_powerShutdownTaskHandle = powerShutdown_constructTask();

    // Task 2 --- Priority = 2
    // TODO: Test microSD Driver with physical connection
    // g_microSDWriteTaskHandle = microSDWrite_constructTask();

    // Task 3 --- Priority = 3
    g_temperatureMonitoringTaskHandle = temperatureMonitoring_constructTask();
    
    // Task 4 --- Priority = 3
    g_task1Handle = testGpio_constructTask(6, 3, &g_TestGpioTaskStruct1, (uint8_t *)g_testGpioTaskStack1);

    // Task 5 --- Priority = 3
    g_task2Handle = testGpio_constructTask(7, 3, &g_TestGpioTaskStruct2, (uint8_t *)g_testGpioTaskStack2);
}

void testGpio(uint32_t pin_config_index){
    //////////////// TEST CODE  ONLY ////////////////
    
    // Input validation (Pins 5-30 only valid pins)
    if (pin_config_index < 5 || pin_config_index > 30){
        exit(1);
    }

    // 1 second delay
    uint32_t time = 1;

    // Call driver init functions from SDK
    GPIO_init();

    // Initialize GPIO pins
    GPIO_setConfig(pin_config_index, GPIO_SET_OUT_AND_DRIVE_LOW);

    while (1)
    {
        sleep(time);
        GPIO_toggle(pin_config_index);
    }
}

void printVar(const char *varName, void *var, char type) {
    if (varName == NULL) {
        varName = "foo"; // Default name if none is provided
    }

    switch (type) {
        case 'd': // Integer
            printf("Variable \"%s\" value: %d\n", varName, *(int *)var);
            break;

        case 'f': // Float
            printf("Variable \"%s\" value: %.2f\n", varName, *(float *)var);
            break;

        case 'c': // Character
            printf("Variable \"%s\" value: %c\n", varName, *(char *)var);
            break;

        case 's': // String
            printf("Variable \"%s\" value: %s\n", varName, (char *)var);
            break;

        case 'u': // Unsigned int
            printf("Variable \"%s\" value: %u\n", varName, *(unsigned int *)var);
            break;

        case 'U': // uint32_t
            printf("Variable \"%s\" value: %u\n", varName, *(uint32_t *)var);
            break;

        case 'i': // int_fast16_t
            printf("Variable \"%s\" value: %d\n", varName, *(int_fast16_t *)var);
            break;
        
        case 'I': // int16_t
            printf("Variable \"%s\" value: %d\n", varName, *(int16_t *)var);
            break;

        default:
            printf("Unsupported type for variable \"%s\"\n", varName);
    }
    fflush(stdout); // Ensure output is flushed immediately
}

void printStr(const char *str) {
    printf("%s", str ? str : "NULL");
    fflush(stdout);
}

int32_t fatfs_getFatTime(void) {
    time_t rawTime;
    struct tm *timeInfo;
    uint32_t fatTime;

    // Get the current time
    time(&rawTime);
    timeInfo = localtime(&rawTime);

    // Convert to FAT time format
    fatTime = ((uint32_t)(timeInfo->tm_year - 80) << 25) | // Year since 1980
              ((uint32_t)(timeInfo->tm_mon + 1) << 21)    | // Month (1-12)
              ((uint32_t)timeInfo->tm_mday << 16)        | // Day (1-31)
              ((uint32_t)timeInfo->tm_hour << 11)        | // Hour (0-23)
              ((uint32_t)timeInfo->tm_min << 5)          | // Minute (0-59)
              ((uint32_t)(timeInfo->tm_sec / 2));          // Second / 2 (0-29)

    return fatTime;
}