/*
Функции обработки PEB.

Маткин Илья Александрович   01.06.2020
*/


#include <windows.h>
#include <stdio.h>
#include <intrin.h>


#include "win_um_internal.h"
#include "peb.h"
#include "teb.h"
#include "ldr.h"



#define PEB_FIELD_OFFSET(field) FIELD_OFFSET(PEB,field)




#define PRINT_PEB_FIELD_OFFSET(field,offset_field,prefix,spec) \
    printf ("+0x%04x ", PEB_FIELD_OFFSET(offset_field));\
    printf (#field ": "spec"\n", prefix peb->field);
#define PRINT_PEB_INT_FIELD_OFFSET(field,offset_field)  PRINT_PEB_FIELD_OFFSET(field,offset_field,,"%d")
#define PRINT_PEB_FIELD(field,spec) PRINT_PEB_FIELD_OFFSET(field,field,,spec)
#define PRINT_PEB_POINTER_FIELD(field)  PRINT_PEB_FIELD(field,"%0p")
#define PRINT_PEB_INT_FIELD(field)  PRINT_PEB_FIELD(field,"%d")
#define PRINT_PEB_INT_FIELD_HEX(field)  PRINT_PEB_FIELD(field,"%08x")
#define PRINT_PEB_PUS_FIELD_HEX(field)  PRINT_PEB_FIELD(field,"%wZ")
#define PRINT_PEB_US_FIELD(field) PRINT_PEB_FIELD_OFFSET(field,field,&,"%wZ")

void PrintPEBStructure (PEB *peb) {

unsigned int i;

    printf ("%0p:\n", peb);

    PRINT_PEB_INT_FIELD(InheritedAddressSpace)
    PRINT_PEB_INT_FIELD(ReadImageFileExecOptions)
    printf ("BeingDebugged: %d\n", peb->BeingDebugged);

#ifdef WUI_OS_WIN10
    printf ("ImageUsesLargePages: %d\n", peb->ImageUsesLargePages);
    printf ("IsProtectedProcess: %d\n", peb->IsProtectedProcess);
    printf ("IsImageDynamicallyRelocated: %d\n", peb->IsImageDynamicallyRelocated);
    printf ("SkipPatchingUser32Forwarders: %d\n", peb->SkipPatchingUser32Forwarders);
    printf ("IsPackagedProcess: %d\n", peb->IsPackagedProcess);
    printf ("IsAppContainer: %d\n", peb->IsAppContainer);
    printf ("IsProtectedProcessLight: %d\n", peb->IsProtectedProcessLight);
    printf ("IsLongPathAwareProcess: %d\n", peb->IsLongPathAwareProcess);
#endif // WUI_OS_WIN10

    PRINT_PEB_POINTER_FIELD(ImageBaseAddress)
    PRINT_PEB_POINTER_FIELD(ProcessParameters)
    printf ("\tCurrentDirectoryPath: %wZ\n", &peb->ProcessParameters->CurrentDirectoryPath);
    printf ("\tCurrentDirectoryHandle: %p\n", peb->ProcessParameters->CurrentDirectoryHandle);
    printf ("\tImagePathName: %wZ\n", &peb->ProcessParameters->ImagePathName);
    printf ("\tDllPath: %wZ\n", &peb->ProcessParameters->DllPath);
    printf ("\tImagePathName: %wZ\n", &peb->ProcessParameters->ImagePathName);
    printf ("\tCommandLine: %wZ\n", &peb->ProcessParameters->CommandLine);

    printf ("\tEnvironment: %S\n", peb->ProcessParameters->Environment);
    printf ("\tStartingX: %d\n", peb->ProcessParameters->StartingX);
    printf ("\tStartingY: %d\n", peb->ProcessParameters->StartingY);
    printf ("\tCountX: %d\n", peb->ProcessParameters->CountX);
    printf ("\tCountY: %d\n", peb->ProcessParameters->CountY);
    printf ("\tCharWidth: %d\n", peb->ProcessParameters->CharWidth);
    printf ("\tCharHeight: %d\n", peb->ProcessParameters->CharHeight);

    printf ("\tWindowTitle: %wZ\n", &peb->ProcessParameters->WindowTitle);
    printf ("\tDesktopInfo: %wZ\n", &peb->ProcessParameters->DesktopInfo);
    printf ("\tShellInfo: %wZ\n", &peb->ProcessParameters->ShellInfo);
    printf ("\tRuntimeData: %wZ\n", &peb->ProcessParameters->RuntimeData);

#if defined(WUI_OS_WIN7) || defined(WUI_OS_WIN10)
    PRINT_PEB_INT_FIELD_OFFSET(ProcessInJob,CrossProcessFlags)
    PRINT_PEB_INT_FIELD_OFFSET(ProcessInitializing,CrossProcessFlags)
    PRINT_PEB_INT_FIELD_OFFSET(ProcessUsingVEH,CrossProcessFlags)
    PRINT_PEB_INT_FIELD_OFFSET(ProcessUsingVCH,CrossProcessFlags)
    PRINT_PEB_INT_FIELD_OFFSET(ProcessUsingFTH,CrossProcessFlags)

#ifdef WUI_OS_WIN10
    printf ("ProcessPreviouslyThrottled: %d\n", peb->ProcessPreviouslyThrottled);
    printf ("ProcessCurrentlyThrottled: %d\n", peb->ProcessCurrentlyThrottled);
    printf ("ProcessImagesHotPatched: %d\n", peb->ProcessImagesHotPatched);
#endif // WUI_OS_WIN10
#endif // WUI_OS_WIN7 || WUI_OS_WIN10
    
    for (i = 0; i < 0x20; ++i) {
        if (peb->ProcessParameters->CurrentDirectores[i].DosPath.Length) {
            printf ("CurrentDirectores[%d]: %wZ\n", i, &peb->ProcessParameters->CurrentDirectores[i].DosPath);
            }
        }

#ifdef WUI_OS_WIN10
    printf ("RedirectionDllName: %wZ\n", &peb->ProcessParameters->RedirectionDllName);
    printf ("HeapPartitionName: %wZ\n", &peb->ProcessParameters->HeapPartitionName);
#endif // WUI_OS_WIN10

    PRINT_PEB_POINTER_FIELD(ProcessHeap)
    PRINT_PEB_INT_FIELD(NumberOfProcessors)
    PRINT_PEB_INT_FIELD_HEX(NtGlobalFlag)
    PRINT_PEB_INT_FIELD(NumberOfHeaps)
    PRINT_PEB_INT_FIELD(MaximumNumberOfHeaps)
    if (peb->ProcessHeaps) {
        PRINT_PEB_POINTER_FIELD(ProcessHeaps)
        for (i = 0; i < peb->NumberOfHeaps; ++i) {
            printf ("\theap %d: %p\n", i, peb->ProcessHeaps[i]);
            }
        }

    PRINT_PEB_POINTER_FIELD(GdiSharedHandleTable)

    PRINT_PEB_INT_FIELD(OSMajorVersion)
    PRINT_PEB_INT_FIELD(OSMinorVersion)
    PRINT_PEB_INT_FIELD(OSBuildNumber)
    PRINT_PEB_INT_FIELD(OSCSDVersion)
    PRINT_PEB_INT_FIELD(OSPlatformId)
    PRINT_PEB_INT_FIELD(ImageSubSystem)
    PRINT_PEB_INT_FIELD(ImageSubSystemMajorVersion)
    PRINT_PEB_INT_FIELD(ImageSubSystemMinorVersion)

#ifdef WUI_OS_WINXP
    PRINT_PEB_POINTER_FIELD(ImageProcessAffinityMask)
#else   // WUI_OS_WINXP
    PRINT_PEB_POINTER_FIELD(ActiveProcessAffinityMask)
#endif  // WUI_OS_WINXP

    PRINT_PEB_INT_FIELD(SessionId)
    PRINT_PEB_POINTER_FIELD(pShimData)
    PRINT_PEB_POINTER_FIELD(AppCompatInfo)
    PRINT_PEB_US_FIELD(CSDVersion)

    PRINT_PEB_POINTER_FIELD(ActivationContextData)
    PRINT_PEB_POINTER_FIELD(ProcessAssemblyStorageMap)
    PRINT_PEB_POINTER_FIELD(SystemDefaultActivationContextData)
    PRINT_PEB_POINTER_FIELD(SystemAssemblyStorageMap)
    PRINT_PEB_POINTER_FIELD(MinimumStackCommit)

#ifdef WUI_OS_WIN10
    PRINT_PEB_POINTER_FIELD(NtGlobalFlag2)
#endif // WUI_OS_WIN10
    

    //PrintModulesInLdrLists (peb->ldr);

    return;
}



void PrintCurrentPEB (void) {

    PrintPEBStructure (GetCurrentPEB());

    return;
}



void PrintModulesInPEBStructure (PEB *peb) {

    PrintModulesInLdrLists (peb->Ldr);

    return;
}



void PrintModulesInCurrentPEB (void) {

    PrintModulesInPEBStructure (GetCurrentPEB());

    return;
}

