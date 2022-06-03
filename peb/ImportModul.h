#pragma once
#include <windows.h>
#include "PEB_Functions.h"

//BOOL LoadDll(char* dllName, PeHeaders* impDllPe);

BOOL ProcImports(PeHeaders* pe);

