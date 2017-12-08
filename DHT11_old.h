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
#if !defined(__DHT11_OLD_H)
#define __DHT11_OLD_H
    
#include <project.h>
    
    
#define DMA_TIMER_BYTES_PER_BURST 2
#define DMA_TIMER_REQUEST_PER_BURST 1
#define DMA_TIMER_SRC_BASE (CYDEV_PERIPH_BASE)
#define DMA_TIMER_DST_BASE (CYDEV_SRAM_BASE)    
    
    
uint8 DMA_Timer_Chan;
uint8 DMA_Timer_TD;


/***************************************
*        Function Prototypes
***************************************/  

void dma_config();

int DHTread_old();
    
#endif

/* [] END OF FILE */
