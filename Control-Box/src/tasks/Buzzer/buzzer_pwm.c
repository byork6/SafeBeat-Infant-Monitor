// buzzer_pwm_task.c - SafeBeat RTOS PWM Buzzer Driver
#include "../../common/common.h"
#include "buzzer_pwm.h"

// --- BUZZER PWM TASK GLOBALS --- //
Task_Struct g_buzzerTaskStruct;
Task_Handle g_buzzerTaskHandle;
uint8_t g_buzzerTaskStack[BUZZER_TASK_STACK_SIZE];

PWM_Handle g_buzzerPwmHandle;
PWM_Params g_buzzerPwmParams;

// --- INIT PWM --- //
void buzzer_init(void) {
    PWM_Params_init(&g_buzzerPwmParams);
    g_buzzerPwmParams.dutyUnits = PWM_DUTY_US;
    g_buzzerPwmParams.periodUnits = PWM_PERIOD_US;
    g_buzzerPwmParams.periodValue = 434;     // ~2300 Hz
    g_buzzerPwmParams.dutyValue = 0;         // Start silent

    g_buzzerPwmHandle = PWM_open(CONFIG_AUDIO_PWM_OUT, &g_buzzerPwmParams);
    if (g_buzzerPwmHandle == NULL) {
        printf("PWM_open failed!\n");
        while (1);
    }

    PWM_start(g_buzzerPwmHandle);
    PWM_setDuty(g_buzzerPwmHandle, 0); // Ensure buzzer is silent at startup
}

// --- BEEP FUNCTION --- //
void buzzer_beep(uint32_t duration_ms) {
    PWM_setDuty(g_buzzerPwmHandle, 217); // 50% duty cycle
    Task_sleep(MS_TO_TICKS(duration_ms));
    PWM_setDuty(g_buzzerPwmHandle, 0);   // Silence
}

// --- BUZZER TASK EXECUTION FUNCTION --- //
void buzzer_executeTask(UArg arg0, UArg arg1) {
    (void)arg0;
    (void)arg1;
    buzzer_init();

    while (1) {
        buzzer_beep(500); // 500ms beep
        Task_sleep(g_taskSleepDuration * 4); // Delay between beeps
    }
}

// --- CONSTRUCTOR FUNCTION --- //
Task_Handle buzzer_constructTask(void) {
    Task_Params taskParams;
    Task_Params_init(&taskParams);
    taskParams.stack = g_buzzerTaskStack;
    taskParams.stackSize = BUZZER_TASK_STACK_SIZE;
    taskParams.priority = BUZZER_TASK_PRIORITY;
    taskParams.arg0 = 0;
    taskParams.arg1 = 0;

    Task_construct(&g_buzzerTaskStruct, buzzer_executeTask, &taskParams, NULL);
    return (Task_Handle)&g_buzzerTaskStruct;
}
