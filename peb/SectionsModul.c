#include "SectionsModul.h"


DWORD GetSectionProtect(DWORD SC)
{   
    DWORD result = 0;
    if (SC & IMAGE_SCN_MEM_NOT_CACHED)
    {
        result = result | PAGE_NOCACHE;
    }

    if (SC & IMAGE_SCN_MEM_EXECUTE)
    {
        if (SC & IMAGE_SCN_MEM_READ)
        {
            if (SC & IMAGE_SCN_MEM_WRITE)
            {
                result = result | PAGE_EXECUTE_READWRITE;
            }
            else
            {
                result = result | PAGE_EXECUTE_READ;
            }
        }
        else 
        {
            if (SC & IMAGE_SCN_MEM_WRITE)
            {
                result = result | PAGE_EXECUTE_READWRITE;
            }
            else
            {
                result = result | PAGE_EXECUTE;
            }            
        }       

    }
    else if (SC & IMAGE_SCN_MEM_READ)
    {
        if (SC & IMAGE_SCN_MEM_WRITE)
        {
            result = result | PAGE_READWRITE;
        }
        else
        {
            result = result | PAGE_READONLY;
        }
    }
	else
    {
        if (SC & IMAGE_SCN_MEM_WRITE)
        {
            result = result | PAGE_READWRITE;
        }
        else
        {
            result = result | PAGE_NOACCESS;
        }
    }   

    return result;
}

//
// Устанавливает права доступа на области памяти секций PE-файла.
//
BOOL SetPeSectionsProtect (PeHeaders *pe) {
    
    DWORD i;
    DWORD oldProtect;
    for (i = 0; i < pe->nthead->FileHeader.NumberOfSections; ++i)
    {    
        if (!VirtualProtect(
            pe->mem + pe->sections[i].VirtualAddress,
            pe->sections[i].Misc.VirtualSize,
            GetSectionProtect(pe->sections[i].Characteristics),
            &oldProtect))
        {
            return FALSE;
        }
    }
    return TRUE;
}
