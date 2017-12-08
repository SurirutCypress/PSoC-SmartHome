#if !defined(WCS1800_H)
#define WCS1800_H

#include <project.h>    
 
/***************************************
*        Function Prototypes
***************************************/
    
void calibrate_zero_voltage(void); 
float32  get_current(void);
void check(void);
    
#endif
