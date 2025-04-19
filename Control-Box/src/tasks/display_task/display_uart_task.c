#include "../../common/common.h"

// Local UART2 handle
static UART2_Handle uart;
static UART2_Params uartParams;

/**
 * Constructs the Display UART task using static allocation.
 */
Task_Handle displayUart_constructTask()
{
    Task_Params TaskParams;
    Task_Params_init(&TaskParams);
    TaskParams.stack = g_displayUartTaskStack;
    TaskParams.stackSize = DISPLAY_UART_TASK_STACK_SIZE;
    TaskParams.priority = DISPLAY_UART_TASK_PRIORITY;

    Task_construct(&g_DisplayUartTaskStruct, displayUart_executeTask, &TaskParams, NULL);
    return (Task_Handle)&g_DisplayUartTaskStruct;
}

/**
 * Sends a complete Nextion command over UART.
 * All commands **must** end with three 0xFF bytes (per Instruction Set section 1.1).
 */
static void sendCmd(const char* cmd)
{
    UART2_write(uart, cmd, strlen(cmd));
    uint8_t end[3] = {0xFF, 0xFF, 0xFF};
    UART2_write(uart, end, 3);
}

/**
 * Sends an integer to a text component by name (ex: heartRateText.txt="75").
 * These object names (like `heartRateText`) are **manually renamed in Nextion Editor**.
 */
static void sendIntToText(const char* objName, int val)
{
    char buf[64];
    snprintf(buf, sizeof(buf), "%s.txt=\"%d\"", objName, val);
    sendCmd(buf);

    // Confirm what was sent
    System_printf("[Display UART] Sent %s → %d\n", objName, val);
    System_flush();
}

/**
 * Sends a `get vaX` command to read a threshold variable from Nextion (Instruction Set section 3.6).
 * `va0`–`va3`are used to store threshold values.
 * The response format is: 0x71 + 4-byte value + 3x 0xFF terminators.
 */
static void queryThreshold(const char* varName, volatile int* target, const char* label)
{
    char cmd[32];
    snprintf(cmd, sizeof(cmd), "get %s", varName);
    sendCmd(cmd);

    uint8_t response[8];
    int bytesRead = UART2_readTimeout(uart, response, sizeof(response), &(struct timespec){.tv_sec = 0, .tv_nsec = 5000000});

    if (bytesRead >= 7 && response[0] == 0x71)
    {
        // Extract 4-byte integer value (little endian)
        *target = response[1] | (response[2] << 8) | (response[3] << 16) | (response[4] << 24);
        System_printf("[Display UART] Read %s = %d\n", label, *target);
    }
    else
    {
        System_printf("[Display UART] Failed to read %s (bytesRead = %d, header = 0x%02X)\n",
                      varName, bytesRead, response[0]);
    }
    System_flush();
}

/**
 * Main Display UART loop — runs forever.
 * - Pushes current HR and RR to GUI (page: main)
 * - Pulls thresholds (va0–va3) set by user on GUI (page: menu)
 * All object and variable names are matched to Nextion Editor setup.
 */
void displayUart_executeTask(UArg arg0, UArg arg1)
{
    (void)arg0;
    (void)arg1;

    UART2_Params_init(&uartParams);
    uartParams.baudRate = 115200; // Required by Nextion Intelligent model
    uartParams.readMode = UART2_Mode_BLOCKING;
    uartParams.writeMode = UART2_Mode_BLOCKING;
    uartParams.readTimeout = 100;
    uartParams.writeTimeout = 100;

    uart = UART2_open(CONFIG_DISPLAY_UART, &uartParams);
    if (uart == NULL)
    {
        System_abort("[Display UART] Failed to open UART\n");
    }

    System_printf("[Display UART] Initialized and running\n");
    System_flush();

    while (1)
    {
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
