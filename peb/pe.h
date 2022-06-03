

#ifndef _PE_H_
#define _PE_H_


#include <windows.h>

//----------------------------------------

// ��������������� ��������� ������������ PE-�����
typedef struct _PeHeaders{

    char                *filename;      // ��� �����

    HANDLE              fd;             // ����� ��������� �����
    HANDLE              mapd;           // ����� �������� ��������
    PBYTE               mem;            // ��������� �� ������ ���������������� �����
    DWORD               filesize;       // ������ ��������������� ����� �����

    IMAGE_DOS_HEADER    *doshead;       // ��������� �� DOS ���������
    IMAGE_NT_HEADERS    *nthead;        // ��������� �� NT ���������

    IMAGE_IMPORT_DESCRIPTOR *impdir;    // ��������� �� ������ ������������ ������� �������
    DWORD               sizeImpdir;     // ������ ������� �������
    DWORD               countImpdes;    // ���������� ��������� � ������� �������

    IMAGE_EXPORT_DIRECTORY  *expdir;    // ��������� �� ������� ��������
    DWORD               sizeExpdir;     // ������ ������� ��������

    IMAGE_SECTION_HEADER    *sections;  // ��������� �� ������� ������ (�� ������ �������)
    DWORD                   countSec;   // ���������� ������

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
