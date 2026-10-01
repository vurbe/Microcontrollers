# Values that are specific to the target MCU and programming hardware.
set(PIC_MCU "18F46K22")
set(PIC_DFP_FAMILY "PIC18F-K_DFP")

# IPECMD short tool name. PKOB means the PKOB3 debugger/programmer found on
# many Curiosity boards. Change this to PKOB4, PK4, SNAP, etc. when needed.
set(PIC_PROGRAMMER "PKOB" CACHE STRING "IPECMD programmer short name")
