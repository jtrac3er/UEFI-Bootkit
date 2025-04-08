#!/bin/bash

# nographic, weil ich in WSL bin

# net none, weil sonst UEFI versucht, in PXE modus von einem netzwerk zu booten. 
# 	erst wenn alles scheitert, dann startet die UEFI console wie gewollt
#   viellleicht auch durch escape erreichbar, keine Ahnung

# -s für debugging, wenn man -d angibt für das skript
# smp <proc>, sodass <proc> cores verwendet werden. Es gibt aber sowieso nur einen einzigen
# bootstrap prozessor, die anderen sind alle in einem Schlafzustand, können aber per Interrupt
# aufgeweckt werden

if [[ "$1" == "-debug" ]]; then
    DEBUG_CMD=" -S -gdb tcp::2345"
fi


qemu-system-x86_64 \
	-drive format=raw,file=virtualdisk.img \
	-nographic \
	-bios /usr/share/ovmf/OVMF.fd \
	-net none \
	-smp 4 \
	$DEBUG_CMD

