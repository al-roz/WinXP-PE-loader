

#include <windows.h>
#include <stdio.h>
#include <intrin.h>


#include "win_um_internal.h"
#include "teb.h"



#define TEB_FIELD_OFFSET(field) FIELD_OFFSET(TEB,field)



#if defined(WUI_X86)
TEB *GetCurrentTEB32 (void) {

    return (TEB*)__readfsdword (FIELD_OFFSET (NT_TIB, Self));
}
#endif


#if defined(WUI_X64)
TEB *GetCurrentTEB64 (void) {

    return (TEB*)__readgsqword (FIELD_OFFSET (NT_TIB, Self));
}
#endif



PEB *GetCurrentPEB(){

    return GetCurrentTEB()->ProcessEnvironmentBlock;
}

#define PRINT_TEB_FIELD_OFFSET(field,offset_field,prefix,spec) \
    printf ("+0x%04x ", TEB_FIELD_OFFSET(offset_field));\
    printf (#field ": "spec"\n", prefix teb->field);
#define PRINT_TEB_INT_FIELD_OFFSET(field,offset_field)  PRINT_TEB_FIELD_OFFSET(field,offset_field,,"%d")
#define PRINT_TEB_FIELD(field,spec) PRINT_TEB_FIELD_OFFSET(field,field,,spec)
#define PRINT_TEB_POINTER_FIELD(field)  PRINT_TEB_FIELD(field,"%0p")
#define PRINT_TEB_INT_FIELD(field)  PRINT_TEB_FIELD(field,"%d")
#define PRINT_TEB_INT_FIELD_HEX(field)  PRINT_TEB_FIELD(field,"%08x")
#define PRINT_TEB_PUS_FIELD_HEX(field)  PRINT_TEB_FIELD(field,"%wZ")
#define PRINT_TEB_US_FIELD(field) PRINT_TEB_FIELD_OFFSET(field,field,&,"%wZ")

void PrintTEBStructure (TEB *teb) {

    printf ("%0p:\n", teb);

    printf ("\tExceptionList: %0p\n", teb->NtTib.ExceptionList);
    printf ("\tStackBase: %0p\n", teb->NtTib.StackBase);
    printf ("\tStackLimit: %0p\n", teb->NtTib.StackLimit);
    printf ("\tSubSystemTib: %0p\n", teb->NtTib.SubSystemTib);

    printf ("+0x%04x ", TEB_FIELD_OFFSET(ClientId));
    printf ("pid: %d\n", teb->ClientId.UniqueProcess);
    printf ("+0x%04x ", TEB_FIELD_OFFSET(ClientId)+sizeof(HANDLE));
    printf ("tid: %d\n", teb->ClientId.UniqueThread);

    PRINT_TEB_POINTER_FIELD(ProcessEnvironmentBlock)
    PRINT_TEB_INT_FIELD_HEX(LastErrorValue)
    PRINT_TEB_POINTER_FIELD(Win32ThreadInfo)
    PRINT_TEB_INT_FIELD(CurrentLocale)
    PRINT_TEB_INT_FIELD_HEX(ExceptionCode)
#ifdef WUI_OS_WIN10
    PRINT_TEB_POINTER_FIELD(ActivationContextStackPointer)
#endif // WUI_OS_WIN10
    
    printf ("+0x%04x ", TEB_FIELD_OFFSET(RealClientId));
    printf ("real pid: %d\n", teb->RealClientId.UniqueProcess);
    printf ("+0x%04x ", TEB_FIELD_OFFSET(RealClientId)+sizeof(HANDLE));
    printf ("real tid: %d\n", teb->RealClientId.UniqueThread);

    PRINT_TEB_INT_FIELD(GdiClientPID)
    PRINT_TEB_INT_FIELD(GdiClientTID)

    PRINT_TEB_POINTER_FIELD(GlSectionInfo)
    PRINT_TEB_POINTER_FIELD(GlSection)
    PRINT_TEB_POINTER_FIELD(GlTable)
    PRINT_TEB_POINTER_FIELD(GlCurrentRC)
    PRINT_TEB_POINTER_FIELD(GlContext)

    PRINT_TEB_INT_FIELD_HEX(LastStatusValue)

    PRINT_TEB_INT_FIELD(IdealProcessor)
    PRINT_TEB_INT_FIELD(IsImpersonating)

    PRINT_TEB_POINTER_FIELD(NlsCache)
    PRINT_TEB_POINTER_FIELD(pShimData)
    //PRINT_TEB_INT_FIELD(HeapVirtualAffinity)

    PRINT_TEB_POINTER_FIELD(ActiveFrame)

#if defined(WUI_OS_WINXP)
    PRINT_TEB_INT_FIELD(SafeThunkCall)
#endif  // WUI_OS_WIXP
    
#if defined(WUI_OS_WIN7) || defined(WUI_OS_WIN10)
    PRINT_TEB_INT_FIELD(IdealProcessorValue)

    PRINT_TEB_INT_FIELD_OFFSET(SafeThunkCall,SameTebFlags)
    PRINT_TEB_INT_FIELD_OFFSET(InDebugPrint,SameTebFlags)
    PRINT_TEB_INT_FIELD_OFFSET(HasFiberData,SameTebFlags)
    PRINT_TEB_INT_FIELD_OFFSET(SkipThreadAttach,SameTebFlags)
    PRINT_TEB_INT_FIELD_OFFSET(WerInShipAssertCode,SameTebFlags)
    PRINT_TEB_INT_FIELD_OFFSET(WerInShipAssertCode,SameTebFlags)
    PRINT_TEB_INT_FIELD_OFFSET(SuppressDebugMsg,SameTebFlags)
    PRINT_TEB_INT_FIELD_OFFSET(DisableUserStackWalk,SameTebFlags)
    PRINT_TEB_INT_FIELD_OFFSET(RtlExceptionAttached,SameTebFlags)
    PRINT_TEB_INT_FIELD_OFFSET(InitialThread,SameTebFlags)

    PRINT_TEB_POINTER_FIELD(ResourceRetValue)
#endif // WUI_OS_WIN7 || WUI_OS_WIN10

#if defined(WUI_OS_WIN10)

    PRINT_TEB_INT_FIELD_OFFSET(SessionAware,SameTebFlags)
    PRINT_TEB_INT_FIELD_OFFSET(LoadOwner,SameTebFlags)
    PRINT_TEB_INT_FIELD_OFFSET(LoaderWorker,SameTebFlags)
    PRINT_TEB_INT_FIELD_OFFSET(SkipLoaderInit,SameTebFlags)

    printf ("+0x%04x ", FIELD_OFFSET(TEB,EffectiveContainerId));
    printf ("EffectiveContainerId: %X-%X-%X-%X%X%X%X%X%X%X%X\n", teb->EffectiveContainerId.Data1,
        teb->EffectiveContainerId.Data2, teb->EffectiveContainerId.Data3,
        teb->EffectiveContainerId.Data4[0],teb->EffectiveContainerId.Data4[1],
        teb->EffectiveContainerId.Data4[2],teb->EffectiveContainerId.Data4[3],
        teb->EffectiveContainerId.Data4[4],teb->EffectiveContainerId.Data4[5],
        teb->EffectiveContainerId.Data4[6],teb->EffectiveContainerId.Data4[7]);

    

#endif // WUI_OS_WIN10
    

    printf("\n");

    return;
}



void PrintCurrentTEB (void) {

    PrintTEBStructure (GetCurrentTEB());

    return;
}
