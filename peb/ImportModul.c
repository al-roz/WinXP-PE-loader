#include "ImportModul.h"

void InitPeDll(PeHeaders* peLib, DWORD libBase)
{
    peLib->mem = libBase;
    peLib->doshead = (IMAGE_DOS_HEADER*)peLib->mem;
    peLib->nthead = peLib->mem + peLib->doshead->e_lfanew;
	peLib->sections = (IMAGE_SECTION_HEADER*)((DWORD) &(peLib->nthead->OptionalHeader) + peLib->nthead->FileHeader.SizeOfOptionalHeader);
    peLib->countSec = peLib->nthead->FileHeader.NumberOfSections;
    GetTablesFromNt(peLib);
}

BOOL LoadDll(char* dllName, PeHeaders* impDllPe)
{
    int i;
    char dllPath[4][256] =
    { 
		"C:\\Windows\\System32\\\0",
        "C:\\Windows\\\0",
        "C:\\Windows\\System32\\Wbem\\\0",
        "\0",
    };
	//debug
    for (i = 0; i < 4; i++)
    {
        strcat(dllPath[i], dllName);
    }

    for (i = 0; i < 4; i++)
    {
        if (LoadPeImage(dllPath[i], dllName, impDllPe))
        {
            return TRUE;
        }        
    }

    return FALSE;
}

void toLow(char *src)
{
	int i = 0;
	for (i = 0; i < strlen(src); i++)
	{
		if (src[i] >= 'A' && src[i] <= 'Z')
		{
			src[i] += 'a' - 'A';	
		}
	}

}

void ParsRedirectedDll(char *ImportName, char* dllName, char* libName, char term)
{
    int k;
    k = 0;
    while (ImportName[k] != term && k < strlen(ImportName))
    {
        k++;
    }
    memcpy(dllName, ImportName, k);
    dllName[k] = 0;
	toLow(dllName);
    strcat(dllName, ".dll");

    memcpy(libName, &ImportName[k + 1], strlen(ImportName) - k - 1);
    libName[strlen(ImportName) - k - 1] = 0;
}

DWORD RedirImport(PeHeaders* pe, char* redirFuncName)
{
    char *funcName;
    DWORD i;

	DWORD* funcAddres;
    DWORD* funcNames;
    WORD* funcNameOrdinals;

	funcAddres = (DWORD*)(pe->mem + pe->expdir->AddressOfFunctions);
	funcNames = (DWORD*)(pe->mem + pe->expdir->AddressOfNames);
	funcNameOrdinals = (DWORD*)(pe->mem + pe->expdir->AddressOfNameOrdinals);

	for (i = 0; i < pe->expdir->NumberOfNames; ++i)
	{
		funcName = funcNames[i] + pe->mem;
		if (strcmp(funcName, redirFuncName) == 0)
		{
			return funcAddres[funcNameOrdinals[i]] + pe->mem;
		}
	}
     
    return NULL;
}

BOOL ImportDll(PeHeaders* pe, PeHeaders* libpe, IMAGE_IMPORT_DESCRIPTOR* impDir)
{
    DWORD pebDllBase;
    DWORD* funcAddres;
    DWORD* funcNames;
    WORD* funcNameOrdinals;
    char* symFt;
    char* symExp;
    char* redirSym;
    char redirLibName[255];
    char redirDll[255];
    LONG_PTR *INT, *IAT;
    int i, j, ordinal;
    unsigned int dllExpdirEndAddres = (unsigned int)libpe->expdir + libpe->sizeExpdir;
    PeHeaders redirPe;
	DWORD adress;
	HMODULE lib;

    funcAddres = (DWORD*)(libpe->mem + libpe->expdir->AddressOfFunctions);
    funcNames = (DWORD*)(libpe->mem + libpe->expdir->AddressOfNames);
    funcNameOrdinals = (DWORD*)(libpe->mem + libpe->expdir->AddressOfNameOrdinals);

	/*lib = LoadLibrary ((char*)pe->mem + impDir->Name);
	if (lib != libpe->mem)
	{
		printf("\n lib mistake: %s\n\n",((char*)pe->mem + impDir->Name) );
	}*/
    if (impDir->OriginalFirstThunk == 0)
    {
        INT = (LONG_PTR)(pe->mem + impDir->FirstThunk);
    }
    else
    {
        INT = (LONG_PTR*)(pe->mem + impDir->OriginalFirstThunk);
    }

    IAT = (LONG_PTR*)(pe->mem + impDir->FirstThunk);

    for (j = 0; INT[j]; ++j)
    {
        if (INT[j] & 0x80000000)
        {
            ordinal = INT[j] & ~0x80000000 - libpe->expdir->Base;
			
            IAT[j] = funcAddres[ordinal] + libpe->mem;
			
			if (IAT[j] >= libpe->expdir && IAT[j] < dllExpdirEndAddres)
			{
				//ParsRedirectedDll(redirSym, redirDll, redirLibName,'#');
				
			}
        }
        else
        {
            symFt = (char*)pe->mem + INT[j] + 2;
            for (i = 0; i < libpe->expdir->NumberOfNames; i++)
            {
                symExp = (char*)(funcNames[i] + libpe->mem);
                if (strcmp(symFt, symExp) == 0)
                {
                    IAT[j] = funcAddres[funcNameOrdinals[i]] + libpe->mem;
					
                    if (IAT[j] >= libpe->expdir && IAT[j] < dllExpdirEndAddres)
                    {
                        redirSym = IAT[j];
                        ParsRedirectedDll(redirSym, redirDll, redirLibName,'.');
						
                        pebDllBase = SearchDllInPEB(redirDll, peb->Ldr);						
                        if (pebDllBase)
                        {
                            InitPeDll(&redirPe, pebDllBase);
                        }
                        else if (!LoadDll(redirDll, &redirPe))
                        {
                            return FALSE;
                        }

                        if (!(IAT[j] = RedirImport(&redirPe, redirLibName)))
                        {
                            return FALSE;
                        }
						/*lib = LoadLibrary(redirDll);
						adress = GetProcAddress(lib, redirLibName);
						if (adress != IAT[j])
						{
							printf("%s my:%d win:%d\n", symExp, &IAT[j], adress);
							IAT[j] = adress;
						}*/
                    }					
                }
            }
        }
    }
	return TRUE;
}

BOOL ProcImports(PeHeaders* pe)
{
    IMAGE_IMPORT_DESCRIPTOR* impDir = pe->impdir;
    PVOID* pebDllBaseAddress;
	DWORD lib;

    while (!(impDir->FirstThunk == 0 && impDir->Characteristics == 0 && impDir->ForwarderChain == 0
        && impDir->Name == 0 && impDir->OriginalFirstThunk == 0 && impDir->TimeDateStamp == 0)) 
    {
        PeHeaders dllPe;
        char* dllName = (char*)(pe->mem + impDir->Name);
		toLow(dllName);
		dllPe.filename = pe->mem + impDir->Name;    
		
        if (pebDllBaseAddress = SearchDllInPEB(dllName,peb->Ldr))
        {
            dllPe.filename = pe->mem + impDir->Name;            
            InitPeDll(&dllPe, pebDllBaseAddress);            
        }
        else if (!LoadDll(dllName, &dllPe))
        {
            return FALSE;
        }

        if (!ImportDll(pe, &dllPe, impDir))
        {
            return FALSE;
        }
        impDir++;
    }
    return TRUE;
}