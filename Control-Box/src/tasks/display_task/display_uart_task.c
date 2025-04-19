#include "../../common/common.h"

// Local UART2 handle
static UART2_Handle uart;
static UART2_Params uartParams;

static UART2_Handle uartHandle;

Task_Handle displayUart_constructTask(void){
    Task_Params taskParams;
    Task_Params_init(&taskParams);
    taskParams.stack = g_displayUartTaskStack;
    taskParams.stackSize = DISPLAY_UART_TASK_STACK_SIZE;
    taskParams.priority = DISPLAY_UART_TASK_PRIORITY;

    Task_construct(&g_DisplayUartTaskStruct, displayUart_executeTask, &taskParams, NULL);
    return (Task_Handle)&g_DisplayUartTaskStruct;
}

void displayUart_executeTask(UArg arg0, UArg arg1){
    (void)arg0;
    (void)arg1;

    UART2_Params_init(&uartParams);
    uartParams.baudRate = 115200; // Required by Nextion Intelligent model
    uartParams.readMode = UART2_Mode_BLOCKING;
    uartParams.writeMode = UART2_Mode_BLOCKING;

    uart = UART2_open(CONFIG_DISPLAY_UART, &uartParams);
    if (uart == NULL)
    {
        printf("[Display UART] Failed to open UART\n");
    }

    printf("[Display UART] Initialized and running\n");

    while (1){
        // These object names were renamed in Nextion Editor
        sendIntToText("heartRateText", g_currentHeartRate);
        sendIntToText("respRateText", g_currentRespiratoryRate);

        // These variables were NOT renamed and are written from menu page buttons
        queryThreshold("va0", &g_hrThresholdLow, "HR LOW");
        queryThreshold("va1", &g_hrThresholdHigh, "HR HIGH");
        queryThreshold("va2", &g_rrThresholdLow, "RR LOW");
        queryThreshold("va3", &g_rrThresholdHigh, "RR HIGH");

        // Updates every 2 seconds
        Task_sleep(MS_TO_TICKS(2000));
    }
}

void displayUart_updateText(const char *msg){
    char buffer[64];
    snprintf(buffer, sizeof(buffer), "ecgText.txt:\"%s\"\xFF\xFF\xFF", msg);

    size_t bytesWritten;
    UART2_write(uartHandle, buffer, strlen(buffer), &bytesWritten);
}


void sendCmd(const char* cmd){
    size_t bytesWritten;
    UART2_write(uartHandle, cmd, strlen(cmd), &bytesWritten);

    uint8_t end[3] = {0xFF, 0xFF, 0xFF};
    UART2_write(uartHandle, end, 3, &bytesWritten);
}

void sendIntToText(const char* objName, int val){
    char buf[64];
    snprintf(buf, sizeof(buf), "%s.txt=\"%d\"", objName, val);
    sendCmd(buf);

    // Confirm what was sent
    printf("[Display UART] Sent %s → %d\n", objName, val);
}

void queryThreshold(const char* varName, volatile int* target, const char* label){
    char cmd[32];
    snprintf(cmd, sizeof(cmd), "get %s", varName);
    sendCmd(cmd);

    uint8_t response[8];
    int_fast16_t status;
    size_t bytesRead;
    status = UART2_readTimeout(uart, response, sizeof(response), &bytesRead, 10000);

    if (bytesRead >= 7 && response[0] == 0x71)
    {
        // Extract 4-byte integer value (little endian)
        *target = response[1] | (response[2] << 8) | (response[3] << 16) | (response[4] << 24);
        printf("[Display UART] Read %s = %d\n", label, *target);
    }
    else
    {
        printf("[Display UART] Failed to read %s (bytesRead = %d, header = 0x%02X)\n",
                      varName, bytesRead, response[0]);
    }
}

