include(CMakeForceCompiler)
cmake_minimum_required(VERSION 3.12)
project(lib-l5 C ASM)

file(MAKE_DIRECTORY util)
file(REMOVE Makefile)
file(RENAME Inc inc)
file(RENAME Src Src)

# 手工完成
# - 移动目录下的*.s *.ld 到 util目录中

# ============= 配置区域 ========================
set(HEAPX heap_4)

# all include dir
set(inc_dir "")

# all src files
set(src_files "")


# ============= 用户代码区域 =====================
set(LIB_L5 Middlewares/libl5)
list(APPEND inc_dir ${LIB_L5}/include)

file(GLOB_RECURSE LIB_L5_INC ${LIB_L5}/include/*.h)
file(GLOB_RECURSE LIB_L5_SRC ${LIB_L5}/source/*.c)

list(APPEND src_files ${LIB_L5_SRC} ${LIB_L5_INC})

# ============= 用户代码区域 =====================

# ================ 依赖库区域 =============================================
set(3rd Middlewares/Third_Party)
set(3rdst Middlewares/ST)


set(rtos_root ${3rd}/FreeRTOS/Source)
set(rtos_gcc ${rtos_root}/portable/GCC)
set(CMSIS Drivers/CMSIS)
set(RTOS_INC ${rtos_root}/include)
set(HAL_INC Drivers/${MCU_SERIES}_HAL_Driver/Inc)
set(CMSIS_DEVICE_INC ${CMSIS}/Device/ST/${MCU_SERIES}/Include)
set(CMSIS_DEVICE_SRC ${CMSIS}/Device/ST/${MCU_SERIES}/Source/Templates)
set(CMSIS_INC ${CMSIS}/Include)

# ================ USB library start =============================================
set(libusb ${3rdst}/STM32_USB_Device_library)
set(libusb_class ${libusb}/Class)

# help tool for add usb dirs
macro(usb_config dir)
    list(APPEND inc_dir ${dir}/Inc)

    file(GLOB usb_tmp ${dir}/Inc/*.h)
    list(APPEND src_files ${usb_tmp})

    file(GLOB usb_tmp ${dir}/Src/*.c)
    list(APPEND src_files ${usb_tmp})

    unset(usb_tmp)
endmacro(usb_config)

macro(setup_usb)
usb_config(${libusb}/Core)
#usb_config("${libusb_class}/AUDIO")
usb_config(${libusb_class}/CDC)
#usb_config("${libusb_class}/CustomHID")
#usb_config("${libusb_class}/DFU")
#usb_config("${libusb_class}/HID")
#usb_config("${libusb_class}/MSC")
endmacro(setup_usb)

# setup_usb()
# ================= USB library end ===============================================

file(GLOB_RECURSE CMSIS_DEVICE_INC_F ${CMSIS_DEVICE_INC}/*.h)
file(GLOB_RECURSE CMSIS_INC_F ${CMSIS_INC}/*.h)
file(GLOB_RECURSE HAL_INC_F ${HAL_INC}/*.h)

file(GLOB_RECURSE RTOS_INC_F ${RTOS_INC}/*.h)
file(GLOB_RECURSE CMSIS_ROTS_INC_F ${rtos_root}/CMSIS_RTOS/*.h)

file(GLOB RTOS_PORT_SRC ${rtos_root}/portable/MemMang/${HEAPX}.c)

set(RTOS_MCU_PORT ${rtos_gcc}/${MCU_CORE_TYPE})
if ("${MCU_CORE_TYPE}" STREQUAL "ARM_CM4")
    set(RTOS_MCU_PORT ${rtos_gcc}/ARM_CM4F)
endif ()

list(APPEND inc_dir ${RTOS_MCU_PORT})
list(APPEND src_files ${RTOS_MCU_PORT}/portmacro.h)
list(APPEND RTOS_PORT_SRC ${RTOS_MCU_PORT}/port.c)

file(GLOB_RECURSE HAL_SRC Drivers/${MCU_SERIES}_HAL_DRIVER/Src/*.c)
file(GLOB RTOS_SRC ${rtos_root}/*.c)
file(GLOB_RECURSE CMSIS_RTOS_SRC ${rtos_root}/CMSIS_RTOS/*.c)

file(GLOB CMSIS_SRC ${CMSIS_DEVICE_SRC}/*.c)

list(APPEND inc_dir
        ${HAL_INC}
        ${CMSIS_DEVICE_INC}
        ${CMSIS_INC}
        ${RTOS_INC}
        ${rtos_root}/CMSIS_RTOS)

list(APPEND src_files
        ${HAL_SRC}
        ${RTOS_PORT_SRC}
        ${RTOS_SRC}
        ${CMSIS_SRC}
        ${CMSIS_RTOS_SRC}

        ${HAL_INC_F}
        ${CMSIS_DEVICE_INC_F}
        ${CMSIS_INC_F}

        ${RTOS_INC_F}
        ${CMSIS_ROTS_INC_F})

# ============================ 依赖库区域 ====================================================

# file 命令
#
# file(WRITE  filename "file content")  # 将"file content" 写入文件 filename
# file(APPEND filename "file content")  # 将"file content" 追加至文件 filename
# file(STRINGS filename varMyfile)      # 读取 filename 的内容，存储到varMyfile变量中

set(USER_INC inc)

file(GLOB_RECURSE USER_INC_F ${USER_INC}/*.h)
file(GLOB_RECURSE USER_SRC src/*.c)

set(SOURCE_FILES
        ${src_files}
        ${USER_SRC}
        ${STARTUP_FILE}

        ${MCU_LINKER_SCRIPT}

        ${USER_INC_F})

# clear all template files
list(FILTER SOURCE_FILES EXCLUDE REGEX .*_template\\.[hc]$)

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-Map=${PROJECT_SOURCE_DIR}/build/${PROJECT_NAME}_${DEVICE_ID}.map,--cref -g -gdwarf-2")

set(ELF_FILE ${PROJECT_NAME}_${DEVICE_ID}.elf)
set(BIN_FILE ${PROJECT_SOURCE_DIR}/build/${PROJECT_NAME}_${DEVICE_ID}.bin)

include_directories(${USER_INC} ${inc_dir})

add_executable(${ELF_FILE} ${SOURCE_FILES})

add_custom_command(TARGET ${ELF_FILE} PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory ${PROJECT_SOURCE_DIR}/build
        COMMENT "Creating build directory")

add_custom_command(TARGET ${ELF_FILE} POST_BUILD
        COMMAND ${CMAKE_OBJCOPY} -O binary $<TARGET_FILE:${ELF_FILE}> ${BIN_FILE}
        COMMAND ${CMAKE_SIZE} $<TARGET_FILE:${ELF_FILE}>
        COMMENT "Building ${BIN_FILE}")
