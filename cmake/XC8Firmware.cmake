include_guard(GLOBAL)
include(CMakeParseArguments)

function(_latest_path output_variable glob_expression)
    file(GLOB candidates LIST_DIRECTORIES true "${glob_expression}")
    if(candidates)
        list(SORT candidates COMPARE NATURAL ORDER DESCENDING)
        list(GET candidates 0 newest_candidate)
        set(${output_variable} "${newest_candidate}" PARENT_SCOPE)
    endif()
endfunction()

function(_find_pic_tools)
    # The cache variables let a user override every auto-detected path with -D.
    set(XC8_CC "" CACHE FILEPATH "Full path to xc8-cc")
    set(PIC_DFP_XC8 "" CACHE PATH "PIC18F-K_DFP version's xc8 directory")
    set(IPECMD "" CACHE FILEPATH "Full path to ipecmd.sh")

    # Package upgrades often remove the previously cached versioned path.
    # Clear only stale entries, then run discovery again below.
    if(XC8_CC AND NOT EXISTS "${XC8_CC}")
        message(STATUS "Cached XC8 path is stale; searching installed versions")
        set(XC8_CC "" CACHE FILEPATH "Full path to xc8-cc" FORCE)
    endif()
    if(PIC_DFP_XC8 AND NOT IS_DIRECTORY "${PIC_DFP_XC8}")
        message(STATUS "Cached DFP path is stale; searching installed versions")
        set(PIC_DFP_XC8 "" CACHE PATH
            "PIC18F-K_DFP version's xc8 directory" FORCE)
    endif()
    if(IPECMD AND NOT EXISTS "${IPECMD}")
        message(STATUS "Cached IPECMD path is stale; searching installed versions")
        set(IPECMD "" CACHE FILEPATH "Full path to ipecmd.sh" FORCE)
    endif()

    if(NOT XC8_CC)
        unset(found_xc8 CACHE)
        find_program(found_xc8 NAMES xc8-cc
            HINTS "$ENV{XC8_ROOT}/bin")
        if(NOT found_xc8)
            _latest_path(found_xc8 "/opt/microchip/xc8/*/bin/xc8-cc")
        endif()
        if(found_xc8)
            set(XC8_CC "${found_xc8}" CACHE FILEPATH "Full path to xc8-cc" FORCE)
        endif()
    endif()

    if(NOT PIC_DFP_XC8)
        _latest_path(found_dfp
            "$ENV{HOME}/.mchp_packs/Microchip/${PIC_DFP_FAMILY}/*/xc8")
        if(found_dfp)
            set(PIC_DFP_XC8 "${found_dfp}" CACHE PATH
                "PIC18F-K_DFP version's xc8 directory" FORCE)
        endif()
    endif()

    if(NOT IPECMD)
        unset(found_ipecmd CACHE)
        find_program(found_ipecmd NAMES ipecmd.sh ipecmd
            HINTS "$ENV{MPLABX_ROOT}/mplab_platform/mplab_ipe")
        if(NOT found_ipecmd)
            _latest_path(found_ipecmd
                "/opt/microchip/mplabx/*/mplab_platform/mplab_ipe/ipecmd.sh")
        endif()
        if(found_ipecmd)
            set(IPECMD "${found_ipecmd}" CACHE FILEPATH
                "Full path to ipecmd.sh" FORCE)
        endif()
    endif()

    if(NOT EXISTS "${XC8_CC}")
        message(FATAL_ERROR
            "xc8-cc was not found. Add it to PATH or configure with "
            "-DXC8_CC=/opt/microchip/xc8/<version>/bin/xc8-cc")
    endif()
    if(NOT IS_DIRECTORY "${PIC_DFP_XC8}")
        message(FATAL_ERROR
            "The ${PIC_DFP_FAMILY} device pack was not found. Install it with "
            "MPLAB X, or configure with -DPIC_DFP_XC8=<pack>/<version>/xc8")
    endif()
    if(NOT EXISTS "${IPECMD}")
        message(FATAL_ERROR
            "ipecmd.sh was not found. Install MPLAB IPE, or configure with "
            "-DIPECMD=/opt/microchip/mplabx/<version>/mplab_platform/"
            "mplab_ipe/ipecmd.sh")
    endif()

    set(XC8_CC "${XC8_CC}" PARENT_SCOPE)
    set(PIC_DFP_XC8 "${PIC_DFP_XC8}" PARENT_SCOPE)
    set(IPECMD "${IPECMD}" PARENT_SCOPE)
endfunction()

function(xc8_add_firmware)
    set(options)
    set(one_value_args NAME DEVICE_FILE)
    set(multi_value_args SOURCES HEADERS INCLUDE_DIRS COMPILE_OPTIONS)
    cmake_parse_arguments(XF "${options}" "${one_value_args}"
                          "${multi_value_args}" ${ARGN})

    if(NOT XF_NAME OR NOT XF_DEVICE_FILE OR NOT XF_SOURCES)
        message(FATAL_ERROR
            "xc8_add_firmware requires NAME, DEVICE_FILE, and SOURCES")
    endif()

    include("${XF_DEVICE_FILE}")
    _find_pic_tools()

    set(absolute_sources)
    foreach(source_file IN LISTS XF_SOURCES)
        get_filename_component(source_file "${source_file}" ABSOLUTE
                               BASE_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
        list(APPEND absolute_sources "${source_file}")
    endforeach()

    set(absolute_headers)
    foreach(header_file IN LISTS XF_HEADERS)
        get_filename_component(header_file "${header_file}" ABSOLUTE
                               BASE_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
        list(APPEND absolute_headers "${header_file}")
    endforeach()

    set(include_flags)
    foreach(include_directory IN LISTS XF_INCLUDE_DIRS)
        get_filename_component(include_directory "${include_directory}" ABSOLUTE
                               BASE_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
        list(APPEND include_flags "-I${include_directory}")
    endforeach()

    set(output_directory "${CMAKE_BINARY_DIR}/firmware")
    set(elf_file "${output_directory}/${XF_NAME}.elf")
    set(hex_file "${output_directory}/${XF_NAME}.hex")

    # xc8-cc is intentionally called once for compile + assemble + link. It
    # creates both the ELF and Intel HEX outputs. This is simpler and more
    # reliable for small lab projects than pretending XC8 is GCC inside CMake.
    add_custom_command(
        OUTPUT "${hex_file}"
        BYPRODUCTS "${elf_file}"
        COMMAND "${CMAKE_COMMAND}" -E make_directory "${output_directory}"
        COMMAND "${XC8_CC}"
                "-mcpu=${PIC_MCU}"
                "-mdfp=${PIC_DFP_XC8}"
                ${include_flags}
                ${XF_COMPILE_OPTIONS}
                ${absolute_sources}
                -o "${elf_file}"
        DEPENDS ${absolute_sources} ${absolute_headers}
        COMMAND_EXPAND_LISTS
        VERBATIM
        COMMENT "Building ${XF_NAME}.hex for PIC${PIC_MCU}"
    )

    add_custom_target(firmware ALL DEPENDS "${hex_file}")

    # -M programs all memory, -Y verifies it, and -OL releases the MCU from
    # reset so the new firmware starts running.
    add_custom_target(program
        COMMAND "${IPECMD}"
                "-P${PIC_MCU}"
                "-TP${PIC_PROGRAMMER}"
                "-F${hex_file}"
                -M -Y -OL
        DEPENDS firmware
        USES_TERMINAL
        VERBATIM
        COMMENT "Programming PIC${PIC_MCU} with ${PIC_PROGRAMMER}"
    )

    message(STATUS "Firmware project : ${XF_NAME}")
    message(STATUS "Target MCU       : PIC${PIC_MCU}")
    message(STATUS "XC8 compiler     : ${XC8_CC}")
    message(STATUS "Device pack      : ${PIC_DFP_XC8}")
    message(STATUS "Programmer       : ${PIC_PROGRAMMER}")
    message(STATUS "IPECMD           : ${IPECMD}")
    message(STATUS "HEX output       : ${hex_file}")
endfunction()
