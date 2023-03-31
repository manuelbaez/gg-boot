#!make
include .config
export $(shell sed 's/=.*//' .config)

ARCH			:= $(shell uname -m | sed s,i[3456789]86,ia32,)
SRCDIR 			:= src
OBJDIR 			:= obj
INCLUDE_DIR		:= include
SOURCE_DIRS		:= modules/config \
				   modules/boot \
				   utils

OBJ_DIRS		:= $(patsubst %,$(OBJDIR)/%,$(SOURCE_DIRS))

_OBJS			:= main.o \
				  utils/encoding-utils.o \
				  utils/app-tools.o \
				  modules/config/config-loader.o \
				  modules/boot/kernel-loader.o \
				  modules/boot/boot-entries.o

OBJS = $(patsubst %,$(OBJDIR)/%,$(_OBJS))

TARGET			= BootX64.efi

EFIINC			= /usr/include/efi
EFILIB			= /usr/lib
EFI_CRT_OBJS	= $(EFILIB)/crt0-efi-$(ARCH).o
EFI_LDS			= $(EFILIB)/elf_$(ARCH)_efi.lds


CFLAGS			= -nostdlib \
				  -fno-stack-protector \
				  -fno-strict-aliasing \
				  -fno-builtin \
				  -fpic \
				  -fshort-wchar \
				  -mno-red-zone \
				  -Wall 

ifeq ($(ARCH),x86_64)
  CFLAGS		+= -DEFI_FUNCTION_WRAPPER
endif

CFLAGS			+= -I$(EFIINC) \
				   -I$(EFIINC)/$(ARCH) \
				   -I$(EFIINC)/protocol \
				   -I$(INCLUDE_DIR) 

ifeq ($(NO_ERROR_CHECK),1)
	CFLAGS += -D NO_ERROR_CHECK
endif

ifeq ($(NO_BOOT_ENTRIES),1)
	CFLAGS += -D NO_BOOT_ENTRIES
endif

ifeq ($(SILENT),1)
	CFLAGS += -D SILENT
endif

ifdef KERNEL_IMAGE_PATH
	CFLAGS += -D KERNEL_IMAGE_PATH="L\"$(KERNEL_IMAGE_PATH)\""
endif


LDFLAGS			= -nostdlib \
				  -znocombreloc \
				  -shared \
				  -no-undefined \
				  -Bsymbolic

LDFLAGS			+= -T $(EFI_LDS) \
				   -L$(EFILIB) \
				   $(EFI_CRT_OBJS)

LIBS			= -lefi \
				  -lgnuefi

OBJCOPYFLAGS	= -j .text \
				  -j .sdata \
				  -j .data \
				  -j .dynamic \
				  -j .dynsym \
				  -j .rel \
				  -j .rela \
				  -j .reloc \
				  --target=efi-app-$(ARCH)

all: create_build_dir BootX64.efi

$(OBJDIR)/%.o : $(SRCDIR)/%.c 
	$(CC) -c -o $@ $< $(CFLAGS)

$(OBJDIR)/BootX64.so:   $(OBJS)
	ld  $(LDFLAGS)  $(OBJS) -o  $@  $(LIBS) 
	

%.efi:  $(OBJDIR)/%.so
	objcopy $(OBJCOPYFLAGS) $^ $@

.PHONY:    clean

clean:
	rm -rf $(OBJDIR) $(TARGET) 

create_build_dir: clean
	mkdir -pv $(OBJ_DIRS)

install:
	mkdir -pv $(BOOT_DIR)/EFI/Boot/
	cp -v $(TARGET) $(BOOT_DIR)/EFI/Boot/
	echo $(KERNEL_PARAMS)> $(BOOT_DIR)/config
