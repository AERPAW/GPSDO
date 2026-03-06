cmake_minimum_required(VERSION 3.13)
include(pico_sdk_import.cmake)
project(gpsdo_firmware)
pico_sdk_init()

file(GLOB_RECURSE SRC "src/*.c")

add_executable(${PROJECT_NAME} ${SRC})

target_link_libraries(${PROJECT_NAME} pico_stdlib pico_multicore pico_unique_id tinyusb_device tinyusb_board hardware_uart)

target_include_directories(${PROJECT_NAME} PRIVATE "src/")

pico_enable_stdio_usb(${PROJECT_NAME} 0)
pico_enable_stdio_uart(${PROJECT_NAME} 0)
pico_add_extra_outputs(${PROJECT_NAME})