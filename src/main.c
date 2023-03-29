#include <efi.h>
#include <efilib.h>
#include <string.h>
#include "encoding-utils.h"
#include "config-loader.h"
#include "constants.h"
#include "generic-utils.h"
#include "boot.h"

EFI_STATUS EFIAPI efi_main(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable)
{

    InitializeLib(ImageHandle, SystemTable);
    EFI_STATUS Status;

    Print(L"Load app info\n");
    EFI_LOADED_IMAGE *AppLoadedImage = NULL;
    LoadAppInfo(ImageHandle, &AppLoadedImage);

    Print(L"Open rootfs\n");
    EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *RootFileSystemProtocol;
    OpenRootFS(ImageHandle, AppLoadedImage, &RootFileSystemProtocol);

    Print(L"Get Kernel Params\n");
    CHAR16 *KernelOptions = NULL;
    GetKernelParams(RootFileSystemProtocol, &KernelOptions);

    Print(L"Start Kernel \n");
    StartKernelImage(ImageHandle, AppLoadedImage, KernelOptions);

    return EFI_SUCCESS;
}
