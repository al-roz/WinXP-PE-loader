
#ifndef _LDR_H_
#define _LDR_H_


#include "win_um_internal.h"


ULONG DefaultCaseInSensitiveHashUnicodeString (WCHAR *str, ULONG size);

ULONG LdrHashUnicodeStringWinXP (WCHAR *str, ULONG size);
ULONG LdrHashUnicodeStringWin7 (WCHAR *str, ULONG size);
ULONG LdrHashUnicodeStringWin10 (WCHAR *str, ULONG size);

#ifdef WUI_OS_WINXP
#define LdrHashUnicodeString    LdrHashUnicodeStringWinXP
#endif // WUI_OS_WINXP

#ifdef WUI_OS_WIN7
#define LdrHashUnicodeString    LdrHashUnicodeStringWin7
#endif // WUI_OS_WIN7

#ifdef WUI_OS_WIN10
#define LdrHashUnicodeString    LdrHashUnicodeStringWin10
#endif // WUI_OS_WIN10

ULONG GetHashTableIndexByDllNameUS (PUNICODE_STRING str);

ULONG GetHashTableIndexByDllName (PWCHAR str);

LDR_DATA_TABLE_ENTRY* FindModuleInLdrListByName (PEB_LDR_DATA *ldr, WCHAR *moduleName);
LDR_DATA_TABLE_ENTRY* FindModuleInCurrentLdrListByName (WCHAR *moduleName);

LDR_DATA_TABLE_ENTRY* FindNtdllInLdrList (PEB_LDR_DATA *ldr);

LIST_ENTRY* FindLdrHashTable (PEB_LDR_DATA *ldr);

void PrintModulesInLdrHashTable (PEB_LDR_DATA *ldr);
void PrintModulesInLdrLists (PEB_LDR_DATA *ldr);

BOOL RemoveModuleFromLdrLists (PEB_LDR_DATA *ldr, WCHAR *moduleName);
BOOL RemoveModuleFromCurrentLdrLists (WCHAR *moduleName);

BOOL RenameModuleInLdrLists (PEB_LDR_DATA *ldr, WCHAR *existName, WCHAR *newName);
BOOL RenameModuleInCurrentLdrLists (WCHAR *existName, WCHAR *newName);


#ifdef WUI_OS_WIN10
PVOID GetRoodLdrpModuleBaseAddressIndex (void);
PVOID GetRoodLdrpMappingInfoIndex (void);

void PrintLdrpModuleBaseAddressIndex (void);
void PrintLdrpMappingInfoIndex (void);

BOOL RemoveFromBalancedTree (PRTL_BALANCED_NODE root);
#endif // WUI_OS_WIN10



#endif  // _LDR_H_
