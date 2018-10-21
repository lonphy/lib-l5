include(CMakeForceCompiler)

# CMAKE_SYSTEM_NAME
# 目标机target所在的操作系统名称, ARM有操作系统写Linux, 无操作系统写Generic
set(CMAKE_SYSTEM_NAME              Generic)
set(CMAKE_SYSTEM_PROCESSOR         cortex-m3)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/build)
set(CMAKE_FIND_ROOT_PATH            /usr/local/arm/gcc-arm-none-eabi/bin)  # 优先查找路径

set(CMAKE_C_COMPILER   arm-none-eabi-gcc)
set(CMAKE_CXX_COMPILER arm-none-eabi-g++)
set(CMAKE_ASM_COMPILER arm-none-eabi-as)
set(CMAKE_OBJCOPY      arm-none-eabi-objcopy)
set(CMAKE_SIZE         arm-none-eabi-size)

set(MCU_FLAG           "-mcpu=cortex-m3 -mthumb")

set(CMAKE_ASM_FLAGS    "${MCU_FLAG} -Wall")
set(COMMON_MARCO       "-DUSE_HAL_LIBRARY -DSTM32F1 -DSTM32F103xB -DDEBUG")
set(MCU_TYPE           ARM_CM3)
set(MCU_FAMILY         STM32F1xx)
set(MCU_NAME           STM32F103x)
set(MCU_LINKER_SCRIPT  ${PROJECT_SOURCE_DIR}/util/STM32F103C8Tx_FLASH.ld)


set(CMAKE_C_)
set(COMMON_FLAGS       "${COMMON_MARCO} ${MCU_FLAG}  -lc -lm --specs=nosys.specs --specs=nano.specs -Wall -g -ffunction-sections -fdata-sections -fno-common -fmessage-length=0")
set(CMAKE_C_FLAGS      "${COMMON_FLAGS} -std=gnu99")
set(CMAKE_CXX_FLAGS    "${COMMON_FLAGS} -std=c++11")

set(CMAKE_EXE_LINKER_FLAGS "-Wl,-gc-sections -T ${MCU_LINKER_SCRIPT}")

