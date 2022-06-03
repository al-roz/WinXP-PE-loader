#include "RelocModul.h"
//
//Обработка таблицы релоков
//
BOOL ProcessReloc(PeHeaders* pe)
{
    DWORD memoryOffset = pe->mem - pe->nthead->OptionalHeader.ImageBase;
    DWORD *fixedAddres;
    WORD *typeAndRVAOffset;
    DWORD countDescriptions;
    DWORD i;
    DWORD offset = 0;
    IMAGE_BASE_RELOCATION* tmpRelocTable = pe->relocDir;
	DWORD type;
	DWORD fixedOffset;

	
    while (offset != pe->sizeRelocDir)
    {
        countDescriptions = (tmpRelocTable->SizeOfBlock - sizeof(IMAGE_BASE_RELOCATION)) / 2;
        typeAndRVAOffset = (WORD*)((DWORD)tmpRelocTable + sizeof(IMAGE_BASE_RELOCATION));
        for (i = 0; i < countDescriptions; i++)
        {
			type = typeAndRVAOffset[i] >> 12;
			fixedOffset = typeAndRVAOffset[i] & 0x0FFF;
			if (type == 0)
				continue;
            if (type == 3)
            {
                fixedAddres = (DWORD*)(pe->mem + tmpRelocTable->VirtualAddress + (typeAndRVAOffset[i] & 0x0FFF));
                *fixedAddres += memoryOffset;
            }
        }
        offset += tmpRelocTable->SizeOfBlock;
        tmpRelocTable = (IMAGE_BASE_RELOCATION*)((DWORD)tmpRelocTable + tmpRelocTable->SizeOfBlock);
    }
	return TRUE;
}