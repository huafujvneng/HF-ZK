#ifndef __APPLICATION_BASICDEFINE_H__
#define __APPLICATION_BASICDEFINE_H__

#include <stdint.h>
#include <stdbool.h>

#define ARM_MATH_CM4
#define __FPU_PRESENT (1)
#include "arm_math.h"

#define STRUCTOFFSET(type, member) (uint32_t)(&(((type *)0)->member))

typedef struct __attribute__((__packed__))
{
	uint8_t		year;
	uint8_t		month;
	uint8_t		day;
	uint8_t		hour;
	
	uint8_t		minute;
	uint8_t		second;
	uint16_t	msec;

} TIME;	


#endif // __APPLICATION_BASICDEFINE_H__
