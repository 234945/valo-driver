#include "imports.h"
#include "mouse.hpp"
#include "clean.hpp"
struct _requests {
	uint32_t    src_pid;
	uint64_t    src_addr;
	uint32_t    dst_pid;
	uint64_t    dst_addr;
	size_t        size;
	size_t transfer;
	std::uintptr_t   buffer;
	uint32_t	pid;
	int request_key;
	std::uintptr_t allocation;
	int x;
	int y;
};

NTSTATUS write_process_memory(uint32_t pid, uint32_t user_pid, uintptr_t addr, uintptr_t buffer, size_t size, size_t* bytes_written)
{
	NTSTATUS status = STATUS_SUCCESS;
	PEPROCESS user_proc;
	PsLookupProcessByProcessId((HANDLE)user_pid, &user_proc);
	PEPROCESS target_proc;
	PsLookupProcessByProcessId((HANDLE)pid, &target_proc);

	size_t processed;
	status = MmCopyVirtualMemory(user_proc, (void*)buffer, target_proc, (void*)addr, size, UserMode, &processed);

	ObDereferenceObject(user_proc);
	ObDereferenceObject(target_proc);

	if (!NT_SUCCESS(status)) return status;
	if (bytes_written) *bytes_written = processed;

	if (processed != size)
		return STATUS_FAIL_CHECK;
	return STATUS_SUCCESS;
}
NTSTATUS read_process_memory(uint32_t pid, uint32_t user_pid, uintptr_t addr, uintptr_t buffer, size_t size, size_t* bytes_read)
{
	NTSTATUS status = STATUS_SUCCESS;
	PEPROCESS user_proc;
	PsLookupProcessByProcessId((HANDLE)user_pid, &user_proc);
	PEPROCESS target_proc;
	PsLookupProcessByProcessId((HANDLE)pid, &target_proc);

	size_t processed;
	status = MmCopyVirtualMemory(target_proc, (void*)addr, user_proc, (void*)buffer, size, UserMode, &processed);
	if (!NT_SUCCESS(status)) return status;
	if (bytes_read) *bytes_read = processed;

	if (processed != size)
		return STATUS_FAIL_CHECK;
	return STATUS_SUCCESS;
}
MOUSE_OBJECT mouse_obj = { 0 };

bool core_dispatcher(_requests* pstruct)
{
	if (pstruct->request_key == 31)//1337
	{
		PEPROCESS game;
		NTSTATUS status = PsLookupProcessByProcessId((HANDLE)pstruct->pid, &game);
		uint64_t deger = (uintptr_t)PsGetProcessSectionBaseAddress(game);
		pstruct->allocation = deger;
		dbg("baseaddr: %p", deger);
	}
	else if (pstruct->request_key == 32)
	{
		NTSTATUS status = read_process_memory(pstruct->pid, pstruct->dst_pid, pstruct->src_addr, pstruct->buffer, pstruct->size, &pstruct->transfer);

	}
	else if (pstruct->request_key == 33)
	{
		NTSTATUS status = write_process_memory(pstruct->pid, pstruct->dst_pid, pstruct->src_addr, pstruct->buffer, pstruct->size, &pstruct->transfer);

	}
	else if (pstruct->request_key == 1337)//31
	{
		if (!mouse_obj.service_callback || !mouse_obj.mouse_device) { mouse::init_mouse(&mouse_obj); }
		mouse::mouse_event(mouse_obj, pstruct->x, pstruct->y, 0);

	}
	
	return true;
}
void hook_fn(std::uintptr_t rcx)
{
	_requests* in = (_requests*)rcx;
	core_dispatcher(in);
}
auto clear_traces()-> bool
{
	UNICODE_STRING driver_name = RTL_CONSTANT_STRING(L"Capcom.sys");//Capcom.sys 0x57cd1415 (timeDateStamp)
	//log("Hello from Kernel Mode");
	clear::clearCache(driver_name, 0x57cd1415);
	FindMmDriverData();
	if (clear::ClearUnloadedDriver(&driver_name, true) == STATUS_SUCCESS) 
		return true;
	
	else 
		return false;
	
}
auto driver_entry() -> const NTSTATUS
{
	bool cleaning_state = clear_traces();
    std::uintptr_t win32k = utils::get_system_module(L"win32k.sys");
	globals::hook_address = win32k + 0x65A18; //0x65A08
	globals::original_hook_pointer = *reinterpret_cast<std::uintptr_t*>(globals::hook_address);
	*reinterpret_cast<std::uintptr_t*>(globals::hook_address) = reinterpret_cast<std::uintptr_t>(&hook_fn);
	return STATUS_SUCCESS;
}