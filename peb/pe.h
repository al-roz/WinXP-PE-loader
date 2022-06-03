

#ifndef _PE_H_
#define _PE_H_


#include <windows.h>

//----------------------------------------

// вспомогательная структура загруженного PE-файла
typedef struct _PeHeaders{

    char                *filename;      // имя файла

    HANDLE              fd;             // хендл открытого файла
    HANDLE              mapd;           // хендл файловой проекции
    PBYTE               mem;            // указатель на память спроецированного файла
    DWORD               filesize;       // размер спроецированной части файла

    IMAGE_DOS_HEADER    *doshead;       // указатель на DOS заголовок
    IMAGE_NT_HEADERS    *nthead;        // указатель на NT заголовок

    IMAGE_IMPORT_DESCRIPTOR *impdir;    // указатель на массив дескрипторов таблицы импорта
    DWORD               sizeImpdir;     // размер таблицы импорта
    DWORD               countImpdes;    // количество элементов в таблице импорта

    IMAGE_EXPORT_DIRECTORY  *expdir;    // указатель на таблицу экспорта
    DWORD               sizeExpdir;     // размер таблицы экспорта

    IMAGE_SECTION_HEADER    *sections;  // указатель на таблицу секций (на первый элемент)
    DWORD                   countSec;   // количество секций

} PeHeaders;

//----------------------------------------


BOOL LoadPeFile(char *filename, PeHeaders *pe, DWORD filesize);

void UnloadPeFile (PeHeaders *pe);

DWORD RvaToOffset (DWORD rva, PeHeaders *pe);

DWORD AlignToTop (DWORD value, DWORD align);

DWORD AlignToBottom (DWORD value, DWORD align);

void PrintPeHeaders (PeHeaders *pe);

void PrintDirectoryTable (PeHeaders *pe);

void PrintImportTable (PeHeaders *pe);

void PrintSectionTable (PeHeaders *pe);

void PrintExportTable (PeHeaders *pe);

void PrintSectionInFileDumpByName (PeHeaders *pe, unsigned int bytesPerLine, char *secname);
void PrintSectionInMemoryDumpByName (PeHeaders *pe, unsigned int bytesPerLine, char *secname);

void PrintFileDump (PeHeaders *pe, unsigned int bytesPerLine);
void PrintImageDump (PeHeaders *pe, unsigned int bytesPerLine);

void PrintHeadersInFileDump (PeHeaders *pe, unsigned int bytesPerLine);
void PrintHeadersInMemoryDump (PeHeaders *pe, unsigned int bytesPerLine);

void PrintBaseReloc (PeHeaders *pe);

//----------------------------------------

#endif  // _PE_H_
