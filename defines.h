#define dbg( content, ... ) DbgPrintEx( 0, 0, "[>] " content, __VA_ARGS__ )
#define rva(instruction, size) ( instruction + size + *reinterpret_cast<long*>(instruction + (size - sizeof(long))))
#define size_align(size) ((size + 0xFFF) & 0xFFFFFFFFFFFFF000)
#define to_lower_c(ch) ((ch >= 'A' && ch <= 'Z') ? (ch + 32) : ch)

typedef struct _RTL_PROCESS_MODULE_INFORMATION
{
	HANDLE section;
	PVOID mapped_base;
	PVOID image_base;
	ULONG image_size;
	ULONG flags;
	USHORT load_order_index;
	USHORT init_order_index;
	USHORT load_count;
	USHORT offset_to_file_name;
	UCHAR  full_path_name[256];
} RTL_PROCESS_MODULE_INFORMATION, *PRTL_PROCESS_MODULE_INFORMATION;

typedef struct _RTL_PROCESS_MODULES
{
	ULONG number_of_modules;
	RTL_PROCESS_MODULE_INFORMATION modules[1];
} RTL_PROCESS_MODULES, *PRTL_PROCESS_MODULES;
typedef enum _SYSTEM_INFORMATION_CLASS
{
	SystemBasicInformation = 0x0,
	SystemProcessorInformation = 0x1,
	SystemPerformanceInformation = 0x2,
	SystemTimeOfDayInformation = 0x3,
	SystemPathInformation = 0x4,
	SystemProcessInformation = 0x5,
	SystemCallCountInformation = 0x6,
	SystemDeviceInformation = 0x7,
	SystemProcessorPerformanceInformation = 0x8,
	SystemFlagsInformation = 0x9,
	SystemCallTimeInformation = 0xa,
	SystemModuleInformation = 0xb,
	SystemLocksInformation = 0xc,
	SystemStackTraceInformation = 0xd,
	SystemPagedPoolInformation = 0xe,
	SystemNonPagedPoolInformation = 0xf,
	SystemHandleInformation = 0x10,
	SystemObjectInformation = 0x11,
	SystemPageFileInformation = 0x12,
	SystemVdmInstemulInformation = 0x13,
	SystemVdmBopInformation = 0x14,
	SystemFileCacheInformation = 0x15,
	SystemPoolTagInformation = 0x16,
	SystemInterruptInformation = 0x17,
	SystemDpcBehaviorInformation = 0x18,
	SystemFullMemoryInformation = 0x19,
	SystemLoadGdiDriverInformation = 0x1a,
	SystemUnloadGdiDriverInformation = 0x1b,
	SystemTimeAdjustmentInformation = 0x1c,
	SystemSummaryMemoryInformation = 0x1d,
	SystemMirrorMemoryInformation = 0x1e,
	SystemPerformanceTraceInformation = 0x1f,
	SystemObsolete0 = 0x20,
	SystemExceptionInformation = 0x21,
	SystemCrashDumpStateInformation = 0x22,
	SystemKernelDebuggerInformation = 0x23,
	SystemContextSwitchInformation = 0x24,
	SystemRegistryQuotaInformation = 0x25,
	SystemExtendServiceTableInformation = 0x26,
	SystemPrioritySeperation = 0x27,
	SystemVerifierAddDriverInformation = 0x28,
	SystemVerifierRemoveDriverInformation = 0x29,
	SystemProcessorIdleInformation = 0x2a,
	SystemLegacyDriverInformation = 0x2b,
	SystemCurrentTimeZoneInformation = 0x2c,
	SystemLookasideInformation = 0x2d,
	SystemTimeSlipNotification = 0x2e,
	SystemSessionCreate = 0x2f,
	SystemSessionDetach = 0x30,
	SystemSessionInformation = 0x31,
	SystemRangeStartInformation = 0x32,
	SystemVerifierInformation = 0x33,
	SystemVerifierThunkExtend = 0x34,
	SystemSessionProcessInformation = 0x35,
	SystemLoadGdiDriverInSystemSpace = 0x36,
	SystemNumaProcessorMap = 0x37,
	SystemPrefetcherInformation = 0x38,
	SystemExtendedProcessInformation = 0x39,
	SystemRecommendedSharedDataAlignment = 0x3a,
	SystemComPlusPackage = 0x3b,
	SystemNumaAvailableMemory = 0x3c,
	SystemProcessorPowerInformation = 0x3d,
	SystemEmulationBasicInformation = 0x3e,
	SystemEmulationProcessorInformation = 0x3f,
	SystemExtendedHandleInformation = 0x40,
	SystemLostDelayedWriteInformation = 0x41,
	SystemBigPoolInformation = 0x42,
	SystemSessionPoolTagInformation = 0x43,
	SystemSessionMappedViewInformation = 0x44,
	SystemHotpatchInformation = 0x45,
	SystemObjectSecurityMode = 0x46,
	SystemWatchdogTimerHandler = 0x47,
	SystemWatchdogTimerInformation = 0x48,
	SystemLogicalProcessorInformation = 0x49,
	SystemWow64SharedInformationObsolete = 0x4a,
	SystemRegisterFirmwareTableInformationHandler = 0x4b,
	SystemFirmwareTableInformation = 0x4c,
	SystemModuleInformationEx = 0x4d,
	SystemVerifierTriageInformation = 0x4e,
	SystemSuperfetchInformation = 0x4f,
	SystemMemoryListInformation = 0x50,
	SystemFileCacheInformationEx = 0x51,
	SystemThreadPriorityClientIdInformation = 0x52,
	SystemProcessorIdleCycleTimeInformation = 0x53,
	SystemVerifierCancellationInformation = 0x54,
	SystemProcessorPowerInformationEx = 0x55,
	SystemRefTraceInformation = 0x56,
	SystemSpecialPoolInformation = 0x57,
	SystemProcessIdInformation = 0x58,
	SystemErrorPortInformation = 0x59,
	SystemBootEnvironmentInformation = 0x5a,
	SystemHypervisorInformation = 0x5b,
	SystemVerifierInformationEx = 0x5c,
	SystemTimeZoneInformation = 0x5d,
	SystemImageFileExecutionOptionsInformation = 0x5e,
	SystemCoverageInformation = 0x5f,
	SystemPrefetchPatchInformation = 0x60,
	SystemVerifierFaultsInformation = 0x61,
	SystemSystemPartitionInformation = 0x62,
	SystemSystemDiskInformation = 0x63,
	SystemProcessorPerformanceDistribution = 0x64,
	SystemNumaProximityNodeInformation = 0x65,
	SystemDynamicTimeZoneInformation = 0x66,
	SystemCodeIntegrityInformation = 0x67,
	SystemProcessorMicrocodeUpdateInformation = 0x68,
	SystemProcessorBrandString = 0x69,
	SystemVirtualAddressInformation = 0x6a,
	SystemLogicalProcessorAndGroupInformation = 0x6b,
	SystemProcessorCycleTimeInformation = 0x6c,
	SystemStoreInformation = 0x6d,
	SystemRegistryAppendString = 0x6e,
	SystemAitSamplingValue = 0x6f,
	SystemVhdBootInformation = 0x70,
	SystemCpuQuotaInformation = 0x71,
	SystemNativeBasicInformation = 0x72,
	SystemErrorPortTimeouts = 0x73,
	SystemLowPriorityIoInformation = 0x74,
	SystemBootEntropyInformation = 0x75,
	SystemVerifierCountersInformation = 0x76,
	SystemPagedPoolInformationEx = 0x77,
	SystemSystemPtesInformationEx = 0x78,
	SystemNodeDistanceInformation = 0x79,
	SystemAcpiAuditInformation = 0x7a,
	SystemBasicPerformanceInformation = 0x7b,
	SystemQueryPerformanceCounterInformation = 0x7c,
	SystemSessionBigPoolInformation = 0x7d,
	SystemBootGraphicsInformation = 0x7e,
	SystemScrubPhysicalMemoryInformation = 0x7f,
	SystemBadPageInformation = 0x80,
	SystemProcessorProfileControlArea = 0x81,
	SystemCombinePhysicalMemoryInformation = 0x82,
	SystemEntropyInterruptTimingInformation = 0x83,
	SystemConsoleInformation = 0x84,
	SystemPlatformBinaryInformation = 0x85,
	SystemThrottleNotificationInformation = 0x86,
	SystemHypervisorProcessorCountInformation = 0x87,
	SystemDeviceDataInformation = 0x88,
	SystemDeviceDataEnumerationInformation = 0x89,
	SystemMemoryTopologyInformation = 0x8a,
	SystemMemoryChannelInformation = 0x8b,
	SystemBootLogoInformation = 0x8c,
	SystemProcessorPerformanceInformationEx = 0x8d,
	SystemSpare0 = 0x8e,
	SystemSecureBootPolicyInformation = 0x8f,
	SystemPageFileInformationEx = 0x90,
	SystemSecureBootInformation = 0x91,
	SystemEntropyInterruptTimingRawInformation = 0x92,
	SystemPortableWorkspaceEfiLauncherInformation = 0x93,
	SystemFullProcessInformation = 0x94,
	SystemKernelDebuggerInformationEx = 0x95,
	SystemBootMetadataInformation = 0x96,
	SystemSoftRebootInformation = 0x97,
	SystemElamCertificateInformation = 0x98,
	SystemOfflineDumpConfigInformation = 0x99,
	SystemProcessorFeaturesInformation = 0x9a,
	SystemRegistryReconciliationInformation = 0x9b,
	MaxSystemInfoClass = 0x9c,
} SYSTEM_INFORMATION_CLASS;

typedef  enum
{
	PS_COSMETIC = 0x00000000,
	PS_ENDCAP_ROUND = 0x00000000,
	PS_JOIN_ROUND = 0x00000000,
	PS_SOLID = 0x00000000,
	PS_DASH = 0x00000001,
	PS_DOT = 0x00000002,
	PS_DASHDOT = 0x00000003,
	PS_DASHDOTDOT = 0x00000004,
	PS_NULL = 0x00000005,
	PS_INSIDEFRAME = 0x00000006,
	PS_USERSTYLE = 0x00000007,
	PS_ALTERNATE = 0x00000008,
	PS_ENDCAP_SQUARE = 0x00000100,
	PS_ENDCAP_FLAT = 0x00000200,
	PS_JOIN_BEVEL = 0x00001000,
	PS_JOIN_MITER = 0x00002000,
	PS_GEOMETRIC = 0x00010000
} PenStyle;

typedef struct {
	LONG lfHeight;
	LONG lfWidth;
	LONG lfEscapement;
	LONG lfOrientation;
	LONG lfWeight;
	BYTE lfItalic;
	BYTE lfUnderline;
	BYTE lfStrikeOut;
	BYTE lfCharSet;
	BYTE lfOutPrecision;
	BYTE lfClipPrecision;
	BYTE lfQuality;
	BYTE lfPitchAndFamily;
	WCHAR lfFaceName[32];
} LOGFONTW;

typedef struct _LDR_DATA_TABLE_ENTRY
{
	LIST_ENTRY in_load_order_links;
	LIST_ENTRY in_memory_order_links;
	LIST_ENTRY in_initialization_order_links;
	PVOID dll_base;
	PVOID entry_point;
	ULONG size_of_image;
	UNICODE_STRING full_dll_name;
	UNICODE_STRING base_dll_name;
	ULONG flags;
	WORD load_count;
	WORD tls_index;
	union
	{
		LIST_ENTRY hash_links;
		struct
		{
			PVOID section_pointer;
			ULONG check_sum;
		};
	};
	union
	{
		ULONG time_date_stamp;
		PVOID loaded_imports;
	};
	void *entry_point_activation_context;
	PVOID patch_information;
	LIST_ENTRY forwarder_links;
	LIST_ENTRY service_tag_links;
	LIST_ENTRY static_links;
} LDR_DATA_TABLE_ENTRY, *PLDR_DATA_TABLE_ENTRY;

typedef struct _RTL_CRITICAL_SECTION
{
	void *debug_info;
	LONG lock_count;
	LONG recursion_count;
	PVOID owning_thread;
	PVOID lock_semaphore;
	ULONG spin_count;
} RTL_CRITICAL_SECTION, *PRTL_CRITICAL_SECTION;

typedef struct _PEB_LDR_DATA
{
	ULONG length;
	UCHAR initialized;
	PVOID ss_handle;
	LIST_ENTRY in_load_order_module_list;
	LIST_ENTRY in_memory_order_module_list;
	LIST_ENTRY in_initialization_order_module_list;
	PVOID entry_in_progress;
} PEB_LDR_DATA, *PPEB_LDR_DATA;

typedef struct _PEB
{
	UCHAR inherited_address_space;
	UCHAR read_image_file_exec_options;
	UCHAR being_debugged;
	UCHAR bit_field;
	ULONG image_uses_large_pages : 1;
	ULONG is_protected_process : 1;
	ULONG is_legacy_process : 1;
	ULONG is_image_dynamically_relocated : 1;
	ULONG spare_bits : 4;
	PVOID mutant;
	PVOID image_base_address;
	PPEB_LDR_DATA ldr;
	void *process_parameters;
	PVOID sub_system_data;
	PVOID process_heap;
	PRTL_CRITICAL_SECTION fast_peb_lock;
	PVOID atl_thunk_s_list_ptr;
	PVOID ifeo_key;
	ULONG cross_process_flags;
	ULONG process_in_job : 1;
	ULONG process_initializing : 1;
	ULONG reserved_bits0 : 30;
	union
	{
		PVOID kernel_callback_table;
		PVOID user_shared_info_ptr;
	};
	ULONG system_reserved[1];
	ULONG spare_ulong;
	void *free_list;
	ULONG tls_expansion_counter;
	PVOID tls_bitmap;
	ULONG tls_bitmap_bits[2];
	PVOID read_only_shared_memory_base;
	PVOID hotpatch_information;
	void **read_only_static_server_data;
	PVOID ansi_code_page_data;
	PVOID oem_code_page_data;
	PVOID unicode_case_table_data;
	ULONG number_of_processors;
	ULONG nt_global_flag;
	LARGE_INTEGER critical_section_timeout;
	ULONG heap_segment_reserve;
	ULONG heap_segment_commit;
	ULONG heap_de_commit_total_free_threshold;
	ULONG heap_de_commit_free_block_threshold;
	ULONG number_of_heaps;
	ULONG maximum_number_of_heaps;
	void **process_heaps;
	PVOID gdi_shared_handle_table;
	PVOID process_starter_helper;
	ULONG gdi_dc_attribute_list;
	PRTL_CRITICAL_SECTION loader_lock;
	ULONG os_major_version;
	ULONG os_minor_version;
	WORD os_build_number;
	WORD oscsd_version;
	ULONG os_platform_id;
	ULONG image_subsystem;
	ULONG image_subsystem_major_version;
	ULONG image_subsystem_minor_version;
	ULONG image_process_affinity_mask;
	ULONG gdi_handle_buffer[34];
	PVOID post_process_init_routine;
	PVOID tls_expansion_bitmap;
	ULONG tls_expansion_bitmap_bits[32];
	ULONG session_id;
	ULARGE_INTEGER app_compat_flags;
	ULARGE_INTEGER app_compat_flags_user;
	PVOID p_shim_data;
	PVOID app_compat_info;
	UNICODE_STRING csd_version;
	void *activation_context_data;
	void *process_assembly_storage_map;
	void *system_default_activation_context_data;
	void *system_assembly_storage_map;
	ULONG minimum_stack_commit;
	void *fls_callback;
	LIST_ENTRY fls_list_head;
	PVOID fls_bitmap;
	ULONG fls_bitmap_bits4[4];
	ULONG fls_high_index;
	void* wer_registration_data;
	void* wer_ship_assert_ptr;
} PEB, *PPEB;

typedef struct _SYSTEM_THREAD_INFORMATION
{
	LARGE_INTEGER kernel_time;
	LARGE_INTEGER user_time;
	LARGE_INTEGER create_time;
	ULONG wait_time;
	PVOID start_address;
	CLIENT_ID client_id;
	KPRIORITY priority;
	LONG base_priority;
	ULONG context_switches;
	ULONG thread_state;
	KWAIT_REASON wait_reason;
}SYSTEM_THREAD_INFORMATION, *PSYSTEM_THREAD_INFORMATION;

typedef struct _SYSTEM_PROCESS_INFO
{
	ULONG next_entry_offset;
	ULONG number_of_threads;
	LARGE_INTEGER working_set_private_size;
	ULONG hard_fault_count;
	ULONG number_of_threads_high_watermark;
	ULONGLONG cycle_time;
	LARGE_INTEGER create_time;
	LARGE_INTEGER user_time;
	LARGE_INTEGER kernel_time;
	UNICODE_STRING image_name;
	KPRIORITY base_priority;
	HANDLE unique_process_id;
	HANDLE inherited_from_unique_process_id;
	ULONG handle_count;
	ULONG session_id;
	ULONG_PTR unique_process_key;
	SIZE_T peak_virtual_size;
	SIZE_T virtual_size;
	ULONG page_fault_count;
	SIZE_T peak_working_set_size;
	SIZE_T working_set_size;
	SIZE_T quota_peak_paged_pool_usage;
	SIZE_T quota_paged_pool_usage;
	SIZE_T quota_peak_non_paged_pool_usage;
	SIZE_T quota_non_paged_pool_usage;
	SIZE_T pagefile_usage;
	SIZE_T peak_pagefile_usage;
	SIZE_T private_page_count;
	LARGE_INTEGER read_operation_count;
	LARGE_INTEGER write_operation_count;
	LARGE_INTEGER other_operation_count;
	LARGE_INTEGER read_transfer_count;
	LARGE_INTEGER write_transfer_count;
	LARGE_INTEGER other_transfer_count;
	SYSTEM_THREAD_INFORMATION threads[1];
}SYSTEM_PROCESS_INFO, *PSYSTEM_PROCESS_INFO;

typedef struct _SYSTEM_BIGPOOL_ENTRY
{
	union {
		PVOID VirtualAddress;
		ULONG_PTR NonPaged : 1;
	};
	ULONG_PTR SizeInBytes;
	union {
		UCHAR Tag[4];
		ULONG TagUlong;
	};
} SYSTEM_BIGPOOL_ENTRY, * PSYSTEM_BIGPOOL_ENTRY;

//burasý saðolsun yoksa yapamýcaktý ahshqwue http://www.geoffchappell.com/studies/windows/km/ntoskrnl/api/ex/sysinfo/bigpool.htm
typedef struct _SYSTEM_BIGPOOL_INFORMATION {
	ULONG Count;
	SYSTEM_BIGPOOL_ENTRY AllocatedInfo[ANYSIZE_ARRAY];
} SYSTEM_BIGPOOL_INFORMATION, * PSYSTEM_BIGPOOL_INFORMATION;

extern "C"
{
	NTKERNELAPI
		PVOID
		PsGetProcessSectionBaseAddress(
			__in PEPROCESS Process
		);
	NTSYSAPI NTSTATUS RtlCreateUserThread(HANDLE, PVOID, BOOLEAN, ULONG, SIZE_T, SIZE_T, PVOID, PVOID, PHANDLE, PCLIENT_ID);
	__declspec( dllimport ) NTSTATUS ZwWaitForMultipleObjects( unsigned long, HANDLE[], WAIT_TYPE, BOOLEAN, LARGE_INTEGER * );
	__declspec( dllimport ) PPEB PsGetProcessPeb( PEPROCESS );
	__declspec( dllimport ) NTSTATUS __stdcall ZwQuerySystemInformation( SYSTEM_INFORMATION_CLASS, void *, unsigned long, unsigned long * );
	NTKERNELAPI
		NTSTATUS
		MmCopyVirtualMemory(
			PEPROCESS SourceProcess,
			PVOID SourceAddress,
			PEPROCESS TarGet,
			PVOID TargetAddress,
			SIZE_T BufferSize,
			KPROCESSOR_MODE PreviousMode,
			PSIZE_T ReturnSize
		);
	__declspec( dllimport ) void *__stdcall RtlFindExportedRoutineByName( void *, PCCH );
}
