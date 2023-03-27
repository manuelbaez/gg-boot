#include <efi.h>
#include <efilib.h>
#include <string.h>
#include "src/utils/encoding-utils.h"

#define SEC_TO_USEC(value) ((value)*1000 * 1000)


EFI_STATUS EFIAPI efi_main(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable)
{

    InitializeLib(ImageHandle, SystemTable);
    EFI_STATUS Status;

    EFI_LOADED_IMAGE *AppLoadedImage;
    // Print((CHAR16 *)L"Loading App Image Info With OpenProtocol");
    Status =
        uefi_call_wrapper(BS->OpenProtocol, 3,
                          ImageHandle,
                          &gEfiLoadedImageProtocolGuid,
                          &AppLoadedImage,
                          ImageHandle,
                          NULL,
                          EFI_OPEN_PROTOCOL_GET_PROTOCOL);

    // uefi_call_wrapper(BS->Stall, 1, SEC_TO_USEC(2));
    // Status = uefi_call_wrapper(BS->HandleProtocol, 3,
    //                            ImageHandle,
    //                            &gEfiLoadedImageProtocolGuid,
    //                            &AppLoadedImage);

    // // AppLoadedImage->DeviceHandle;

    // uefi_call_wrapper()

    CHAR16 *logo = L"   ______   "
                   "   ______   \n"
                   " /        \\"
                   " /        \\\n"
                   "|           "
                   "|           \n"
                   "|      ___  "
                   "|      ___  \n"
                   "|         | "
                   "|         | \n"
                   " \\_______/ "
                   " \\_______/ \n"
                   " Por pendejos \n"
                   " Para pendejos \n";

    EFI_HANDLE LinuxImageHandle;
    // UINTN NumHandles;
    // UINTN Index;
    // EFI_HANDLE *SFS_Handles;
    EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *RootFileSystemProtocol;
    CHAR16 *KernelFileName = L"vmlinuz";
    EFI_DEVICE_PATH_PROTOCOL *FilePath;
    UINTN ExitDataSize;
    EFI_LOADED_IMAGE *LoadedImage;

    Print((CHAR16 *)logo);

    // Print(L"This isn't Done by iterating fs handles\n");
    Status =
        uefi_call_wrapper(BS->OpenProtocol, 5,
                          AppLoadedImage->DeviceHandle,
                          &gEfiSimpleFileSystemProtocolGuid,
                          (VOID **)&RootFileSystemProtocol,
                          ImageHandle,
                          NULL,
                          EFI_OPEN_PROTOCOL_GET_PROTOCOL);

#ifdef FAILSAFE_BUILD
    if (Status != EFI_SUCCESS)
    {
        Print(L"Protocol is not supported - %r\n", Status);
        uefi_call_wrapper(BS->Stall, 1, SEC_TO_USEC(2));
        return Status;
    }
#endif
    Print(L"Starting boot\n");
    uefi_call_wrapper(BS->Stall, 1, SEC_TO_USEC(1));

    EFI_FILE_PROTOCOL *ConfigFileHandle;
    EFI_FILE_PROTOCOL *root;
    EFI_FILE_INFO *FileInfo;
    uefi_call_wrapper(RootFileSystemProtocol->OpenVolume, 2, RootFileSystemProtocol, &root);
    Status = uefi_call_wrapper(root->Open, 5,
                               root,
                               &ConfigFileHandle,
                               L"\\config",
                               EFI_FILE_MODE_READ,
                               0);
    if (EFI_ERROR(Status))
    {
        Print(L"Failed to open the file. Status: %r\n", Status);
        uefi_call_wrapper(BS->Stall, 1, SEC_TO_USEC(5));
        return Status;
    }
    UINTN BufferSize;
    BufferSize = 0;
    UINTN FileInfoSize;
    Status = uefi_call_wrapper(ConfigFileHandle->GetInfo, 4, ConfigFileHandle, &gEfiFileInfoGuid, &FileInfoSize, NULL);
    if (Status != EFI_BUFFER_TOO_SMALL)
    {
        Print(L"Failed to get the file size. Status: %r\n", Status);
        return Status;
    }
    FileInfo = AllocatePool(FileInfoSize);
    if (FileInfo == NULL)
    {
        Print(L"Failed to allocate memory for the file info.\n");
        uefi_call_wrapper(BS->Stall, 1, SEC_TO_USEC(5));
        return EFI_OUT_OF_RESOURCES;
    }
    Status = uefi_call_wrapper(ConfigFileHandle->GetInfo, 4, ConfigFileHandle, &gEfiFileInfoGuid, &FileInfoSize, FileInfo);
    if (EFI_ERROR(Status))
    {
        Print(L"Failed to get the file info. Status: %r\n", Status);
        uefi_call_wrapper(BS->Stall, 1, SEC_TO_USEC(5));
        return Status;
    }
    BufferSize = FileInfo->FileSize;
    Print(L"Buffer size: %d\n", BufferSize);
    VOID *Buffer;
    Buffer = AllocatePool(BufferSize);
    if (Buffer == NULL)
    {
        Print(L"Failed to allocate memory for the file contents.\n");
        uefi_call_wrapper(BS->Stall, 1, SEC_TO_USEC(5));
        return EFI_OUT_OF_RESOURCES;
    }
    Status = uefi_call_wrapper(ConfigFileHandle->Read, 3, ConfigFileHandle, &BufferSize, Buffer);
    Print(L"Read file status: %r\n", Status);

    if (EFI_ERROR(Status))
    {
        Print(L"Failed to read the file contents. Status: %r\n", Status);
        uefi_call_wrapper(BS->Stall, 1, SEC_TO_USEC(5));
        return Status;
    }
    CHAR16 *KernelOptions = AllocatePool(BufferSize*2 + sizeof(CHAR16));
    ConvertChar8ToChar16((CHAR8 *)Buffer, KernelOptions, BufferSize*2 + sizeof(CHAR16));

    Print(L"Kernel Params:\n");
    Print(KernelOptions);

    FreePool(Buffer);
    ConfigFileHandle->Close(ConfigFileHandle);
    // Load the OS Image
    Print(L"Load Kernel Image\n");

    FilePath = FileDevicePath(AppLoadedImage->DeviceHandle, KernelFileName);
    Status =
        uefi_call_wrapper(BS->LoadImage,
                          6,
                          FALSE,
                          ImageHandle,
                          FilePath,
                          (VOID *)NULL,
                          0,
                          &LinuxImageHandle);
#ifdef FAILSAFE_BUILD
    if (Status != EFI_SUCCESS)
    {
        Print(L"Could not load the image - %r\n", Status);
        uefi_call_wrapper(BS->Stall, 1, SEC_TO_USEC(2));
    }
#endif
    Print(L"Loaded the image with success\n");
    // Print(L"Image start:\n");
    Status =
        uefi_call_wrapper(BS->OpenProtocol, 3,
                          LinuxImageHandle,
                          &gEfiLoadedImageProtocolGuid,
                          &LoadedImage,
                          ImageHandle,
                          NULL,
                          EFI_OPEN_PROTOCOL_GET_PROTOCOL);
#ifdef FAILSAFE_BUILD
    if (Status != EFI_SUCCESS)
    {
        Print(L"LoadImageInfo - %r\n", Status);
        uefi_call_wrapper(BS->Stall, 1, SEC_TO_USEC(2));
    }
#endif

    Print(L"Found Image Info \n");

    CHAR16 *BootOptions = AllocatePool((StrLen(LoadedImage->LoadOptions) + StrLen(KernelOptions)) * 2 + 1);

    StrCpy(BootOptions, LoadedImage->LoadOptions);
    StrCat(BootOptions, KernelOptions);
    LoadedImage->LoadOptions = BootOptions;
    LoadedImage->LoadOptionsSize = StrLen(LoadedImage->LoadOptions) * 2 + 1;

    // uefi_call_wrapper(BS->Stall, 1, SEC_TO_USEC(2));
    Print(L"Assigned boot options \n");
#ifdef FAILSAFE_BUILD
    if (Status != EFI_SUCCESS)
    {
        Print(L"Could not load the image params - %r %x\n", Status, Status);
        Print(L"Exit data size: %d\n", ExitDataSize);
        uefi_call_wrapper(BS->Stall, 1, SEC_TO_USEC(2));
    }
#endif

    // Print(L"Options are: \n");
    // uefi_call_wrapper(ST->ConOut->OutputString, 2, ST->ConOut, LoadedImage->LoadOptions);
    // Print(L"\n");
    Print(L"\nKernel will start in 5 seconds \n");
    // uefi_call_wrapper(BS->Stall, 1, SEC_TO_USEC(5));
    Status =
        uefi_call_wrapper(BS->CloseProtocol, 4,
                          AppLoadedImage->DeviceHandle,
                          &gEfiSimpleFileSystemProtocolGuid,
                          ImageHandle,
                          NULL);

#ifdef FAILSAFE_BUILD
    if (Status != EFI_SUCCESS)
    {
        Print(L"Could not close the bootfs - %r \n", Status);
        Print(L"Exit data size: %d\n", ExitDataSize);
        uefi_call_wrapper(BS->Stall, 1, SEC_TO_USEC(5));
        return Status;
    }
#endif

    Status =
        uefi_call_wrapper(BS->ExitBootServices, 2, ImageHandle, 0);

    // uefi_call_wrapper(BS->UninstallMultipleProtocolInterfaces, 4, AppLoadedImage->DeviceHandle, &gEfiBlockIoProtocolGuid, &BlockIo, NULL);
    Status =
        uefi_call_wrapper(BS->StartImage, 3, LinuxImageHandle, &ExitDataSize, (CHAR16 **)NULL);
#ifdef FAILSAFE_BUILD
    if (Status != EFI_SUCCESS)
    {
        Print(L"Could not start the image - %r %x\n", Status, Status);
        Print(L"Exit data size: %d\n", ExitDataSize);
        uefi_call_wrapper(BS->Stall, 1, SEC_TO_USEC(2));
    }
#endif
    return EFI_SUCCESS;
}
