/*********************************************************************************
* File Name: usb_protocols.c
*
* Description:
*  Source code for the protocols used by the USB.
NOTE: WAITING ON USB VENDOR AND PRODUCT ID
*
*
**********************************************************************************
 * Copyright Naresuan University, Phitsanulok Thailand
 * Released under Creative Commons Attribution-ShareAlike  3.0 (CC BY-SA 3.0 US)
*********************************************************************************/

#include <project.h>
#include "usb_protocols.h"
#include "stdio.h"
#include "stdlib.h"
extern char LCD_str[];  // for debug

union UsbUnion {
  uint8  usb[21];
  SensorData data;
} usb_union;

/******************************************************************************
* Function Name: USB_CheckInput
*******************************************************************************
*
* Summary:
*  Check if any incoming USB data and store it to the input buffer
*
* Parameters:
*  uint8 buffer: array of where the data is stored
*
* Return:
*  true (1) if data has been inputed or false (0) if no data
*
* Global variables:
*  OUT_ENDPOINT:  number that is the endpoint coming out of the computer
*
*******************************************************************************/

uint8 USB_CheckInput(uint8 buffer[]) {
//    LCD_Position(1,0);
//    LCD_PrintString("checking");
    if(USBFS_GetEPState(OUT_ENDPOINT) == USBFS_OUT_BUFFER_FULL) {
        
        /* There is data coming in, get the number of bytes*/
        uint8 OUT_COUNT = USBFS_GetEPCount(OUT_ENDPOINT);
        /* Read the OUT endpoint and store data in OUT_Data_buffer */
        USBFS_ReadOutEP(OUT_ENDPOINT, buffer, OUT_COUNT);
        /* Re-enable OUT endpoint */
        USBFS_EnableOutEP(OUT_ENDPOINT);
//        LCD_Position(1,0);
//        sprintf(LCD_str, "cc:%c|%c ", buffer[0], buffer[1]);
//        LCD_PrintString(LCD_str);
        return true;
    }
    
    return false;
}

/******************************************************************************
* Function Name: USB_Export_Data
*************************************************************************************
*
* Summary:
*  Take a buffer as input and export it, the size of bytes to send is also inputted
*
* Parameters:
*  uint8 array array: array of data to export
*  uint16 size: the number of bytes to send in the array
*
* Return:
*  None
*
* Global variables:
*  MAX_BUFFER_SIZE:  the number of bytes the UBS device can hold
*
*******************************************************************************************/

void USB_Export_Data(SensorData data, uint16 size) {
    usb_union.data = data;
    
    for (int i=0; i < size; i=i+MAX_BUFFER_SIZE) {
        while(USBFS_GetEPState(IN_ENDPOINT) != USBFS_IN_BUFFER_EMPTY)
        {
        }

        if(USBFS_GetEPState(IN_ENDPOINT) == USBFS_IN_BUFFER_EMPTY){
            USBFS_LoadInEP(IN_ENDPOINT, &usb_union.usb[0], 21);  // TODO: Fix this
            USBFS_EnableOutEP(OUT_ENDPOINT);
        }
    }

}

/* [] END OF FILE */
