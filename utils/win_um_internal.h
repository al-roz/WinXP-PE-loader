/*

Заголовочный файл с внутренними структурами данных
пользовательского режима ОС Windows NT.

Структуры получены из отладочных символов под WinDbg 
и с использованием сайта https://ntdiff.github.io/.

Проверено на следующих системах
WinXP SP3 x86
Win7 SP1 x86(WoW64), x64
Win10 18362 x86(WoW64), x64

Маткин Илья Александрович   30.05.2020
*/

#ifndef _WIN_UM_INTERNAL_H_
#define _WIN_UM_INTERNAL_H_


#include <windows.h>

//#define WUI_NOT_DEFAULT
//#define WUI_X86         1
//#define WUI_OS_WIN10    1
//#define WUI_WIN10_VERSION   1

#ifndef WUI_NOT_DEFAULT
#define WUI_DEFAULT
#endif // WUI_NOT_DEFAULT

#ifdef WUI_DEFAULT
#define WUI_OS_WINXP   1
//#define WUI_OS_WIN7    1
//define WUI_OS_WIN10   1

#ifdef _WIN64
#define WUI_X64         1
#else   // _WIN64
#define WUI_X86         1
#endif  // _WIN64

//#define WUI_WIN10_VERSION   10240
//#define WUI_WIN10_VERSION   10586
//#define WUI_WIN10_VERSION   14393
//#define WUI_WIN10_VERSION   15063
//#define WUI_WIN10_VERSION   16299
//#define WUI_WIN10_VERSION   17134
//#define WUI_WIN10_VERSION   17763
//#define WUI_WIN10_VERSION   18362
#define WUI_WIN10_VERSION   19041
//#define WUI_WIN10_VERSION   19569

#endif // WUI_DEFAULT

#if defined(WUI_X86) && defined(WUI_X64)
#error "set only one arch"
#endif

#ifdef WUI_X86
#define IF_DEF_WUI_X86(exp) exp;
#define IF_DEF_WUI_X64(exp) ;
#endif // WUI_X86
#ifdef WUI_X64
#define IF_DEF_WUI_X86(exp) ;
#define IF_DEF_WUI_X64(exp) exp;
#endif // WUI_X64



#ifdef WUI_X86
#define WUI_ARCH_PSIZE  4
#else   // WUI_X86
#ifdef WUI_X64
#define WUI_ARCH_PSIZE  8
#else   // WUI_X64
#error "Don't set arch"
#endif  // WUI_X64
#endif  // WUI_X86


#if !defined(WUI_OS_WINXP) && !defined(WUI_OS_WIN7) && !defined(WUI_OS_WIN10)
#error "Don't set os version"
#endif

#if defined(WUI_OS_WIN10) && !defined(WUI_WIN10_VERSION)
#error "Don't set win10 version"
#endif

#if WUI_OS_WINXP + WUI_OS_WIN7 + WUI_OS_WIN10 > 1
#error "Too many os are installed"
#endif


typedef void (*PPEBLOCKROUTINE)(PVOID PebLock); 

//typedef void** PPVOID;

//#pragma pack (push, 1)
typedef struct _UNICODE_STRING {

    USHORT    Length;
    USHORT    MaximumLength;
#pragma pack (push, WUI_ARCH_PSIZE)
    PWSTR     Buffer;
#pragma pack (pop)

} UNICODE_STRING, *PUNICODE_STRING;
//#pragma pack (pop)


//#pragma pack (push, 1)
typedef struct _STRING {

    USHORT    Length;
    USHORT    MaximumLength;
    PCHAR     Buffer;

} STRING, *PSTRING;
//#pragma pack (pop)


//#pragma pack (push, 1)
typedef struct _RTL_DRIVE_LETTER_CURDIR {

    USHORT                  Flags;
    USHORT                  Length;
    ULONG                   TimeStamp;
    STRING                  DosPath;

} RTL_DRIVE_LETTER_CURDIR, *PRTL_DRIVE_LETTER_CURDIR;
//#pragma pack (pop)


//#pragma pack (push, 4)
typedef struct _PEB_LDR_DATA_WINXP {

    ULONG               Length;
    BOOLEAN             Initialized;
    PVOID               SsHandle;
    LIST_ENTRY          InLoadOrderModuleList;
    LIST_ENTRY          InMemoryOrderModuleList;
    LIST_ENTRY          InInitializationOrderModuleList;
    PVOID               EntryInProgress;

} PEB_LDR_DATA_WINXP, *PPEB_LDR_DATA_WINXP;
//#pragma pack (pop)


//#pragma pack (push, 4)
typedef struct _PEB_LDR_DATA_WIN7 {

    ULONG               Length;
    BOOLEAN             Initialized;
    PVOID               SsHandle;
    LIST_ENTRY          InLoadOrderModuleList;
    LIST_ENTRY          InMemoryOrderModuleList;
    LIST_ENTRY          InInitializationOrderModuleList;
    PVOID               EntryInProgress;
// WIN7 new
    ULONG_PTR           ShutdownInProgress;
    PVOID               ShutdownThreadId;

} PEB_LDR_DATA_WIN7, *PPEB_LDR_DATA_WIN7;
//#pragma pack (pop)


//typedef struct _PEB_LDR_DATA_WIN7 _PEB_LDR_DATA_WIN10;
typedef PEB_LDR_DATA_WIN7 PEB_LDR_DATA_WIN10;
typedef PPEB_LDR_DATA_WIN7 PPEB_LDR_DATA_WIN10;


//#pragma pack (push, 1)
typedef struct _LDR_DATA_TABLE_ENTRY_WINXP {
    LIST_ENTRY      InLoadOrderLinks;
    LIST_ENTRY      InMemoryOrderLinks;
    LIST_ENTRY      InInitializationOrderLinks;
    PVOID           DllBase;
    PVOID           EntryPoint;
    ULONG           SizeOfImage;
    UNICODE_STRING  FullDllName;
    UNICODE_STRING  BaseDllName;
    ULONG           Flags;
    USHORT          LoadCount;
    USHORT          TlsIndex;
    union {
        LIST_ENTRY  HashLinks;
        struct {
            PVOID   SectionPointer;
            ULONG   CheckSum;
        };
    };
    union {
        struct {
            ULONG   TimeDateStamp;
        };
        struct {
            PVOID   LoadedImports;
        };
    };
    struct _ACTIVATION_CONTEXT * EntryPointActivationContext;
 
    PVOID           PatchInformation;
 
} LDR_DATA_TABLE_ENTRY_WINXP, *PLDR_DATA_TABLE_ENTRY_WINXP;
//#pragma pack (pop)


//#pragma pack (push, 1)
typedef struct _LDR_DATA_TABLE_ENTRY_WIN7 {
    LIST_ENTRY      InLoadOrderLinks;
    LIST_ENTRY      InMemoryOrderLinks;
    LIST_ENTRY      InInitializationOrderLinks;
    PVOID           DllBase;
    PVOID           EntryPoint;
    ULONG           SizeOfImage;
#pragma pack (push, WUI_ARCH_PSIZE)
    UNICODE_STRING  FullDllName;
#pragma pack (pop)
    UNICODE_STRING  BaseDllName;
    ULONG           Flags;
    USHORT          LoadCount;
    USHORT          TlsIndex;
    union {
        LIST_ENTRY  HashLinks;
        struct {
            PVOID   SectionPointer;
            ULONG   CheckSum;
        };
    };
    union {
        struct {
            ULONG   TimeDateStamp;
        };
        struct {
            PVOID   LoadedImports;
        };
    };
    struct _ACTIVATION_CONTEXT * EntryPointActivationContext;
 
    PVOID           PatchInformation;

    LIST_ENTRY      ForwarderLinks;
    LIST_ENTRY      ServiceTagLinks;
    LIST_ENTRY      StaticLinks;
    PVOID           ContextInformation;
    ULONG_PTR       OriginalBase;
    LARGE_INTEGER   LoadTime;
 
} LDR_DATA_TABLE_ENTRY_WIN7, *PLDR_DATA_TABLE_ENTRY_WIN7;
//#pragma pack (pop)


//#pragma pack (push, 1)
typedef struct _LDR_SERVICE_TAG_RECORD {
    struct _LDR_SERVICE_TAG_RECORD*     Next;
    ULONG                               ServiceTag;
} LDR_SERVICE_TAG_RECORD, *PLDR_SERVICE_TAG_RECORD; /* size: 0x0008 */
//#pragma pack (pop)


//#pragma pack (push, 1)
typedef struct _LDRP_CSLIST {
    PSINGLE_LIST_ENTRY* Tail;
} LDRP_CSLIST, *PLDRP_CSLIST;
//#pragma pack (pop)


typedef enum _LDR_DDAG_STATE {
    LdrModulesMerged = -5,
    LdrModulesInitError = -4,
    LdrModulesSnapError = -3,
    LdrModulesUnloaded = -2,
    LdrModulesUnloading = -1,
    LdrModulesPlaceHolder = 0,
    LdrModulesMapping = 1,
    LdrModulesMapped = 2,
    LdrModulesWaitingForDependencies = 3,
    LdrModulesSnapping = 4,
    LdrModulesSnapped = 5,
    LdrModulesCondensed = 6,
    LdrModulesReadyToInit = 7,
    LdrModulesInitializing = 8,
    LdrModulesReadyToRun = 9,
} LDR_DDAG_STATE, *PLDR_DDAG_STATE;


//#pragma pack (push, 4)
typedef struct _LDR_DDAG_NODE {
    LIST_ENTRY              Modules;
    PLDR_SERVICE_TAG_RECORD* ServiceTagList;
    ULONG                   LoadCount;
    ULONG                   LoadWhileUnloadingCount;
    ULONG                   LowestLink;
    PLDRP_CSLIST            Dependencies;
    PLDRP_CSLIST            IncomingDependencies;
    LDR_DDAG_STATE          State;
#pragma pack (push, WUI_ARCH_PSIZE)
    SINGLE_LIST_ENTRY       CondenseLink;   
#pragma pack (pop)
    ULONG                   PreorderNumber;
} LDR_DDAG_NODE, *PLDR_DDAG_NODE; /* size: 0x002c */
//#pragma pack (pop)


//#pragma pack (push, 1)
typedef struct _RTL_BALANCED_NODE {
    union{
        struct _RTL_BALANCED_NODE* Children[2];
        struct {
            struct _RTL_BALANCED_NODE* Left;
            struct _RTL_BALANCED_NODE* Right;
            };
        };
    union
    {
        UCHAR Red : 1;
        UCHAR Balance : 2;
        ULONG_PTR ParentValue;
    };
} RTL_BALANCED_NODE, *PRTL_BALANCED_NODE;
//#pragma pack (pop)


typedef enum _LDR_DLL_LOAD_REASON {

    LoadReasonStaticDependency = 0,
    LoadReasonStaticForwarderDependency = 1,
    LoadReasonDynamicForwarderDependency = 2,
    LoadReasonDelayloadDependency = 3,
    LoadReasonDynamicLoad = 4,
    LoadReasonAsImageLoad = 5,
    LoadReasonAsDataLoad = 6,
#if WUI_WIN10_VERSION >= 16299
    LoadReasonEnclavePrimary = 7,
    LoadReasonEnclaveDependency = 8,
#endif  // WUI_WIN10_VERSION >= 16299
    LoadReasonUnknown = -1,

} LDR_DLL_LOAD_REASON, *PLDR_DLL_LOAD_REASON;


//#pragma pack (push, 1)
typedef struct _LDR_DATA_TABLE_ENTRY_WIN10 {
    LIST_ENTRY      InLoadOrderLinks;
    LIST_ENTRY      InMemoryOrderLinks;
    LIST_ENTRY      InInitializationOrderLinks;
    PVOID           DllBase;
    PVOID           EntryPoint;
    ULONG           SizeOfImage;
#pragma pack (push, WUI_ARCH_PSIZE)
    UNICODE_STRING  FullDllName;
#pragma pack (pop)
    UNICODE_STRING  BaseDllName;
    union {
        UCHAR           FlagGroup[4];
        ULONG           Flags;
        struct {
            ULONG       PackagedBinary : 1;
            ULONG       MarkedForRemoval : 1;
            ULONG       ImageDll : 1;
            ULONG       LoadNotificationsSent : 1;
            ULONG       TelemetryEntryProcessed : 1;
            ULONG       ProcessStaticImport : 1;
            ULONG       InLegacyLists : 1;
            ULONG       InIndexes : 1;
            ULONG       ShimDll : 1;
            ULONG       InExceptionTable : 1;
            ULONG       ReservedFlags1 : 2;
            ULONG       LoadInProgress : 1;
            ULONG       LoadConfigProcessed : 1;
            ULONG       EntryProcessed : 1;
            ULONG       ProtectDelayLoad : 1;
            ULONG       ReservedFlags3 : 2;
            ULONG       DontCallForThreads : 1;
            ULONG       ProcessAttachCalled : 1;
            ULONG       ProcessAttachFailed : 1;
            ULONG       CorDeferredValidate : 1;
            ULONG       CorImage : 1;
            ULONG       DontRelocate : 1;
            ULONG       CorILOnly : 1;
#if WUI_WIN10_VERSION < 17134
            ULONG       ReservedFlags5 : 3;
#else   // WUI_WIN10_VERSION < 17134
            ULONG       ChpeImage : 1;
            ULONG       ReservedFlags5 : 2;
#endif  // WUI_WIN10_VERSION < 17134
            ULONG       Redirected : 1;
            ULONG       ReservedFlags6 : 2;
            ULONG       CompatDatabaseProcessed : 1;
            };
        };
    USHORT          ObsoleteLoadCount;
    USHORT          TlsIndex;
    LIST_ENTRY      HashLinks;
    ULONG           TimeDateStamp;
    struct _ACTIVATION_CONTEXT * EntryPointActivationContext;
    PVOID           Lock;
    PLDR_DDAG_NODE          DdagNode;
    LIST_ENTRY              NodeModuleLink;
    struct _LDRP_LOAD_CONTEXT* LoadContext;
    PVOID                   ParentDllBase;
    PVOID                   SwitchBackContext;
    RTL_BALANCED_NODE       BaseAddressIndexNode;
    RTL_BALANCED_NODE       MappingInfoIndexNode;
    ULONG_PTR               OriginalBase;
    LARGE_INTEGER           LoadTime;
    ULONG                   BaseNameHashValue;
    LDR_DLL_LOAD_REASON     LoadReason;
    ULONG                   ImplicitPathOptions;
    ULONG                   ReferenceCount;
#if WUI_WIN10_VERSION >= 14393
    ULONG                   DependentLoadFlags;
#if WUI_WIN10_VERSION < 15063
    CHAR                    __PADDING__[1];
#else   // WUI_WIN10_VERSION < 15063
    UCHAR                   SigningLevel;
    LONG                    __PADDING__[3];
#endif  // WUI_WIN10_VERSION < 15063
#endif  //  WUI_WIN10_VERSION >= 14393
} LDR_DATA_TABLE_ENTRY_WIN10, *PLDR_DATA_TABLE_ENTRY_WIN10;
//#pragma pack (pop)


typedef struct _PEB_FREE_BLOCK {

    struct _PEB_FREE_BLOCK    *Next;
    ULONG                     Size;

} PEB_FREE_BLOCK, *PPEB_FREE_BLOCK;


//#pragma pack (push, WUI_ARCH_PSIZE)
typedef struct _RTL_USER_PROCESS_PARAMETERS_WINXP {

    ULONG                   MaximumLength;
    ULONG                   Length;
    ULONG                   Flags;
    ULONG                   DebugFlags;
    PVOID                   ConsoleHandle;
    ULONG                   ConsoleFlags;
    HANDLE                  StdInputHandle;
    HANDLE                  StdOutputHandle;
    HANDLE                  StdErrorHandle;
    UNICODE_STRING          CurrentDirectoryPath;
    HANDLE                  CurrentDirectoryHandle;
    UNICODE_STRING          DllPath;
    UNICODE_STRING          ImagePathName;
    UNICODE_STRING          CommandLine;
    PVOID                   Environment;
    ULONG                   StartingX;
    ULONG                   StartingY;
    ULONG                   CountX;
    ULONG                   CountY;
    ULONG                   CharWidth;
    ULONG                   CharHeight;
    ULONG                   FillAttribute;
    ULONG                   WindowFlags;
    ULONG                   ShowWindowFlags;
    UNICODE_STRING          WindowTitle;
    UNICODE_STRING          DesktopInfo;
    UNICODE_STRING          ShellInfo;
    UNICODE_STRING          RuntimeData;
    RTL_DRIVE_LETTER_CURDIR CurrentDirectores[0x20];

} RTL_USER_PROCESS_PARAMETERS_WINXP, *PRTL_USER_PROCESS_PARAMETERS_WINXP;
//#pragma pack (pop)


//#pragma pack (push, WUI_ARCH_PSIZE)
typedef struct _RTL_USER_PROCESS_PARAMETERS_WIN7 {

    ULONG                   MaximumLength;
    ULONG                   Length;
    ULONG                   Flags;
    ULONG                   DebugFlags;
    PVOID                   ConsoleHandle;
    ULONG                   ConsoleFlags;
    HANDLE                  StdInputHandle;
    HANDLE                  StdOutputHandle;
    HANDLE                  StdErrorHandle;
    UNICODE_STRING          CurrentDirectoryPath;
    HANDLE                  CurrentDirectoryHandle;
    UNICODE_STRING          DllPath;
    UNICODE_STRING          ImagePathName;
    UNICODE_STRING          CommandLine;
    PVOID                   Environment;
    ULONG                   StartingX;
    ULONG                   StartingY;
    ULONG                   CountX;
    ULONG                   CountY;
    ULONG                   CharWidth;
    ULONG                   CharHeight;
    ULONG                   FillAttribute;
    ULONG                   WindowFlags;
    ULONG                   ShowWindowFlags;
    UNICODE_STRING          WindowTitle;
    UNICODE_STRING          DesktopInfo;
    UNICODE_STRING          ShellInfo;
    UNICODE_STRING          RuntimeData;
    RTL_DRIVE_LETTER_CURDIR CurrentDirectores[0x20];

// WIN7 new
    volatile ULONG_PTR        EnvironmentSize;
    volatile ULONG_PTR        EnvironmentVersion;

} RTL_USER_PROCESS_PARAMETERS_WIN7, *PRTL_USER_PROCESS_PARAMETERS_WIN7;
//#pragma pack (pop)


//#pragma pack (push, WUI_ARCH_PSIZE)
typedef struct _RTL_USER_PROCESS_PARAMETERS_WIN10 {

    ULONG                   MaximumLength;
    ULONG                   Length;
    ULONG                   Flags;
    ULONG                   DebugFlags;
    PVOID                   ConsoleHandle;
    ULONG                   ConsoleFlags;
    HANDLE                  StdInputHandle;
    HANDLE                  StdOutputHandle;
    HANDLE                  StdErrorHandle;
    UNICODE_STRING          CurrentDirectoryPath;
    HANDLE                  CurrentDirectoryHandle;
    UNICODE_STRING          DllPath;
    UNICODE_STRING          ImagePathName;
    UNICODE_STRING          CommandLine;
    PVOID                   Environment;
    ULONG                   StartingX;
    ULONG                   StartingY;
    ULONG                   CountX;
    ULONG                   CountY;
    ULONG                   CharWidth;
    ULONG                   CharHeight;
    ULONG                   FillAttribute;
    ULONG                   WindowFlags;
    ULONG                   ShowWindowFlags;
    UNICODE_STRING          WindowTitle;
    UNICODE_STRING          DesktopInfo;
    UNICODE_STRING          ShellInfo;
    UNICODE_STRING          RuntimeData;
    RTL_DRIVE_LETTER_CURDIR CurrentDirectores[0x20];
// WIN7 new
    volatile ULONG_PTR      EnvironmentSize;
    volatile ULONG_PTR      EnvironmentVersion;
// WIN10 new
    PVOID                   PackageDependencyData;
    ULONG                   ProcessGroupId;
    ULONG                   LoaderThreads;
    UNICODE_STRING          RedirectionDllName;
    UNICODE_STRING          HeapPartitionName;
    PULONGLONG              DefaultThreadpoolCpuSetMasks;
    ULONG                   DefaultThreadpoolCpuSetMaskCount;
    ULONG                   DefaultThreadpoolThreadMaximum;

} RTL_USER_PROCESS_PARAMETERS_WIN10, *PRTL_USER_PROCESS_PARAMETERS_WIN10;
//#pragma pack (pop)


//#pragma pack (push, 1)
typedef struct _CLIENT_ID
{
    PVOID UniqueProcess;
    PVOID UniqueThread;
} CLIENT_ID, *PCLIENT_ID;
//#pragma pack (pop)


//#pragma pack (push, 1)
typedef struct _PEB_WINXP {

    BOOLEAN                 InheritedAddressSpace;
    BOOLEAN                 ReadImageFileExecOptions;
    BOOLEAN                 BeingDebugged;
    UCHAR                   SpareBool;
    HANDLE                  Mutant;
    PVOID                   ImageBaseAddress;
    PPEB_LDR_DATA_WINXP     Ldr;
    PRTL_USER_PROCESS_PARAMETERS_WINXP ProcessParameters;
    PVOID                   SubSystemData;
    PVOID                   ProcessHeap;
    PRTL_CRITICAL_SECTION   FastPebLock;
    PPEBLOCKROUTINE         FastPebLockRoutine;
    PPEBLOCKROUTINE         FastPebUnlockRoutine;
    ULONG                   EnvironmentUpdateCount;
    PVOID                   *KernelCallbackTable;
    PVOID                   SystemReserved;
    ULONG                   AtlThunkSListPtr32;
    PPEB_FREE_BLOCK         FreeList;
    ULONG                   TlsExpansionCounter;
    PVOID                   TlsBitmap;
    ULONG                   TlsBitmapBits[0x2];
    PVOID                   ReadOnlySharedMemoryBase;
    PVOID                   ReadOnlySharedMemoryHeap;
    PVOID                   *ReadOnlyStaticServerData;
    PVOID                   AnsiCodePageData;
    PVOID                   OemCodePageData;
    PVOID                   UnicodeCaseTableData;
    ULONG                   NumberOfProcessors;
    ULONG                   NtGlobalFlag;
    //BYTE                    Spare2[0x4];
    LARGE_INTEGER           CriticalSectionTimeout;
    ULONG_PTR               HeapSegmentReserve;
    ULONG_PTR               HeapSegmentCommit;
    ULONG_PTR               HeapDeCommitTotalFreeThreshold;
    ULONG_PTR               HeapDeCommitFreeBlockThreshold;
    ULONG                   NumberOfHeaps;
    ULONG                   MaximumNumberOfHeaps;
    PVOID                   *ProcessHeaps;
    PVOID                   GdiSharedHandleTable;
    PVOID                   ProcessStarterHelper;
    PVOID                   GdiDCAttributeList;
    PVOID                   LoaderLock;
    ULONG                   OSMajorVersion;
    ULONG                   OSMinorVersion;
    USHORT                  OSBuildNumber;
    USHORT                  OSCSDVersion;
    ULONG                   OSPlatformId;
    ULONG                   ImageSubSystem;
    ULONG                   ImageSubSystemMajorVersion;
    ULONG                   ImageSubSystemMinorVersion;
    ULONG_PTR               ImageProcessAffinityMask;
    ULONG                   GdiHandleBuffer[0x22];
    PVOID                   PostProcessInitRoutine;
    PVOID                   TlsExpansionBitmap;
    ULONG                   TlsExpansionBitmapBits[0x20];
    ULONG                   SessionId;

    ULARGE_INTEGER          AppCompatFlags;
    ULARGE_INTEGER          AppCompatFlagsUser;
    PVOID                   pShimData;
    PVOID                   AppCompatInfo;
    UNICODE_STRING          CSDVersion;
    PVOID                   ActivationContextData;
    PVOID                   ProcessAssemblyStorageMap;
    PVOID                   SystemDefaultActivationContextData;
    PVOID                   SystemAssemblyStorageMap;
    ULONG_PTR               MinimumStackCommit;

} PEB_WINXP, *PPEB_WINXP;
//#pragma pack (pop)


//#pragma pack (push, WUI_ARCH_PSIZE)
//#pragma pack(push)
//__declspec(align(8))
typedef struct _PEB_WIN7 {

    BOOLEAN                 InheritedAddressSpace;
    BOOLEAN                 ReadImageFileExecOptions;
    BOOLEAN                 BeingDebugged;
    union {
        UCHAR               BitField;
        struct {
            UCHAR           ImageUsesLargePages:1;
            UCHAR           IsProtectedProcess:1;
            UCHAR           IsLegacyProcess:1;
            UCHAR           IsImageDynamicallyRelocated:1;
            UCHAR           SkipPatchingUser32Forwarders:1;
            UCHAR           SpareBits:3;
            };
        };
#pragma pack (push, WUI_ARCH_PSIZE)
    HANDLE                  Mutant;
#pragma pack (pop)
    PVOID                   ImageBaseAddress;
    PPEB_LDR_DATA_WIN7      Ldr;
    PRTL_USER_PROCESS_PARAMETERS_WIN7   ProcessParameters;
    PVOID                   SubSystemData;
    PVOID                   ProcessHeap;
    PRTL_CRITICAL_SECTION   FastPebLock;
    PVOID                   AtlThunkSListPtr;
    PVOID                   IFEOKey;
    union {
        ULONG               CrossProcessFlags;
        struct {
            ULONG           ProcessInJob : 1;
            ULONG           ProcessInitializing : 1;
            ULONG           ProcessUsingVEH : 1;
            ULONG           ProcessUsingVCH : 1;
            ULONG           ProcessUsingFTH : 1;
            ULONG           ReservedBits0 : 27;
            };
        };

    union {
        PVOID               KernelCallbackTable;
        PVOID               UserSharedInfoPtr;
        };

    ULONG                   SystemReserved[1];
    ULONG                   AtlThunkSListPtr32;
    PVOID                   ApiSetMap;
    ULONG                   TlsExpansionCounter;
    PVOID                   TlsBitmap;
    ULONG                   TlsBitmapBits[0x2];
    PVOID                   ReadOnlySharedMemoryBase;
    PVOID                   HotpatchInformation;
    PVOID                   *ReadOnlyStaticServerData;
    PVOID                   AnsiCodePageData;
    PVOID                   OemCodePageData;
    PVOID                   UnicodeCaseTableData;
    ULONG                   NumberOfProcessors;
    ULONG                   NtGlobalFlag;
    //BYTE                    Spare2[0x4];
    LARGE_INTEGER           CriticalSectionTimeout;
    ULONG_PTR               HeapSegmentReserve;
    ULONG_PTR               HeapSegmentCommit;
    ULONG_PTR               HeapDeCommitTotalFreeThreshold;
    ULONG_PTR               HeapDeCommitFreeBlockThreshold;
    ULONG                   NumberOfHeaps;
    ULONG                   MaximumNumberOfHeaps;
    PVOID                   *ProcessHeaps;
    PVOID                   GdiSharedHandleTable;
    PVOID                   ProcessStarterHelper;
    PVOID                   GdiDCAttributeList;
    PVOID                   LoaderLock;
    ULONG                   OSMajorVersion;
    ULONG                   OSMinorVersion;
    USHORT                  OSBuildNumber;
    USHORT                  OSCSDVersion;
    ULONG                   OSPlatformId;
    ULONG                   ImageSubSystem;
    ULONG                   ImageSubSystemMajorVersion;
    ULONG                   ImageSubSystemMinorVersion;
    ULONG_PTR               ActiveProcessAffinityMask;
#ifdef WUI_X86
    ULONG                   GdiHandleBuffer[34];
#endif // WUI_X86
#ifdef WUI_X64
    ULONG                   GdiHandleBuffer[60];
#endif // WUI_X64
    PVOID                   PostProcessInitRoutine;
    PVOID                   TlsExpansionBitmap;
    ULONG                   TlsExpansionBitmapBits[0x20];
    ULONG                   SessionId;

    ULARGE_INTEGER          AppCompatFlags;
    ULARGE_INTEGER          AppCompatFlagsUser;
    PVOID                   pShimData;
    PVOID                   AppCompatInfo;
#pragma pack(push, WUI_ARCH_PSIZE)
    UNICODE_STRING          CSDVersion;
#pragma pack(pop)
    const struct _ACTIVATION_CONTEXT_DATA      *ActivationContextData;
    struct _ASSEMBLY_STORAGE_MAP   *ProcessAssemblyStorageMap;
    const struct _ACTIVATION_CONTEXT_DATA      *SystemDefaultActivationContextData;
    struct _ASSEMBLY_STORAGE_MAP   *SystemAssemblyStorageMap;
    ULONG_PTR               MinimumStackCommit;

// WIN7
    struct _FLS_CALLBACK_INFO   *FlsCallback;
    LIST_ENTRY              FlsListHead;
    PVOID                   FlsBitmap;
    ULONG                   FlsBitmapBits[4];
    ULONG                   FlsHighIndex;
    PVOID                   WerRegistrationData;
    PVOID                   WerShipAssertPtr;
    PVOID                   pContextData;
    PVOID                   pImageHeaderHash;
    union {
        ULONG               TracingFlags;
        struct {
            ULONG           HeapTracingEnabled : 1;
            ULONG           CritSecTracingEnabled : 1;
            ULONG           SpareTracingBits : 30;
            };
        };

    LONG                    __PADDING__[1];

} PEB_WIN7, *PPEB_WIN7;
//#pragma pack (pop)


//#pragma pack (push, 1)
typedef struct _PEB_WIN10 {

    BOOLEAN                 InheritedAddressSpace;
    BOOLEAN                 ReadImageFileExecOptions;
    BOOLEAN                 BeingDebugged;
    union {
        UCHAR               BitField;
        struct {
            UCHAR           ImageUsesLargePages:1;
            UCHAR           IsProtectedProcess:1;
            UCHAR           IsImageDynamicallyRelocated:1;
            UCHAR           SkipPatchingUser32Forwarders:1;
            UCHAR           IsPackagedProcess:1;
            UCHAR           IsAppContainer:1;
            UCHAR           IsProtectedProcessLight:1;
#if WUI_WIN10_VERSION < 14393
            UCHAR           SpareBits:1;
#else   // WUI_WIN10_VERSION < 14393
            UCHAR           IsLongPathAwareProcess:1;
#endif  // WUI_WIN10_VERSION < 14393
            };
        };
#ifdef WUI_X64
    UCHAR                   Padding0[4];
#endif // WUI_X64
    HANDLE                  Mutant;
    PVOID                   ImageBaseAddress;
    PPEB_LDR_DATA_WIN10      Ldr;
    PRTL_USER_PROCESS_PARAMETERS_WIN10  ProcessParameters;
    PVOID                   SubSystemData;
    PVOID                   ProcessHeap;
    PRTL_CRITICAL_SECTION   FastPebLock;
    PSLIST_HEADER volatile  AtlThunkSListPtr;
    PVOID                   IFEOKey;
    union {
        ULONG               CrossProcessFlags;
        struct {
            ULONG           ProcessInJob : 1;
            ULONG           ProcessInitializing : 1;
            ULONG           ProcessUsingVEH : 1;
            ULONG           ProcessUsingVCH : 1;
            ULONG           ProcessUsingFTH : 1;
#if WUI_WIN10_VERSION < 15063
            ULONG           ReservedBits0 : 27;
#else   // WUI_WIN10_VERSION < 15063
            ULONG           ProcessPreviouslyThrottled : 1;
            ULONG           ProcessCurrentlyThrottled : 1;
#if WUI_WIN10_VERSION < 17763
            ULONG           ReservedBits0 : 25;
#else   // WUI_WIN10_VERSION < 17763
            ULONG           ProcessImagesHotPatched : 1;
            ULONG           ReservedBits0 : 24;
#endif  // WUI_WIN10_VERSION < 17763
#endif  // WUI_WIN10_VERSION < 15063
            };
        };

#ifdef WUI_X64
    UCHAR                   Padding1[4];
#endif // WUI_X64
    union {
        PVOID               KernelCallbackTable;
        PVOID               UserSharedInfoPtr;
        };
    ULONG                   SystemReserved;
#if WUI_WIN10_VERSION < 19569
    ULONG                   AtlThunkSListPtr32;
#else   // WIN10_VERSION < 19569
    PSLIST_HEADER volatile  AtlThunkSListPtr32;
#endif  // WIN10_VERSION < 19569
    PVOID                   ApiSetMap;
    ULONG                   TlsExpansionCounter;
#ifdef WUI_X64
    UCHAR                   Padding2[4];
#endif // WUI_X64
    PVOID                   TlsBitmap;
    ULONG                   TlsBitmapBits[0x2];
    PVOID                   ReadOnlySharedMemoryBase;
#if WUI_WIN10_VERSION < 15063
    PVOID                   SparePvoid0;
#else   // WUI_WIN10_VERSION < 15063
    PVOID                   SharedData;
#endif  // WUI_WIN10_VERSION < 15063
    PVOID                   *ReadOnlyStaticServerData;
    PVOID                   AnsiCodePageData;
    PVOID                   OemCodePageData;
    PVOID                   UnicodeCaseTableData;
    ULONG                   NumberOfProcessors;
    ULONG                   NtGlobalFlag;
    //BYTE                    Spare2[0x4];
    LARGE_INTEGER           CriticalSectionTimeout;
    ULONG_PTR               HeapSegmentReserve;
    ULONG_PTR               HeapSegmentCommit;
    ULONG_PTR               HeapDeCommitTotalFreeThreshold;
    ULONG_PTR               HeapDeCommitFreeBlockThreshold;
    ULONG                   NumberOfHeaps;
    ULONG                   MaximumNumberOfHeaps;
    PVOID                   *ProcessHeaps;
    PVOID                   GdiSharedHandleTable;
    PVOID                   ProcessStarterHelper;
    ULONG                   GdiDCAttributeList;
#ifdef WUI_X64
    UCHAR                   Padding3[4];
#endif // WUI_X64
    PRTL_CRITICAL_SECTION   LoaderLock;
    ULONG                   OSMajorVersion;
    ULONG                   OSMinorVersion;
    USHORT                  OSBuildNumber;
    USHORT                  OSCSDVersion;
    ULONG                   OSPlatformId;
    ULONG                   ImageSubSystem;
    ULONG                   ImageSubSystemMajorVersion;
    ULONG                   ImageSubSystemMinorVersion;
#ifdef WUI_X64
    UCHAR                   Padding4[4];
#endif // WUI_X64
    ULONG_PTR               ActiveProcessAffinityMask;
#ifdef WUI_X86
    ULONG                   GdiHandleBuffer[34];
#else    // WUI_X86
    ULONG                   GdiHandleBuffer[60];
#endif  // WUI_X86
    PVOID                   PostProcessInitRoutine;
    PVOID                   TlsExpansionBitmap;
    ULONG                   TlsExpansionBitmapBits[32];
    ULONG                   SessionId;
#ifdef WUI_X64
    UCHAR                   Padding5[4];
#endif // WUI_X64
    ULARGE_INTEGER          AppCompatFlags;
    ULARGE_INTEGER          AppCompatFlagsUser;
    PVOID                   pShimData;
    PVOID                   AppCompatInfo;
    UNICODE_STRING          CSDVersion;
    const struct _ACTIVATION_CONTEXT_DATA      *ActivationContextData;
    struct _ASSEMBLY_STORAGE_MAP   *ProcessAssemblyStorageMap;
    const struct _ACTIVATION_CONTEXT_DATA      *SystemDefaultActivationContextData;
    struct _ASSEMBLY_STORAGE_MAP   *SystemAssemblyStorageMap;
    ULONG_PTR               MinimumStackCommit;

// WIN7
#if WUI_WIN10_VERSION < 18362
    struct _FLS_CALLBACK_INFO   *FlsCallback;
    LIST_ENTRY              FlsListHead;
    PVOID                   FlsBitmap;
    ULONG                   FlsBitmapBits[4];
    ULONG                   FlsHighIndex;
#else   // WUI_WIN10_VERSION < 18362
    PVOID                   SparePointers[4];
    ULONG                   SpareUlongs[5];
#endif  // WUI_WIN10_VERSION < 18362

    PVOID                   WerRegistrationData;
    PVOID                   WerShipAssertPtr;
    PVOID                   pUnused;
    PVOID                   pImageHeaderHash;
    union {
        ULONG               TracingFlags;
        struct {
            ULONG           HeapTracingEnabled : 1;
            ULONG           CritSecTracingEnabled : 1;
            ULONG           LibLoaderTracingEnabled : 1;
            ULONG           SpareTracingBits : 29;
            };
        };

// WIN10
#ifdef WUI_X64
    UCHAR                   Padding6[4];
#endif // WUI_X64
    ULONGLONG               CsrServerReadOnlySharedMemoryBase;

#if WUI_WIN10_VERSION >= 10586
    ULONG_PTR               TppWorkerpListLock;
    LIST_ENTRY              TppWorkerpList;
    PVOID                   WaitOnAddressHashTable[128];
#if WUI_WIN10_VERSION < 16299
    LONG                    __PADDING__[1];
#endif  // WUI_WIN10_VERSION < 16299
#endif  // WUI_WIN10_VERSION >= 10586

#if WUI_WIN10_VERSION >= 16299
    PVOID                   TelemetryCoverageHeader;
    ULONG                   CloudFileFlags;
#if WUI_WIN10_VERSION < 17134
    LONG                    __PADDING__[1];
#endif  // WUI_WIN10_VERSION < 17134
#endif  // WUI_WIN10_VERSION >= 16299

#if WUI_WIN10_VERSION >= 17134
    ULONG                   CloudFileDiagFlags;
    CHAR                    PlaceholderCompatibilityMode;
    CHAR                    PlaceholderCompatibilityModeReserved[7];
#endif  // WUI_WIN10_VERSION >= 17134

#if WUI_WIN10_VERSION >= 17763
    struct _LEAP_SECOND_DATA *LeapSecondData;
    union {
        ULONG               LeapSecondFlags;
        struct {
            ULONG           SixtySecondEnabled : 1;
            ULONG           Reserved : 31;
            };
        };
    ULONG                   NtGlobalFlag2;
#ifdef WUI_X86
    LONG                    __PADDING__[1];
#endif // WUI_X86
#endif  // WUI_WIN10_VERSION >= 17763

} PEB_WIN10, *PPEB_WIN10;
//#pragma pack (pop)


//#pragma pack (push, 1)
typedef struct _ACTIVATION_CONTEXT_STACK_WINXP {
    ULONG                   Flags;
    ULONG                   NextCookieSequenceNumber;
    PVOID                   ActiveFrame;
    LIST_ENTRY              FrameListCache;
} ACTIVATION_CONTEXT_STACK_WINXP, *PACTIVATION_CONTEXT_STACK_WINXP;
//#pragma pack (pop)


//#pragma pack (push, 1)
typedef struct _RTL_ACTIVATION_CONTEXT_STACK_FRAME {
    struct _RTL_ACTIVATION_CONTEXT_STACK_FRAME  *Previous;
    struct _ACTIVATION_CONTEXT  *ActivationContext;
    ULONG                   Flags;
#ifdef WUI_X64
    LONG                    __PADDING__[1];
#endif  // WUI_X64
} RTL_ACTIVATION_CONTEXT_STACK_FRAME, *PRTL_ACTIVATION_CONTEXT_STACK_FRAME;
//#pragma pack (pop)


//#pragma pack (push, 1)
typedef struct _ACTIVATION_CONTEXT_STACK_WIN7 {
    RTL_ACTIVATION_CONTEXT_STACK_FRAME* ActiveFrame;
    LIST_ENTRY              FrameListCache;
    ULONG                   Flags;
    ULONG                   NextCookieSequenceNumber;
    ULONG                   StackId;
#ifdef WUI_X64
    LONG                    __PADDING__[1];
#endif  // WUI_X64
} ACTIVATION_CONTEXT_STACK_WIN7, *PACTIVATION_CONTEXT_STACK_WIN7; /* size: 0x0018 */
//#pragma pack (pop)

//typedef struct _ACTIVATION_CONTEXT_STACK_WIN7 struct _ACTIVATION_CONTEXT_STACK_WIN10;
typedef ACTIVATION_CONTEXT_STACK_WIN7 ACTIVATION_CONTEXT_STACK_WIN10;
typedef PACTIVATION_CONTEXT_STACK_WIN7 PACTIVATION_CONTEXT_STACK_WIN10;


//#pragma pack (push, 1)
typedef struct _GDI_TEB_BATCH_WINXP {
    ULONG                   Offset;
    HANDLE                  HDC;
    ULONG                   Buffer[310];
} GDI_TEB_BATCH_WINXP, PGDI_TEB_BATCH_WINXP;
//#pragma pack (pop)

//typedef struct _GDI_TEB_BATCH_WINXP struct _GDI_TEB_BATCH_WIN7;
typedef GDI_TEB_BATCH_WINXP GDI_TEB_BATCH_WIN7;
typedef PGDI_TEB_BATCH_WINXP PGDI_TEB_BATCH_WIN7;


//#pragma pack (push, 1)
typedef struct _GDI_TEB_BATCH_WIN10 {
    struct {
        ULONG               Offset : 31;
        ULONG               HasRenderingCommand : 1;
        };
#pragma pack (push, WUI_ARCH_PSIZE)
    HANDLE                  HDC;
#pragma pack (pop)
    ULONG                   Buffer[310];
} GDI_TEB_BATCH_WIN10, PGDI_TEB_BATCH_WIN10;
//#pragma pack (pop)



typedef struct _Wx86ThreadState {
    PULONG                  CallBx86Eip;
    PVOID                   DeallocationCpu;
    UCHAR                   UseKnownWx86Dll;
    CHAR                    OleStubInvoked;
} Wx86ThreadState;


//#pragma pack (push, WUI_ARCH_PSIZE)
typedef struct _TEB_ACTIVE_FRAME_CONTEXT {
    ULONG                   Flags;
    PCHAR                   FrameName;
} TEB_ACTIVE_FRAME_CONTEXT, PTEB_ACTIVE_FRAME_CONTEXT;
//#pragma pack (pop)


//#pragma pack (push, WUI_ARCH_PSIZE)
typedef struct _TEB_ACTIVE_FRAME {
    ULONG                   Flags;
    struct _TEB_ACTIVE_FRAME *Previous;
    struct _TEB_ACTIVE_FRAME_CONTEXT *Context;
} TEB_ACTIVE_FRAME, *PTEB_ACTIVE_FRAME;
//#pragma pack (pop)


//#pragma pack (push, 1)
typedef struct _TEB_WINXP {

    NT_TIB                  NtTib;
    PVOID                   EnvironmentPointer;
    CLIENT_ID               ClientId;
    PVOID                   ActiveRpcHandle;
    PVOID                   ThreadLocalStoragePointer;
    PPEB_WINXP              ProcessEnvironmentBlock;
    ULONG                   LastErrorValue;
    ULONG                   CountOfOwnedCriticalSections;
    PVOID                   CsrClientThread;
    PVOID                   Win32ThreadInfo;
    ULONG                   User32Reserved[26];
    ULONG                   UserReserved[5];
    PVOID                   WOW32Reserved;
    ULONG                   CurrentLocale;
    ULONG                   FpSoftwareStatusRegister;
    PVOID                   SystemReserved1[0x36];
    //PVOID                   Spare1;
    ULONG                   ExceptionCode;
    ACTIVATION_CONTEXT_STACK_WINXP    ActivationContextStack;
    UCHAR                   SpareBytes1[24];
    GDI_TEB_BATCH_WINXP     GdiTebBatch;
    CLIENT_ID               RealClientId;
    PVOID                   GdiCachedProcessHandle;
    ULONG                   GdiClientPID;
    ULONG                   GdiClientTID;
    PVOID                   GdiThreadLocaleInfo;
    ULONG_PTR               Win32ClientInfo[62];
    PVOID                   glDispatchTable[233];
    ULONG_PTR               glReserved1[29];
    PVOID                   GlReserved2;
    PVOID                   GlSectionInfo;
    PVOID                   GlSection;
    PVOID                   GlTable;
    PVOID                   GlCurrentRC;
    PVOID                   GlContext;
    NTSTATUS                LastStatusValue;
    UNICODE_STRING          StaticUnicodeString;
    WCHAR                   StaticUnicodeBuffer[0x105];
    PVOID                   DeallocationStack;
    PVOID                   TlsSlots[0x40];
    LIST_ENTRY              TlsLinks;
    PVOID                   Vdm;
    PVOID                   ReservedForNtRpc;
    PVOID                   DbgSsReserved[0x2];
    ULONG                   HardErrorDisabled;
    PVOID                   Instrumentation[0x10];
    PVOID                   WinSockData;
    ULONG                   GdiBatchCount;
    UCHAR                   InDbgPrint;
    UCHAR                   FreeStackOnTermination;
    UCHAR                   HasFiberData;
    UCHAR                   IdealProcessor;
    ULONG                   Spare3;
    PVOID                   ReservedForPerf;
    PVOID                   ReservedForOle;
    ULONG                   WaitingOnLoaderLock;
    Wx86ThreadState         Wx86Thread;
    PVOID                   *TlsExpansionSlots;
    ULONG                   ImpersonationLocale;
    ULONG                   IsImpersonating;
    PVOID                   NlsCache;
    PVOID                   pShimData;
    ULONG                   HeapVirtualAffinity;
    PVOID                   CurrentTransactionHandle;
    PTEB_ACTIVE_FRAME       ActiveFrame;
    UCHAR                   SafeThunkCall;
    UCHAR                   BooleanSpare[3];

} TEB_WINXP, *PTEB_WINXP;
//#pragma pack (pop)


//#pragma pack (push, 1)
typedef struct _PROCESSOR_NUMBER {
    USHORT                  Group;
    UCHAR                   Number;
    UCHAR                   Reserved;
} PROCESSOR_NUMBER, *PPROCESSOR_NUMBER;
//#pragma pack (pop)


//#pragma pack (push, 1)
typedef struct _TEB_WIN7 {

    NT_TIB                  NtTib;
    PVOID                   EnvironmentPointer;
    CLIENT_ID               ClientId;
    PVOID                   ActiveRpcHandle;
    PVOID                   ThreadLocalStoragePointer;
    PPEB_WIN7               ProcessEnvironmentBlock;
    ULONG                   LastErrorValue;
    ULONG                   CountOfOwnedCriticalSections;
    PVOID                   CsrClientThread;
    PVOID                   Win32ThreadInfo;
    ULONG                   User32Reserved[26];
    ULONG                   UserReserved[5];
    PVOID                   WOW32Reserved;
    ULONG                   CurrentLocale;
    ULONG                   FpSoftwareStatusRegister;
    PVOID                   SystemReserved1[0x36];
    //PVOID                   Spare1;
    ULONG                   ExceptionCode;
    ACTIVATION_CONTEXT_STACK_WIN7    ActivationContextStack;
#ifdef WUI_X86
    UCHAR                   SpareBytes[36];
#else   // WUI_X86
    UCHAR                   SpareBytes[24];
#endif  // WUI_X86
    ULONG                   TxFsContext;
    GDI_TEB_BATCH_WIN7      GdiTebBatch;
    CLIENT_ID               RealClientId;
    PVOID                   GdiCachedProcessHandle;
    ULONG                   GdiClientPID;
    ULONG                   GdiClientTID;
    PVOID                   GdiThreadLocaleInfo;
    ULONG_PTR               Win32ClientInfo[62];
    PVOID                   glDispatchTable[233];
    ULONG_PTR               glReserved1[29];
    PVOID                   GlReserved2;
    PVOID                   GlSectionInfo;
    PVOID                   GlSection;
    PVOID                   GlTable;
    PVOID                   GlCurrentRC;
    PVOID                   GlContext;
    NTSTATUS                LastStatusValue;
    UNICODE_STRING          StaticUnicodeString;
    WCHAR                   StaticUnicodeBuffer[0x105];
    PVOID                   DeallocationStack;
    PVOID                   TlsSlots[0x40];
    LIST_ENTRY              TlsLinks;
    PVOID                   Vdm;
    PVOID                   ReservedForNtRpc;
    PVOID                   DbgSsReserved[0x2];
    ULONG                   HardErrorMode;
    PVOID                   Instrumentation[9];
    GUID                    ActivityId;
    PVOID                   SubProcessTag;
    PVOID                   EtwLocalData;
    PVOID                   EtwTraceData;
    PVOID                   WinSockData;
    ULONG                   GdiBatchCount;
    union {
        PROCESSOR_NUMBER    CurrentIdealProcessor;
        ULONG               IdealProcessorValue;
        struct {
            UCHAR           ReservedPad0;
            UCHAR           ReservedPad1;
            UCHAR           ReservedPad2;
            UCHAR           IdealProcessor;
            };
        };
    ULONG                   GuaranteedStackBytes;
    PVOID                   ReservedForPerf;
    PVOID                   ReservedForOle;
    ULONG                   WaitingOnLoaderLock;
    PVOID                   SavedPriorityState;
    ULONG_PTR               SoftPatchPtr1;
    PVOID                   ThreadPoolData;
    PVOID                   *TlsExpansionSlots;
    ULONG                   MuiGeneration;
    ULONG                   IsImpersonating;
    PVOID                   NlsCache;
    PVOID                   pShimData;
    ULONG                   HeapVirtualAffinity;
    PVOID                   CurrentTransactionHandle;
    PTEB_ACTIVE_FRAME       ActiveFrame;
    PVOID                   FlsData;
    PVOID                   PreferredLanguages;
    PVOID                   UserPrefLanguages;
    PVOID                   MergedPrefLanguages;
    ULONG                   MuiImpersonation;
    union {
        volatile USHORT     CrossTebFlags;
        USHORT              SpareCrossTebBits:16;
        };

    union {
        USHORT              SameTebFlags;
        struct  {
            USHORT          SafeThunkCall : 1;
            USHORT          InDebugPrint : 1;
            USHORT          HasFiberData : 1;
            USHORT          SkipThreadAttach : 1;
            USHORT          WerInShipAssertCode : 1;
            USHORT          RanProcessInit : 1;
            USHORT          ClonedThread : 1;
            USHORT          SuppressDebugMsg : 1;
            USHORT          DisableUserStackWalk : 1;
            USHORT          RtlExceptionAttached : 1;
            USHORT          InitialThread : 1;
            USHORT          SpareSameTebBits : 5;
            };
        };

    PVOID                   TxnScopeEnterCallback;
    PVOID                   TxnScopeExitCallback;
    PVOID                   TxnScopeContext;
    ULONG                   LockCount;
    ULONG                   SpareUlong0;
    PVOID                   ResourceRetValue;

} TEB_WIN7, *PTEB_WIN7;
//#pragma pack (pop)


//#pragma pack (push, 1)
typedef struct _TEB_WIN10 {

    NT_TIB                  NtTib;
    PVOID                   EnvironmentPointer;
    CLIENT_ID               ClientId;
    PVOID                   ActiveRpcHandle;
    PVOID                   ThreadLocalStoragePointer;
    PPEB_WIN10              ProcessEnvironmentBlock;
    ULONG                   LastErrorValue;
    ULONG                   CountOfOwnedCriticalSections;
    PVOID                   CsrClientThread;
    PVOID                   Win32ThreadInfo;
    ULONG                   User32Reserved[26];
    ULONG                   UserReserved[5];
    PVOID                   WOW32Reserved;
    ULONG                   CurrentLocale;
    ULONG                   FpSoftwareStatusRegister;
    PVOID                   ReservedForDebuggerInstrumentation[16];

#if WUI_WIN10_VERSION <= 10586
// 10240
// 10586
    PVOID                   SystemReserved1[38];
#else   // WUI_WIN10_VERSION <= 10586
    #if WUI_WIN10_VERSION <= 14393
    // 14393
    #ifdef WUI_X86
        PVOID                   SystemReserved1[36];
    #endif  // WUI_X86
    #ifdef WUI_X64
        PVOID                   SystemReserved1[37];
    #endif  // WUI_X64
        UCHAR                   WorkingOnBehalfTicket[8];
    #else   // WUI_WIN10_VERSION <= 14393
        #if WUI_WIN10_VERSION <= 15063
        // 15063
        #ifdef WUI_X86
            PVOID                   SystemReserved1[30];
        #endif  // WUI_X86
        #ifdef WUI_X64
            PVOID                   SystemReserved1[32];
        #endif  // WUI_X64
            ACTIVATION_CONTEXT_STACK_WIN10    _ActivationStack;
            UCHAR                   WorkingOnBehalfTicket[8];
        #else   // WUI_WIN10_VERSION < 15063
            #if WUI_WIN10_VERSION <= 17134
            // 16299, 17134
            #ifdef WUI_X64
                PVOID                   SystemReserved1[30];
            #endif // WUI_X64
            #ifdef WUI_X86
                PVOID                   SystemReserved1[26];
            #endif // WUI_X86
                CHAR                    PlaceholderCompatibilityMode;
                CHAR                    PlaceholderReserved[11];
                ULONG                   ProxiedProcessId;
                ACTIVATION_CONTEXT_STACK_WIN10    _ActivationStack;
                UCHAR                   WorkingOnBehalfTicket[8];
            #else   // WUI_WIN10_VERSION <= 17134
                // 17763, 19569
                #ifdef WUI_X64
                    PVOID                   SystemReserved1[30];
                #endif // WUI_X64
                #ifdef WUI_X86
                    PVOID                   SystemReserved1[26];
                #endif // WUI_X86
                    CHAR                    PlaceholderCompatibilityMode;
                    UCHAR                   PlaceholderHydrationAlwaysExplicit;
                    CHAR                    PlaceholderReserved[10];
                    ULONG                   ProxiedProcessId;
                    ACTIVATION_CONTEXT_STACK_WIN10    _ActivationStack;
                    UCHAR                   WorkingOnBehalfTicket[8];
            #endif  // WUI_WIN10_VERSION <= 17134
        #endif  // WUI_WIN10_VERSION <= 15063
    #endif  // WUI_WIN10_VERSION <= 14393
#endif  // WUI_WIN10_VERSION <= 10586

    ULONG                   ExceptionCode;
#ifdef WUI_X64
    UCHAR                   Padding0[4];
#endif // WUI_X64
    PACTIVATION_CONTEXT_STACK_WIN10    ActivationContextStackPointer;
    ULONG_PTR               InstrumentationCallbackSp;
    ULONG_PTR               InstrumentationCallbackPreviousPc;
    ULONG_PTR               InstrumentationCallbackPreviousSp;
#ifdef WUI_X86
    UCHAR                   SpareBytes[23];
#endif // WUI_X86
    ULONG                   TxFsContext;
#ifdef WUI_X64
    UCHAR                   InstrumentationCallbackDisabled;
#if WUI_WIN10_VERSION < 17763
    UCHAR                   Padding1[3];
#else   // WUI_WIN10_VERSION
    UCHAR                   UnalignedLoadStoreExceptions;
    UCHAR                   Padding1[2];
#endif  // WUI_WIN10_VERSION
#endif // WUI_X64
    GDI_TEB_BATCH_WIN10     GdiTebBatch;
    CLIENT_ID               RealClientId;
    PVOID                   GdiCachedProcessHandle;
    ULONG                   GdiClientPID;
    ULONG                   GdiClientTID;
    PVOID                   GdiThreadLocaleInfo;
    ULONG_PTR               Win32ClientInfo[62];
    PVOID                   glDispatchTable[233];
    ULONG_PTR               glReserved1[29];
    PVOID                   GlReserved2;
    PVOID                   GlSectionInfo;
    PVOID                   GlSection;
    PVOID                   GlTable;
    PVOID                   GlCurrentRC;
    PVOID                   GlContext;
    NTSTATUS                LastStatusValue;
#ifdef WUI_X64
    UCHAR                   Padding2[4];
#endif // WUI_X64
    UNICODE_STRING          StaticUnicodeString;
    WCHAR                   StaticUnicodeBuffer[0x105];
#ifdef WUI_X64
    UCHAR                   Padding3[6];
#endif WUI_X64
    PVOID                   DeallocationStack;
    PVOID                   TlsSlots[0x40];
    LIST_ENTRY              TlsLinks;
    PVOID                   Vdm;
    PVOID                   ReservedForNtRpc;
    PVOID                   DbgSsReserved[0x2];
    ULONG                   HardErrorMode;
#ifdef WUI_X64
    UCHAR                   Padding4[4];
    PVOID                   Instrumentation[11];
#endif // WUI_X64
#ifdef WUI_X86
    PVOID                   Instrumentation[9];
#endif // WUI_X86
    GUID                    ActivityId;
    PVOID                   SubProcessTag;
    PVOID                   PerflibData;
    PVOID                   EtwTraceData;
    PVOID                   WinSockData;
    ULONG                   GdiBatchCount;
    union {
        PROCESSOR_NUMBER    CurrentIdealProcessor;
        ULONG               IdealProcessorValue;
        struct {
            UCHAR           ReservedPad0;
            UCHAR           ReservedPad1;
            UCHAR           ReservedPad2;
            UCHAR           IdealProcessor;
            };
        };
    ULONG                   GuaranteedStackBytes;
#ifdef WUI_X64
    UCHAR                   Padding5[4];
#endif // WUI_X64
    PVOID                   ReservedForPerf;
    PVOID                   ReservedForOle;
    ULONG                   WaitingOnLoaderLock;
#ifdef WUI_X64
    UCHAR                   Padding6[4];
#endif // WUI_X64
    PVOID                   SavedPriorityState;
    ULONG_PTR               ReservedForCodeCoverage;
    PVOID                   ThreadPoolData;
    PVOID                   *TlsExpansionSlots;
    PVOID                   DeallocationBStore;
    PVOID                   BStoreLimit;
    ULONG                   MuiGeneration;
    ULONG                   IsImpersonating;
    PVOID                   NlsCache;
    PVOID                   pShimData;

// 10240
// 10586
// 14393
// 16299, 17134
#if WUI_WIN10_VERSION < 17763
    USHORT                  HeapVirtualAffinity;
    USHORT                  LowFragHeapDataSlot;
#else   // WUI_WIN10_VERSION
// 17763
// 19569
    ULONG                   HeapData;
#endif  // WUI_WIN10_VERSION
#ifdef WUI_X64
    UCHAR                   Padding7[4];
#endif // WUI_X64
    PVOID                   CurrentTransactionHandle;
    PTEB_ACTIVE_FRAME       ActiveFrame;
    PVOID                   FlsData;
    PVOID                   PreferredLanguages;
    PVOID                   UserPrefLanguages;
    PVOID                   MergedPrefLanguages;
    ULONG                   MuiImpersonation;
    union {
        volatile USHORT     CrossTebFlags;
        USHORT              SpareCrossTebBits:16;
        };

    union {
        USHORT              SameTebFlags;
        struct  {
            USHORT          SafeThunkCall : 1;
            USHORT          InDebugPrint : 1;
            USHORT          HasFiberData : 1;
            USHORT          SkipThreadAttach : 1;
            USHORT          WerInShipAssertCode : 1;
            USHORT          RanProcessInit : 1;
            USHORT          ClonedThread : 1;
            USHORT          SuppressDebugMsg : 1;
            USHORT          DisableUserStackWalk : 1;
            USHORT          RtlExceptionAttached : 1;
            USHORT          InitialThread : 1;
            USHORT          SessionAware : 1;
            USHORT          LoadOwner : 1;
            USHORT          LoaderWorker : 1;
#if WUI_WIN10_VERSION < 15063
            USHORT          SpareSameTebBits : 2;
#else   // WUI_WIN10_VERSION
            USHORT          SkipLoaderInit : 1;
            USHORT          SpareSameTebBits : 1;
#endif  // WUI_WIN10_VERSION
            };
        };

    PVOID                   TxnScopeEnterCallback;
    PVOID                   TxnScopeExitCallback;
    PVOID                   TxnScopeContext;
    ULONG                   LockCount;
    LONG                    WowTebOffset;
    PVOID                   ResourceRetValue;
    PVOID                   ReservedForWdf;
    ULONGLONG               ReservedForCrt;
    GUID                    EffectiveContainerId;

} TEB_WIN10, *PTEB_WIN10;
//#pragma pack (pop)





#ifdef WUI_OS_WINXP
typedef PEB_WINXP PEB;
typedef TEB_WINXP TEB;
typedef LDR_DATA_TABLE_ENTRY_WINXP LDR_DATA_TABLE_ENTRY;
typedef PEB_LDR_DATA_WINXP PEB_LDR_DATA;
#endif  // WUI_OS_WINXP


#ifdef WUI_OS_WIN7
typedef PEB_WIN7 PEB;
typedef TEB_WIN7 TEB;
typedef LDR_DATA_TABLE_ENTRY_WIN7 LDR_DATA_TABLE_ENTRY;
typedef PEB_LDR_DATA_WIN7 PEB_LDR_DATA;
#endif  // WUI_OS_WIN7

#ifdef WUI_OS_WIN10
typedef PEB_WIN10 PEB;
typedef TEB_WIN10 TEB;
typedef LDR_DATA_TABLE_ENTRY_WIN10 LDR_DATA_TABLE_ENTRY;
typedef PEB_LDR_DATA_WIN10 PEB_LDR_DATA;
#endif  // WUI_OS_WIN10







#define NT_SUCCESS(Status) ((NTSTATUS)(Status) >= 0)


NTSYSAPI
WCHAR
NTAPI
RtlUpcaseUnicodeChar(
    WCHAR SourceCharacter
    );


NTSYSAPI
BOOLEAN
NTAPI
RtlEqualUnicodeString(
    const UNICODE_STRING *String1,
    const UNICODE_STRING *String2,
    BOOLEAN CaseInSensitive
    );

NTSYSAPI
VOID
NTAPI
RtlInitUnicodeString(
    PUNICODE_STRING DestinationString,
    PCWSTR SourceString
    );


NTSYSAPI NTSTATUS RtlHashUnicodeString(
  PUNICODE_STRING String,
  BOOLEAN          CaseInSensitive,
  ULONG            HashAlgorithm,
  PULONG           HashValue
);


#define IsListEmpty(ListHead) \
    ((ListHead)->Flink == (ListHead))


FORCEINLINE
BOOLEAN
RemoveEntryList(
    __in PLIST_ENTRY Entry
    )
{
    PLIST_ENTRY Blink;
    PLIST_ENTRY Flink;

    Flink = Entry->Flink;
    Blink = Entry->Blink;
    Blink->Flink = Flink;
    Flink->Blink = Blink;
    return (BOOLEAN)(Flink == Blink);
}



FORCEINLINE
VOID
InsertTailList(
    __inout PLIST_ENTRY ListHead,
    __inout PLIST_ENTRY Entry
    )
{
    PLIST_ENTRY Blink;

    Blink = ListHead->Blink;
    Entry->Flink = ListHead;
    Entry->Blink = Blink;
    Blink->Flink = Entry;
    ListHead->Blink = Entry;
}


#endif  // _WIN_UM_INTERNAL_H_
