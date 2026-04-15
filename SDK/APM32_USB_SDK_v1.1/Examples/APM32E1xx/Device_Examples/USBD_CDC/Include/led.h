
/* Define to prevent recursive inclusion */
#ifndef __LED
#define __LED
#include "main.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef void (*func)(Led_TypeDef Led);
typedef struct __attribute__((packed,aligned(4)))
{
	func ledon;
	func ledoff;
	
}LED_Function;

extern LED_Function led_func;











#ifdef __cplusplus
}
#endif

#endif
