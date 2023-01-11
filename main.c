#include <efi.h>
#include <efilib.h>
#include <string.h>
// // #include <Library/UefiBootServicesTableLib.h>
// // #include <Library/DevicePathLib.h>
// // #include <Protocol/DevicePath.h>
// // #include <Protocol/PciIo.h>

#define SEC_TO_USEC(value) ((value)*1000 * 1000)
CHAR16 *FileName = (CHAR16 *)L"vmlinuz";

EFI_STATUS EFIAPI efi_main(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable)
{

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

    InitializeLib(ImageHandle, SystemTable);
    EFI_HANDLE LinuxImageHandle;
    EFI_STATUS Status;
    UINTN NumHandles;
    UINTN Index;
    EFI_HANDLE *SFS_Handles;
    EFI_BLOCK_IO_PROTOCOL *BlkIo;
    CONST CHAR16 *FileName = L"vmlinuz";
    EFI_DEVICE_PATH_PROTOCOL *FilePath;
    UINTN ExitDataSize;
    EFI_LOADED_IMAGE *LoadedImage;

    Print((CHAR16 *)logo);
    uefi_call_wrapper(BS->Stall, 1, SEC_TO_USEC(2));

    Status = uefi_call_wrapper(BS->LocateHandleBuffer,
                               5,
                               ByProtocol,
                               &gEfiSimpleFileSystemProtocolGuid,
                               NULL,
                               &NumHandles,
                               &SFS_Handles);

    if (Status != EFI_SUCCESS)
    {
        Print(L"Could not find handles - %r\n", Status);
        uefi_call_wrapper(BS->Stall, 1, SEC_TO_USEC(2));

        return Status;
    }

    for (Index = 0; Index < NumHandles; Index++)
    {
        Print((CHAR16 *)L"Iterating %d\n", Index);
        Status = uefi_call_wrapper(BS->OpenProtocol, 5,
                                   SFS_Handles[Index],
                                   &gEfiSimpleFileSystemProtocolGuid,
                                   (VOID **)&BlkIo,
                                   ImageHandle,
                                   NULL,
                                   EFI_OPEN_PROTOCOL_GET_PROTOCOL);

        if (Status != EFI_SUCCESS)
        {
            Print(L"Protocol is not supported - %r\n", Status);
            uefi_call_wrapper(BS->Stall, 1, SEC_TO_USEC(2));
            return Status;
        }
        FilePath = FileDevicePath(SFS_Handles[Index], FileName);
        Status = uefi_call_wrapper(BS->LoadImage,
                                   6,
                                   FALSE,
                                   ImageHandle,
                                   FilePath,
                                   (VOID *)NULL,
                                   0,
                                   &LinuxImageHandle);
        if (Status != EFI_SUCCESS)
        {
            Print(L"Could not load the image - %r\n", Status);
            uefi_call_wrapper(BS->Stall, 1, SEC_TO_USEC(2));

            continue;
        }
        Print(L"Loaded the image with success\n");
        Print(L"Image start:\n");
        Status = uefi_call_wrapper(BS->HandleProtocol, 3,
                                   LinuxImageHandle,
                                   &gEfiLoadedImageProtocolGuid,
                                   &LoadedImage);

        Print(L"Old Kernel Options: %r \n", LoadedImage->LoadOptions);
        CHAR16 *kernelOptions = L"root=/dev/sda2 quiet fastboot";
        Print(L"New Kernel Options: %r \n", kernelOptions);
        LoadedImage->LoadOptionsSize = StrLen(kernelOptions);
        memcpy(LoadedImage->LoadOptions, kernelOptions, LoadedImage->LoadOptionsSize);
        Print(L"Kernel Options Set To: %r \n", LoadedImage->LoadOptions);

        uefi_call_wrapper(BS->Stall, 1, SEC_TO_USEC(10));

        if (Status != EFI_SUCCESS)
        {
            Print(L"Could not load the image params - %r %x\n", Status, Status);
            Print(L"Exit data size: %d\n", ExitDataSize);
            uefi_call_wrapper(BS->Stall, 1, SEC_TO_USEC(2));

            continue;
        }

        Status = uefi_call_wrapper(BS->StartImage, 3, LinuxImageHandle, &ExitDataSize, (CHAR16 **)L"quiet");
        if (Status != EFI_SUCCESS)
        {
            Print(L"Could not start the image - %r %x\n", Status, Status);
            Print(L"Exit data size: %d\n", ExitDataSize);
            uefi_call_wrapper(BS->Stall, 1, SEC_TO_USEC(2));

            continue;
        }
    }
    // gBS->StartImage
    // Status = uefi_call_wrapper(BS->LoadImage, 6, FALSE, &ImageHandle, L"fs0:\\vmlinuz", NULL, NULL, &LinuxImageHandle);
    // // ExitBootServices();
    // if (Status != EFI_SUCCESS)
    // {
    //     Print(L"Could not load the image - %r\n", Status);
    //     uefi_call_wrapper(BS->Stall, 1, SEC_TO_USEC(2));
    //     return Status;
    // }
    // // Status = uefi_call_wrapper(BS->ExitBootServices, 1, L"fs0:\\vmlinuz");

    Print((CHAR16 *)L"Es un pendejo amigos5!\n");
    // uefi_call_wrapper(BS->Stall, 1, SEC_TO_USEC(2));
    uefi_call_wrapper(BS->Stall, 1, SEC_TO_USEC(10));
    return EFI_SUCCESS;
}

// #include <efi.h>
// #include <efilib.h>

// EFI_STATUS EFIAPI efi_main(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable)
// {
//     InitializeLib(ImageHandle, SystemTable);
//     Print((CHAR16 *)L"Es un pendejo amigos5!\n");
//     uefi_call_wrapper(BS->Stall, 1, SEC_TO_USEC(2));
//     UINTN NumHandles;
//     UINTN Index;
//     EFI_HANDLE *SFS_Handles;
//     EFI_HANDLE AppImageHandle = NULL;
//     EFI_STATUS Status = EFI_SUCCESS;
//     EFI_BLOCK_IO_PROTOCOL *BlkIo;
//     CONST CHAR16 *FileName = L"vmlinuz";
//     EFI_DEVICE_PATH_PROTOCOL *FilePath;
//     EFI_LOADED_IMAGE_PROTOCOL *ImageInfo;
//     UINTN ExitDataSize;

//     Status = gBS->LocateHandleBuffer(
//         ByProtocol,
//         &gEfiSimpleFileSystemProtocolGuid,
//         NULL,
//         &NumHandles,
//         &SFS_Handles);

//     if (Status != EFI_SUCCESS)
//     {
//         Print(L"Could not find handles - %r\n", Status);
//         uefi_call_wrapper(BS->Stall, 1, SEC_TO_USEC(2));

//         return Status;
//     }

//     for (Index = 0; Index < NumHandles; Index++)
//     {
//         Status = gBS->OpenProtocol(
//             SFS_Handles[Index],
//             &gEfiSimpleFileSystemProtocolGuid,
//             (VOID **)&BlkIo,
//             ImageHandle,
//             NULL,
//             EFI_OPEN_PROTOCOL_GET_PROTOCOL);

//         if (Status != EFI_SUCCESS)
//         {
//             Print(L"Protocol is not supported - %r\n", Status);
//             uefi_call_wrapper(BS->Stall, 1, SEC_TO_USEC(2));

//             return Status;
//         }

//         FilePath = FileDevicePath(SFS_Handles[Index], FileName);
//         Status = gBS->LoadImage(
//             FALSE,
//             ImageHandle,
//             FilePath,
//             (VOID *)NULL,
//             0,
//             &AppImageHandle);

//         if (Status != EFI_SUCCESS)
//         {
//             Print(L"Could not load the image - %r\n", Status);
//             uefi_call_wrapper(BS->Stall, 1, SEC_TO_USEC(2));

//             continue;
//         }

//         Print(L"Loaded the image with success\n");
//         Status = gBS->OpenProtocol(
//             AppImageHandle,
//             &gEfiLoadedImageProtocolGuid,
//             (VOID **)&ImageInfo,
//             ImageHandle,
//             (VOID *)NULL,
//             EFI_OPEN_PROTOCOL_GET_PROTOCOL);

//         Print(L"ImageInfo opened\n");

//         if (!EFI_ERROR(Status))
//         {
//             Print(L"ImageSize = %d\n", ImageInfo->ImageSize);
//             uefi_call_wrapper(BS->Stall, 1, SEC_TO_USEC(2));
//         }

//         Print(L"Image start:\n");
//         Status = gBS->StartImage(AppImageHandle, &ExitDataSize, (CHAR16 **)NULL);
//         if (Status != EFI_SUCCESS)
//         {
//             Print(L"Could not start the image - %r %x\n", Status, Status);
//             Print(L"Exit data size: %d\n", ExitDataSize);
//             uefi_call_wrapper(BS->Stall, 1, SEC_TO_USEC(2));

//             continue;
//         }
//     }

//     return Status;
// }
