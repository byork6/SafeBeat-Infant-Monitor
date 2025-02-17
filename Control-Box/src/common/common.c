#include "common.h"

// Declare global vars
// Global task sleep duration in ticks -- Global variable used to change task delay dynamically for temperature_monitoring_task.
int g_taskSleepDuration = DEFAULT_TASK_SLEEP_DURATION;

void createAllResources() {
    // Create tasks for TI-RTOS7 --- Order them from lowest to highest priority.
    // Task 1 --- Priority = 1
    g_powerShutdownTaskHandle = powerShutdown_constructTask();

    // Task 2 --- Priority = 2
    g_microSDWriteTaskHandle = microSDWrite_constructTask();
    
    // Task 3 --- Priority = 3
    g_task1Handle = testGpio_constructTask(6, RED_LIGHT_BLINK_PRIORITY, &g_TestGpioTaskStruct1, (uint8_t *)g_testGpioTaskStack1);

    // Task 4 --- Priority = 3
    g_task2Handle = testGpio_constructTask(7, GREEN_LIGHT_BLINK_PRIORITY, &g_TestGpioTaskStruct2, (uint8_t *)g_testGpioTaskStack2);

    // Task 5 --- Priority = 4
    g_temperatureMonitoringTaskHandle = temperatureMonitoring_constructTask();
}

void logData(int heartRate, int respiratoryRate, const char* timestamp) {
    char logEntry[128] = {0};  // Temporary buffer for formatted string

    snprintf(logEntry, sizeof(logEntry), "Heart Rate: %d, Respiratory Rate: %d, Timestamp: %s\n",
            heartRate, respiratoryRate, timestamp);

    appendToSDQueue(logEntry);  // Append formatted string to queue
    //TODO: Add appendToDisplayQueue(logEntry); function. This will use a separate circular queue for display screen.
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

void testGpio(uint32_t pin_config_index){   
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
