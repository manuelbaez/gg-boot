#include "config-loader.h"
#include "constants.h"

EFI_STATUS GetKernelParams(EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *RootFileSystemProtocol, OUT CHAR16 **KernelOptions)
{
	EFI_FILE_PROTOCOL *ConfigFileHandle;
	EFI_FILE_PROTOCOL *root;
	EFI_FILE_INFO *FileInfo = AllocatePool(1024);
	EFI_STATUS Status = EFI_SUCCESS;
#ifndef SILENT
	Print(L"Open Boot Volume \n");
#endif
	uefi_call_wrapper(RootFileSystemProtocol->OpenVolume, 2, RootFileSystemProtocol, &root);
	// Print(L"Open config file \n");
	Status = uefi_call_wrapper(root->Open, 5,
							   root,
							   &ConfigFileHandle,
							   L"\\config",
							   EFI_FILE_MODE_READ,
							   0);
#ifndef NO_ERROR_CHECK
	if (EFI_ERROR(Status))
	{
		Print(L"Failed to open the file. Status: %r\n", Status);
		uefi_call_wrapper(BS->Stall, 1, SEC_TO_USEC(5));
		return Status;
	}
#endif
	UINTN BufferSize;
	BufferSize = 0;
	UINTN FileInfoSize;
#ifndef SILENT
	Print(L"Get config file info \n");
#endif
	Status = uefi_call_wrapper(ConfigFileHandle->GetInfo, 4, ConfigFileHandle, &gEfiFileInfoGuid, &FileInfoSize, FileInfo);
	FreePool(FileInfo);
	FileInfo = AllocatePool(FileInfoSize);
#ifndef NO_ERROR_CHECK
	if (FileInfo == NULL)
	{
		Print(L"Failed to allocate memory for the file info.\n");
		uefi_call_wrapper(BS->Stall, 1, SEC_TO_USEC(5));
		return EFI_OUT_OF_RESOURCES;
	}
#endif
	Status = uefi_call_wrapper(ConfigFileHandle->GetInfo, 4, ConfigFileHandle, &gEfiFileInfoGuid, &FileInfoSize, FileInfo);
#ifndef NO_ERROR_CHECK
	if (EFI_ERROR(Status))
	{
		Print(L"Failed to get the file info. Status: %r\n", Status);
		uefi_call_wrapper(BS->Stall, 1, SEC_TO_USEC(5));
		return Status;
	}
#endif
	BufferSize = FileInfo->FileSize;
	VOID *Buffer;
	Buffer = AllocatePool(BufferSize);
#ifndef NO_ERROR_CHECK
	if (Buffer == NULL)
	{
		Print(L"Failed to allocate memory for the file contents.\n");
		uefi_call_wrapper(BS->Stall, 1, SEC_TO_USEC(5));
		return EFI_OUT_OF_RESOURCES;
	}
#endif
	Status = uefi_call_wrapper(ConfigFileHandle->Read, 3, ConfigFileHandle, &BufferSize, Buffer);
#ifndef NO_ERROR_CHECK
	if (EFI_ERROR(Status))
	{
		Print(L"Failed to read the file contents. Status: %r\n", Status);
		uefi_call_wrapper(BS->Stall, 1, SEC_TO_USEC(5));
		return Status;
	}
#endif

	*KernelOptions = AllocatePool(BufferSize * 2 + sizeof(CHAR16));
	ConvertChar8ToChar16((CHAR8 *)Buffer, *KernelOptions, BufferSize * 2);

	FreePool(Buffer);
	FreePool(FileInfo);
	uefi_call_wrapper(ConfigFileHandle->Close, 1, ConfigFileHandle);
	uefi_call_wrapper(root->Close, 1, root);
	return Status;
}
