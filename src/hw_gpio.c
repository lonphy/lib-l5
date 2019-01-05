#include <lib_l5.h>
#include "hw_gpio.h"

void hw_gpio_init(void) {
    /* GPIO Ports Clock Enable */
    LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOA);
    LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOB);
    LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOC);
    LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOD);

#ifdef STM32F4
    LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOE);
    LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOF);
    LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOH);
#endif

#if defined(STM32F1)
    LL_GPIO_SetPinMode(WIFI_ENABLE_GPIO, WIFI_ENABLE_PIN, LL_GPIO_MODE_OUTPUT);
    LL_GPIO_SetPinSpeed(WIFI_ENABLE_GPIO, WIFI_ENABLE_PIN, LL_GPIO_SPEED_FREQ_LOW);
    LL_GPIO_SetOutputPin(WIFI_ENABLE_GPIO, WIFI_ENABLE_PIN);
#endif
}
