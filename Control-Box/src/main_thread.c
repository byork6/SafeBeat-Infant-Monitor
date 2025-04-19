#include "common/common.h"

// DS1307 I2C address
#define DS1307_ADDR         0x68

I2C_Handle i2c;
I2C_Params i2cParams;
I2C_Transaction i2cTransaction;
uint8_t txBuffer[1];
uint8_t rxBuffer[7]; // 7 bytes of time info

void realTimeClockI2C_init (){
    I2C_Params_init(&i2cParams);
    i2cParams.bitRate = I2C_400kHz;
}

// Convert from BCD to decimal
uint8_t bcdToDec(uint8_t val) {
    return ((val >> 4) * 10) + (val & 0x0F);
}

void readRTCAndPrintTime() {

    i2c = I2C_open(CONFIG_RTC_I2C, &i2cParams);
    if (i2c == NULL) {
        printf("I2C open failed!\n");
        return;
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

        printf("Time: %02d:%02d:%02d Date: %02d/%02d/20%02d\n", hour, min, sec, month, day, year);
    } else {
        printf("I2C read failed!\n");
    }

    I2C_close(i2c);
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

void *mainThread(void *arg0){

    // Retrieve power-reset reason
    PowerCC26X2_ResetReason resetReason = PowerCC26X2_getResetReason();

    // If we are waking up from shutdown, we do something extra.
    if (resetReason == PowerCC26X2_RESET_SHUTDOWN_IO){
        // Application code must always disable the IO latches when coming out of shutdown
        PowerCC26X2_releaseLatches();
    }

    // Initialize the board with TI-Driver config & custom config if needed
    initBOARD();
    
    // Enable the power policy -- If all tasks are blocked the idleLoop will execute the power policy. 
    // If the powerbutton is pushed Power_shutdown() will be forced.
    Power_enablePolicy();
    
    // createAllResources();

    I2C_init();

    realTimeClockI2C_init();

    // setRTC(0, 8, 30, 5, 4, 17, 25);

    // Ideally called from within a task
    while (1){
        readRTCAndPrintTime();
        volatile uint32_t i, j;
        for (i = 0; i < 100; i++) {
            for (j = 0; j < 4000; j++) {
                __asm(" nop");
            }    
        }
    }

    return NULL;
}
