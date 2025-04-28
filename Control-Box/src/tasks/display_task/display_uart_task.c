#include "../../common/common.h"

// UART2 handle and RX buffer
static UART2_Handle uart;
static UART2_Params uartParams;
static uint8_t rxBuffer[8]; // Expecting threshold responses to be 8 bytes

// Threshold values
static int hrLower = 0, hrUpper = 0, rrLower = 0, rrUpper = 0;

// Forward declaration of the callback
static void uartCallback(UART2_Handle handle, void *buffer, size_t count, void *userArg, int_fast16_t status);

Task_Handle displayUart_constructTask(void) {
    Task_Params taskParams;
    Task_Params_init(&taskParams);
    taskParams.stack = g_displayUartTaskStack;
    taskParams.stackSize = DISPLAY_UART_TASK_STACK_SIZE;
    taskParams.priority = DISPLAY_UART_TASK_PRIORITY;
    Task_construct(&g_DisplayUartTaskStruct, displayUart_executeTask, &taskParams, NULL);
    return (Task_Handle)&g_DisplayUartTaskStruct;
}

// Helper to send a properly formatted command
void sendCmd(const char* cmd) {
    size_t bytesWritten;
    UART2_write(uart, cmd, strlen(cmd), &bytesWritten);
    const char* end = "/xFF/xFF/xFF";
    // uint8_t end[3] = {0xFF, 0xFF, 0xFF};
    UART2_write(uart, end, strlen(end), &bytesWritten);
    printf("Sending: %s%s\n", cmd, end);
}

// Helper to write an int to a text component
void sendIntToText(const char* objName, int val) {
    char buf[64];
    snprintf(buf, sizeof(buf), "%s.txt=\"%d\"", objName, val);
    sendCmd(buf);
    printf("[SEND] %s = %d\n", objName, val);
}

// Helper to send an integer to a numeric component (e.g., n0.val=123)
void sendIntToNumber(const char* objName, int val) {
    char buf[64];
    snprintf(buf, sizeof(buf), "%s.val=%d", objName, val);
    sendCmd(buf);
    printf("[SEND NUM] %s = %d\n", objName, val);
}

// Helper to request and parse a number value from display
int queryNumericVar(const char* varName) {
    char cmd[32];
    snprintf(cmd, sizeof(cmd), "get %s", varName);
    sendCmd(cmd);

    uint8_t response[8];
    size_t bytesRead;
    int_fast16_t status = UART2_readTimeout(uart, response, sizeof(response), &bytesRead, 10000);

    if (bytesRead >= 7 && response[0] == 0x71) {
        int result = response[1] | (response[2] << 8) | (response[3] << 16) | (response[4] << 24);
        return result;
    } else {
        printf("[QUERY] Failed to read %s (bytesRead = %d, header = 0x%02X)\n",
               varName, bytesRead, response[0]);
        return -1;
    }
}

void writeHex(uint8_t hexVal){
        size_t bytesWritten;
        UART2_write(uart, &hexVal, 1, &bytesWritten);
}

void displayUart_executeTask(UArg a0, UArg a1) {
    UART2_Params_init(&uartParams);
    uartParams.baudRate = 115200;
    uartParams.readMode = UART2_Mode_BLOCKING;
    uartParams.writeMode = UART2_Mode_BLOCKING;
    uart = UART2_open(CONFIG_DISPLAY_UART, &uartParams);

    if (!uart) {
        printf("[UART] Failed to open UART\n");
        return;
    }

    printf("[UART] UART opened successfully\n");

    GPIO_write(CONFIG_BUZZER, DRIVE_GPIO_LOW);

    // Force startup to main page
    // sendCmd("page startup");
    Task_sleep(MS_TO_TICKS(500));

    int loopCount = 0;
    while (1) {
        printf("In display task\n");
        // Simulate values from AFE driver
        int hr = 80 + (loopCount % 10);  // e.g., 80 to 89
        int rr = 20 + (loopCount % 5);   // e.g., 20 to 24

        // sendIntToNumber("n0", hr); // Send HR to a numeric component like n4
        // sendIntToNumber("n1", rr); // Send RR to a numeric component like n5

        // uint8_t data = 0xFF;

        writeHex(0x6E);
        writeHex(0x30);
        writeHex(0x2E);
        writeHex(0x76);
        writeHex(0x61);
        writeHex(0x6C);
        writeHex(0x3D);
        writeHex(0x32);
        writeHex(0x33);

        Task_sleep(MS_TO_TICKS(200));  // Delay before next update

        writeHex(0xff);
        writeHex(0xff);
        writeHex(0xff);


        // sendCmd("n0.val=100");
        // const char* cmd = "t5.txt";
        // printf("Wrote cmd: %s\n", cmd);
        // Task_sleep(g_taskSleepDuration);
        // cmd = "=\"";
        // UART2_write(uart, cmd, strlen(cmd), &bytesWritten);
        // printf("Wrote cmd: %s\n", cmd);
        // Task_sleep(g_taskSleepDuration);
        // cmd = "100";
        // UART2_write(uart, cmd, strlen(cmd), &bytesWritten);
        // printf("Wrote cmd: %s\n", cmd);
        // Task_sleep(g_taskSleepDuration);
        // cmd = "\"";
        // UART2_write(uart, cmd, strlen(cmd), &bytesWritten);
        // printf("Wrote cmd: %s\n", cmd);
        // // Task_sleep(g_taskSleepDuration);
        // cmd = "/xFF/xFF/xFF";
        // UART2_write(uart, cmd, strlen(cmd), &bytesWritten);
        // printf("Wrote cmd: %s\n", cmd);
        // Task_sleep(g_taskSleepDuration);

        // Periodically fetch threshold values from display
        // if (loopCount % 5 == 0) {
        //     hrLower = queryNumericVar("HRL");
        //     hrUpper = queryNumericVar("HRU");
        //     rrLower = queryNumericVar("RRL");
        //     rrUpper = queryNumericVar("RRU");
        //     printf("[THRESHOLDS] HR: %d-%d | RR: %d-%d\n", hrLower, hrUpper, rrLower, rrUpper);
        // }

        // Check if HR or RR is out of threshold range
        /*if (hr < hrLower || hr > hrUpper || rr < rrLower || rr > rrUpper) {
            // --- BUZZER CODE --- //
            GPIO_write(CONFIG_BUZZER, DRIVE_GPIO_HIGH);
            printf("Buzzer ON\n");

            Task_sleep(g_taskSleepDuration);
            Task_sleep(g_taskSleepDuration);
            Task_sleep(g_taskSleepDuration);
            Task_sleep(g_taskSleepDuration);

            GPIO_write(CONFIG_BUZZER, DRIVE_GPIO_LOW);
            printf("Buzzer OFF\n");
        }*/

        loopCount++;
        Task_sleep(MS_TO_TICKS(2000));  // Delay before next update
    }
}

static void uartCallback(UART2_Handle handle, void *buffer, size_t count, void *userArg, int_fast16_t status) {
    printf("[UART] Callback received %u bytes:\n", (unsigned int)count);
    uint8_t *data = (uint8_t *)buffer;
    for (size_t i = 0; i < count; i++) {
        printf("0x%02X ", data[i]);
    }
    printf("\n");

    if (count == 7 && data[0] == 0x65) {
        printf("[Touch] Page=%d, Component=%d, Event=0x%02X\n", data[1], data[2], data[3]);
    } else {
        printf("[Touch] Not a valid 0x65 packet\n");
    }

    UART2_read(uart, rxBuffer, sizeof(rxBuffer), NULL);
}
