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
    CHAR16 *FileName = L"vmlinuz";
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
        Status = uefi_call_wrapper(BS->OpenProtocol, 3,
                                   LinuxImageHandle,
                                   &gEfiLoadedImageProtocolGuid,
                                   &LoadedImage,
                                   ImageHandle,
                                   NULL,
                                   EFI_OPEN_PROTOCOL_GET_PROTOCOL);

        if (Status != EFI_SUCCESS)
        {
            Print(L"LoadImageInfo - %r\n", Status);
            uefi_call_wrapper(BS->Stall, 1, SEC_TO_USEC(2));

            continue;
        }
        Print(L"Found Image Info \n");

        // LoadedImage->LoadOptionsSize = StrLen((CHAR16 *)LoadedImage->LoadOptions);
        // struct _EFI_LOAD_OPTION *loadOptions = LoadedImage->LoadOptions;
        // Print(L"Old Kernel Options: %r \n", loadOptions->OptionalData[2]);
        // // Print(L"root=/dev/sda2 quiet fastboot");
        // Print(L"New Kernel Options: %r \n", kernelOptions);
        // LoadedImage->LoadOptionsSize = StrLen(kernelOptions);
        // memcpy(&LoadedImage->LoadOptions, &kernelOptions, LoadedImage->LoadOptionsSize);
        // Print(L"Kernel Options Set To: %r \n", LoadedImage->LoadOptions);

        CHAR16 *kernelOptions = L"root=/dev/sda2 loglevel=1 quiet fastboot";
        CHAR16 *BootOptions = L"";

        StrCpy(BootOptions, LoadedImage->LoadOptions);
        StrCat(BootOptions, kernelOptions);
        LoadedImage->LoadOptions = BootOptions;
        LoadedImage->LoadOptionsSize = StrLen(LoadedImage->LoadOptions);
        // memcpy(&loadOptions->OptionalData, &kernelOptions, StrLen(kernelOptions));

        // LoadedImage->LoadOptions[0]= &kernelOptions;
        // EFI_SHELL_PARAMETERS_PROTOCOL *parametersProtocol; /* = {
        //     .Argv =  &kernelOptions,
        //     .Argc = StrLen(kernelOptions)};*/

        // EFI_GUID efiShellParametersGUID = EFI_SHELL_PARAMETERS_PROTOCOL_GUID;

        // Status = uefi_call_wrapper(BS->OpenProtocol, 3,
        //                            LinuxImageHandle,
        //                            &efiShellParametersGUID,
        //                            &parametersProtocol,
        //                            ImageHandle,
        //                            NULL,
        //                            EFI_OPEN_PROTOCOL_GET_PROTOCOL);
        // if (Status != EFI_SUCCESS)
        // {
        //     Print(L"Image Parameters - %r\n", Status);
        //     uefi_call_wrapper(BS->Stall, 1, SEC_TO_USEC(2));

        //     continue;
        // }
        // Print(L"Found Image Parameters");

        uefi_call_wrapper(BS->Stall, 1, SEC_TO_USEC(2));

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

    Print((CHAR16 *)L"Es un pendejo amigos5!\n");
    // uefi_call_wrapper(BS->Stall, 1, SEC_TO_USEC(2));
    uefi_call_wrapper(BS->Stall, 1, SEC_TO_USEC(10));
    return EFI_SUCCESS;
}
