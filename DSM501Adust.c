/* ========================================
 *
 * Copyright YOUR COMPANY, THE YEAR
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * ========================================
*/
#include "stdio.h"
#include "math.h"
#include "DSM501Adust.h"

float32  Dust;
//float32 low_ratio;
static uint8 index_ = 0;
static uint16 values[SAMPLES_TO_COLLECT];
extern char LCD_str[40];  // for debug

CY_ISR(DustSensorRead) {
    values[index_] = Timer_Dust_ReadCounter();
    Timer_Dust_WriteCounter(0);
    index_++;
    index_ %= SAMPLES_TO_COLLECT;
}
//
//float32 calculate_Dust(void){
//    // - calculate average high occupancy for last second
//    // - calculate low occupancy
//    // - calc low to high ratio
//    // - calc concentration
//    uint32 high_time  = 0;
//    uint32 Total_time  = 1000000;//50ms*20value=1000ms
//    for (uint8 i=0; i <SAMPLES_TO_COLLECT; i++) {
//        high_time += values[i];  //วัดค่า
//    
//    }
//    uint32  low_time = Total_time - high_time;
//
//    float32 low_ratio = (float32) low_time/ (float32) Total_time;
////    LCD_Position(1,0);
////    sprintf(LCD_str, "r:%1.4f,%lu ",low_ratio, low_time);
////    LCD_PrintString(LCD_str);
////    CyDelay(1000);
//
//    //float Particle = 0.0021*(ratio*ratio)+0.0913*ratio-0.0251;//[mg/m3]
//  
//    float32 Dust = 0.0021*pow(low_ratio,2)+0.0913*low_ratio-0.0251;//[mg/m3]  using spec sheet curve
//    
////    LCD_Position(1,0);
////    sprintf(LCD_str, "Dust:%1.4f ",Dust);
////    LCD_PrintString(LCD_str);
////    CyDelay(1000);
//	return Dust;
//}

float32 calculate_Dust(void){
    // - calculate average high occupancy for last second
    // - calculate low occupancy
    // - calc low to high ratio
    // - calc concentration
    uint32 high_time  = 0;
    uint32 Total_time  = 1000000;//50ms*20value=1000ms
    for (uint8 i=0; i <SAMPLES_TO_COLLECT; i++) {
        high_time += values[i];  
    
    }
    uint32  low_time = Total_time - high_time;

    float32 low_ratio = (float32) low_time/ (float32) Total_time;

  
    float32 Dust = 0.0021*pow(low_ratio,2)+0.0913*low_ratio-0.0251;//[mg/m3]  using spec sheet curve
    

	return Dust;
}

