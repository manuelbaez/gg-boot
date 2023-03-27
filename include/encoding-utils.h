#include <efi.h>
#include <efilib.h>
#include <string.h>

UINTN AsciiStrLen(IN CONST CHAR8 *String);
EFI_STATUS ConvertChar8ToChar16(IN CHAR8 *SrcString, OUT CHAR16 *DestString, IN UINTN DestSize);