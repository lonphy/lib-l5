//
// Created by lonphy on 2018/10/20.
//

#ifndef __LIB_L5_MCU_H__
#define __LIB_L5_MCU_H__

/* ----------- mcu family core header file ----------------*/
#if defined(STM32F1)

#include <stm32f1xx_ll_cortex.h>
#include <stm32f1xx_ll_bus.h>
#include <stm32f1xx_ll_rcc.h>
#include <stm32f1xx_ll_system.h>
#include <stm32f1xx_ll_utils.h>
#include <stm32f1xx_ll_gpio.h>
#include <stm32f1xx_ll_exti.h>
#include <stm32f1xx_ll_dma.h>
#include <stm32f1xx_ll_usart.h>
#include <stm32f1xx_ll_pwr.h>
#include <stm32f1xx_ll_i2c.h>
#include <stm32f1xx_ll_crc.h>

#ifndef NVIC_PRIORITYGROUP_0
#define NVIC_PRIORITYGROUP_0         ((uint32_t)0x00000007) /*!< 0 bit  for pre-emption priority,
                                                                 4 bits for subpriority */
#define NVIC_PRIORITYGROUP_1         ((uint32_t)0x00000006) /*!< 1 bit  for pre-emption priority,
                                                                 3 bits for subpriority */
#define NVIC_PRIORITYGROUP_2         ((uint32_t)0x00000005) /*!< 2 bits for pre-emption priority,
                                                                 2 bits for subpriority */
#define NVIC_PRIORITYGROUP_3         ((uint32_t)0x00000004) /*!< 3 bits for pre-emption priority,
                                                                 1 bit  for subpriority */
#define NVIC_PRIORITYGROUP_4         ((uint32_t)0x00000003) /*!< 4 bits for pre-emption priority,
                                                                 0 bit  for subpriority */
#endif

#define L5_NVIC_SetPriority(irq, priority) do{              \
    NVIC_SetPriority(irq, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), priority,0)); \
}while(0)

#elif defined(STM32F4)

#include <stm32f4xx_hal.h>

#elif defined(STM32H7)
#error not support
#else
#error no MCU specified
#endif
/* ----------- mcu family core header file ----------------*/



#endif //__LIB_L5_MCU_H__
