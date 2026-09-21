# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Initialise pico_sdk from installed location
# (note this can come from environment, CMake cache etc)

# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.3.1)
set(toolchainVersion 15_2_Rel1)
set(picotoolVersion 2.3.1)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================
set(PICO_BOARD pico CACHE STRING "Board type")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

project(bad_raspberry C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# Add executable. Default name is the project name, version 0.1
# List any source files to include in compilation here. 
add_executable(bad_raspberry main.c )

pico_set_program_name(bad_raspberry "bad_raspberry")
pico_set_program_version(bad_raspberry "0.1")

# Modify the below lines to enable/disable output over UART/USB
pico_enable_stdio_uart(bad_raspberry 0)
pico_enable_stdio_usb(bad_raspberry 0)

# Add the standard library to the build
target_link_libraries(bad_raspberry
        pico_stdlib)

# Add the standard include files to the build
target_include_directories(bad_raspberry PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}
)

target_link_libraries(bad_raspberry
        hardware_spi
        hardware_gpio)

pico_add_extra_outputs(bad_raspberry)

