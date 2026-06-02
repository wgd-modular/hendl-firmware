cmake_minimum_required(VERSION 3.13)

# Pull in Pico SDK (must be before project())
if(DEFINED PICO_SDK_PATH)
    include(${PICO_SDK_PATH}/external/pico_sdk_import.cmake)
else()
    include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)
endif()

project(hendl C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

pico_sdk_init()

add_executable(hendl
    src/main.c
    src/adc_mux.c
    src/pwm_out.c
    src/shift_reg.c
    src/trigger.c
    src/button.c
    src/normalization.c
    src/channel.c
    src/modes/lfo.c
    src/modes/envelope.c
    src/modes/sample_hold.c
    src/modes/turing.c
    src/modes/euclidean.c
    src/modes/smooth_random.c
)

target_include_directories(hendl PRIVATE include)

target_link_libraries(hendl
    pico_stdlib
    hardware_adc
    hardware_pwm
    hardware_gpio
    hardware_timer
    pico_multicore
    m
)

# Note: FPU flags are set by the Pico SDK toolchain for RP2350 (Cortex-M33).
# Do NOT override -mfloat-abi here — the SDK sets softfp for ABI compatibility
# with its prebuilt libraries. The M33 FPU is still used for computation.

# USB serial for debug; disable for production
pico_enable_stdio_usb(hendl 0)
pico_enable_stdio_uart(hendl 0)

pico_add_extra_outputs(hendl)
