# PIC18F46K22 XC8 + CMake workflow

This template uses CMake as an explicit firmware task runner:

1. `xc8-cc` compiles, assembles, links, and emits `.elf` + `.hex`.
2. The `firmware` target represents the resulting HEX image.
3. The `program` target depends on `firmware`, then calls MPLAB IPE's
   `ipecmd.sh` to program and verify the PIC through PKOB.

It intentionally does not register XC8 as a normal CMake C compiler. That
avoids fake compiler IDs, forced compiler tests, and incomplete `.p1` rules.

## Folder responsibilities

- `cmake/XC8Firmware.cmake`: shared compiler/tool discovery and build/program
  logic. Edit rarely.
- `cmake/devices/PIC18F46K22.cmake`: MCU, device-pack family, and programmer.
- `projects/Lab3/CMakeLists.txt`: only Lab3's sources, headers, include paths,
  and compiler options.
- `projects/Lab3/src` and `include`: the actual firmware.

Do not use `picpio.ini` in this flow. It belongs to a separate tool and the old
file selected PIC18F26K22, not PIC18F46K22.

## 1. Verify installed tools

Run:

```bash
command -v cmake
find /opt/microchip/xc8 -type f -name xc8-cc -print
find /opt/microchip/mplabx -type f -name ipecmd.sh -print
find "$HOME/.mchp_packs/Microchip/PIC18F-K_DFP" \
  -mindepth 2 -maxdepth 2 -type d -name xc8 -print
```

The template auto-selects the newest matching XC8, MPLAB X/IPE, and device
pack paths. You can override any choice during configuration.

## 2. Configure once

From the root of this template:

```bash
cmake -S . -B build/Lab3 -G Ninja -DFIRMWARE_PROJECT=Lab3
```

If auto-detection fails, give the three paths explicitly:

```bash
cmake -S . -B build/Lab3 -G Ninja \
  -DFIRMWARE_PROJECT=Lab3 \
  -DXC8_CC=/opt/microchip/xc8/v3.10/bin/xc8-cc \
  -DPIC_DFP_XC8="$HOME/.mchp_packs/Microchip/PIC18F-K_DFP/1.17.312/xc8" \
  -DIPECMD=/opt/microchip/mplabx/v6.25/mplab_platform/mplab_ipe/ipecmd.sh
```

Replace `v6.25` with the version actually printed by the earlier `find`.

## 3. Build without touching the PIC

```bash
cmake --build build/Lab3 --target firmware --verbose
```

Expected outputs:

```text
build/Lab3/firmware/Lab3.elf
build/Lab3/firmware/Lab3.hex
```

## 4. Connect and program

Connect the Curiosity board, then run:

```bash
cmake --build build/Lab3 --target program --verbose
```

That one command rebuilds only when inputs changed, programs all memory,
verifies the device, and releases it from reset.

If the board contains PKOB4 instead of PKOB3, reconfigure once:

```bash
cmake -S . -B build/Lab3 -DPIC_PROGRAMMER=PKOB4
```

Then rerun the `program` command.

## Daily workflow

After the initial configure step, edit source code and use only:

```bash
cmake --build build/Lab3 --target program
```

Useful maintenance commands:

```bash
cmake --build build/Lab3 --target firmware --verbose
cmake --build build/Lab3 --target clean
cmake -S . -B build/Lab3 -DFIRMWARE_PROJECT=Lab3
```

Delete `build/Lab3` only when changing generators or recovering from a stale
CMake cache. Normal source edits do not require reconfiguration.

## Add another project

Copy `projects/Lab3` to a new folder, change its source list and output `NAME`,
then configure it into a separate build directory:

```bash
cmake -S . -B build/NewProject -G Ninja -DFIRMWARE_PROJECT=NewProject
cmake --build build/NewProject --target program
```

Keeping one build directory per firmware project prevents one project's cached
MCU, programmer, or paths from leaking into another project.
