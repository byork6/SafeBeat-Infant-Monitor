#include "../../common/common.h"

Task_Struct g_DisplayUartTaskStruct;
Task_Handle g_displayUartTaskHandle;
uint8_t g_displayUartTaskStack[DISPLAY_UART_TASK_STACK_SIZE];

static UART2_Handle uartHandle;

void displayUart_updateText(const char *msg)
{
    char buffer[64];
    snprintf(buffer, sizeof(buffer), "ecgText.txt=\"%s\"\xFF\xFF\xFF", msg);
    UART_write(uartHandle, buffer, strlen(buffer));
}

void displayUart_executeTask(UArg a0, UArg a1)
{
    UART2_Params uartParams;
    UART2_Params_init(&uartParams);
    uartParams.baudRate = 9600;
    uartParams.writeMode = UART2_Mode_BLOCKING;
    uartParams.readMode = UART2_Mode_BLOCKING;

    uartHandle = UART2_open(CONFIG_UART_0, &uartParams);
    if (uartHandle == NULL) {
        System_abort("UART_open failed");
    }

    while (1)
    {
        // Optional: read from display queue if you enable it
        Task_sleep(MS_TO_TICKS(1000));
    }
}

Task_Handle displayUart_constructTask(void)
{
    Task_Params taskParams;
    Task_Params_init(&taskParams);
    taskParams.stack = g_displayUartTaskStack;
    taskParams.stackSize = DISPLAY_UART_TASK_STACK_SIZE;
    taskParams.priority = DISPLAY_UART_TASK_PRIORITY;

    Task_construct(&g_DisplayUartTaskStruct, displayUart_executeTask, &taskParams, NULL);
    return (Task_Handle)&g_DisplayUartTaskStruct;
}
