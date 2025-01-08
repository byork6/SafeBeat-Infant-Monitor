#include "common.h"
#include "test_functions.h"

void testGpio(uint32_t pin_config_index){
    //////////////// TEST CODE  ONLY ////////////////
    
    // Input validation (Pins 5-30 only valid pins)
    if (pin_config_index < 5 || pin_config_index > 30){
        exit(1);
    }

    // 1 second delay
    uint32_t time = 1;

    // Call driver init functions from SDK
    GPIO_init();

    // Initialize GPIO pins
    GPIO_setConfig(pin_config_index, GPIO_SET_OUT_AND_DRIVE_LOW);

    while (1)
    {
        sleep(time);
        GPIO_toggle(pin_config_index);
    }
}

void printVar(const char *varName, void *var, char type) {
    if (varName == NULL) {
        varName = "foo"; // Default name if none is provided
    }

    switch (type) {
        case 'd': // Integer
            printf("Variable \"%s\" value: %d\n", varName, *(int *)var);
            break;
        case 'f': // Float
            printf("Variable \"%s\" value: %.2f\n", varName, *(float *)var);
            break;
        case 'c': // Character
            printf("Variable \"%s\" value: %c\n", varName, *(char *)var);
            break;
        case 's': // String
            printf("Variable \"%s\" value: %s\n", varName, (char *)var);
            break;
        case 'u':
            printf("Variable \"%s\" value: %s\n", varName, *(unsigned int *)var);
            break;
        default:
            printf("Unsupported type for variable \"%s\"\n", varName);
    }
    fflush(stdout); // Ensure output is flushed immediately
}

void printStr(const char *str) {
    printf("%s", str ? str : "NULL");
    fflush(stdout);
}