#include "../../common/common.h"
#include "EVE2_Library/hw_api.h"
#include "ti_drivers_config.h"

// Define global variable for spi handle --- declared in common.h using extern

Task_Handle displayDriver_constructTask(){
    // Construct task
    Task_Params TaskParams;

    Task_Params_init(&TaskParams);
    TaskParams.stack = g_displayDriverTaskStack;
    TaskParams.stackSize = DISPLAY_DRIVER_TASK_STACK_SIZE;
    TaskParams.priority = DISPLAY_DRIVER_TASK_PRIORITY;
    TaskParams.arg0 = 0;
    TaskParams.arg1 = 0;

    Task_construct(&g_DisplayDriverTaskStruct, displayDriver_executeTask, &TaskParams, NULL);
    g_displayDriverTaskHandle = (Task_Handle)&g_DisplayDriverTaskStruct;
    return (Task_Handle)&g_DisplayDriverTaskStruct;
}

//Minimal Test version 
void displayDriver_executeTask(UArg arg0, UArg arg1) {
    (void)arg0;
    (void)arg1;

    printf("[Display Task] Starting...\n");

    SPI_Params_init(&g_spiDisplayParams);
    g_spiDisplayParams.bitRate = 1000000;
    g_spiDisplayParams.dataSize = 8;
    g_spiDisplayParams.frameFormat = SPI_POL0_PHA0;

    do{
        g_spiDisplayHandle = SPI_open(CONFIG_DISPLAY_SPI, &g_spiDisplayParams);
        if (g_spiDisplayHandle == NULL) {
            printf("SPI_open FAILED\n");
            Task_sleep(g_taskSleepDuration);
        }
        else{
            printf("SPI_open SUCCESS\n");
            break;
        }
    }while(1);

    int initResult = FT81x_Init(DISPLAY_70, BOARD_EVE2, TOUCH_TPC);
    // if (!initResult) {
    //     printf("[Display Task] FT81x_Init failed. Aborting task.\n");
    //     return;
    // }

    // printf("[Display Task] FT81x_Init completed successfully.\n");

    // wr8(REG_PWM_DUTY + RAM_REG, 128);  // Backlight
    // wr8(REG_PCLK + RAM_REG, 1);        // Pixel clock

    // printf("[Display Task] Drawing MatrixOrbital test screen...\n");
    // ClearScreen();
    // MakeScreen_MatrixOrbital(30);

    // printf("[Display Task] Display setup complete. Idling now.\n");
    // ****************** MATRIX ORBITAL BULLSHIT *********************

    while (1) {
        Task_sleep(g_taskSleepDuration);
    }
}

void ClearScreen(void){
	// Send_CMD(CMD_DLSTART);
	// Send_CMD(CLEAR_COLOR_RGB(0, 0, 0));
	// Send_CMD(CLEAR(1, 1, 1));
	// Send_CMD(DISPLAY());
	// Send_CMD(CMD_SWAP);
	// UpdateFIFO();                            // Trigger the CoProcessor to start processing commands out of the FIFO
	// Wait4CoProFIFOEmpty();                   // wait here until the coprocessor has read and executed every pending command.		
	// HAL_Delay(10);
}

//unsure if this is test code or code for when display starts up -Charan
void MakeScreen_MatrixOrbital(uint8_t DotSize){
// 	// Send_CMD(CMD_DLSTART);                   //Start a new display list
// 	// Send_CMD(VERTEXFORMAT(0));               //setup VERTEX2F to take pixel coordinates
// 	// Send_CMD(CLEAR_COLOR_RGB(0, 0, 0));      //Determine the clear screen color
// 	// Send_CMD(CLEAR(1, 1, 1));	               //Clear the screen and the curren display list
// 	// Send_CMD(COLOR_RGB(26, 26, 192));        // change colour to blue
// 	// Send_CMD(POINT_SIZE(DotSize * 16));      // set point size to DotSize pixels. Points = (pixels x 16)
// 	// Send_CMD(BEGIN(POINTS));                 // start drawing point
// 	// Send_CMD(TAG(1));                        // Tag the blue dot with a touch ID
// 	// Send_CMD(VERTEX2F(Display_Width() / 2, Display_Height() / 2));     // place blue point
// 	// Send_CMD(END());                         // end drawing point
// 	// Send_CMD(COLOR_RGB(255, 255, 255));      //Change color to white for text
// 	// Cmd_Text(Display_Width() / 2, Display_Height() / 2, 30, OPT_CENTER, " MATRIX         ORBITAL"); //Write text in the center of the screen
// 	// Send_CMD(DISPLAY());                     //End the display list
// 	// Send_CMD(CMD_SWAP);                      //Swap commands into RAM
// 	// UpdateFIFO();                            // Trigger the CoProcessor to start processing the FIFO
// }

// //i worked on this
// void renderDisplay(int heartRate, int respirationRate) {
//     printf("HR: %d  |  RR: %d\n", heartRate, respirationRate); //for debugging

//     int screenWidth = Display_Width();
//     int screenHeight = Display_Height();

//     Send_CMD(CMD_DLSTART);
//     Send_CMD(CLEAR_COLOR_RGB(0, 0, 0)); // Black background
//     Send_CMD(CLEAR(1, 1, 1));

//     // Title bar
//     Send_CMD(COLOR_RGB(0, 255, 255)); // Cyan title
//     Cmd_Text(screenWidth / 2, screenHeight / 10, 30, OPT_CENTER, "INFANT ECG MONITOR");

//     // Heart Rate
//     char hrBuffer[50];
//     printf(hrBuffer, "Heart Rate: %d BPM", heartRate);
//     Send_CMD(COLOR_RGB(255, 255, 255)); // White
//     Cmd_Text(screenWidth / 2, screenHeight / 3, 31, OPT_CENTER, hrBuffer);

//     // Alarm Logic
//     bool alarm = (heartRate < LOW_HEART_RATE_THRESHOLD_BPM || heartRate > HIGH_HEART_RATE_THRESHOLD_BPM ||
//                   respirationRate < LOW_RESPIRATORY_RATE_THRESHOLD_BRPM || respirationRate > HIGH_RESPIRATORY_RATE_THRESHOLD_BRPM);

//     if (alarm) {
//         Send_CMD(COLOR_RGB(255, 0, 0)); // Red text
//         Cmd_Text(screenWidth / 2, (2 * screenHeight) / 3, 30, OPT_CENTER, "ALERT: VITALS OUT OF RANGE");

//         // Red blinking indicator (you can later toggle visibility every other render)
//         Send_CMD(POINT_SIZE(30 * 16)); // Circle size
//         Send_CMD(BEGIN(POINTS));
//         Send_CMD(VERTEX2F((screenWidth / 2) * 16, (8 * screenHeight / 10) * 16)); // Lower center
//         Send_CMD(END());
//     }

//     Send_CMD(DISPLAY());
//     Send_CMD(CMD_SWAP);
//     UpdateFIFO();

//     // We need to replace printf() with FT813 draw-text commands once graphics integration starts
}
