include("${CMAKE_CURRENT_LIST_DIR}/rule.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/file.cmake")

set(AssemblyEnv_default_library_list )

# Handle files with suffix (s|as|asm|AS|ASM|As|aS|Asm), for group default-XC8
if(AssemblyEnv_default_default_XC8_FILE_TYPE_assemble)
add_library(AssemblyEnv_default_default_XC8_assemble OBJECT ${AssemblyEnv_default_default_XC8_FILE_TYPE_assemble})
    AssemblyEnv_default_default_XC8_assemble_rule(AssemblyEnv_default_default_XC8_assemble)
    list(APPEND AssemblyEnv_default_library_list "$<TARGET_OBJECTS:AssemblyEnv_default_default_XC8_assemble>")

endif()

# Handle files with suffix S, for group default-XC8
if(AssemblyEnv_default_default_XC8_FILE_TYPE_assemblePreprocess)
add_library(AssemblyEnv_default_default_XC8_assemblePreprocess OBJECT ${AssemblyEnv_default_default_XC8_FILE_TYPE_assemblePreprocess})
    AssemblyEnv_default_default_XC8_assemblePreprocess_rule(AssemblyEnv_default_default_XC8_assemblePreprocess)
    list(APPEND AssemblyEnv_default_library_list "$<TARGET_OBJECTS:AssemblyEnv_default_default_XC8_assemblePreprocess>")

endif()

# Handle files with suffix [cC], for group default-XC8
if(AssemblyEnv_default_default_XC8_FILE_TYPE_compile)
add_library(AssemblyEnv_default_default_XC8_compile OBJECT ${AssemblyEnv_default_default_XC8_FILE_TYPE_compile})
    AssemblyEnv_default_default_XC8_compile_rule(AssemblyEnv_default_default_XC8_compile)
    list(APPEND AssemblyEnv_default_library_list "$<TARGET_OBJECTS:AssemblyEnv_default_default_XC8_compile>")

endif()

# Handle files with suffix elf, for group default-XC8
if(AssemblyEnv_default_default_XC8_FILE_TYPE_objcopy_lss)
add_library(AssemblyEnv_default_default_XC8_objcopy_lss OBJECT ${AssemblyEnv_default_default_XC8_FILE_TYPE_objcopy_lss})
    AssemblyEnv_default_default_XC8_objcopy_lss_rule(AssemblyEnv_default_default_XC8_objcopy_lss)
    list(APPEND AssemblyEnv_default_library_list "$<TARGET_OBJECTS:AssemblyEnv_default_default_XC8_objcopy_lss>")

endif()


# Main target for this project
add_executable(AssemblyEnv_default_image_egC35XTy ${AssemblyEnv_default_library_list})

set_target_properties(AssemblyEnv_default_image_egC35XTy PROPERTIES
    OUTPUT_NAME "default"
    SUFFIX ".elf"
    ADDITIONAL_CLEAN_FILES "${output_extensions}"
    RUNTIME_OUTPUT_DIRECTORY "${AssemblyEnv_default_output_dir}")
target_link_libraries(AssemblyEnv_default_image_egC35XTy PRIVATE ${AssemblyEnv_default_default_XC8_FILE_TYPE_link})
# Add the link options from the rule file.
AssemblyEnv_default_link_rule( AssemblyEnv_default_image_egC35XTy)


#Add objcopy steps
AssemblyEnv_default_objcopy_lss_rule(AssemblyEnv_default_image_egC35XTy)

