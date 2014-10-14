#!/bin/bash

DIR=$(cd -P -- "$(dirname -- "${BASH_SOURCE:-$0}")" && printf '%s\n' "$(pwd -P)")

# Prefix of the toolchain's binaries
toolchain_prefix="msp430-elf-"

# Run gdb, connect to mspdebug, and launch some GDB commands
${toolchain_prefix}gdb -x $DIR/gdb-cmds.txt
