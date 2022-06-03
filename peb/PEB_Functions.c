#include "PEB_Functions.h"

void CharToWChar(char* charstr, WCHAR* wcharstr)
{
	int i;
	int charstrLen = strlen(charstr);
	for (i = 0; i < charstrLen; ++i)
	{
		wcharstr[i] = (WORD)charstr[i];
	}
	wcharstr[i] = 0;
}

BOOL AddElementInPeb(PeHeaders* dllPe, char* filenameWithPath, char* filenameWithoutPath)
{
	PWCHAR wcFilenameWithPath, wcFilenameWithoutPath;
	PEB_LDR_DATA* ldr = peb->Ldr;
	LDR_DATA_TABLE_ENTRY *pDllEntry;
	LDR_DATA_TABLE_ENTRY *pebHead;
	int i;
	ULONG index;
	LIST_ENTRY* hashTable;

	wcFilenameWithoutPath = (PWCHAR)malloc(sizeof(WORD) * (strlen(filenameWithoutPath) + 1));

	if (!wcFilenameWithoutPath)
	{
		return FALSE;
	}

	wcFilenameWithPath = (PWCHAR)malloc(sizeof(WORD) * (strlen(filenameWithPath) + 1));
	if (!wcFilenameWithPath)
	{
		return FALSE;
	}

	pDllEntry = (LDR_DATA_TABLE_ENTRY*)malloc(sizeof(LDR_DATA_TABLE_ENTRY));
	if (!pDllEntry)
	{
		return FALSE;
	}

	CharToWChar(filenameWithPath, wcFilenameWithPath);
	CharToWChar(filenameWithoutPath, wcFilenameWithoutPath);

	pDllEntry->DllBase = dllPe->mem;
	pDllEntry->SizeOfImage = dllPe->nthead->OptionalHeader.SizeOfImage;
	pDllEntry->EntryPoint = dllPe->mem + dllPe->nthead->OptionalHeader.AddressOfEntryPoint;
	pDllEntry->Flags = dllPe->nthead->FileHeader.Characteristics;
	//RtlInitUnicodeString(&newLDREntry->BaseDllName, modName);
	RtlInitUnicodeString(&pDllEntry->BaseDllName, wcFilenameWithoutPath);
	RtlInitUnicodeString(&pDllEntry->FullDllName, wcFilenameWithPath);
	pDllEntry->LoadCount = -1;
	pDllEntry->TlsIndex = 0;

	//InLoadOrderLinks
	//pebHead = CONTAINING_RECORD(&ldr->InLoadOrderModuleList, LDR_DATA_TABLE_ENTRY, InLoadOrderLinks);
	InsertTailList(&ldr->InLoadOrderModuleList, &pDllEntry->InLoadOrderLinks);

	//InInitializationOrderList

	//pebHead = CONTAINING_RECORD(&ldr->InInitializationOrderModuleList, LDR_DATA_TABLE_ENTRY, InInitializationOrderLinks);
	InsertTailList(&ldr->InInitializationOrderModuleList, &pDllEntry->InInitializationOrderLinks);
	//InMemoryOrderLinks

	//pebHead = CONTAINING_RECORD(&ldr->InMemoryOrderModuleList, LDR_DATA_TABLE_ENTRY, InMemoryOrderLinks);
	InsertTailList(&ldr->InMemoryOrderModuleList, &pDllEntry->InMemoryOrderLinks);

	//hash List
	index = GetHashTableIndexByDllName(wcFilenameWithoutPath);

	hashTable = FindLdrHashTable(ldr);
	if (!hashTable) {
		return FALSE;
	}

	InsertTailList(hashTable + index, &pDllEntry->HashLinks);
	

	return TRUE;
}

PVOID* SearchDllInPEB(char* dllName, PEB_LDR_DATA* ldr)
{
	LDR_DATA_TABLE_ENTRY* pebHead;
	LDR_DATA_TABLE_ENTRY* pebCurrnet;
	LDR_DATA_TABLE_ENTRY* content;
	
	
	WCHAR wcDllName[256];
	CharToWChar(dllName, wcDllName);
	pebHead = &ldr->InLoadOrderModuleList;
	pebCurrnet = pebHead->InLoadOrderLinks.Flink;
	while (pebCurrnet != pebHead)
	{	
		content = CONTAINING_RECORD(&pebCurrnet->InLoadOrderLinks, LDR_DATA_TABLE_ENTRY, InLoadOrderLinks);
		if (pebCurrnet->BaseDllName.Buffer && strcmp(pebCurrnet->BaseDllName.Buffer, wcDllName) == 0)
		{
			return pebCurrnet->DllBase;
		}
		pebCurrnet = pebCurrnet->InLoadOrderLinks.Flink;
		
		
	}
	return NULL;
}
