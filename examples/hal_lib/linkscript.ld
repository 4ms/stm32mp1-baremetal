OUTPUT_FORMAT("elf32-littlearm", "elf32-littlearm", "elf32-littlearm")
OUTPUT_ARCH(arm)
ENTRY(_Reset)

MEMORY
{
    ROM (rx) : ORIGIN = 0xC2000040, LENGTH = 1M
    RAM (rwx): ORIGIN = 0xC0200000, LENGTH = 1M
	SRAM1 (rwx) : ORIGIN = 0x10000000, LENGTH = 128K
	SRAM2 (rwx) : ORIGIN = 0x10020000, LENGTH = 128K
	SRAM3 (rwx) : ORIGIN = 0x10040000, LENGTH = 64K
	SRAM4 (rwx) : ORIGIN = 0x10050000, LENGTH = 64K
}

__HEAP_SIZE = 0x40000;

SECTIONS
{
    .text : {
        *(.vector_table)
        *(.text)
        *(.text*)
					/* Todo: check if we need the next 3 lines */
		*(.glue_7)         /* glue arm to thumb code */
		*(.glue_7t)        /* glue thumb to arm code */
		*(.eh_frame)

		KEEP (*(.init)) /* libc ctors */
		KEEP (*(.fini)) /* libc dtors */

        . = ALIGN(8);
     } > ROM

	/* .rodata sections (constants, strings, etc.) */
	.rodata :
	{
		. = ALIGN(8);
		*(.rodata)         
		*(.rodata*) 
		. = ALIGN(8);
	} > ROM

	/* used for unwinding (probably not used, but is ignored if your app doens't use exceptions */
	.ARM.extab	 : { *(.ARM.extab* .gnu.linkonce.armextab.*) } >ROM
	.ARM : {
		__exidx_start = .;
		*(.ARM.exidx*)
		__exidx_end = .;
	} >ROM

	.preinit_array :
	{
		. = ALIGN(8);
		PROVIDE_HIDDEN (__preinit_array_start = .);
		KEEP (*(.preinit_array*))
		PROVIDE_HIDDEN (__preinit_array_end = .);
		. = ALIGN(8);
	} > ROM

	.init_array :
	{
		. = ALIGN(8);
		PROVIDE_HIDDEN (__init_array_start = .);
		KEEP (*(SORT(.init_array.*)))
		KEEP (*(.init_array*))
		PROVIDE_HIDDEN (__init_array_end = .);
		. = ALIGN(8);
	} > ROM

	.fini_array :
	{
		. = ALIGN(8);
		PROVIDE_HIDDEN (__fini_array_start = .);
		KEEP (*(SORT(.fini_array.*)))
		KEEP (*(.fini_array*))
		PROVIDE_HIDDEN (__fini_array_end = .);
		. = ALIGN(8);
	} > ROM


    _text_end = .;

    .data : AT(_text_end)
    {
        . = ALIGN(8);
        _data_start = .;
        *(.data)
        *(.data*)
        . = ALIGN(8);
        _data_end = .;
    } > RAM

    .bss : {
        _bss_start = .;
        *(.bss)
        *(.bss*)   /* required for some malloc calls */
		*(COMMON)  /* required for libc, such as __lock___atexit_recursive_mutex */
        . = ALIGN(8);
        _bss_end = .;
    } > RAM

	.heap (NOLOAD):
	{
		. = ALIGN(8);
		_sheap = .;
		. += __HEAP_SIZE;
		_eheap = .;
		. = ALIGN(256);
		_ram_aligned_end = .;
	} > RAM
	. = ALIGN(4);
	end = . ;

    _user_stack_start = _ram_aligned_end; 
    _user_stack_end = _user_stack_start + 0x10000;

    _svc_stack_start = _user_stack_end;
    _svc_stack_end = _svc_stack_start + 0x100;

    _irq_stack_start = _svc_stack_end;
    _irq_stack_end = _irq_stack_start + 0x100; 

    _fiq_stack_start = _irq_stack_end;
    _fiq_stack_end = _fiq_stack_start + 0x100;


	.ARM.attributes 0 : { *(.ARM.attributes) }
}
