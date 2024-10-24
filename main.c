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

/*
 *  ======== empty.c template ========
 */

// TI inlcusions
/* For usleep() */
#include <unistd.h>
#include <stdint.h>
#include <stddef.h>

/* Driver Header files */
#include <ti/drivers/GPIO.h>
// #include <ti/drivers/I2C.h>
#include <ti/drivers/SPI.h>
// #include <ti/drivers/Watchdog.h>

/* Driver configuration */
#include "ti_drivers_config.h"


// PROJECT INCLUSIONS
// Project Header Files
#include "functions.h"

// Define GPIO Macros
#define GPIO_SET_OUT_AND_DRIVE_LOW (GPIO_CFG_OUT_STD | GPIO_CFG_OUT_LOW)
#define GPIO_SET_OUT_AND_DRIVE_HIGH (GPIO_CFG_OUT_STD | GPIO_CFG_OUT_HIGH)

/*
 *  ======== mainThread ========
 */


void *mainThread(void *arg0)
{
    ////////// EXAMPLE CODE //////////
    /*
    // 1 second delay
    uint32_t time = 1;

    // Call driver init functions
    GPIO_init();
    // I2C_init();
    // SPI_init();
    // Watchdog_init();

    // Configure the LED pin
    GPIO_setConfig(CONFIG_GPIO_LED_0, GPIO_CFG_OUT_STD | GPIO_CFG_OUT_LOW);

    // Turn on user LED
    GPIO_write(CONFIG_GPIO_LED_0, CONFIG_GPIO_LED_ON);

    while (1)
    {
        sleep(time);
        GPIO_toggle(CONFIG_GPIO_LED_0);
    }
    */
    //////////////////////////////////


    //START OF PROJECT CODE
    // Init local variables
    uint32_t time = 1;

    // TI Init functions
    GPIO_init();
    SPI_init();

    // Configure GPIO
    configGPIO();

    // Configure SPI
    configSPI();

    while (1){
        // Loop delay of 1s
        sleep(1);
        
        // Run-time code goes here
        // TODO
        
    }

}

void configGPIO(void){
    // TODO: Set up initial config for GPIO pins
    // Set GPIO Pins 5, 7-17 as output and drive low on startup for LCD.
    GPIO_setConfig(5, GPIO_SET_OUT_AND_DRIVE_LOW)
    GPIO_setConfig(7, GPIO_SET_OUT_AND_DRIVE_LOW)
    GPIO_setConfig(8, GPIO_SET_OUT_AND_DRIVE_LOW)
    GPIO_setConfig(9, GPIO_SET_OUT_AND_DRIVE_LOW)
    GPIO_setConfig(10, GPIO_SET_OUT_AND_DRIVE_LOW)
    GPIO_setConfig(11, GPIO_SET_OUT_AND_DRIVE_LOW)
    GPIO_setConfig(12, GPIO_SET_OUT_AND_DRIVE_LOW)
    GPIO_setConfig(13, GPIO_SET_OUT_AND_DRIVE_LOW)
    GPIO_setConfig(14, GPIO_SET_OUT_AND_DRIVE_LOW)
    GPIO_setConfig(15, GPIO_SET_OUT_AND_DRIVE_LOW)
    GPIO_setConfig(16, GPIO_SET_OUT_AND_DRIVE_LOW)
    GPIO_setConfig(17, GPIO_SET_OUT_AND_DRIVE_LOW)

}

void configSPI(void){
    // TODO: Set up initial config for SPI pins
}