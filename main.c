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
#include "project.h"
#include "stdio.h"
#include "math.h"
#include "stdbool.h"
#include "usb_protocols.h"
#include "globals.h"

// local files
#include "DHT11.h"
#include "tsl2561.h"
#include "MQ5Gas.h"
#include "DSM501Adust.h"
#include "WCS1800Current.h"
//#include "curr_sensor.h"



#define RELAY_ON     1
#define REALY_OFF    0
#define LED_ON       1
#define LED_OFF      0
uint8 motion = false;

bool Power = false;
bool power_change_flag;
bool sensor_read_flag = false;
bool read_sensor_flag = true;

static uint16 zero_current_voltage = 1645;

uint8 usb_data = 0;
uint8 count = 0;
uint8 TSL2561_ready = true;
uint32 R0_MQ5;


char LCD_str[32];
uint8 OUT_buffer[20];
uint8 IN_buffer[20];

extern uint8 temperature;
extern uint8 humidity;
extern uint8 DHT_bytes[5];

//extern float32 Current;


CY_ISR_PROTO(DMA_Done);
CY_ISR_PROTO(DustSensorRead);


CY_ISR(isr_timer_Handler){
    read_sensor_flag = true;
    Timer_read_sensors_ReadStatusRegister();
    
    isr_PIR_act_Enable();
}
CY_ISR(isr_motion_Handler){
    motion = true;
    isr_PIR_act_Disable();
    Pin_SW1_ClearInterrupt();
}

SensorData all_data;
void read_sensor_data(void);

int main(void)
{
    isr_DHT_DONE_StartEx(DMA_Done);
    isr_Dust_StartEx(DustSensorRead);
    isr_read_StartEx(isr_timer_Handler);
    isr_1_StartEx(isr_timer_Handler);
    isr_PIR_act_StartEx(isr_motion_Handler);
    CyGlobalIntEnable; /* Enable global interrupts. */
     
    USBFS_Start(0, USBFS_DWR_VDDD_OPERATION);  // initialize the USB
    while(!USBFS_bGetConfiguration());  //Wait till it the usb gets its configuration from the PC
    USBFS_EnableOutEP(OUT_ENDPOINT);

    dma_config();
    LCD_Start();
    LCD_ClearDisplay();
    I2C_Start();
    ADC_Start();
    AMux_Start();
    Timer_Start();
    Timer_Dust_Start();
  
   
 
    ADC_StartConvert();
    ADC_IsEndConversion(ADC_WAIT_FOR_RESULT); 
    uint32 get_pollution_level = ADC_GetResult32();// get polution lvl
    uint32 MQ5_volt = ADC_CountsTo_mVolts(get_pollution_level);
    uint32 R0_air = (5000/MQ5_volt)*7500-7500;
    R0_MQ5 = R0_air*0.15;//clean air = 6.5, clu = 1/6.5 = 0.15,1125.0
    
    
//    LCD_Position(1,0);
//    sprintf(LCD_str, "LPG: %lu,%lu", R0_MQ5,R0_air );
//    LCD_PrintString(LCD_str);
//    CyDelay(100);
  

    TSL2561_enable();
    uint16 clipThreshold = TSL2561_CLIPPING_13MS;
    //DHT11_bytes();
    DHTread();
    //CyDelay(50);
    

    for(;;){
//        LCD_Position(0,0);
//        LCD_PrintString("Start2");
        uint8 input = USB_CheckInput(OUT_buffer);
        
        if ( input == true ) {
            switch(OUT_buffer[0]) {
                case 'F':;
                    Power = false;
                    Pin_2_Write(REALY_OFF);
//                    LCD_Position(0,0);
//                    LCD_PrintString("cehck "); 
                    CyDelay(300);
                    calibrate_zero_voltage();
//                    
                    break;
                case 'N':;
                    Power = true;
                    Pin_2_Write(RELAY_ON);
                    break;
                case 'S':;
                    Timer_read_sensors_Stop();
                    break;
                case 'R' : ;
                    Timer_read_sensors_Start();
                    break;
            OUT_buffer[0] = 0;    
            }
            
        }
        
        
        if (read_sensor_flag == true) {
//            LCD_Position(1,0);
//            LCD_PrintString("cehck flag"); 
            read_sensor_data();
            motion = Pin_SW1_Read();
            read_sensor_flag = false;
            // lcd print
//            LCD_Position(0,0);
//            LCD_PrintString(" On ");
        }
        else {
//            LCD_Position(0,0);
//            LCD_PrintString(" Off ");
          
            //CyDelay(50);
        }
        


        if (motion==0){
            all_data.motion = 0;
            Pin_1_Write(LED_OFF);
           
            LCD_Position(0,0);
            LCD_PrintString("   no move");
            
        }
        else {
            all_data.motion = 1;
            Pin_1_Write(LED_ON);
            LCD_Position(0,0);
            LCD_PrintString("Movement");  
        }
            LCD_Position(1,0);
            sprintf(LCD_str, "read:%i ",Pin_SW1_Read());
            LCD_PrintString(LCD_str);
            CyDelay(300);
    }
}
 
void read_sensor_data(void) {

    all_data.humidity   = DHT_bytes[0];  
    all_data.temperture = DHT_bytes[2];
   
//    LCD_Position(1,0);
//    LCD_PrintString("check2 ");
    
    all_data.lux = tsl2561_get_lux();
    
    all_data.Dust = calculate_Dust();
    
    //all_data.Current = get_current();
    
    
    AMux_Select(0);
    ADC_StartConvert();
    ADC_IsEndConversion(ADC_WAIT_FOR_RESULT);
    uint32 get_pollution_level = ADC_GetResult32(); 
    uint32 MQ5_volt = ADC_CountsTo_mVolts(get_pollution_level);
    float32 Rs = (5000.00/MQ5_volt)*7500.00-7500.00;//float
    float32 ratio = (float32)Rs/(float32)R0_MQ5;
    all_data.LPG = pow(ratio/5.9875,-2.4685);//1/m = 1/-0.4051 = -2.4685
    
    
//    LCD_Position(1,0);
//    sprintf(LCD_str, "LPG:%2.4f ppm",ratio);
//    LCD_PrintString(LCD_str);
//    CyDelay(100);


    all_data.Current = get_current();
    
//    AMux_Select(1);
//    ADC_StartConvert();
//    ADC_IsEndConversion(ADC_WAIT_FOR_RESULT);
//    uint16 adc_counts = ADC_GetResult16();
//    uint16 millivolts = ADC_CountsTo_mVolts(adc_counts);
//    all_data.Current = (millivolts-zero_current_voltage)/50.3;


   
//    LCD_Position(0,0);
//    sprintf(LCD_str, "C3:%2.4f  A",all_data.Current);
//    LCD_PrintString(LCD_str);
    
    all_data.ElectricPower = 220*(float32)all_data.Current;

    USB_Export_Data(all_data, 21);
}


