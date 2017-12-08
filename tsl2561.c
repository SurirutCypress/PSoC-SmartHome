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
#include "tsl2561.h"
#include "stdio.h"



uint8 read_buffer[16];
uint8 write_buffer[16];

extern char LCD_str[40];

static uint8 I2C_ADDRESS = TSL2561_ADDR_LOW;

static void TSL2561_write8(uint8 _register, uint8 data);
static uint8 TSL2561_read8(uint8 _register);
static uint16 TSL2561_read(uint8 _register);

void TSL2561_enable(void) {
     TSL2561_write8(0x80, 0x03);
}
void TSL2561_disable(void){
     TSL2561_write8(0x80, 0x00);
}
uint8 TSL2561_read_ID(void) {
    
     return TSL2561_read8(0x8A);
    
}
void Adafruit_TSL2561_Unified_enable(void) {
  /* Enable the device by setting the control bit to 0x03 */
  TSL2561_write8(TSL2561_COMMAND_BIT | TSL2561_REGISTER_CONTROL, TSL2561_CONTROL_POWERON);
}
void Adafruit_TSL2561_Unified_getData (uint16 *channel0, uint16 *channel1)
{
  /* Enable the device by setting the control bit to 0x03 */
  enable();
}
  
bool Adafruit_TSL2561_Unified_begin(void)  {
  /* Make sure we're actually connected */
  uint8_t x = TSL2561_read8(TSL2561_REGISTER_ID);
  if (!(x & 0x0A))
  {
    return false;
  }
  _tsl2561Initialised = true;

  /* Set default integration time and gain */
  setIntegrationTime(_tsl2561IntegrationTime);
  setGain(_tsl2561Gain);

  /* Note: by default, the device is in power down mode on bootup */
  disable();

  return true;
}

void setIntegrationTime(tsl2561IntegrationTime time) {
  /* Enable the device by setting the control bit to 0x03 */
    enable();

  /* Update the timing register */
    write8(TSL2561_COMMAND_BIT | TSL2561_REGISTER_TIMING, time | _tsl2561Gain);

  /* Update value placeholders */
    _tsl2561IntegrationTime = time;

  /* Turn the device off to save power */
    disable();
}
void Adafruit_TSL2561_Unified_setGain(tsl2561Gain_t gain)
{

  /* Enable the device by setting the control bit to 0x03 */
  enable();

  /* Update the timing register */
  write8(TSL2561_COMMAND_BIT | TSL2561_REGISTER_TIMING, _tsl2561IntegrationTime | gain);

  /* Update value placeholders */
  _tsl2561Gain = gain;

  /* Turn the device off to save power */
  disable();
}


uint16 TSL2561_read_channel1(void) {
     return TSL2561_read16(0x9E);   
}
uint16 TSL2561_read_channel0(void) {
     return TSL2561_read16(0x8C);   
}

   
static void TSL2561_write8(uint8 _register, uint8 data) {
    
    uint8 status = 0;
    write_buffer[0] = _register;
    write_buffer[1] = data;
    do {
        status = I2C_MasterWriteBuf(I2C_ADDRESS, (uint8 *)write_buffer, 2, I2C_MODE_COMPLETE_XFER);
        for(;;) {
            if (0x00 != (I2C_MasterStatus() & I2C_MSTAT_WR_CMPLT)) {
                break;
            }
        }
    }
    while(status != I2C_MSTR_NO_ERROR);
}


static uint8 TSL2561_read8(uint8 _register){
    uint8 data;
    uint8 temp;
    do {
        temp = I2C_MasterWriteBuf(I2C_ADDRESS, &_register, 1, I2C_MODE_NO_STOP);
        for(;;) {
            if (0x00 != (I2C_MasterStatus() & I2C_MSTAT_WR_CMPLT)) {
                break;
            }
        }
    }
    while (temp != I2C_MSTR_NO_ERROR);
    I2C_MasterClearWriteBuf();

    do {
        temp = I2C_MasterReadBuf(I2C_ADDRESS, (uint8 *)read_buffer, 1, I2C_MODE_REPEAT_START);
        for(;;) {
            if (0x00 != (I2C_MasterStatus() & I2C_MSTAT_RD_CMPLT)) {
                break;
            }
        }
    }
    while (temp != I2C_MSTR_NO_ERROR);
    data = read_buffer[0];
    I2C_MasterClearReadBuf();
    return data;

}

uint16 TSL2561_read16(uint8 _register){
    uint16 data = 0;
    uint8  temp;
    uint8  command = _register | 0x10;
    do {
        temp = I2C_MasterWriteBuf(I2C_ADDRESS, &command, 1, I2C_MODE_NO_STOP);
        for(;;) {
            if (0x00 != (I2C_MasterStatus() & I2C_MSTAT_WR_CMPLT)) {
                break;
            }
        }
    }
    while (temp != I2C_MSTR_NO_ERROR);
    I2C_MasterClearWriteBuf();
    do {
        temp = I2C_MasterReadBuf(I2C_ADDRESS, (uint8 *)read_buffer, 2, I2C_MODE_REPEAT_START);
        for(;;) {
            if (0x00 != (I2C_MasterStatus() & I2C_MSTAT_RD_CMPLT)) {
                break;
            }
        }
    }
    while (temp != I2C_MSTR_NO_ERROR);
    
    //    data = read_buffer[0];
//    data |= (read_buffer[1] << 8);
//    
//    LCD_Position(1, 0);
//    sprintf(LCD_str, "data: %i, %i  ", read_buffer[0], read_buffer[1]<<8);
//    LCD_PrintString(LCD_str);
    data = (read_buffer[1] << 8);
    
    data += read_buffer[0];
    
    I2C_MasterClearReadBuf();
    return data;
}

uint32 tsl2561_get_lux(void) {
    uint16 clipThreshold = TSL2561_CLIPPING_402MS;
    
    uint16 channel0 = TSL2561_read16(TSL2561_COMMAND_BIT | TSL2561_WORD_BIT | TSL2561_REGISTER_CHAN0_LOW);
    uint16 channel1 = TSL2561_read16(TSL2561_COMMAND_BIT | TSL2561_WORD_BIT | TSL2561_REGISTER_CHAN1_LOW);
    
    if ((channel0 > clipThreshold) || (channel1 > clipThreshold)) {
        return 65536;
    }
    unsigned long chScale = (1 << TSL2561_LUX_CHSCALE);
    chScale = chScale << 4;
    
    
    channel0 = (channel0 * chScale) >> TSL2561_LUX_CHSCALE;
    channel1 = (channel1 * chScale) >> TSL2561_LUX_CHSCALE;
    
    unsigned long ratio1 = 0;
    if (channel0 != 0) ratio1 = (channel1 << (TSL2561_LUX_RATIOSCALE+1)) / channel0;

    /* round the ratio value */
    unsigned long ratio = (ratio1 + 1) >> 1;

    unsigned int b, m;
    
    if ((ratio >= 0) && (ratio <= TSL2561_LUX_K1C))
        {b=TSL2561_LUX_B1C; m=TSL2561_LUX_M1C;}
    else if (ratio <= TSL2561_LUX_K2C)
        {b=TSL2561_LUX_B2C; m=TSL2561_LUX_M2C;}
    else if (ratio <= TSL2561_LUX_K3C)
        {b=TSL2561_LUX_B3C; m=TSL2561_LUX_M3C;}
    else if (ratio <= TSL2561_LUX_K4C)
        {b=TSL2561_LUX_B4C; m=TSL2561_LUX_M4C;}
    else if (ratio <= TSL2561_LUX_K5C)
        {b=TSL2561_LUX_B5C; m=TSL2561_LUX_M5C;}
    else if (ratio <= TSL2561_LUX_K6C)
        {b=TSL2561_LUX_B6C; m=TSL2561_LUX_M6C;}
    else if (ratio <= TSL2561_LUX_K7C)
        {b=TSL2561_LUX_B7C; m=TSL2561_LUX_M7C;}
    else if (ratio > TSL2561_LUX_K8C)
        {b=TSL2561_LUX_B8C; m=TSL2561_LUX_M8C;}
        
    unsigned long temp;
    temp = ((channel0*b) - (channel1*m));
    /* Do not allow negative lux value */
    if (temp< 0) temp = 0;
    /* Round lsb (2^(LUX_SCALE-1)) */
    temp += (1 << (TSL2561_LUX_LUXSCALE-1));
    /* Strip off fractional portion */
    uint32_t lux = temp >> TSL2561_LUX_LUXSCALE;
    /* Signal I2C had no errors */
    return lux;
}

