/*

Файл pe_loader.h - заголовочный файл модуля pe_loader.c.

Маткин Илья Александрович   07.03.2014

*/

#ifndef _PE_LOADER_H_
#define _PE_LOADER_H_

#include <windows.h>
#include "PEB_Functions.h"

#include "PeHeader.h"
#include "ImportModul.h"
#include "SectionsModul.h"

//----------------------------------------


//----------------------------------------

BOOL LoadPeImage (char* filename, char* filenameWithoutPath, PeHeaders* pe);

BOOL LoadDll(char* dllName, PeHeaders* impDllPe);

BOOL ImportDll(PeHeaders* pe, PeHeaders* libpe, IMAGE_IMPORT_DESCRIPTOR* impDir);

BOOL LoadPeFile(char *filename, PeHeaders *pe, DWORD filesize);

void UnloadPeFile (PeHeaders *pe);

DWORD RvaToOffset (DWORD rva, PeHeaders *pe);

DWORD AlignToTop (DWORD value, DWORD align);

DWORD AlignToBottom (DWORD value, DWORD align);

//----------------------------------------

#endif  // _PE_LOADER_H_
