#include "../../common/common.h"
#include "EVE2_Library/hw_api.h"

// Define global variable for spi handle --- declared in common.h using extern
SPI_Handle g_spiDisplayHandle = NULL;

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

/*void displayDriver_executeTask(UArg arg0, UArg arg1) {
    (void)arg0;
    (void)arg1;
    int i = 0;
    
    HAL_Eve_Reset_HW();

    // Init SPI
    while (g_spiDisplayHandle == NULL){
        SPI_Params spiParams;
        spiParams.bitRate = 1000000;                     // 1 MHz (slow and safe)
        spiParams.frameFormat = SPI_POL0_PHA0;           // FT81x expects SPI Mode 0
        spiParams.dataSize = 8;                          // 8 bits per word

        SPI_Params_init(&spiParams);
        g_spiDisplayHandle = SPI_open(CONFIG_DISPLAY_SPI, &spiParams);

        if (g_spiDisplayHandle != NULL){
            printf("Display SPI initialized.\n");
            break;
        }
        else{
            printf("Display SPI not initialized, trying again...\n");
            Task_sleep(g_taskSleepDuration);
        }
    }

    printf("Selecting display...\n");
    HAL_SPI_Enable();

    // Init Display
    printf("Entering FT81x_init...\n");
    FT81x_Init(DISPLAY_70, BOARD_EVE2, TOUCH_TPC); 
    printf("Clear screen...\n");
    ClearScreen();
    printf("Make screen...\n");
    MakeScreen_MatrixOrbital(30);

    printf("Entering displayDriver_executeTask()...\n");

    while (1) {
        i++;
        printf("Display driver Count: %d\n", i);

        // Placeholder test values — replace with real data later
        int testHeartRate = 110;
        int testRespirationRate = 35;

        GPIO_write(SD_SPI_CSN_PIN, 1);
        GPIO_write(DISPLAY_SPI_CSN_PIN, 0);
        
        renderDisplay(testHeartRate, testRespirationRate); //added this line to call the function

        Task_sleep(g_taskSleepDuration);
    }
}*/
//Minimal Test version 
void displayDriver_executeTask(UArg arg0, UArg arg1) {
    (void)arg0;
    (void)arg1;

    printf("[Display Task] Starting...\n");

    HAL_Eve_Reset_HW();
    printf("[Display Task] Reset completed.\n");

    while (g_spiDisplayHandle == NULL) {
        SPI_Params spiParams;
        SPI_Params_init(&spiParams);
        spiParams.bitRate = 1000000;
        spiParams.frameFormat = SPI_POL0_PHA0;
        spiParams.dataSize = 8;

        g_spiDisplayHandle = SPI_open(CONFIG_DISPLAY_SPI, &spiParams);

        if (g_spiDisplayHandle != NULL) {
            printf("[Display Task] SPI opened successfully.\n");
            break;
        } else {
            printf("[Display Task] SPI open failed. Retrying...\n");
            Task_sleep(g_taskSleepDuration);
        }
    }

    GPIO_write(SD_SPI_CSN_PIN, 1);
    GPIO_write(DISPLAY_SPI_CSN_PIN, 0);
    HAL_SPI_Enable();

    /* **************************************************************************************************** */
    // NOTE: I am not sure this catch should go here, this is already done inside FT81x_Init. If anything
    // it would be good to do this with the initResult like shown below or just read the value of reg_id but
    // not stop execution if it is not 0x07C, this is because we will not get that as a return val until we do some stuff
    // that is inside the FT81x_init function.

    // uint8_t reg_id = rd8(0x302000); // REG_ID
    // printf("[Display Task] REG_ID = 0x%02X (expected 0x7C)\n", reg_id);

    // if (reg_id != 0x7C) {
    //     printf("[Display Task] REG_ID check failed. Display may not be responding.\n");
    //     return;
    // }
    // 
    // printf("[Display Task] REG_ID confirmed. Proceeding with initialization.\n")
    /* **************************************************************************************************** */

    wr8(REG_PWM_DUTY + RAM_REG, 128);  // Backlight
    wr8(REG_PCLK + RAM_REG, 1);        // Pixel clock

    int initResult = FT81x_Init(DISPLAY_70, BOARD_EVE2, TOUCH_TPC);
    if (!initResult) {
        printf("[Display Task] FT81x_Init failed. Aborting task.\n");
        return;
    }

    printf("[Display Task] FT81x_Init completed successfully.\n");

    printf("[Display Task] Drawing MatrixOrbital test screen...\n");
    ClearScreen();
    MakeScreen_MatrixOrbital(30);

    printf("[Display Task] Display setup complete. Idling now.\n");

    while (1) {
        Task_sleep(g_taskSleepDuration);
    }
}

void ClearScreen(void){
	Send_CMD(CMD_DLSTART);
	Send_CMD(CLEAR_COLOR_RGB(0, 0, 0));
	Send_CMD(CLEAR(1, 1, 1));
	Send_CMD(DISPLAY());
	Send_CMD(CMD_SWAP);
	UpdateFIFO();                            // Trigger the CoProcessor to start processing commands out of the FIFO
	Wait4CoProFIFOEmpty();                   // wait here until the coprocessor has read and executed every pending command.		
	HAL_Delay(10);
}

//unsure if this is test code or code for when display starts up -Charan
void MakeScreen_MatrixOrbital(uint8_t DotSize){
	Send_CMD(CMD_DLSTART);                   //Start a new display list
	Send_CMD(VERTEXFORMAT(0));               //setup VERTEX2F to take pixel coordinates
	Send_CMD(CLEAR_COLOR_RGB(0, 0, 0));      //Determine the clear screen color
	Send_CMD(CLEAR(1, 1, 1));	               //Clear the screen and the curren display list
	Send_CMD(COLOR_RGB(26, 26, 192));        // change colour to blue
	Send_CMD(POINT_SIZE(DotSize * 16));      // set point size to DotSize pixels. Points = (pixels x 16)
	Send_CMD(BEGIN(POINTS));                 // start drawing point
	Send_CMD(TAG(1));                        // Tag the blue dot with a touch ID
	Send_CMD(VERTEX2F(Display_Width() / 2, Display_Height() / 2));     // place blue point
	Send_CMD(END());                         // end drawing point
	Send_CMD(COLOR_RGB(255, 255, 255));      //Change color to white for text
	Cmd_Text(Display_Width() / 2, Display_Height() / 2, 30, OPT_CENTER, " MATRIX         ORBITAL"); //Write text in the center of the screen
	Send_CMD(DISPLAY());                     //End the display list
	Send_CMD(CMD_SWAP);                      //Swap commands into RAM
	UpdateFIFO();                            // Trigger the CoProcessor to start processing the FIFO
}

//i worked on this
void renderDisplay(int heartRate, int respirationRate) {
    printf("HR: %d  |  RR: %d\n", heartRate, respirationRate); //for debugging

    int screenWidth = Display_Width();
    int screenHeight = Display_Height();

    Send_CMD(CMD_DLSTART);
    Send_CMD(CLEAR_COLOR_RGB(0, 0, 0)); // Black background
    Send_CMD(CLEAR(1, 1, 1));

    // Title bar
    Send_CMD(COLOR_RGB(0, 255, 255)); // Cyan title
    Cmd_Text(screenWidth / 2, screenHeight / 10, 30, OPT_CENTER, "INFANT ECG MONITOR");

    // Heart Rate
    char hrBuffer[50];
    sprintf(hrBuffer, "Heart Rate: %d BPM", heartRate);
    Send_CMD(COLOR_RGB(255, 255, 255)); // White
    Cmd_Text(screenWidth / 2, screenHeight / 3, 31, OPT_CENTER, hrBuffer);

    // Alarm Logic
    bool alarm = (heartRate < LOW_HEART_RATE_THRESHOLD_BPM || heartRate > HIGH_HEART_RATE_THRESHOLD_BPM ||
                  respirationRate < LOW_RESPIRATORY_RATE_THRESHOLD_BRPM || respirationRate > HIGH_RESPIRATORY_RATE_THRESHOLD_BRPM);

    if (alarm) {
        Send_CMD(COLOR_RGB(255, 0, 0)); // Red text
        Cmd_Text(screenWidth / 2, (2 * screenHeight) / 3, 30, OPT_CENTER, "ALERT: VITALS OUT OF RANGE");

        // Red blinking indicator (you can later toggle visibility every other render)
        Send_CMD(POINT_SIZE(30 * 16)); // Circle size
        Send_CMD(BEGIN(POINTS));
        Send_CMD(VERTEX2F((screenWidth / 2) * 16, (8 * screenHeight / 10) * 16)); // Lower center
        Send_CMD(END());
    }

    Send_CMD(DISPLAY());
    Send_CMD(CMD_SWAP);
    UpdateFIFO();

    // We need to replace printf() with FT813 draw-text commands once graphics integration starts
}
