#include <lib_l5.h>
#include "hw_gpio.h"

/* GPIO Ports Clock Enable */
void hw_gpio_init(void) {
#ifdef STM32F1
    LL_APB2_GRP1_EnableClock(
            LL_APB2_GRP1_PERIPH_GPIOA |
            LL_APB2_GRP1_PERIPH_GPIOB |
            LL_APB2_GRP1_PERIPH_GPIOC |
            LL_APB2_GRP1_PERIPH_GPIOD
    );

#elif STM32F4
    LL_AHB1_GRP1_EnableClock(
            LL_AHB1_GRP1_PERIPH_GPIOA |
            LL_AHB1_GRP1_PERIPH_GPIOB |
            LL_AHB1_GRP1_PERIPH_GPIOC |
            LL_AHB1_GRP1_PERIPH_GPIOD |
            LL_AHB1_GRP1_PERIPH_GPIOE |
            LL_AHB1_GRP1_PERIPH_GPIOF |
            LL_AHB1_GRP1_PERIPH_GPIOH
    );
#endif
}
