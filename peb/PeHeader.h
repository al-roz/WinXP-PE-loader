#pragma once
// ��������������� ��������� ������������ PE-�����
#include <windows.h>
typedef struct _PeHeaders {

    char* filename;      // ��� �����

    HANDLE              fd;             // ����� ��������� �����
    HANDLE              mapd;           // ����� �������� ��������
    PBYTE               mem;            // ��������� �� ������ ���������������� �����
    DWORD               filesize;       // ������ ��������������� ����� �����

    IMAGE_DOS_HEADER* doshead;       // ��������� �� DOS ���������
    IMAGE_NT_HEADERS* nthead;        // ��������� �� NT ���������

    IMAGE_IMPORT_DESCRIPTOR* impdir;    // ��������� �� ������ ������������ ������� �������
    DWORD               sizeImpdir;     // ������ ������� �������
    DWORD               countImpdes;    // ���������� ��������� � ������� �������

    IMAGE_EXPORT_DIRECTORY* expdir;    // ��������� �� ������� ��������
    DWORD               sizeExpdir;     // ������ ������� ��������

    IMAGE_BASE_RELOCATION* relocDir;    //���������� �� ������� ����������
    DWORD                sizeRelocDir;  //������ ������� �������

    IMAGE_SECTION_HEADER* sections;  // ��������� �� ������� ������ (�� ������ �������)
    DWORD                   countSec;   // ���������� ������

} PeHeaders;