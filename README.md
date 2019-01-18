# lib-l5
stm32f1/f4 ext module

#### Config CLion

set `Preferences`->`Build,Execution,Deployment`->`CMake`->`CMake Options` to:
  `-DCMAKE_TOOLCHAIN_FILE=stm32.cmake`


#### Tested MCU

- STM32F103C8Tx(no test)
    - esp8266 -> usart2
    - oled(ssd1306) -> IIC1
    - lcd12864(ST7565P) -> driven directly by GPIO
    - lcd1602  -> driven directly by GPIO

- STM32F103RCTx
    - esp8266       -> uart4 with Rx/Tx dma
    - oled(ssd1306) -> I2C2 with block mode
    - at24c02       -> I2C1 with block mode
    - led           -> driven directly by GPIO

- STM32F407ZGTx(no test)
    - esp8266 -> usart2
    - oled(ssd1306) -> IIC1
    - lcd12864(ST7565P) -> driven directly by GPIO
    - lcd1602  -> driven directly by GPIO