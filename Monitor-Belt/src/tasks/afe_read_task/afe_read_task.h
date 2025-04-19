#pragma once

// --- DEFINITIONS --- //
#include "ti/drivers/SPI.h"
#define AFE_READ_TASK_STACK_SIZE        (AFE_READ_STACK_SIZE)
#define AFE_READ_TASK_PRIORITY          (AFE_READ_PRIORITY)
Task_Handle g_afeReadTaskHandle;
Task_Struct g_AfeReadTaskStruct;
uint8_t g_afeReadTaskStack[AFE_READ_TASK_STACK_SIZE];

Semaphore_Handle g_afeDataReadyHandle;
Semaphore_Struct g_afeDataReadyStruct;

SPI_Handle g_afeSpiHandle;

// --- FUNCTION PROTOTYPES --- //

Task_Handle afeRead_constructTask(void);

void afeRead_executeTask(UArg arg0, UArg arg1);

void afe_spiInit(void);

void afe_writeRegister(uint8_t regAddr, uint16_t data);

void afe_configureADAS(void);

void afe_handleDRDY(uint_least8_t index);