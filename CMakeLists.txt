cmake_minimum_required(VERSION 3.12)
project(rtos C ASM)

file(MAKE_DIRECTORY util)
file(REMOVE Makefile)
file(RENAME Inc inc)
file(RENAME Src Src)

# 手工完成
# - 移动目录下的*.s *.ld 到 util目录中

# ============= 配置区域 ========================
#set(USE_DSP 1)
set(HEAPX heap_4)
set(ALL_DEPS_INC "")
set(SRC_FILES "")


# ============= 用户代码区域 =====================
set(LIB_L5 Middlewares/libl5)
list(APPEND ALL_DEPS_INC ${LIB_L5}/include)

file(GLOB_RECURSE LIB_L5_INC ${LIB_L5}/include/*.h)
file(GLOB_RECURSE LIB_L5_SRC ${LIB_L5}/source/*.c)

list(APPEND SRC_FILES ${LIB_L5_SRC} ${LIB_L5_INC})

# ============= 用户代码区域 =====================

# ================ 依赖库区域 =============================================
set(3rd Middlewares/Third_Party)
set(rtos_root ${3rd}/FreeRTOS/Source)
set(CMSIS Drivers/CMSIS)
set(RTOS_INC ${rtos_root}/include)
set(HAL_INC Drivers/${MCU_FAMILY}_HAL_Driver/Inc)
set(CMSIS_DEVICE_INC ${CMSIS}/Device/ST/${MCU_FAMILY}/Include)
set(CMSIS_INC ${CMSIS}/Include)


file(GLOB_RECURSE CMSIS_DEVICE_INC_F ${CMSIS_DEVICE_INC}/*.h)
file(GLOB_RECURSE CMSIS_INC_F ${CMSIS_INC}/*.h)
file(GLOB_RECURSE HAL_INC_F ${HAL_INC}/*.h)

file(GLOB_RECURSE RTOS_INC_F ${RTOS_INC}/*.h)
file(GLOB_RECURSE CMSIS_ROTS_INC_F ${rtos_root}/CMSIS_RTOS/*.h)
file(GLOB_RECURSE RTOS_PORT_INC_F ${rtos_root}/portable/GCC/${MCU_TYPE}/*.h)

file(GLOB_RECURSE HAL_SRC Drivers/${MCU_FAMILY}_HAL_DRIVER/Src/*.c)


file(GLOB RTOS_PORT_SRC
        ${rtos_root}/portable/MemMang/${HEAPX}.c
        ${rtos_root}/portable/GCC/${MCU_TYPE}/PORT.c)
file(GLOB RTOS_SRC ${rtos_root}/*.c)
file(GLOB_RECURSE CMSIS_RTOS_SRC ${rtos_root}/CMSIS_RTOS/*.c)

list(APPEND ALL_DEPS_INC
        ${HAL_INC}
        ${CMSIS_DEVICE_INC}
        ${CMSIS_INC}
        ${RTOS_INC}
        ${rtos_root}/CMSIS_RTOS
        ${rtos_root}/portable/GCC/${MCU_TYPE})

list(APPEND SRC_FILES
        ${HAL_SRC}
        ${RTOS_PORT_SRC}
        ${RTOS_SRC}
        ${CMSIS_RTOS_SRC}

        ${HAL_INC_F}
        ${CMSIS_DEVICE_INC_F}
        ${CMSIS_INC_F}

        ${RTOS_INC_F}
        ${CMSIS_ROTS_INC_F}
        ${RTOS_PORT_INC_F}
        )

# ============================= dsp lib ==========================
if (${USE_DSP})
    set(FPU_FLAG           "-mfloat-abi=soft -mfpu=vfp")
    set(CMAKE_C_FLAGS "${FPU_FLAG} ${CMAKE_C_FLAGS}  -D__FPU_PRESENT -DARM_MATH_CM3")
    set(CMAKE_CXX_FLAGS "${FPU_FLAG} ${CMAKE_CXX_FLAGS}  -D__FPU_PRESENT -DARM_MATH_CM3")
    set(CMAKE_ASM_FLAGS "${FPU_FLAG} ${CMAKE_ASM_FLAGS}")

    set(DSP_SRC_DIR ${CMSIS}/DSP_Lib/Source)
    file(GLOB_RECURSE DSP_SRC ${DSP_SRC_DIR}/*.c)
    list(APPEND SRC_FILES ${DSP_SRC})

    # printf/sprint float support
    set(CMAKE_EXE_LINKER_FLAGS "-u _printf_float ${CMAKE_EXE_LINKER_FLAGS}")

endif ()
# ============================= dsp lib ==========================

# ============================ 依赖库区域 ====================================================

# file 命令
#
# file(WRITE  filename "file content")  # 将"file content" 写入文件 filename
# file(APPEND filename "file content")  # 将"file content" 追加至文件 filename
# file(STRINGS filename varMyfile)      # 读取 filename 的内容，存储到varMyfile变量中

set(USER_INC inc)

file(GLOB_RECURSE USER_INC_F ${USER_INC}/*.h)
file(GLOB_RECURSE USER_SRC src/*.c)
file(GLOB_RECURSE CMSIS_STARTUP util/*.s)

set(SOURCE_FILES
        ${SRC_FILES}
        ${USER_SRC}
        ${CMSIS_STARTUP}

        ${MCU_LINKER_SCRIPT}

        ${USER_INC_F}
        )

# clear all template files
list(FILTER SOURCE_FILES EXCLUDE REGEX .*_template\\.[hc]$)

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-Map=${PROJECT_SOURCE_DIR}/build/${PROJECT_NAME}.map,--cref -g -gdwarf-2")

set(ELF_FILE ${PROJECT_NAME}.elf)
set(BIN_FILE ${PROJECT_SOURCE_DIR}/build/${PROJECT_NAME}.bin)

include_directories(${USER_INC} ${ALL_DEPS_INC})


add_executable(${PROJECT_NAME}.elf ${SOURCE_FILES})

add_custom_command(TARGET ${ELF_FILE} PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory ${PROJECT_SOURCE_DIR}/build
        COMMENT "Creating build directory")

add_custom_command(TARGET ${ELF_FILE} POST_BUILD
        COMMAND ${CMAKE_OBJCOPY} -Obinary $<TARGET_FILE:${PROJECT_NAME}.elf> ${BIN_FILE}
        COMMAND ${CMAKE_SIZE} $<TARGET_FILE:${PROJECT_NAME}.elf>
        COMMENT "Building ${BIN_FILE}")
