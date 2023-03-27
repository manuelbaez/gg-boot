ARCH			= $(shell uname -m | sed s,i[3456789]86,ia32,)
SRCDIR 			:= src
OBJDIR 			:= obj
INCLUDE_DIR		:= include
SOURCE_DIRS		:= modules/config \
				   utils

OBJ_DIRS		:= $(patsubst %,$(OBJDIR)/%,$(SOURCE_DIRS))

_OBJS			:= main.o \
				  utils/encoding-utils.o \
				  modules/config/config-loader.o

OBJS = $(patsubst %,$(OBJDIR)/%,$(_OBJS))

# INCLUDE_FILES	= encoding-utils.h \
# 				  config-loader.h

# DEPS			= $(patsubst %,$(INCLUDE_DIR)/%,$(INCLUDE_FILES))

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

# all: clean $(TARGET)
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

create_build_dir:
	rm -rfv obj
	mkdir -pv $(OBJ_DIRS)

install:
	cp -v $(TARGET) /boot/EFI/Boot/
