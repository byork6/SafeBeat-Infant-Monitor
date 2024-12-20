/*
 * Copyright (c) 2015-2019, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "common.h"
#include "tasks/test_gpio_task.h"

void *mainThread(void *arg0)
{
    // For testing GPIO pins one at a time without using RTOS
    // testGPIO(5);

    /////// PROJECT CODE ///////
    // Init local variables
    uint32_t time = 1;

    // Call TI driver initializations
    initBOARD();

    // Call custom board configurations
    configBOARD();

    // Create tasks for TI-RTOS
    // Task 1
    uint32_t pinToTest = 6;
    uint32_t taskPriority = 1;
    testGpio_createTask(pinToTest, taskPriority, &g_TestGpioTaskStruct1, (uint8_t *)g_testGpioTaskStack1);

    // Task 2
    pinToTest = 7;
    taskPriority = 1;
    testGpio_createTask(pinToTest, taskPriority, &g_TestGpioTaskStruct2, (uint8_t *)g_testGpioTaskStack2);

    // Start TI-RTOS BIOS execution
    BIOS_start();

    return NULL;
}
