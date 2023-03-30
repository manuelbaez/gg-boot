#include "boot.h"

EFI_STATUS SetBootEntries(EFI_DEVICE_PATH_PROTOCOL *devicePath, CHAR16 *description, CHAR16 *kernelParams)
{
	EFI_STATUS Status;
	UINTN paramsSize = StrSize(kernelParams);
	EFI_LOAD_OPTION bootOption = {
		.Attributes = LOAD_OPTION_ACTIVE,
		.FilePathListLength = DevicePathSize(devicePath),
		.Description = description,
		.FilePathList = devicePath,
		.OptionalData = kernelParams,
		.OptionalDataSize = paramsSize};

	UINTN bootOptionSize = sizeof(bootOption) + StrSize(description) + DevicePathSize(devicePath) + paramsSize;
	Status = uefi_call_wrapper(gRT->SetVariable,
							   5,
							   description,
							   &gEfiGlobalVariableGuid,
							   EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
							   bootOptionSize,
							   &bootOption);
	return Status;
}