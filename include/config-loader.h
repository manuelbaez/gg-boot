#include <efi.h>
#include <efilib.h>
#include <string.h>
#include "encoding-utils.h"

#ifndef KERNEL_CONFIG_FILE_PATH
#define KERNEL_CONFIG_FILE_PATH L"\\config"
#endif

EFI_STATUS GetKernelParams(EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *RootFileSystemProtocol, OUT CHAR16 **KernelOptions);