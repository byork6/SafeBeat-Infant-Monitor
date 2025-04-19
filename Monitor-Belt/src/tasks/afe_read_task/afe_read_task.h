#pragma once

// --- DEFINITIONS --- //
#define AFE_READ_TASK_STACK_SIZE        (AFE_READ_STACK_SIZE)
#define AFE_READ_TASK_PRIORITY          (AFE_READ_PRIORITY)
Task_Handle g_afeReadTaskHandle;
Task_Struct g_AfeReadTaskStruct;
uint8_t g_afeReadTaskStack[AFE_READ_TASK_STACK_SIZE];

Semaphore_Handle g_afeDataReadyHandle;
Semaphore_Struct g_afeDataReadyStruct;

SPI_Handle g_afeSpiHandle;

#define ECG_FRAME_DATA_LEN 16
#define ECG_QUEUE_SIZE 10


// --- TYPE DEFINITIONS --- //
typedef struct {
    uint8_t data[ECG_FRAME_DATA_LEN];
} ECGFrame;

typedef struct {
    ECGFrame buffer[ECG_QUEUE_SIZE];
    uint8_t head;
    uint8_t tail;
    uint8_t count;
} ECGQueue;

ECGQueue ecgQueue;

// --- AFE FUNCTION PROTOTYPES --- //

Task_Handle afeRead_constructTask(void);

void afeRead_executeTask(UArg arg0, UArg arg1);

void afe_spiInit(void);

void afe_writeRegister(uint8_t regAddr, uint16_t data);

void afe_configureADAS(void);

void afe_handleDRDY(uint_least8_t index);

// --- ECG FUNCTION PROTOTYPES --- //
void ecg_queue_init(void);

bool ecg_enqueue(const ECGFrame* frame);

bool ecg_dequeue(ECGFrame* out_frame);

bool ecg_queue_is_full(void);

bool ecg_queue_is_empty(void);