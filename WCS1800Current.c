#include <project.h>
#include "WCS1800Current.h"
#include "math.h"

char LCD_str[40];


static uint16 zero_current_voltage = 1645;


float32 get_current(void) {
    AMux_Select(1);
    ADC_StartConvert();// start it converting
    ADC_IsEndConversion(ADC_WAIT_FOR_RESULT);
    uint16 adc_counts = ADC_GetResult16();
    uint16 millivolts = ADC_CountsTo_mVolts(adc_counts);
//    
    float32 Current = (millivolts-zero_current_voltage)/50.3;
//    LCD_Position(1,0);
//    sprintf(LCD_str, "I:%f A",Current);
//    LCD_PrintString(LCD_str);
    return Current;
}
//uint16 Vout = 0.0631*I+2.5197; 5volt ,//uint16 I = ((volt-2.5197)/0.0631);
//uint16 Vout = 0.0503*I+1.6456; 3.3volt

void calibrate_zero_voltage(void) {
    AMux_Select(1);
    ADC_StartConvert();// start it converting
    ADC_IsEndConversion(ADC_WAIT_FOR_RESULT);
    uint16 adc_counts = ADC_GetResult16();
    zero_current_voltage = ADC_CountsTo_mVolts(adc_counts);
//    LCD_Position(0,0);
//    sprintf(LCD_str, "zer volt:%f mV",zero_current_voltage);
//    LCD_PrintString(LCD_str);
}

void check(void) {
    
}

  

    
//uint16 Vout = 0.0631*I+2.5197; 5volt ,//uint16 I = ((volt-2.5197)/0.0631);
//uint16 Vout = 0.0503*I+1.6456; 3.3volt