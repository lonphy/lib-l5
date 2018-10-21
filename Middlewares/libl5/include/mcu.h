//
// Created by lonphy on 2018/10/20.
//

#ifndef __LIB_L5_MCU_H__
#define __LIB_L5_MCU_H__

/* ----------- mcu family core header file ----------------*/
#if defined(STM32F1)

#include <stm32f1xx_hal.h>

#elif defined(STM32F4)

#include <stm32f4xx_hal.h>

#elif defined(STM32H7)

#include <stm32h7xx_hal.h>

#else
    #error no MCU specified
#endif
/* ----------- mcu family core header file ----------------*/



#endif //__LIB_L5_MCU_H__
