/*

Модуль pe_loader.c.

Функции загрузки PE-файла.

Маткин Илья Александрович   07.03.2014

*/

#include <windows.h>
#include <stdio.h>
#include "pe_loader.h"
#include "system.h"

//----------------------------------------

BOOL AllocImageMemory (PeHeaders *pe);

//----------------------------------------

//
// Выделяет память под образ PE-файла.
//
BOOL AllocImageMemory (PeHeaders *pe) {

    pe->mem = VirtualAlloc ((PVOID)pe->nthead->OptionalHeader.ImageBase,
                            pe->nthead->OptionalHeader.SizeOfImage, 
                            MEM_COMMIT | MEM_RESERVE, 
                            PAGE_READWRITE);
    if (pe->mem == NULL) {
        if (pe->nthead->FileHeader.Characteristics & IMAGE_FILE_RELOCS_STRIPPED) {
            //VirtualFree (pe->mem, 0, MEM_RELEASE);
            printf ("Error alloc image memory\n");
            return FALSE;
            }

        pe->mem = VirtualAlloc (0, pe->nthead->OptionalHeader.SizeOfImage, 
                                MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
        if (pe->mem == NULL) {
            printf ("Error alloc image memory\n");
            return FALSE;
            }
        
        }

    return TRUE;
}


//
// Загружает в память секции PE-файла.
//
BOOL LoadPeSections (PeHeaders *pe) {

    DWORD i;
    DWORD bytesRead;

    for (i = 0; i < pe->nthead->FileHeader.NumberOfSections; ++i) {
        SetFilePointer (pe->fd, pe->sections[i].PointerToRawData, NULL, FILE_BEGIN);
        if (!ReadFile (pe->fd, pe->mem + pe->sections[i].VirtualAddress, 
                       pe->sections[i].SizeOfRawData, &bytesRead, NULL)) {
            printf ("Error load %d section\n", i);
            return FALSE;
            }
        }

    return TRUE;
}

void GetTablesFromNt(PeHeaders* pe)
{
    // получаем инфомацию об экспорте
    if (pe->nthead->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress) {
        pe->expdir = (IMAGE_EXPORT_DIRECTORY*)
            (pe->mem + pe->nthead->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress);
        pe->sizeExpdir = pe->nthead->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].Size;
    }
    else {
        pe->expdir = 0;
        pe->sizeExpdir = 0;
    }

    // получаем информацию об импорте
    if (pe->nthead->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress) {
        pe->impdir = (IMAGE_IMPORT_DESCRIPTOR*)
            (pe->mem + pe->nthead->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress);
        pe->sizeImpdir = pe->nthead->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].Size;
    }
    else {
        pe->impdir = 0;
        pe->sizeImpdir = 0;
    }

    // получение информации о релоках 
    if (pe->nthead->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC].VirtualAddress)
    {
        pe->relocDir = (IMAGE_BASE_RELOCATION*)(pe->mem + pe->nthead->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC].VirtualAddress);
        pe->sizeRelocDir = pe->nthead->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC].Size;
    }
    else
    {
        pe->relocDir = 0;
        pe->sizeRelocDir = 0;
    }
}

//
// обработка таблицы импорта
//
//https://firststeps.ru/mfc/winapi/
//
// обработка таблицы импорта
//
BOOL ProcessImports(PeHeaders* pe)
{
    IMAGE_IMPORT_DESCRIPTOR* impDir = pe->impdir;
    char *libName;
    DWORD i;
    LONG_PTR *INT, *IAT;
    void* pointer;    
    
    while (!(impDir->FirstThunk == 0 && impDir->Characteristics == 0 && impDir->ForwarderChain == 0
        && impDir->Name == 0 && impDir->OriginalFirstThunk == 0 && impDir->TimeDateStamp == 0))
    {
        HMODULE lib;
        libName = (char*)(pe->mem + impDir->Name);
        lib = LoadLibrary(libName);

        if (!lib)
        {
            return FALSE;
        }

        if (lib->unused)
        {
            if (impDir->OriginalFirstThunk)
            {
                INT = (LONG_PTR*)(pe->mem + impDir->OriginalFirstThunk);
            }
            else
            {
                INT = (LONG_PTR*)(pe->mem + impDir->FirstThunk);
            }

            IAT = (LONG_PTR*)(pe->mem + impDir->FirstThunk);
            if (!IAT || !INT)
            {
                return FALSE;
            }
            for (i = 0; INT[i]; i++)
            {   
                char* procName = (char*)(pe->mem + INT[i] + 2);
                pointer = GetProcAddress(lib, procName);
                if (!pointer)
                {
                    return FALSE;
                }
                IAT[i] = pointer;
            }
        }        
        impDir++;
    }
    return TRUE;
}

//
// Загружает в память заголовки PE-файла.
//
BOOL LoadPeHeaders (char *filename, PeHeaders *pe) {

    BYTE dosHeader[sizeof(IMAGE_DOS_HEADER)];
    //IMAGE_DOS_HEADER dosHeader;
    BYTE ntHeader[sizeof(IMAGE_NT_HEADERS)];
    DWORD bytesRead;

    pe->fd = CreateFile(filename ,      // имя файла
                        GENERIC_READ,   // права доступа
                        0,
                        NULL,
                        OPEN_EXISTING,          // открываемый файл должен существовать
                        FILE_ATTRIBUTE_NORMAL,
                        NULL);
    if(pe->fd == INVALID_HANDLE_VALUE){
        printf("Error open file\n");
        return FALSE;
        }

    if (!ReadFile (pe->fd, dosHeader, sizeof(IMAGE_DOS_HEADER), &bytesRead, NULL)) {
        CloseHandle (pe->fd);
        printf ("Error read dos header\n");
        return FALSE;
        }
    
    SetFilePointer (pe->fd, ((IMAGE_DOS_HEADER*)dosHeader)->e_lfanew, NULL, FILE_BEGIN);

    if (!ReadFile (pe->fd, ntHeader, sizeof(IMAGE_NT_HEADERS), &bytesRead, NULL)) {
        CloseHandle (pe->fd);
        printf ("Error read nt headers\n");
        return FALSE;
        }

    pe->nthead = (IMAGE_NT_HEADERS*)ntHeader;

    if (!AllocImageMemory (pe)) {
        CloseHandle (pe->fd);
        return FALSE;
        }

    SetFilePointer (pe->fd, 0, NULL, FILE_BEGIN);

    if (!ReadFile (pe->fd, pe->mem, pe->nthead->OptionalHeader.SizeOfHeaders, &bytesRead, NULL)) {
        VirtualFree (pe->mem, 0, MEM_RELEASE);
        CloseHandle (pe->fd);
        printf ("Error read nt headers\n");
        return FALSE;
        }

    pe->doshead = (IMAGE_DOS_HEADER*) pe->mem;
    pe->nthead = (IMAGE_NT_HEADERS*)(pe->mem + ((IMAGE_DOS_HEADER*)dosHeader)->e_lfanew);
    pe->sections = (IMAGE_SECTION_HEADER*)((DWORD) &(pe->nthead->OptionalHeader) + pe->nthead->FileHeader.SizeOfOptionalHeader);
    pe->countSec = pe->nthead->FileHeader.NumberOfSections;

    pe->filesize = GetFileSize (pe->fd, NULL);

    return TRUE;
}


//
// Формирует в памяти образ PE-файла.
//
BOOL LoadPeImage (char *filename, char* filenameWithoutPath, PeHeaders *pe) {

    if (!LoadPeHeaders (filename, pe)) {
        return FALSE;
        }

    if (!LoadPeSections (pe)) {
        CloseHandle (pe->fd);
        return FALSE;
        }
    
    GetTablesFromNt(pe);
	
	AddElementInPeb(pe, filename, filenameWithoutPath);
		
    // обработка релоков
    if (pe->nthead->OptionalHeader.ImageBase != ((DWORD)pe->mem))
    {      
        if (!ProcessReloc(pe))
        {
            return FALSE;
        }
    }

	

    if (pe->nthead->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress) {
        if (!ProcImports(pe))
        {
            return FALSE;
        }
    }

    // обработка импорта
    /*if (pe->nthead->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress) {
        if (!ProcessImports(pe))
        {
            return FALSE;
        }
    }*/

    // после обработки образа выставляем права доступа
    SetPeSectionsProtect (pe);

    // передача управления точке входа
    if (pe->nthead->FileHeader.Characteristics & IMAGE_FILE_DLL)
    {
          return ((BOOL(__stdcall*)(HINSTANCE, DWORD, LPVOID))(pe->mem + pe->nthead->OptionalHeader.AddressOfEntryPoint))(NULL, DLL_PROCESS_ATTACH, NULL);
    }
    else if (pe->nthead->FileHeader.Characteristics & IMAGE_FILE_EXECUTABLE_IMAGE)
    {
        ((void (*)(void))(pe->mem + pe->nthead->OptionalHeader.AddressOfEntryPoint))();
    }

    return TRUE;//7d5c
}

//
// Возвращает файловое смещение по RVA.
//
DWORD RvaToOffset (DWORD rva, PeHeaders *pe) {

    unsigned int i;
    IMAGE_SECTION_HEADER *sections = pe->sections;
    unsigned int NumberSection = pe->countSec;


    if (rva > pe->nthead->OptionalHeader.SizeOfImage){
        return 0;
        }

    //проходим по всем секциям и ищем
    //в какую попадает RVA
    for (i = 0; i < NumberSection; ++i) {
        if( (rva >= sections[i].VirtualAddress) && 
            (rva <= sections[i].VirtualAddress + sections[i].Misc.VirtualSize))
            return rva - sections[i].VirtualAddress + sections[i].PointerToRawData;
        }

    return 0;
}


//
// Выравнивает значение с кратностью align к верхней границе.
//
DWORD AlignToTop (DWORD value, DWORD align) {


    DWORD mask = ~ (align - 1);

    return (value + align - 1) & mask;
}


//
// Выравнивает значение с кратностью align к нижней границе.
//
DWORD AlignToBottom (DWORD value, DWORD align) {


    DWORD mask = ~ (align - 1);

    return value & mask;
}


//
// Возвращает индекс секции по имени
//
unsigned int GetSectionIndexByName (PeHeaders *pe, char *secname) {


    unsigned int indexSec;

    for (indexSec = 0; indexSec < pe->countSec; ++indexSec) {
        
        if (!_strnicmp (pe->sections[indexSec].Name, secname, 8)) {
            return indexSec;
            }
        }

    return 0xFFFFFFFF;
}

//----------------------------------------

