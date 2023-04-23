namespace globals
{
	uintptr_t ntoskrnl = 0; //
	uintptr_t win32kbase = 0; //
	uintptr_t win32kfull = 0; //
	//yukardakiler eskiden kalma quad chained yapýoydum þimdi 1 taneye geçirdim.
	 
	std::uintptr_t original_hook_pointer = 0;
	std::uintptr_t hook_address = 0;
}