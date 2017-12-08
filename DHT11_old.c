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
#include "DHT11_old.h"

extern int temperature;
extern int humidity;
extern uint16 values[40];
uint16 test_nums[40];

int DHTread_old() 
{     
   uint8 IState;
   IState=CyEnterCriticalSection();  
   uint8 bits[5]; 
   uint8 cnt = 7; 
   uint8 idx = 0;
   int   calc=0; 
   int   timeout=0; 
   for (uint8 i=0; i< 5; i++)  
       bits[i] = 0; 
   DHT_Write(0u); // MCU sends out start signal
   CyDelay(19); 
   DHT_Write(1u); //  for up and wait for response
   while(DHT_Read()==1) 
   { 
       timeout++; 
       if(timeout>500) 
           goto r99;  //DHT error function 
   } 
   while(DHT_Read()==0) 
   {         
       timeout++; 
       if(timeout>500) 
           goto r99; //DHT error function 
   } 
   calc=timeout; 
   timeout=0; 
   while(DHT_Read()==1); 
   for (uint8 i=0; i<40; i++) 
	{ 
       timeout=0; 
       while(DHT_Read()==0); 
       while(DHT_Read()==1) 
           timeout++; 
       //Data acquiring point 
       if ((timeout) > (calc/2)) 
           bits[idx] |= (1 << cnt); 
       if (cnt == 0)   // next byte? 
   	{ 
   		cnt = 7;    // restart at MSB 
   		idx++;      // next byte! 
   	} 
   	else cnt--; 
   } 
   humidity    = bits[0];  
   temperature = bits[2];  
   CyExitCriticalSection(IState); 
   CyDelay(1); 
   return 0; 
   r99:    //Goto label for error in DHT reading 
       humidity    = 99;  
       temperature = 99;  
       CyExitCriticalSection(IState); 
       return 99; 
}

void DMA_config() {
    // initialize test nums
    for(uint8 i=0; i<40; i++){
        test_nums[i]=i;
    }
    
    DMA_Timer_Chan = DMA_Timer_DmaInitialize(DMA_TIMER_BYTES_PER_BURST, DMA_TIMER_REQUEST_PER_BURST, 
        HI16(DMA_TIMER_SRC_BASE), HI16(DMA_TIMER_DST_BASE));
    DMA_Timer_TD = CyDmaTdAllocate();
    
    CyDmaTdSetConfiguration(DMA_Timer_TD, 86, DMA_Timer_TD, DMA_Timer__TD_TERMOUT_EN | CY_DMA_TD_INC_DST_ADR);
    CyDmaTdSetAddress(DMA_Timer_TD, LO16((uint32)Timer_DHT_COUNTER_LSB_PTR), LO16((uint32)&values));
    CyDmaChSetInitialTd(DMA_Timer_Chan, DMA_Timer_TD);
    CyDmaChEnable(DMA_Timer_Chan,DMA_Timer_TD);
    
  
   }


/* [] END OF FILE */
