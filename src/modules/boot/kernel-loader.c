#include "boot.h"
#include "constants.h"

EFI_STATUS StartKernelImage(EFI_HANDLE ImageHandle, EFI_LOADED_IMAGE *AppLoadedImage, CHAR16 *KernelOptions)
{
	EFI_STATUS Status;
	EFI_HANDLE LinuxImageHandle;
	CHAR16 *KernelFileName = L"vmlinuz";
	EFI_DEVICE_PATH_PROTOCOL *FilePath;
	UINTN ExitDataSize;
	EFI_LOADED_IMAGE *LoadedImage;

	Print(L"Starting boot\n");

	// Load the OS Image
	Print(L"Load Kernel Image\n");

	FilePath = FileDevicePath(AppLoadedImage->DeviceHandle, KernelFileName);
	Status = SetBootEntries(FilePath, L"Linux", KernelOptions);
	if (Status != EFI_SUCCESS)
	{
		Print(L"Could not Add boot Entry - %r\n", Status);
	}
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
	Print(LoadedImage->LoadOptions);
	Print(L"\nAssigned boot options \n");
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
	Print(L"\nKernel will start now \n");
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
	return Status;
}