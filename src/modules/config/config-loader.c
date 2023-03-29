#include "config-loader.h"
#include "constants.h"

EFI_STATUS GetKernelParams(EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *RootFileSystemProtocol, OUT CHAR16 **KernelOptions)
{
	EFI_FILE_PROTOCOL *ConfigFileHandle;
	EFI_FILE_PROTOCOL *root;
	EFI_FILE_INFO *FileInfo;
	EFI_STATUS Status;
	Print(L"Open Boot Volume \n");
	uefi_call_wrapper(RootFileSystemProtocol->OpenVolume, 2, RootFileSystemProtocol, &root);
	Print(L"Open config file \n");
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
	Print(L"Get config file info \n");
	Status = uefi_call_wrapper(ConfigFileHandle->GetInfo, 4, ConfigFileHandle, &gEfiFileInfoGuid, &FileInfoSize, NULL);

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

	*KernelOptions = AllocatePool(BufferSize * 2);
	ConvertChar8ToChar16((CHAR8 *)Buffer, *KernelOptions, BufferSize * 2);

	FreePool(Buffer);
	FreePool(FileInfo);
	uefi_call_wrapper(ConfigFileHandle->Close, 1, ConfigFileHandle);
	return EFI_SUCCESS;
}
