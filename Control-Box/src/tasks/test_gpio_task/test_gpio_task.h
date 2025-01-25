#pragma once

// Define task parameters
#define TEST_GPIO_TASK_STACK_SIZE   512
#define TEST_GPIO_TASK_PRIORITY     1
Task_Struct g_TestGpioTaskStruct1;
Task_Handle g_task1Handle;
uint8_t g_testGpioTaskStack1[TEST_GPIO_TASK_STACK_SIZE];
Task_Struct g_TestGpioTaskStruct2;
Task_Handle g_task2Handle;
uint8_t g_testGpioTaskStack2[TEST_GPIO_TASK_STACK_SIZE];

/**
* @brief Creates an RTOS task for GPIO operations.
*
* This function initializes a TI-RTOS task using the Task_construct API. It sets up 
* the stack, priority, and parameters required for the RTOS task execution.
* The task is statically allocated, making it suitable for real-time systems
* where dynamic memory allocation is not desirable.
*
* @param pinNumber Specifies the GPIO pin number to be used by the task.
*                  This value is passed to the task's execution function as
*                  an argument for GPIO-specific operations.
* @param taskPriority The priority of the task in the RTOS scheduler. Tasks
*                     with lower numerical priority values have higher precedence.
* @param taskStruct A pointer to a Task_Struct object used to manage the task's
*                   control information. This must be statically allocated.
* @param taskStack A pointer to a statically allocated memory array used as the
*                  task's stack. The array size must match the stack size configured
*                  in the task parameters.
*
* @retval Returns the Task_Handle from Task_construct or Task_create
*
* @note Example function call -> testGpio_createTask(pinToTest, taskPriority, &g_TestGpioTaskStruct1, (uint8_t *)g_testGpioTaskStack1);
*       If a task will not have multple instances you can remove the last two 
*       parameters and refrence the global variables inside the function itself.
* 
* @return None
*/
Task_Handle testGpio_createTask(uint32_t pinNumber, uint32_t taskPriority, Task_Struct *taskStruct, uint8_t *taskStack);

/**
 * @brief Executes the testGpio task by toggling the selected pin.
 *
 * This function contains the execution logic for the testGpio task. It configures
 * the specified GPIO pin as output and toggles its state at a fixed interval.
 * The function runs indefinitely as part of the RTOS task loop, using `Task_sleep`
 * to introduce a delay between toggles.
 *
 * @param arg0 The GPIO pin number to configure and toggle (passed from TaskParams struct inside testGpio_createTask).
 * @param arg1 Unused argument (can be set to 0).
 *
 * @return None
 */
void testGpio_executeTask(UArg arg0, UArg arg1);

/**
* @brief Initializes a Task_Params structure with default values.
*
* This function sets up a `Task_Params` structure with default values provided by the TI-RTOS library.
* It must be called before configuring or modifying the structure to ensure all fields are properly initialized.
*
* @param params Pointer to a `Task_Params` structure to be initialized. The structure's memory must be
*               allocated before calling this function.  Params can be accessed using dot notation
*               like "taskParams.stack" or "taskParams.priority" after creation.
*
* @note After initialization, the fields in the `Task_Params` structure can be customized as needed before
*       passing it to other TI-RTOS task functions, such as `Task_construct` or `Task_create`.
*
* @return None
*/
// void Task_Params_init(*taskParams);

/**
* @brief  Statically constructs and initializes a TI-RTOS task.
*
* This function creates a statically allocated task using the Task_construct API. 
* It sets up the task function, task stack, priority, and optional arguments 
* needed for execution. Task_construct is ideal for real-time applications where 
* dynamic memory allocation is undesirable. Task_create() is an alternate function
* call that uses dynamic memory allocation instead.
*
* @param taskStruct     Pointer to a Task_Struct object that will hold the task's 
*                       control information. This must be statically allocated.
* @param taskFunction   Function pointer to the task's entry function. The function 
*                       must have the signature: void (UArg arg0, UArg arg1).
* @param taskParams     Pointer to a Task_Params structure containing task 
*                       configuration, such as stack size, stack location, 
*                       task priority, and task arguments. taskParams object created by
*                       Task_params_init().
* @param eb             Pointer to an Error_Block for error handling. If not used, 
*                       this can be set to NULL.
*
* @note The task function is executed when the TI-RTOS scheduler starts, and it 
*       runs according to the specified priority and stack configuration.
*
* @return None
*/
// void Task_construct(*taskStruct, taskFunction, *taskParams, *eb)
