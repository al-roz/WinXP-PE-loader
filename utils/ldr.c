
#include <stdio.h>

#include "win_um_internal.h"
#include "ldr.h"
#include "peb.h"


//
// Соответствует вызову
// RtlHashUnicodeString (..., TRUE, 0, ...)
// который используется для подсчета хеша в Win10
//
ULONG DefaultCaseInSensitiveHashUnicodeString (WCHAR *str, ULONG size) {

WCHAR *p;
ULONG hash = 0;
//unsigned int i;

    //for (i = 0; i < size; ++i) {
        //WCHAR c = str[i];
    for (p = str + size - 1; p >= str; --p) {
        WCHAR c = *p;
        if (c <= L'z') {
            c = (c >= L'a' && c <= L'z') ? c - L'a' + L'A' : c;
            hash = hash * 0x1003F + c;
            }
        //printf ("[debug] hash: %x\n", hash);
        }

    return hash;
}



ULONG LdrHashTableSize = 32;

ULONG LdrHashUnicodeStringWinXP (WCHAR *str, ULONG size) {

    return (RtlUpcaseUnicodeChar((str[0])) - 1);
}



ULONG LdrHashUnicodeStringWin7 (WCHAR *str, ULONG size) {

WCHAR *p;
ULONG hash = 0;

    for (p = str + size - 1; p >= str; --p) {
        hash += RtlUpcaseUnicodeChar(*p) * 0x1003F;
        }

    return hash;
}




#ifdef WUI_X64
ULONG LdrHashUnicodeStringWin10 (WCHAR *str, ULONG size) {

UNICODE_STRING tmp;
ULONG retValue = 0;

    RtlInitUnicodeString (&tmp, str);

    if (NT_SUCCESS (RtlHashUnicodeString (&tmp, TRUE, 0, &retValue))) {
        return retValue;
        }
    else {
        return -1;
        }
}
#else   // WUI_X64
ULONG LdrHashUnicodeStringWin10 (WCHAR *str, ULONG size) {

    return DefaultCaseInSensitiveHashUnicodeString (str, size);
}
#endif  // WUI_X64



ULONG GetHashTableIndexByDllNameUS (PUNICODE_STRING str) {

    return LdrHashUnicodeString (str->Buffer, str->Length / sizeof(str->Buffer[0])) % LdrHashTableSize;
}


ULONG GetHashTableIndexByDllName (PWCHAR str) {

    return LdrHashUnicodeString (str, (ULONG)wcslen(str)) % LdrHashTableSize;
}


LDR_DATA_TABLE_ENTRY* FindModuleInLdrListByName (PEB_LDR_DATA *ldr, WCHAR *moduleName) {


UNICODE_STRING moduleNameUS;
LDR_DATA_TABLE_ENTRY *current;
LIST_ENTRY *currentListEntry;

    RtlInitUnicodeString (&moduleNameUS, moduleName);

    currentListEntry = ldr->InLoadOrderModuleList.Flink;

    do{

        current = CONTAINING_RECORD(currentListEntry, LDR_DATA_TABLE_ENTRY, InLoadOrderLinks);

        if (RtlEqualUnicodeString (&current->BaseDllName, &moduleNameUS, TRUE)) {
            return current;
            }
        currentListEntry = currentListEntry->Flink;

    } while (currentListEntry != &ldr->InLoadOrderModuleList);

    return NULL;
}



LDR_DATA_TABLE_ENTRY* FindModuleInCurrentLdrListByName (WCHAR *moduleName) {

    return FindModuleInLdrListByName (GetCurrentLdr(), moduleName);
}



#define NTDLL_NAME  L"ntdll.dll"

LDR_DATA_TABLE_ENTRY* FindNtdllInLdrList (PEB_LDR_DATA *ldr) {

    return FindModuleInLdrListByName (ldr, NTDLL_NAME);
}


//
// Возвращает адрес ntdll!LdrpHashTable
//
LIST_ENTRY* FindLdrHashTable (PEB_LDR_DATA *ldr) {

LDR_DATA_TABLE_ENTRY* ntdllEntry = FindNtdllInLdrList (ldr);
ULONG_PTR ntdllStartAddress, ntdllEndAddress;
LDR_DATA_TABLE_ENTRY *current;
LIST_ENTRY *currentListEntry;

    if (!ntdllEntry) {
        return NULL;
        }

    ntdllStartAddress = (ULONG_PTR)ntdllEntry->DllBase;
    ntdllEndAddress = ntdllStartAddress + ntdllEntry->SizeOfImage - 1;

    currentListEntry = ntdllEntry->HashLinks.Blink;
    do{
        ULONG_PTR probeAddress = (ULONG_PTR) currentListEntry;
        if (ntdllStartAddress <= probeAddress && probeAddress <= ntdllEndAddress) {
            LIST_ENTRY *hashTable;
            ULONG ntdllHashTableIndex = GetHashTableIndexByDllName (NTDLL_NAME);
            if (ntdllHashTableIndex == -1) {
                return NULL;
                }
            hashTable = currentListEntry - ntdllHashTableIndex;
            printf ("[debug] LdrpHashTable: %p, ntdll index: %d\n", hashTable, ntdllHashTableIndex);
            return hashTable;
            }

        current = CONTAINING_RECORD(currentListEntry, LDR_DATA_TABLE_ENTRY, HashLinks);

        currentListEntry = currentListEntry->Flink;

    } while (currentListEntry != &ntdllEntry->HashLinks);

    return NULL;
}



void PrintModulesInLdrHashTable (PEB_LDR_DATA *ldr) {

LIST_ENTRY *hashTable = FindLdrHashTable (ldr);
unsigned int i;
    
    if (!hashTable) {
        return;
        }

    for (i = 0; i < LdrHashTableSize; ++i) {
        LIST_ENTRY *currentHead =  &hashTable[i];

        if (!IsListEmpty (currentHead)) {
            LDR_DATA_TABLE_ENTRY *current;
            LIST_ENTRY *currentListEntry = currentHead->Flink;
            printf ("%0p LdrpHashTable[%d]:\n", currentHead, i);
            do {

                current = CONTAINING_RECORD(currentListEntry, LDR_DATA_TABLE_ENTRY, HashLinks);

                wprintf (L"\t%0p %wZ\t", current, &current->BaseDllName);
                printf ("%016p - %p", current->DllBase, (ULONG_PTR)current->DllBase + current->SizeOfImage);
#ifdef WUI_OS_WIN10
                printf (" %08X", current->BaseNameHashValue);
#endif // WUI_OS_WIN10
                printf ("\n");

                currentListEntry = currentListEntry->Flink;

                } while (currentListEntry != currentHead);
            }
        }

    printf ("%p\n", hashTable);

    return;
}



void PrintModulesInLdrLists (PEB_LDR_DATA *ldr) {

LDR_DATA_TABLE_ENTRY *current;
LIST_ENTRY *currentListEntry;
    printf ("\nList in load order\n");
    current = (LDR_DATA_TABLE_ENTRY*) ldr->InLoadOrderModuleList.Flink;
    currentListEntry = ldr->InLoadOrderModuleList.Flink;
    do{

        current = CONTAINING_RECORD(currentListEntry, LDR_DATA_TABLE_ENTRY, InLoadOrderLinks);

        wprintf (L"%0p %s\t", current, current->BaseDllName.Buffer);
        printf ("%0p - %p\n", current->DllBase, (ULONG_PTR)current->DllBase + current->SizeOfImage);

        currentListEntry = currentListEntry->Flink;

    } while (currentListEntry != &ldr->InLoadOrderModuleList);


    printf ("\nList in memory order\n");
    currentListEntry = ldr->InMemoryOrderModuleList.Flink;
    do{

        current = CONTAINING_RECORD(currentListEntry, LDR_DATA_TABLE_ENTRY, InMemoryOrderLinks);

        wprintf (L"%0p %s\t", current, current->BaseDllName.Buffer);
        printf ("%0p - %p\n", current->DllBase, (ULONG_PTR)current->DllBase + current->SizeOfImage);

        currentListEntry = currentListEntry->Flink;

    } while (currentListEntry != &ldr->InMemoryOrderModuleList);


    printf ("\nList in initialization order\n");
    currentListEntry = ldr->InInitializationOrderModuleList.Flink;
    do{
        current = CONTAINING_RECORD(currentListEntry, LDR_DATA_TABLE_ENTRY, InInitializationOrderLinks);

        wprintf (L"%0p %s\t", current, current->BaseDllName.Buffer);
        printf ("%0p - %p\n", current->DllBase, (ULONG_PTR)current->DllBase + current->SizeOfImage);

        currentListEntry = currentListEntry->Flink;

    } while (currentListEntry != &ldr->InInitializationOrderModuleList);


    PrintModulesInLdrHashTable (ldr);

#ifdef WUI_OS_WIN10
    PrintLdrpModuleBaseAddressIndex();

    PrintLdrpMappingInfoIndex();
#endif // WUI_OS_WIN10

    return;
}



BOOL RemoveModuleFromLdrLists (PEB_LDR_DATA *ldr, WCHAR *moduleName) {

LDR_DATA_TABLE_ENTRY* moduleEntry = FindModuleInLdrListByName (ldr, moduleName);

    if (!moduleEntry) {
        return FALSE;
        }

    RemoveEntryList (&moduleEntry->InLoadOrderLinks);
    RemoveEntryList (&moduleEntry->InMemoryOrderLinks);
    //RemoveEntryList (&moduleEntry->InInitializationOrderLinks);
    RemoveEntryList (&moduleEntry->HashLinks);

#ifdef WUI_OS_WIN10
    RemoveEntryList (&moduleEntry->DdagNode->Modules);

    RemoveFromBalancedTree (&moduleEntry->BaseAddressIndexNode);
    RemoveFromBalancedTree (&moduleEntry->MappingInfoIndexNode);
#endif // WUI_OS_WIN10

    return TRUE;
}



BOOL RemoveModuleFromCurrentLdrLists (WCHAR *moduleName) {

    return RemoveModuleFromLdrLists (GetCurrentLdr(), moduleName);
}



BOOL RenameModuleInLdrLists (PEB_LDR_DATA *ldr, WCHAR *existName, WCHAR *newName) {

LDR_DATA_TABLE_ENTRY* moduleEntry;
LIST_ENTRY *hashTable;
ULONG moduleHashTableIndex;

    moduleEntry = FindModuleInLdrListByName (ldr, existName);
    if (!moduleEntry) {
        return FALSE;
        }

    hashTable = FindLdrHashTable (ldr);
    if (!hashTable) {
        return FALSE;
        }

    moduleHashTableIndex = GetHashTableIndexByDllName (newName);
    if (moduleHashTableIndex == -1) {
        return FALSE;
        }

    RemoveEntryList (&moduleEntry->HashLinks);

    RtlInitUnicodeString (&moduleEntry->BaseDllName, newName);
#ifdef WUI_OS_WIN10
    {
    moduleEntry->BaseNameHashValue = LdrHashUnicodeString (newName, (ULONG)wcslen(newName));
    }
#endif // WUI_OS_WIN10

    InsertTailList (hashTable + moduleHashTableIndex, &moduleEntry->HashLinks);

    return TRUE;
}



BOOL RenameModuleInCurrentLdrLists (WCHAR *existName, WCHAR *newName) {

    return RenameModuleInLdrLists (GetCurrentLdr(), existName, newName);
}


#ifdef WUI_OS_WIN10
PVOID GetRoodLdrpModuleBaseAddressIndex (void) {

LDR_DATA_TABLE_ENTRY *ntdllEntry = FindNtdllInLdrList (GetCurrentLdr());
PRTL_BALANCED_NODE node;

    if (!ntdllEntry) {
        return NULL;
        }

    node = &ntdllEntry->BaseAddressIndexNode;

    while (node->ParentValue & ~7) {
        node = (PRTL_BALANCED_NODE)(node->ParentValue & ~7);
        }

    return node;
}

#define GetPointerFromParentValue(parentValue)  ((PRTL_BALANCED_NODE)((parentValue) & ~7))

PVOID GetRoodLdrpMappingInfoIndex (void) {

LDR_DATA_TABLE_ENTRY *ntdllEntry;
PRTL_BALANCED_NODE node;

    ntdllEntry = FindNtdllInLdrList (GetCurrentLdr());

    if (!ntdllEntry) {
        return NULL;
        }

    node = &ntdllEntry->MappingInfoIndexNode;

    while (node->ParentValue & ~7) {
        node = (PRTL_BALANCED_NODE)(node->ParentValue & ~7);
        }

    return node;
}



unsigned int GetBalancedSubtreeNodesCount (PRTL_BALANCED_NODE node) {

    if (!node) {
        return 0;
        }

    return 1 + GetBalancedSubtreeNodesCount (node->Left) + GetBalancedSubtreeNodesCount (node->Right);
}



PRTL_BALANCED_NODE GetBalancedSubtreeLeftmostNode (PRTL_BALANCED_NODE root) {

PRTL_BALANCED_NODE node;

    if (!root) {
        return NULL;
        }

    node = root->Left;

    while (node->Left) {
        node = node->Left;
        }

    return node;
}




BOOL RemoveFromBalancedTree (PRTL_BALANCED_NODE root) {

PRTL_BALANCED_NODE parent = GetPointerFromParentValue(root->ParentValue);
PRTL_BALANCED_NODE *changedLink;
PRTL_BALANCED_NODE movedEntry;

    if (!parent) {
        return FALSE;
        }

    if (parent->Left == root) {
        changedLink = &parent->Left;
        }

    if (parent->Right == root) {
        changedLink = &parent->Right;
        }

    if (!root->Right) {
        PRTL_BALANCED_NODE movedEntry = root->Left;
        *changedLink = movedEntry;
        if (movedEntry) {
            movedEntry->ParentValue = root->ParentValue;
            }
        return TRUE;
        }
    
    movedEntry = GetBalancedSubtreeLeftmostNode (root->Right->Left);
    if (movedEntry) {
        PRTL_BALANCED_NODE movedEntryParent = (PRTL_BALANCED_NODE)movedEntry->ParentValue;
        
        movedEntryParent->Left = movedEntry->Right;
        if (movedEntry->Right) {
            movedEntry->Right->ParentValue = movedEntry->ParentValue;
            }
        movedEntry->Right = root->Right;
        root->Right->ParentValue = (ULONG_PTR)movedEntry;
        }
    else {
        movedEntry = root->Right;
        }

    movedEntry->Left = root->Left;
    if (root->Left) {
        root->Left->ParentValue = (ULONG_PTR)movedEntry;
        }

    *changedLink = movedEntry;
    movedEntry->ParentValue = root->ParentValue;
        

    return TRUE;
}



#define PRINT_TAB(tabCount) {unsigned int i; for (i = 0; i < tabCount; ++i) putchar('\t');}

void PrintFormattedLdrpModuleBaseAddressIndexNode (PRTL_BALANCED_NODE node, ULONG tabCount) {

LDR_DATA_TABLE_ENTRY *moduleEntry;

    if (node->Right) {
        PrintFormattedLdrpModuleBaseAddressIndexNode (node->Right, tabCount + 1);
        }

    moduleEntry = CONTAINING_RECORD(node,LDR_DATA_TABLE_ENTRY,BaseAddressIndexNode);

    PRINT_TAB(tabCount);
    printf ("%p (%p) %wZ\n", moduleEntry->OriginalBase, moduleEntry->DllBase, &moduleEntry->BaseDllName);

    if (node->Left) {
        PrintFormattedLdrpModuleBaseAddressIndexNode (node->Left, tabCount + 1);
        }

}



void PrintLdrpModuleBaseAddressIndexNode (PRTL_BALANCED_NODE node) {

LDR_DATA_TABLE_ENTRY *moduleEntry;

    if (node->Left) {
        PrintLdrpModuleBaseAddressIndexNode (node->Left);
        }

    if (node->Right) {
        PrintLdrpModuleBaseAddressIndexNode (node->Right);
        }

    moduleEntry = CONTAINING_RECORD(node,LDR_DATA_TABLE_ENTRY,BaseAddressIndexNode);

    printf ("%0p <-\t", node->Left);
    printf ("%0p %wZ\t", &moduleEntry->BaseAddressIndexNode, &moduleEntry->BaseDllName);
    printf ("%p", moduleEntry->DllBase);
    printf ("\t-> %0p\n", node->Right);
    
}



void PrintLdrpModuleBaseAddressIndex (void) {

    PRTL_BALANCED_NODE node;

    node = (PRTL_BALANCED_NODE)GetRoodLdrpModuleBaseAddressIndex();

    printf ("\n********** LdrpModuleBaseAddressIndex **********\n");

    if (node) {
        //PrintLdrpModuleBaseAddressIndexNode (node);
        PrintFormattedLdrpModuleBaseAddressIndexNode (node, 0);
        }

    return;
}



void PrintFormattedLdrpMappingInfoIndexNode (PRTL_BALANCED_NODE node, ULONG tabCount) {

LDR_DATA_TABLE_ENTRY *moduleEntry;

    if (node->Right) {
        PrintFormattedLdrpMappingInfoIndexNode (node->Right, tabCount + 1);
        }

    moduleEntry = CONTAINING_RECORD(node,LDR_DATA_TABLE_ENTRY,MappingInfoIndexNode);

    PRINT_TAB(tabCount);
    printf ("%0p %wZ\n", moduleEntry->DllBase, &moduleEntry->BaseDllName);

    if (node->Left) {
        PrintFormattedLdrpMappingInfoIndexNode (node->Left, tabCount + 1);
        }

}


void PrintLdrpMappingInfoIndexNode (PRTL_BALANCED_NODE node) {

LDR_DATA_TABLE_ENTRY *moduleEntry;

    if (node->Left) {
        PrintLdrpMappingInfoIndexNode (node->Left);
        }

    if (node->Right) {
        PrintLdrpMappingInfoIndexNode (node->Right);
        }

    moduleEntry = CONTAINING_RECORD(node,LDR_DATA_TABLE_ENTRY,MappingInfoIndexNode);

    printf ("%0p <-\t", node->Left);
    printf ("%0p %wZ\t", &moduleEntry->MappingInfoIndexNode, &moduleEntry->BaseDllName);
    printf ("%p", moduleEntry->DllBase);
    printf ("\t-> %0p\n", node->Right);
    
}



void PrintLdrpMappingInfoIndex (void) {

    PRTL_BALANCED_NODE node;

    node = (PRTL_BALANCED_NODE)GetRoodLdrpMappingInfoIndex();

    printf ("\n********** MappingInfoIndex **********\n");

    if (node) {
        //PrintLdrpMappingInfoIndexNode (node);
        PrintFormattedLdrpMappingInfoIndexNode (node, 0);
        }

    return;
}


#endif // WUI_OS_WIN10
