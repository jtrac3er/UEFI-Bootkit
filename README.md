# Bootkit Test

## Goal

This project should help me to understand bootkits on UEFI. The goal is to disable some CPU cores on startup
and reserve some memory in UEFI for the bootkit-code, then use the disabled CPUs to run the bootkit code in 
background, without the OS noticing it. 


## Capabilities

Yet, the code runs on QEMU and also on VirtualBox (both use EDKII UEFI). The program by now contains only tests to disable CPU Cores and allocating memory. This works fine. 

The next steps would be to combine those two and get a core to execute the bootkit code in background. 

The code requires to be run before an OS bootloader. The ultimate goal was to use TianoCore to convert the bootloader code to
a firmware and then flash it on PCs, but since there are protections against such kinds of attacks (like Intel BootGuard) which
prevent this method, this could be more complicated than expected. 



