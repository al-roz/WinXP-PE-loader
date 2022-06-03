#pragma once
// вспомогательная структура загруженного PE-файла
#include <windows.h>
typedef struct _PeHeaders {

    char* filename;      // имя файла

    HANDLE              fd;             // хендл открытого файла
    HANDLE              mapd;           // хендл файловой проекции
    PBYTE               mem;            // указатель на память спроецированного файла
    DWORD               filesize;       // размер спроецированной части файла

    IMAGE_DOS_HEADER* doshead;       // указатель на DOS заголовок
    IMAGE_NT_HEADERS* nthead;        // указатель на NT заголовок

    IMAGE_IMPORT_DESCRIPTOR* impdir;    // указатель на массив дескрипторов таблицы импорта
    DWORD               sizeImpdir;     // размер таблицы импорта
    DWORD               countImpdes;    // количество элементов в таблице импорта

    IMAGE_EXPORT_DIRECTORY* expdir;    // указатель на таблицу экспорта
    DWORD               sizeExpdir;     // размер таблицы экспорта

    IMAGE_BASE_RELOCATION* relocDir;    //указателья на таблицу директорий
    DWORD                sizeRelocDir;  //Размер таблицы релоков

    IMAGE_SECTION_HEADER* sections;  // указатель на таблицу секций (на первый элемент)
    DWORD                   countSec;   // количество секций

} PeHeaders;