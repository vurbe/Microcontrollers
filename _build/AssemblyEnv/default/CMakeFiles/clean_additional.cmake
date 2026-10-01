# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "")
  file(REMOVE_RECURSE
  "/home/aaa/Projects/MPLABProjects/Microcontrollers/out/AssemblyEnv/default.cmf"
  "/home/aaa/Projects/MPLABProjects/Microcontrollers/out/AssemblyEnv/default.hex"
  "/home/aaa/Projects/MPLABProjects/Microcontrollers/out/AssemblyEnv/default.hxl"
  "/home/aaa/Projects/MPLABProjects/Microcontrollers/out/AssemblyEnv/default.mum"
  "/home/aaa/Projects/MPLABProjects/Microcontrollers/out/AssemblyEnv/default.o"
  "/home/aaa/Projects/MPLABProjects/Microcontrollers/out/AssemblyEnv/default.sdb"
  "/home/aaa/Projects/MPLABProjects/Microcontrollers/out/AssemblyEnv/default.sym"
  )
endif()
