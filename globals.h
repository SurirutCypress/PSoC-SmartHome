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
#if !defined(_GLOBALS_H)
#define _GLOBALS_H

#include <project.h>

typedef struct {
    float32 LPG;
    float32 Dust;
    float32 Current;
    float32 ElectricPower;
    uint16 lux;
    uint8 motion;
    uint8 temperture;
    uint8 humidity;
 } SensorData;
  

#endif

/* [] END OF FILE */
