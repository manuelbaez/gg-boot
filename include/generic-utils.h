#include <efi.h>
#include <efilib.h>
#include <string.h>

EFI_STATUS LoadAppInfo(EFI_HANDLE ImageHandle,  OUT EFI_LOADED_IMAGE **AppLoadedImage);
EFI_STATUS OpenRootFS(EFI_HANDLE ImageHandle,EFI_LOADED_IMAGE *AppLoadedImage ,OUT EFI_SIMPLE_FILE_SYSTEM_PROTOCOL **RootFileSystemProtocol);
