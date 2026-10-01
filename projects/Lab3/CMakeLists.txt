include("${CMAKE_SOURCE_DIR}/cmake/XC8Firmware.cmake")

xc8_add_firmware(
    NAME Lab3
    DEVICE_FILE "${CMAKE_SOURCE_DIR}/cmake/devices/PIC18F46K22.cmake"
    SOURCES
        src/main.c
    HEADERS
        include/Configuration.h
    INCLUDE_DIRS
        include
    COMPILE_OPTIONS
        -O2
)
