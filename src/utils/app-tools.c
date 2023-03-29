#include "generic-utils.h"

EFI_STATUS LoadAppInfo(EFI_HANDLE ImageHandle, OUT EFI_LOADED_IMAGE **AppLoadedImage)
{
	return uefi_call_wrapper(BS->OpenProtocol, 3,
					  ImageHandle,
					  &gEfiLoadedImageProtocolGuid,
					  &*AppLoadedImage,
					  ImageHandle,
					  NULL,
					  EFI_OPEN_PROTOCOL_GET_PROTOCOL);
}

EFI_STATUS OpenRootFS(EFI_HANDLE ImageHandle, EFI_LOADED_IMAGE *AppLoadedImage, OUT EFI_SIMPLE_FILE_SYSTEM_PROTOCOL **RootFileSystemProtocol)
{
	return uefi_call_wrapper(BS->OpenProtocol, 5,
							 AppLoadedImage->DeviceHandle,
							 &gEfiSimpleFileSystemProtocolGuid,
							 (VOID **)&*RootFileSystemProtocol,
							 ImageHandle,
							 NULL,
							 EFI_OPEN_PROTOCOL_GET_PROTOCOL);
}
