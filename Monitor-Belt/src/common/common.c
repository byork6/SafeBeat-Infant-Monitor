#include "common.h"

// Define global variables
// Global task sleep duration in ticks -- Global variable used to change all task delays dynamically for temperature_monitoring_task.
int g_taskSleepDuration = DEFAULT_TASK_SLEEP_DURATION;

void logData(int heartRate, int respiratoryRate, const char* timestamp) {
    char logEntry[128] = {0};  // Temporary buffer for formatted string

    snprintf(logEntry, sizeof(logEntry), "Heart Rate: %d, Respiratory Rate: %d, Timestamp: %s\n",
            heartRate, respiratoryRate, timestamp);

    appendToSdAndDisplayQueue(logEntry);  // Append formatted string to circular queues
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
