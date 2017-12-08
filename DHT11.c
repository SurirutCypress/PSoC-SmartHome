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
#include <project.h>
#include "DHT11.h"

uint8 DMA_Timer_TD;
uint8 index_DHT11 = 0;

uint16 values[50];

int temperature = 0;
int humidity = 0;
uint8 DHT_bytes[5];



CY_ISR(DMA_Done)
{
    CyDmaChDisable(DMA_Timer_TD);
    calc_temp_humidty();
    Timer_DHT_WriteCounter(9999);
    Timer_DHT_Sleep();
    
}
void dma_config() {
    // initialize test nums
    
    DMA_Timer_Chan = DMA_Timer_DmaInitialize(DMA_TIMER_BYTES_PER_BURST, DMA_TIMER_REQUEST_PER_BURST, 
        HI16(DMA_TIMER_SRC_BASE), HI16(DMA_TIMER_DST_BASE));
    DMA_Timer_TD = CyDmaTdAllocate();
    
    CyDmaTdSetConfiguration(DMA_Timer_TD, 86, DMA_Timer_TD, DMA_Timer__TD_TERMOUT_EN | CY_DMA_TD_INC_DST_ADR);
    CyDmaTdSetAddress(DMA_Timer_TD, LO16((uint32)Timer_DHT_COUNTER_LSB_PTR), LO16((uint32)&values));
    CyDmaChSetInitialTd(DMA_Timer_Chan, DMA_Timer_TD);
    CyDmaChEnable(DMA_Timer_Chan,DMA_Timer_TD);
}


void DHTread(){
    /* start sensor  */
    for (uint8 i=0; i< 5; i++)  
       DHT_bytes[i] = 0; 
   
    DHT_Write(0u); // MCU sends out start signal //บิตแรก
    CyDelay(19); 
    DHT_Write(1u); //  for up and wait for response
    while(DHT_Read()==1) 
    {}
    while(DHT_Read()==0) 
    {}
    while(DHT_Read()==1) 
    {}
    CyDmaChDisable(DMA_Timer_TD);
    
    /* read sensor  */
    Timer_DHT_Start();
    index_DHT11 = 0;
}

void calc_temp_humidty() { //อ่านbyt
    uint8 cnt=7;
    uint8 idx=0;
    for (uint8 i=4; i < 40; i++) {  
        if ((values[i-1]-values[i])>50){
            DHT_bytes[idx] |= (1<<cnt);
        }
        if (cnt==0){  //next byte?
           idx++;     // next byte!
           cnt=7;     // restart at MSB
        }
        else {
            cnt--;
        }
    }
    humidity    = DHT_bytes[0];  
    temperature = DHT_bytes[2]; 
}

/* [] END OF FILE */
