cmake_minimum_required(VERSION 3.20)

# XC8 is driven explicitly by cmake/XC8Firmware.cmake.  Declaring no native
# languages avoids CMake's GCC/Clang compiler-detection assumptions.
project(PIC18Projects LANGUAGES NONE)

set(FIRMWARE_PROJECT "Lab3" CACHE STRING
    "Folder name under projects/ to configure")

set(project_directory "${CMAKE_CURRENT_SOURCE_DIR}/projects/${FIRMWARE_PROJECT}")
if(NOT EXISTS "${project_directory}/CMakeLists.txt")
    file(GLOB available_projects RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}/projects"
         "${CMAKE_CURRENT_SOURCE_DIR}/projects/*")
    message(FATAL_ERROR
        "Unknown FIRMWARE_PROJECT='${FIRMWARE_PROJECT}'. "
        "Available project folders: ${available_projects}")
endif()

add_subdirectory("${project_directory}" "project")
