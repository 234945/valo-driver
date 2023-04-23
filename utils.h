namespace utils
{

    uintptr_t get_system_module(const wchar_t* name)
    {
        NTSTATUS status = STATUS_SUCCESS;
        ANSI_STRING s_name;
        UNICODE_STRING su_name;
        RtlInitUnicodeString(&su_name, name);
        RtlUnicodeStringToAnsiString(&s_name, &su_name, TRUE);

        PRTL_PROCESS_MODULES pModules = NULL;
        uint32_t szModules = 0;

        status = ZwQuerySystemInformation(SystemModuleInformation, 0, szModules, (PULONG)&szModules);
        if (!szModules)
        {
            RtlFreeAnsiString(&s_name);
            return 0;
        }

        pModules = (PRTL_PROCESS_MODULES)ExAllocatePool(NonPagedPool, szModules);
        if (!pModules)
        {
            RtlFreeAnsiString(&s_name);
            return 0;
        }
        RtlZeroMemory(pModules, szModules);

        status = ZwQuerySystemInformation(SystemModuleInformation, pModules, szModules, (PULONG)&szModules);
        if (!NT_SUCCESS(status))
        {
            RtlFreeAnsiString(&s_name);
            ExFreePool(pModules);
            return 0;
        }

        uintptr_t modBase = 0;
        PRTL_PROCESS_MODULE_INFORMATION pMods = pModules->modules;
        for (ULONG i = 0; i < pModules->number_of_modules && !modBase; i++)
        {
            RTL_PROCESS_MODULE_INFORMATION pMod = pMods[i];
            char* fullPath = (char*)pMod.full_path_name;
            if (fullPath && strlen(fullPath) > 0)
            {
                int32_t lastFound = -1;
                char* baseFullPath = (char*)pMod.full_path_name;
                while (*fullPath != 0)
                {
                    if (*fullPath == '\\')
                        lastFound = (fullPath - baseFullPath) + 1;
                    fullPath++;
                }

                if (lastFound >= 0)
                    fullPath = baseFullPath + lastFound;
            }
            else continue;

            ANSI_STRING s_fullPath;
            RtlInitAnsiString(&s_fullPath, fullPath);
            if (RtlEqualString(&s_fullPath, &s_name, TRUE))
                modBase = (uintptr_t)pMod.image_base;
        }
        RtlFreeAnsiString(&s_name);
        ExFreePool(pModules);
        return modBase;
    }
    auto find_pattern( const uintptr_t base, const char *pattern, const char *mask, const char *section = "" ) -> const uintptr_t
    {
        const auto compare_bytes = [&]( const uintptr_t base, const size_t size, const char *pattern, const char *mask ) -> const uintptr_t
        {
            const auto check_mask = []( const char *base, const char *pattern, const char *mask ) -> bool
            {
                for ( ; *mask; ++base, ++pattern, ++mask )
                {
                    if ( *mask == 'x' && *base != *pattern )
                    {
                        return false;
                    }
                }

                return true;
            };

            const auto range = size - strlen( mask );

            for ( auto i = 0ull; i < range; ++i )
            {
                if ( check_mask( ( const char * )base + i, pattern, mask ) )
                {
                    return base + i;
                }
            }

            return 0;
        };

        const auto dos = reinterpret_cast< IMAGE_DOS_HEADER * >( base );

        const auto nt = reinterpret_cast< IMAGE_NT_HEADERS * >( base + dos->e_lfanew );

        const auto sections = IMAGE_FIRST_SECTION( nt );

        for ( size_t i = 0; i < nt->FileHeader.NumberOfSections; i++ )
        {
            const auto current_section = &sections[i];

            if ( strlen( section ) > 1 && strcmp( section, ( char * )current_section->Name ) != 0 )
            {
                continue;
            }

            if ( current_section->Characteristics & IMAGE_SCN_MEM_EXECUTE )
            {
                const auto match = compare_bytes( base + current_section->VirtualAddress, current_section->Misc.VirtualSize, pattern, mask );

                if ( match )
                {
                    return match;
                }
            }
        }

        return 0;
    }

    NTSTATUS find_module_section(std::uintptr_t imageAddress, const char* sectionName, std::uintptr_t* sectionBase, std::size_t* sectionSize) {
        if (!imageAddress || reinterpret_cast<PIMAGE_DOS_HEADER>(imageAddress)->e_magic != 0x5A4D)
            return {};

        const auto ntHeader = reinterpret_cast<PIMAGE_NT_HEADERS64>(imageAddress + reinterpret_cast<PIMAGE_DOS_HEADER>(
            imageAddress)->e_lfanew);
        auto sectionHeader = IMAGE_FIRST_SECTION(ntHeader);

        for (std::uint16_t i = 0; i < ntHeader->FileHeader.NumberOfSections; ++i, ++sectionHeader)
            if (strstr(reinterpret_cast<const char*>(&sectionHeader->Name), sectionName)) {
                *sectionBase = imageAddress + sectionHeader->VirtualAddress;
                *sectionSize = sectionHeader->Misc.VirtualSize;
                return STATUS_SUCCESS;
            }

        return STATUS_NOT_FOUND;
    }

    std::uintptr_t find_pattern_size(std::uint8_t* base, const std::size_t size, const char* pattern, const char* mask) {
        const auto patternSize = strlen(mask);

        for (std::size_t i = {}; i < size - patternSize; i++)
        {
            for (std::size_t j = {}; j < patternSize; j++)
            {
                if (mask[j] != '?' && *reinterpret_cast<std::uint8_t*>(base + i + j) != static_cast<std::uint8_t>(pattern[j]))
                    break;

                if (j == patternSize - 1)
                    return reinterpret_cast<std::uintptr_t>(base) + i;
            }
        }

        return {};
    }

    auto get_system_information( const SYSTEM_INFORMATION_CLASS information_class ) -> const void *
    {
        unsigned long size = 32;
        char buffer[32];

        ZwQuerySystemInformation( information_class, buffer, size, &size );

        const auto info = ExAllocatePoolZero( NonPagedPool, size, 'KETO' );

        if ( !info )
        {
            return nullptr;
        }

        if ( ZwQuerySystemInformation( information_class, info, size, &size ) != STATUS_SUCCESS )
        {
            ExFreePool( info );
            return nullptr;
        }

        return info;
    }   

    template <typename t1, typename t2>
    auto str_i_cmp( t1 src, t2 dst, bool two ) -> bool
    {
        if ( !src || !dst )
        {
            return false;
        }

        wchar_t c1, c2;
        do 
        {
            c1 = *src++;
            c2 = *dst++;

            c1 = to_lower_c( c1 ); 
            c2 = to_lower_c( c2 );
            
            if ( !c1 && ( two ? !c2 : 1 ) )
            {
                return true;
            }
               
        } while ( c1 == c2 );

        return false;
    }

    auto get_eprocess( const char *target_name ) -> const PEPROCESS
    {
        PEPROCESS process = 0;

        const auto info = ( SYSTEM_PROCESS_INFO * )get_system_information( SystemProcessInformation );

        if ( !info )
        {
            return 0;
        }

        auto current = info;

        while ( true )
        {
            const wchar_t *process_name = current->image_name.Buffer;

            if ( !MmIsAddressValid( ( void * )process_name ) )
            {
                goto end;
            }

            if ( !str_i_cmp( target_name, process_name, true ) )
            {
                goto end;
            }

            if ( !current->unique_process_id )
            {
                goto end;
            }

            if ( PsLookupProcessByProcessId( current->unique_process_id, &process ) != STATUS_SUCCESS )
            {
                goto end;
            }

            break;

        end:
            if ( !current->next_entry_offset )
            {
                break;
            }

            current = reinterpret_cast< SYSTEM_PROCESS_INFO * > ( ( reinterpret_cast< uintptr_t > ( current ) + current->next_entry_offset ) );
        }

        ExFreePoolWithTag( info, 'KETO' );

        return process;
    }

    auto get_process_id( const char *target_name ) -> const int
    {
        int pid = 0;

        const auto info = ( SYSTEM_PROCESS_INFO * )get_system_information( SystemProcessInformation );

        if ( !info )
        {
            return 0;
        }

        auto current = info;

        while ( true )
        {
            const wchar_t *process_name = current->image_name.Buffer;

            if ( !MmIsAddressValid( ( void * )process_name ) )
            {
                goto end;
            }

            if ( !str_i_cmp( target_name, process_name, true ) )
            {
                goto end;
            }

            if ( !current->unique_process_id )
            {
                goto end;
            }

            pid = reinterpret_cast<int> ( current->unique_process_id );

            break;

        end:
            if ( !current->next_entry_offset )
            {
                break;
            }

            current = reinterpret_cast< SYSTEM_PROCESS_INFO * > ( ( reinterpret_cast< uintptr_t > ( current ) + current->next_entry_offset ) );
        }

        ExFreePoolWithTag( info, 'KETO' );

        return pid;
    }

    auto get_kernel_module( const char *name ) -> const uintptr_t
    {
        const auto to_lower = []( char *string ) -> const char *{
            for ( char *pointer = string; *pointer != '\0'; ++pointer )
            {
                *pointer = ( char )( short )tolower( *pointer );
            }

            return string;
        };

        const auto info = ( PRTL_PROCESS_MODULES )get_system_information( SystemModuleInformation );

        if ( !info )
        {
            return 0;
        }

        for ( auto i = 0ull; i < info->number_of_modules; ++i )
        {
            const auto &module = info->modules[i];

            if ( strcmp( to_lower( ( char * )module.full_path_name + module.offset_to_file_name ), name ) == 0 )
            {
                const auto address = module.image_base;

                ExFreePool( info );

                return reinterpret_cast< uintptr_t > ( address );
            }
        }

        ExFreePool( info );

        return 0;
    }

    std::uintptr_t find_guarded_region()
    {
        PSYSTEM_BIGPOOL_INFORMATION pool_information = 0;

        ULONG information_length = 0;
        NTSTATUS status = ZwQuerySystemInformation(SystemBigPoolInformation, &information_length, 0, &information_length);

        while (status == STATUS_INFO_LENGTH_MISMATCH)
        {
            if (pool_information)
                ExFreePool(pool_information);

            pool_information = (PSYSTEM_BIGPOOL_INFORMATION)ExAllocatePool(NonPagedPool, information_length);
            status = ZwQuerySystemInformation(SystemBigPoolInformation, pool_information, information_length, &information_length);
        }

        if (pool_information)
        {
            for (int i = 0; i < pool_information->Count; i++)
            {
                SYSTEM_BIGPOOL_ENTRY* allocation_entry = &pool_information->AllocatedInfo[i];
                std::uintptr_t virtual_address = (uintptr_t)allocation_entry->VirtualAddress & ~1ull;

                if (allocation_entry->NonPaged && allocation_entry->SizeInBytes == 0x200000)
                {
                    ExFreePool(pool_information);

                    return virtual_address;
                }
            }
        }

        return 0;
    }

    void free_user_memory(void* Ptr) 
    {
        SIZE_T SizeUL = 0;
        ZwFreeVirtualMemory(ZwCurrentProcess(), &Ptr, &SizeUL, MEM_RELEASE);
    }

    void* allocate_user_memory(ULONG Size, ULONG Protect = PAGE_READWRITE) 
    {
        PVOID AllocBase = nullptr; SIZE_T SizeUL = size_align(Size);

        if (!ZwAllocateVirtualMemory(ZwCurrentProcess(), &AllocBase, 0, &SizeUL, MEM_COMMIT, Protect)) 
        {
            RtlZeroMemory(AllocBase, SizeUL);
        }

        return AllocBase;
    }

    void get_module_export( const uintptr_t base, const char *name, std::uintptr_t* function_pointer )
    {
        *function_pointer = reinterpret_cast<std::uintptr_t>(RtlFindExportedRoutineByName( reinterpret_cast< void * >( base ), name ) );
    }

    auto attach_process( const int pid ) -> const PEPROCESS
    {
        if ( !pid )
        {
            return 0;
        }

        PEPROCESS process = 0;
        if ( PsLookupProcessByProcessId( reinterpret_cast< HANDLE >( pid ), &process ) != STATUS_SUCCESS )
        {
            return 0;
        }

        KeAttachProcess( process );

        return process;
    }

    auto detach_process( const PEPROCESS& process ) -> bool
    {
        if ( !process )
        {
            return false;
        }

        KeDetachProcess( );
        
        ObfDereferenceObject( process );

        return true;
    }

    std::uintptr_t find_process_module(size_t ProcessId, const wchar_t* wModuleName)
    {
        PEPROCESS process = NULL;
        PVOID baseAddress = NULL;

        NTSTATUS status = PsLookupProcessByProcessId((HANDLE)ProcessId, &process);

        if (NT_SUCCESS(status))
        {
            KeAttachProcess(process);

            UNICODE_STRING moduleName{};
            RtlInitUnicodeString(&moduleName, wModuleName);

            PLIST_ENTRY list = &(PsGetProcessPeb(process)->ldr->in_load_order_module_list);

            for (PLIST_ENTRY entry = list->Flink; entry != list; )
            {
                PLDR_DATA_TABLE_ENTRY mod = CONTAINING_RECORD(entry, LDR_DATA_TABLE_ENTRY, in_load_order_links);

                if (RtlCompareUnicodeString(&mod->base_dll_name, &moduleName, TRUE) == 0)
                {
                    baseAddress = mod->dll_base;
                }

                entry = mod->in_load_order_links.Flink;
            }

            KeDetachProcess();

            ObDereferenceObject(process);
        }

        return (UINT64)baseAddress;
    }

    auto sleep( long long ms ) -> void
    {
        LARGE_INTEGER delay;
        delay.QuadPart = -ms * 10000;
        KeDelayExecutionThread( KernelMode, false, &delay );
    }

    auto get_random_address( ) -> const uintptr_t
    {
        LARGE_INTEGER tick;
        KeQueryTickCount( &tick );

        if ( globals::win32kbase > globals::ntoskrnl )
        {
            return ( RtlRandomEx( &tick.LowPart ) % globals::ntoskrnl + globals::win32kbase );
        }

        return ( RtlRandomEx( &tick.LowPart ) % globals::win32kbase + globals::ntoskrnl );
    }
#define WINDOWS_1803 17134
#define WINDOWS_1809 17763
#define WINDOWS_1903 18362
#define WINDOWS_1909 18363
#define WINDOWS_2004 19041
#define WINDOWS_20H2 19569
#define WINDOWS_21H1 20180

    DWORD GetUserDirectoryTableBaseOffset()
    {
        RTL_OSVERSIONINFOW ver = { 0 };
        RtlGetVersion(&ver);

        switch (ver.dwBuildNumber)
        {
        case WINDOWS_1803:
            return 0x0278;
            break;
        case WINDOWS_1809:
            return 0x0278;
            break;
        case WINDOWS_1903:
            return 0x0280;
            break;
        case WINDOWS_1909:
            return 0x0280;
            break;
        case WINDOWS_2004:
            return 0x0388;
            break;
        case WINDOWS_20H2:
            return 0x0388;
            break;
        case WINDOWS_21H1:
            return 0x0388;
            break;
        default:
            return 0x0388;
        }
    }
    //bundan sonrasý frostieste ait.
    //check normal dirbase if 0 then get from UserDirectoryTableBas
    ULONG_PTR GetProcessCr3(PEPROCESS pProcess)
    {
        PUCHAR process = (PUCHAR)pProcess;
        ULONG_PTR process_dirbase = *(PULONG_PTR)(process + 0x28); //dirbase x64, 32bit is 0x18
        if (process_dirbase == 0)
        {
            DWORD UserDirOffset = GetUserDirectoryTableBaseOffset();
            ULONG_PTR process_userdirbase = *(PULONG_PTR)(process + UserDirOffset);
            return process_userdirbase;
        }
        return process_dirbase;
    }

    ULONG_PTR GetKernelDirBase()
    {
        PUCHAR process = (PUCHAR)PsGetCurrentProcess();
        ULONG_PTR cr3 = *(PULONG_PTR)(process + 0x28); //dirbase x64, 32bit is 0x18
        return cr3;
    }

    NTSTATUS ReadPhysicalAddress(PVOID TargetAddress, PVOID lpBuffer, SIZE_T Size, SIZE_T* BytesRead)
    {
        MM_COPY_ADDRESS AddrToRead = { 0 };
        AddrToRead.PhysicalAddress.QuadPart = (LONGLONG)TargetAddress;
        return MmCopyMemory(lpBuffer, AddrToRead, Size, MM_COPY_MEMORY_PHYSICAL, BytesRead);
    }

    //MmMapIoSpaceEx limit is page 4096 byte
    NTSTATUS WritePhysicalAddress(PVOID TargetAddress, PVOID lpBuffer, SIZE_T Size, SIZE_T* BytesWritten)
    {
        if (!TargetAddress)
            return STATUS_UNSUCCESSFUL;

        PHYSICAL_ADDRESS AddrToWrite = { 0 };
        AddrToWrite.QuadPart = (LONGLONG)TargetAddress;

        PVOID pmapped_mem = MmMapIoSpaceEx(AddrToWrite, Size, PAGE_READWRITE);

        if (!pmapped_mem)
            return STATUS_UNSUCCESSFUL;

        memcpy(pmapped_mem, lpBuffer, Size);

        *BytesWritten = Size;
        MmUnmapIoSpace(pmapped_mem, Size);
        return STATUS_SUCCESS;
    }

#define PAGE_OFFSET_SIZE 12
    static const uint64_t PMASK = (~0xfull << 8) & 0xfffffffffull;

    uint64_t TranslateLinearAddress(uint64_t directoryTableBase, uint64_t virtualAddress) {
        directoryTableBase &= ~0xf;

        uint64_t pageOffset = virtualAddress & ~(~0ul << PAGE_OFFSET_SIZE);
        uint64_t pte = ((virtualAddress >> 12) & (0x1ffll));
        uint64_t pt = ((virtualAddress >> 21) & (0x1ffll));
        uint64_t pd = ((virtualAddress >> 30) & (0x1ffll));
        uint64_t pdp = ((virtualAddress >> 39) & (0x1ffll));

        SIZE_T readsize = 0;
        uint64_t pdpe = 0;
        ReadPhysicalAddress((void*)(directoryTableBase + 8 * pdp), &pdpe, sizeof(pdpe), &readsize);
        if (~pdpe & 1)
            return 0;

        uint64_t pde = 0;
        ReadPhysicalAddress((void*)((pdpe & PMASK) + 8 * pd), &pde, sizeof(pde), &readsize);
        if (~pde & 1)
            return 0;

        /* 1GB large page, use pde's 12-34 bits */
        if (pde & 0x80)
            return (pde & (~0ull << 42 >> 12)) + (virtualAddress & ~(~0ull << 30));

        uint64_t pteAddr = 0;
        ReadPhysicalAddress((void*)((pde & PMASK) + 8 * pt), &pteAddr, sizeof(pteAddr), &readsize);
        if (~pteAddr & 1)
            return 0;

        /* 2MB large page */
        if (pteAddr & 0x80)
            return (pteAddr & PMASK) + (virtualAddress & ~(~0ull << 21));

        virtualAddress = 0;
        ReadPhysicalAddress((void*)((pteAddr & PMASK) + 8 * pte), &virtualAddress, sizeof(virtualAddress), &readsize);
        virtualAddress &= PMASK;

        if (!virtualAddress)
            return 0;

        return virtualAddress + pageOffset;
    }
    NTSTATUS ReadVirtual(uint64_t dirbase, uint64_t address, uint8_t* buffer, SIZE_T size, SIZE_T* read)
    {
        uint64_t paddress = TranslateLinearAddress(dirbase, address);
        return ReadPhysicalAddress((void*)paddress, buffer, size, read);
    }

    NTSTATUS WriteVirtual(uint64_t dirbase, uint64_t address, uint8_t* buffer, SIZE_T size, SIZE_T* written)
    {
        uint64_t paddress = TranslateLinearAddress(dirbase, address);
        return WritePhysicalAddress((void*)paddress, buffer, size, written);
    }

    //
    NTSTATUS ReadProcessMemory(int pid, PVOID Address, PVOID AllocatedBuffer, SIZE_T size, SIZE_T* read)
    {
        PEPROCESS pProcess = NULL;
        if (pid == 0) return STATUS_UNSUCCESSFUL;

        NTSTATUS NtRet = PsLookupProcessByProcessId((HANDLE)pid, &pProcess);
        if (NtRet != STATUS_SUCCESS) return NtRet;

        ULONG_PTR process_dirbase = GetProcessCr3(pProcess);
        ObDereferenceObject(pProcess);

        SIZE_T CurOffset = 0;
        SIZE_T TotalSize = size;
        while (TotalSize)
        {

            uint64_t CurPhysAddr = TranslateLinearAddress(process_dirbase, (ULONG64)Address + CurOffset);
            if (!CurPhysAddr) return STATUS_UNSUCCESSFUL;

            ULONG64 ReadSize = min(PAGE_SIZE - (CurPhysAddr & 0xFFF), TotalSize);
            SIZE_T BytesRead = 0;
            NtRet = ReadPhysicalAddress((void*)CurPhysAddr, (PVOID)((ULONG64)AllocatedBuffer + CurOffset), ReadSize, &BytesRead);
            TotalSize -= BytesRead;
            CurOffset += BytesRead;
            if (NtRet != STATUS_SUCCESS) break;
            if (BytesRead == 0) break;
        }

        *read = CurOffset;
        return NtRet;
    }

    NTSTATUS WriteProcessMemory(int pid, PVOID Address, PVOID AllocatedBuffer, SIZE_T size, SIZE_T* written)
    {
        PEPROCESS pProcess = NULL;
        if (pid == 0) return STATUS_UNSUCCESSFUL;

        NTSTATUS NtRet = PsLookupProcessByProcessId((HANDLE)pid, &pProcess);
        if (NtRet != STATUS_SUCCESS) return NtRet;

        ULONG_PTR process_dirbase = GetProcessCr3(pProcess);
        ObDereferenceObject(pProcess);

        SIZE_T CurOffset = 0;
        SIZE_T TotalSize = size;
        while (TotalSize)
        {
            uint64_t CurPhysAddr = TranslateLinearAddress(process_dirbase, (ULONG64)Address + CurOffset);
            if (!CurPhysAddr) return STATUS_UNSUCCESSFUL;

            ULONG64 WriteSize = min(PAGE_SIZE - (CurPhysAddr & 0xFFF), TotalSize);
            SIZE_T BytesWritten = 0;
            NtRet = WritePhysicalAddress((void*)CurPhysAddr, (PVOID)((ULONG64)AllocatedBuffer + CurOffset), WriteSize, &BytesWritten);
            TotalSize -= BytesWritten;
            CurOffset += BytesWritten;
            if (NtRet != STATUS_SUCCESS) break;
            if (BytesWritten == 0) break;
        }

        *written = CurOffset;
        return NtRet;
    }
}