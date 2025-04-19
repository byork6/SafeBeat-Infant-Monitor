#include "common.h"

// Define global variables
// Global task sleep duration in ticks -- Global variable used to change all task delays dynamically for temperature_monitoring_task.
int g_taskSleepDuration = DEFAULT_TASK_SLEEP_DURATION;
CircularQueue sdMemQueue = { .head = 0, .tail = 0, .size = 0 };
CircularQueue displayMemQueue = { .head = 0, .tail = 0, .size = 0 };

// DS1307 I2C address
#define DS1307_ADDR         0x68
I2C_Handle i2c;
I2C_Params i2cParams;
I2C_Transaction i2cTransaction;
uint8_t txBuffer[1];
uint8_t rxBuffer[7];

// --- BOARD INIT --- //
void initBOARD(void){
    Power_init();
    GPIO_init();
    SPI_init();
    SDFatFS_init();
    Temperature_init();
    I2C_init();
    realTimeClockI2C_init();
}

// --- RTOS FUNCTION DEFINITIONS --- //
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

    // Task 5 --- Priority = 5
    g_uartBridgeTaskHandle = uartBridge_constructTask();
    
    // Task 6 --- Priority = 6
    g_temperatureMonitoringTaskHandle = temperatureMonitoring_constructTask();
}

// --- DISPLAY & SD CARD FUNCTION DEFINITIONS --- //
void logData(int heartRate, int respiratoryRate, const char* timestamp) {
    char logEntry[128] = {0};  // Temporary buffer for formatted string

    snprintf(logEntry, sizeof(logEntry), "Heart Rate: %d, Respiratory Rate: %d, Timestamp: %s\n",
            heartRate, respiratoryRate, timestamp);

    // FOR DEBUGGING
    printf("PASSED TO LOG DATA --- Heart Rate: %d, Respiratory Rate: %d, Timestamp: %s\n", heartRate, respiratoryRate, timestamp);

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

// --- RTC FUNCTION DEFINITIONS --- //
void realTimeClockI2C_init (){
    I2C_Params_init(&i2cParams);
    i2cParams.bitRate = I2C_400kHz;
}

// Convert from BCD to decimal
uint8_t bcdToDec(uint8_t val) {
    return ((val >> 4) * 10) + (val & 0x0F);
}

char* readRTC() {
    static char rtcString[64];

    i2c = I2C_open(CONFIG_RTC_I2C, &i2cParams);
    if (i2c == NULL) {
        snprintf(rtcString, sizeof(rtcString), "I2C open failed!");
        return rtcString;
    }

    // Step 1: Point to register 0x00
    txBuffer[0] = 0x00;
    i2cTransaction.targetAddress = DS1307_ADDR;
    i2cTransaction.writeBuf = txBuffer;
    i2cTransaction.writeCount = 1;
    i2cTransaction.readBuf = rxBuffer;
    i2cTransaction.readCount = 7;

    if (I2C_transfer(i2c, &i2cTransaction)) {
        uint8_t sec  = bcdToDec(rxBuffer[0] & 0x7F);
        uint8_t min  = bcdToDec(rxBuffer[1]);
        uint8_t hour = bcdToDec(rxBuffer[2] & 0x3F);
        uint8_t day  = bcdToDec(rxBuffer[4]);
        uint8_t month= bcdToDec(rxBuffer[5]);
        uint8_t year = bcdToDec(rxBuffer[6]);
        snprintf(rtcString, sizeof(rtcString),
                 "Time: %02d:%02d:%02d Date: %02d/%02d/20%02d",
                 hour, min, sec, month, day, year);
    } else {
        snprintf(rtcString, sizeof(rtcString), "RTC read failed!");
    }

    I2C_close(i2c);
    return rtcString;
}

void setRTC(uint8_t hour,  uint8_t min, uint8_t sec, uint8_t day,
            uint8_t month, uint8_t date,  uint8_t year){
    uint8_t txBuffer[8];

    i2c = I2C_open(CONFIG_RTC_I2C, &i2cParams);
    if (i2c == NULL) {
        printf("I2C open failed!\n");
        return;
    }

    // Set starting register to 0x00
    txBuffer[0] = 0x00;
    txBuffer[1] = ((sec / 10) << 4) | (sec % 10);
    txBuffer[2] = ((min / 10) << 4) | (min % 10);
    txBuffer[3] = ((hour / 10) << 4) | (hour % 10);
    txBuffer[4] = ((day / 10) << 4) | (day % 10);      // Day of week (1–7)
    txBuffer[5] = ((date / 10) << 4) | (date % 10);
    txBuffer[6] = ((month / 10) << 4) | (month % 10);
    txBuffer[7] = ((year / 10) << 4) | (year % 10);    // Last two digits

    i2cTransaction.targetAddress = DS1307_ADDR;
    i2cTransaction.writeBuf = txBuffer;
    i2cTransaction.writeCount = 8;
    i2cTransaction.readBuf = NULL;
    i2cTransaction.readCount = 0;

    if (I2C_transfer(i2c, &i2cTransaction)) {
        printf("RTC time set successfully\n");
    } else {
        printf("Failed to set RTC time\n");
    }

    I2C_close(i2c);
}

// --- GPIO FUNCTION DEFINITIONS --- //
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
