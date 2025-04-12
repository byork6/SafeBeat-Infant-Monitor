#include "common.h"

// Define global variables
// Global task sleep duration in ticks -- Global variable used to change all task delays dynamically for temperature_monitoring_task.
int g_taskSleepDuration = DEFAULT_TASK_SLEEP_DURATION;
CircularQueue sdMemQueue = { .head = 0, .tail = 0, .size = 0 };
CircularQueue displayMemQueue = { .head = 0, .tail = 0, .size = 0 };

void createAllResources() {
    // Create tasks for TI-RTOS7 --- Order them from lowest to highest priority.
    // Task 1 --- Priority = 1
    g_powerShutdownTaskHandle = powerShutdown_constructTask();

    // Task 2 --- Priority = 2
    g_microSdWriteTaskHandle = microSdWrite_constructTask();

    // Task 3 --- Priority = 3
    g_displayDriverTaskHandle = displayDriver_constructTask();
    
    // Task 4 --- Priority = 4
    g_task1Handle = testGpio_constructTask(6, RED_LIGHT_BLINK_PRIORITY, &g_TestGpioTaskStruct1, (uint8_t *)g_testGpioTaskStack1);

    // Task 5 --- Priority = 4
    g_task2Handle = testGpio_constructTask(7, GREEN_LIGHT_BLINK_PRIORITY, &g_TestGpioTaskStruct2, (uint8_t *)g_testGpioTaskStack2);

    // Task 5 --- Priority = 5
    g_uartBridgeTaskHandle = uartBridge_constructTask();
    
    // Task 6 --- Priority = 6
    g_temperatureMonitoringTaskHandle = temperatureMonitoring_constructTask();
}

void logData(int heartRate, int respiratoryRate, const char* timestamp) {
    char logEntry[128] = {0};  // Temporary buffer for formatted string

    snprintf(logEntry, sizeof(logEntry), "Heart Rate: %d, Respiratory Rate: %d, Timestamp: %s\n",
            heartRate, respiratoryRate, timestamp);

    appendToSdAndDisplayQueue(logEntry);  // Append formatted string to circular queues
}

void appendToSdAndDisplayQueue(const char *data) {
    int len = strlen(data);

    // Append to sd queue
    if (sdMemQueue.size + len >= CIRCULAR_QUEUE_SIZE) {
        printf("SD queue full! Data loss possible.\n");
    }
    else{
        for (int i = 0; i < len; i++) {
            sdMemQueue.buffer[sdMemQueue.tail] = data[i];
            sdMemQueue.tail = (sdMemQueue.tail + 1) % CIRCULAR_QUEUE_SIZE;
        }
        sdMemQueue.size += len;
        sdMemQueue.buffer[sdMemQueue.tail] = '\0';
    }

    // Append to display queue
    // TODO: ONCE FUNCTION IS WRITTEN TO EMPTY THIS QUEUE YOU CAN UNCOMMENT
    // if (displayMemQueue.size + len >= CIRCULAR_QUEUE_SIZE) {
    //     printf("Display queue full! Data loss possible.\n");
    //     return;
    // }
    // else{
    //     for (int i = 0; i < len; i++) {
    //         displayMemQueue.buffer[displayMemQueue.tail] = data[i];
    //         displayMemQueue.tail = (displayMemQueue.tail + 1) % CIRCULAR_QUEUE_SIZE;
    //     }
    //     displayMemQueue.size += len;
    //     displayMemQueue.buffer[displayMemQueue.tail] = '\0';
    // }
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
