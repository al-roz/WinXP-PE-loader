#pragma once
#include "win_um_internal.h"
#include "peb.h"
#include "ldr.h"
#include "teb.h"
#include "PeHeader.h"

PEB* peb;

BOOL AddElementInPeb(PeHeaders* dllPe, char* filenameWithPath, char* filenameWithoutPaht);

PVOID* SearchDllInPEB(char* dllName, PEB_LDR_DATA* ldr);