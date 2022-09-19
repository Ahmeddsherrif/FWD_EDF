

#ifndef _BIT_MATH_H
#define _BIT_MATH_H



//********** Bits Manipulation Macros ***************************

#define SET_BIT(reg,bitN)    ((reg) |= (1<<bitN))     // Sets a bit (bitN) in a register (reg)
#define CLR_BIT(reg,bitN)  ((reg) &= ~(1<<bitN))      // Clears a bit (bitN) in a register (reg)
#define TOG_BIT(reg,bitN) ((reg) ^= (1<<bitN))        // Toggles a bit (bitN) in a register (reg)
#define GET_BIT(reg,bitN)    (1 & ((reg)>>bitN))      // Returns value of bit (bitN) in a register (reg)


			
#endif
