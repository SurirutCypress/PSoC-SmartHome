#include "math.h"
#include"MQ5Gas.h"

void init_sensor(void) {
    ADC_Start();  // start adc
    ADC_StartConvert();  // start it converting
 
}

uint32 get_pollution_level(void) {
    return ADC_GetResult16();
}
