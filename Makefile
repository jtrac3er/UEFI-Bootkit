
OUT_FILE = main.efi

EFI_DIR = ./gnu-efi
MOUNTPOINT = ./hdd-mount

# verwendeter Compiler
CC=gcc

# Source datei namen in /src
# bootkit wird noch weggelassen
SOURCES = main.c memory.c proc.c helper.c pci.c

SRC_FILES = $(addprefix src/, $(SOURCES))
OBJ_FILES = $(addprefix obj/, $(SOURCES:.c=.o))
HDR_FILES = $(addprefix inc/, $(SOURCES:.c=.h))
#OBJ_FILES := $(addprefix obj/, $(OBJ_FILES:.cpp=.o))

CUSTOM_SECTION_NAME = .bootkit

all: $(OUT_FILE)
	make mount-hdd
	make copy
	make umount-hdd


create-hdd:
	dd if=/dev/zero of=virtualdisk.img bs=1M count=100
	mkfs.fat -F 32 virtualdisk.img

mount-hdd:
	mkdir -p ./hdd-mount
	sudo mount -o loop,rw virtualdisk.img $(MOUNTPOINT)

umount-hdd:
	sudo umount ./hdd-mount
	rm -r $(MOUNTPOINT)

copy:
	sudo cp $(OUT_FILE) $(MOUNTPOINT)/$(OUT_FILE)


obj/:
	mkdir -p obj

obj/%.o: src/%.c inc/%.h | obj/
	$(CC) -I$(EFI_DIR)/inc -fpic -ffreestanding -fno-stack-protector -fno-stack-check \
	  -fshort-wchar -mno-red-zone -maccumulate-outgoing-args -c $< -o $@ -g \
	  -I edk2/MdePkg/Include -I inc


$(OUT_FILE): $(OBJ_FILES)
	ld -shared -Bsymbolic -L$(EFI_DIR)/x86_64/lib -L$(EFI_DIR)/x86_64/gnuefi \
	 -T$(EFI_DIR)/gnuefi/elf_x86_64_efi.lds $(EFI_DIR)/x86_64/gnuefi/crt0-efi-x86_64.o \
	 $(OBJ_FILES) -o obj/$(OUT_FILE).so -lgnuefi -lefi

	objcopy -j .text -j .sdata -j .data -j .rodata -j .dynamic -j .dynsym  -j .rel \
	-j .rela -j .rel.* -j .rela.* -j .reloc --target efi-app-x86_64 --subsystem=10 \
	 obj/$(OUT_FILE).so $(OUT_FILE) -j$(CUSTOM_SECTION_NAME)


clean:
	rm -r obj
	rm $(OUT_FILE)
