/*

Маткин Илья Александрович   22.05.2020
*/


#include <windows.h>
#include <stdio.h>


#include "win_um_internal.h"
#include "peb.h"
#include "ldr.h"

BOOL AddElementInLDR(PEB_LDR_DATA *ldr)
{
	/*LIST_ENTRY      InLoadOrderLinks;
    LIST_ENTRY      InMemoryOrderLinks;
    LIST_ENTRY      InInitializationOrderLinks;*/

	LDR_DATA_TABLE_ENTRY *newLDREntry;
	LDR_DATA_TABLE_ENTRY *firstLib;
	LDR_DATA_TABLE_ENTRY *lastLib;	
	UNICODE_STRING dllName, fullDllName;
	ULONG index ;
	LIST_ENTRY *hashTable;	

	newLDREntry = (LDR_DATA_TABLE_ENTRY*)malloc(sizeof(LDR_DATA_TABLE_ENTRY));

	dllName.MaximumLength = 256;
	dllName.Length = 7;
	dllName.Buffer = L"Roz.dll";

	fullDllName.Buffer = L"Roz.dll";
	dllName.Length = 7;
	dllName.MaximumLength = 512;

	newLDREntry->BaseDllName = dllName;
	newLDREntry->FullDllName = fullDllName;

	index = GetHashTableIndexByDllName("Roz.dll");

	hashTable = FindLdrHashTable (ldr);
    if (!hashTable) {
        return FALSE;
        }

	


//InLoadOrderLinks
	lastLib = CONTAINING_RECORD(ldr->InLoadOrderModuleList.Blink, LDR_DATA_TABLE_ENTRY, InLoadOrderLinks);
	firstLib = CONTAINING_RECORD(&ldr->InLoadOrderModuleList, LDR_DATA_TABLE_ENTRY, InLoadOrderLinks);

	lastLib->InLoadOrderLinks.Flink = &newLDREntry->InLoadOrderLinks;
	firstLib->InLoadOrderLinks.Blink = &newLDREntry->InLoadOrderLinks;

	newLDREntry->InLoadOrderLinks.Flink = &firstLib->InLoadOrderLinks;
	newLDREntry->InLoadOrderLinks.Blink = &lastLib->InLoadOrderLinks;

//InInitializationOrderList
	lastLib = CONTAINING_RECORD(ldr->InInitializationOrderModuleList.Blink, LDR_DATA_TABLE_ENTRY, InInitializationOrderLinks);
	firstLib = CONTAINING_RECORD(&ldr->InInitializationOrderModuleList, LDR_DATA_TABLE_ENTRY, InInitializationOrderLinks);

	lastLib->InInitializationOrderLinks.Flink = &newLDREntry->InInitializationOrderLinks;
	firstLib->InInitializationOrderLinks.Blink = &newLDREntry->InInitializationOrderLinks;

	newLDREntry->InInitializationOrderLinks.Flink = &firstLib->InInitializationOrderLinks;
	newLDREntry->InInitializationOrderLinks.Blink = &lastLib->InInitializationOrderLinks;
//InMemoryOrderLinks

	lastLib = CONTAINING_RECORD(ldr->InMemoryOrderModuleList.Blink, LDR_DATA_TABLE_ENTRY, InMemoryOrderLinks);
	firstLib = CONTAINING_RECORD(&ldr->InMemoryOrderModuleList, LDR_DATA_TABLE_ENTRY, InMemoryOrderLinks);

	lastLib->InMemoryOrderLinks.Flink = &newLDREntry->InMemoryOrderLinks;
	firstLib->InMemoryOrderLinks.Blink = &newLDREntry->InMemoryOrderLinks;

	newLDREntry->InMemoryOrderLinks.Flink = &firstLib->InMemoryOrderLinks;
	newLDREntry->InMemoryOrderLinks.Blink = &lastLib->InMemoryOrderLinks;	
	
	//newLDREntry->EntryPoint = 123;

	InsertTailList (hashTable + index, &newLDREntry->HashLinks);
	//newLDREntry->BaseNameHashValue = LdrHashUnicodeString (L"Roz.dll", (ULONG)wcslen(L"Roz.dll"));
	RtlInitUnicodeString (&newLDREntry->BaseDllName, L"Roz.dll");
	newLDREntry->DllBase = &newLDREntry;
	//newLDREntry->EntryPoint = &newLDREntry + 20;
	newLDREntry->SizeOfImage = 123;
	//newLDREntry->TlsIndex = 0;	
		
	return TRUE;
};

int main() {

HMODULE mod;
PEB *peb;
PEB_LDR_DATA *ldr;

    PrintCurrentPEB();

    PrintModulesInCurrentPEB();
    
    mod = LoadLibrary ("advapi32.dll");

    PrintModulesInCurrentPEB();

    //*

	peb = GetCurrentPEB();
	ldr = peb->Ldr;	
	
	
	AddElementInLDR(ldr);

	PrintModulesInCurrentPEB();

	mod = GetModuleHandle("Roz.dll");
	mod = LoadLibrary("Roz.dll");
	mod = GetModuleHandle(L"Roz.dll");
	mod = LoadLibrary(L"Roz.dll");
    return 0;
}
