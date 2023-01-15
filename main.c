#include <efi.h>
#include <efilib.h>
#include <string.h>

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

    // CHAR16 *logo = L"   ______   "
    //                "   ______   \n"
    //                " /        \\"
    //                " /        \\\n"
    //                "|           "
    //                "|           \n"
    //                "|      ___  "
    //                "|      ___  \n"
    //                "|         | "
    //                "|         | \n"
    //                " \\_______/ "
    //                " \\_______/ \n"
    //                " Por pendejos \n"
    //                " Para pendejos \n";

    EFI_HANDLE LinuxImageHandle;
    // UINTN NumHandles;
    // UINTN Index;
    // EFI_HANDLE *SFS_Handles;
    EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *RootFileSystemProtocol;
    CHAR16 *KernelFileName = L"vmlinuz";
    EFI_DEVICE_PATH_PROTOCOL *FilePath;
    UINTN ExitDataSize;
    EFI_LOADED_IMAGE *LoadedImage;

    // Print((CHAR16 *)logo);

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
    // Read the configuration file

    // CHAR16 *kernelOptions = L"root=PARTUUID=0600a512-de71-6c4c-ad7c-d3326881bce6 rw quiet fastboot";
    CHAR16 *kernelOptions = L"root=/dev/sda2 rw quiet fastboot";
    // Print(L"Loading Config File\n");

    // EFI_FILE_PROTOCOL *file;
    // UINTN buffer_size = 0;
    // CHAR16 *buffer = L"";
    // EFI_FILE_PROTOCOL *root;
    // EFI_FILE_INFO *fileInfo;
    // uefi_call_wrapper(RootFileSystemProtocol->OpenVolume, 2, RootFileSystemProtocol, &root);
    // // Open the file
    // uefi_call_wrapper(root->Open, 5,
    //                   root,
    //                   &file,
    //                   L"config",
    //                   EFI_FILE_MODE_READ,
    //                   EFI_FILE_READ_ONLY | EFI_FILE_HIDDEN | EFI_FILE_SYSTEM);

    // // Get the file size
    // uefi_call_wrapper(file->GetInfo, 4, file, &gEfiFileInfoGuid, &buffer_size, NULL);
    // fileInfo = AllocatePool(buffer_size);

    // uefi_call_wrapper(file->GetInfo, 4, file, &gEfiFileInfoGuid, &buffer_size, fileInfo);
    // buffer = AllocatePool(fileInfo->FileSize);

    // Print(L"Setting position to 0 \n");

    // UINTN position = 0;
    // // Read the file
    // uefi_call_wrapper(file->SetPosition, 2, file, position);
    // Print(L"Position Set To 0 \n");

    // Status = uefi_call_wrapper(file->Read, 3, file, &fileInfo->FileSize, buffer);

    // if (Status != EFI_SUCCESS)
    // {
    //     Print(L"Could not read file %r \n", fileInfo->FileSize);
    // }

    // Print(L"File size is:  %r \n", buffer_size);
    // Print(L"Config is: \n");
    // Print(L"Config is:  %r \n", buffer);
    // uefi_call_wrapper(ST->ConOut->OutputString, 2, ST->ConOut, kernelOptions);
    // Print(L"\n");
    // uefi_call_wrapper(ST->ConOut->OutputString, 2, ST->ConOut, buffer);
    // Print(L"\n");
    // // kernelOptions = buffer;
    // // Close the file
    // uefi_call_wrapper(file->Close, 1, file);
    // uefi_call_wrapper(BS->Stall, 1, SEC_TO_USEC(4));

    // Load the OS Image

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
    // Print(L"Loaded the image with success\n");
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

    // Print(L"Found Image Info \n");

    CHAR16 *BootOptions = AllocatePool((StrLen(LoadedImage->LoadOptions) + StrLen(kernelOptions)) * 2 + 1);

    StrCpy(BootOptions, LoadedImage->LoadOptions);
    StrCat(BootOptions, kernelOptions);
    LoadedImage->LoadOptions = BootOptions;
    LoadedImage->LoadOptionsSize = StrLen(LoadedImage->LoadOptions) * 2 + 1;

    // uefi_call_wrapper(BS->Stall, 1, SEC_TO_USEC(2));
    // Print(L"Assigned boot options \n");
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
    // Print(L"\nKernel will start in 5 seconds \n");
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

    // Print((CHAR16 *)L"Es un pendejo amigos5!\n");
    return EFI_SUCCESS;
}
