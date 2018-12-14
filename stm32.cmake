include(CMakeForceCompiler)

#set(DEVICE_ID STM32F407ZGTx)
set(DEVICE_ID STM32F103RCTx)

##########################################################################################
set(CMAKE_SYSTEM_NAME  Generic)
set(CMAKE_C_COMPILER   arm-none-eabi-gcc)
set(CMAKE_CXX_COMPILER arm-none-eabi-g++)
set(CMAKE_ASM_COMPILER arm-none-eabi-as)
set(CMAKE_OBJCOPY      arm-none-eabi-objcopy)
set(CMAKE_SIZE         arm-none-eabi-size)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/build)

# 设置mcu信息
macro(set_mcu_info id)
    string(SUBSTRING ${id} 0 7 MCU_FAMILY)
    string(SUBSTRING ${id} 0 9 MCU_NAME)
    string(SUBSTRING ${id} 10 1 MCU_FLASH_SIZE)

    set(MCU_FLAG   "-mthumb")

    if (${MCU_FAMILY} STREQUAL "STM32F1")
        set(MCU_CORE_TYPE ARM_CM3)
        set(CMAKE_SYSTEM_PROCESSOR cortex-m3)
        set(MCU_FLAG "${MCU_FLAG} -mcpu=cortex-m3")
    elseif (${MCU_FAMILY} STREQUAL "STM32F4")
        set(MCU_CORE_TYPE ARM_CM4)
        set(CMAKE_SYSTEM_PROCESSOR cortex-m4)
        set(MCU_FLAG "${MCU_FLAG} -mcpu=cortex-m4")
        set(MCU_FLAG "${MCU_FLAG} -mfpu=fpv4-sp-d16")
        set(MCU_FLAG "${MCU_FLAG} -mfloat-abi=hard")
    elseif (${MCU_FAMILY} STREQUAL "STM32H7")
        set(MCU_CORE_TYPE ARM_CM7)
        set(CMAKE_SYSTEM_PROCESSOR cortex-m7)
        set(MCU_FLAG "${MCU_FLAG} -mcpu=cortex-m7")
    endif ()

    if (${MCU_FLASH_SIZE} STREQUAL "8")
        set(MCU_REF       ${MCU_NAME}xB)
    elseif (${MCU_FLASH_SIZE} STREQUAL "C")
        set(MCU_REF       ${MCU_NAME}xE)
    elseif (${MCU_FLASH_SIZE} STREQUAL "G")
        set(MCU_REF       ${MCU_NAME}xx)
    elseif (${MCU_FLASH_SIZE} STREQUAL "I")
        set(MCU_REF       ${MCU_NAME}xx)
    endif ()
    unset(MCU_FLASH_SIZE)

    set(MCU_SERIES        ${MCU_FAMILY}xx)
    set(DRIVER_TPL_ROOT   ${PROJECT_SOURCE_DIR}/Drivers/CMSIS/Device/ST/${MCU_SERIES}/Source/Templates/gcc/)
    set(STARTUP_FILE      startup_${MCU_REF}.s)
    string(TOLOWER        ${STARTUP_FILE} STARTUP_FILE)
    set(STARTUP_FILE      ${DRIVER_TPL_ROOT}/${STARTUP_FILE})
    set(LINKER_SCRIPT     ${PROJECT_SOURCE_DIR}/util/${DEVICE_ID}_FLASH.ld)

endmacro(set_mcu_info)

# 这只编译器信息
macro(setup_compiler)

    set(COMMON_MARCO "-DUSE_HAL_LIBRARY -D${MCU_FAMILY} -D${MCU_REF}")
    set(COMMON_FLAGS "${COMMON_MARCO} ${MCU_FLAG}")
    set(COMMON_FLAGS "${COMMON_FLAGS} -lc -lm --specs=nosys.specs --specs=nano.specs")
    set(COMMON_FLAGS "${COMMON_FLAGS} -Wall -g")
    set(COMMON_FLAGS "${COMMON_FLAGS} -ffunction-sections -fdata-sections -fno-common -fmessage-length=0")

    set(CMAKE_C_FLAGS "${COMMON_FLAGS} -std=gnu99")
    set(CMAKE_CXX_FLAGS "${COMMON_FLAGS} -std=c++11")

    set(CMAKE_ASM_FLAGS "${MCU_FLAG} -Wall")

    set(CMAKE_EXE_LINKER_FLAGS "-Wl,-gc-sections -T ${LINKER_SCRIPT}")

    unset(MCU_FLAGS)
    unset(COMMON_MARCO)
    unset(COMMON_FLAGS)
endmacro(setup_compiler)


set_mcu_info(${DEVICE_ID})
setup_compiler()