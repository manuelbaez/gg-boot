#include <efi.h>
#include <efilib.h>
#include <string.h>

#ifndef _BOOT_H
#define _BOOT_H

#define LOAD_OPTION_HIDDEN 0x00000004
typedef struct
{
	UINT32 Attributes;
	UINT16 FilePathListLength;
	CHAR16 *Description;
	EFI_DEVICE_PATH_PROTOCOL *FilePathList;
	VOID *OptionalData;
	UINT32 OptionalDataSize;
} EFI_BOOT_ENTRY;

typedef struct
{
	UINT32 Attributes;
	UINT16 FilePathListLength;
	CHAR16 *Description;
	EFI_DEVICE_PATH_PROTOCOL *FilePathList;
	VOID *OptionalData;
	UINT32 OptionalDataSize;
} EFI_LOAD_OPTION;

EFI_STATUS StartKernelImage(EFI_HANDLE ImageHandle, EFI_LOADED_IMAGE *AppLoadedImage, CHAR16 *KernelOptions);
EFI_STATUS SetBootEntries(EFI_DEVICE_PATH_PROTOCOL *devicePath, CHAR16 *description, CHAR16 *kernelParams);
#endif