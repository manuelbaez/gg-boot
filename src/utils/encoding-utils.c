#include "encoding-utils.h"

UINTN AsciiStrLen(IN CONST CHAR8 *String)
{
	UINTN Length = 0;
	while (*String != '\0')
	{
		Length++;
		String++;
	}
	return Length;
}

EFI_STATUS ConvertChar8ToChar16(
	IN CHAR8 *SrcString,
	OUT CHAR16 *DestString,
	IN UINTN DestSize)
{
	if (SrcString == NULL || DestString == NULL || DestSize < 2)
	{
		return EFI_INVALID_PARAMETER;
	}

	UINTN SrcSize = AsciiStrLen(SrcString);
	if (SrcSize == 0)
	{
		return EFI_SUCCESS;
	}

	UINTN DestMaxSize = DestSize / sizeof(CHAR16) - 1; // reserve space for null-terminator
	UINTN i;
	for (i = 0; i < DestMaxSize && i < SrcSize; i++)
	{
		DestString[i] = (CHAR16)SrcString[i];
	}
	DestString[i] = L'\0';

	if (i == DestMaxSize && SrcSize > DestMaxSize)
	{
		// Destination buffer was too small
		return EFI_BUFFER_TOO_SMALL;
	}

	return EFI_SUCCESS;
}