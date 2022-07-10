/* SYSRAM starts at 0x2FFC'0000 but first 0x2400 bytes are reserved for
   the BootROM data/bss segments. FSBL will be loaded at 0x2FFC2400.
*/
MEMORY {
    SYSRAM (rwx)    :   ORIGIN = 0x2FFC2400, LENGTH = (256K - 0x2400)
}

ENTRY(_Reset);

__STACK_START = 0x30000000;
__HEAP_SIZE = 0x0;

SECTIONS {
    /* 0x100 bytes at the start reserved for header */
    .header : {
        FILL(0xFF);
        BYTE(0xFF);
        . = 0x100;
    } >SYSRAM

    .text : {
        *(.vector_table)
        *(.text)
        *(.text*)

		*(.glue_7)         /* glue arm to thumb code */
		*(.glue_7t)        /* glue thumb to arm code */
		*(.eh_frame)

        KEEP(*(.init))
        KEEP(*(.fini))
    } >SYSRAM

    .rodata : {
        . = ALIGN(4);
        *(.rodata)
        *(.rodata*)
        . = ALIGN(4);
    } >SYSRAM

    .preinit_array : {
        PROVIDE_HIDDEN (__preinit_array_start = .);
        KEEP (*(.preinit_array*))
        PROVIDE_HIDDEN (__preinit_array_end = .);
    } >SYSRAM

    .init_array : {
        PROVIDE_HIDDEN (__init_array_start = .);
        KEEP (*(SORT(.init_array.*)))
        KEEP (*(.init_array*))
        PROVIDE_HIDDEN (__init_array_end = .);
    } >SYSRAM

    .fini_array : {
        PROVIDE_HIDDEN (__fini_array_start = .);
        KEEP (*(SORT(.fini_array.*)))
        KEEP (*(.fini_array*))
        PROVIDE_HIDDEN (__fini_array_end = .);
    } >SYSRAM

    _text_end = .;

    .data : {
        . = ALIGN(4);
        _data_start = .;        
        *(.data)       
        *(.data*)        

        . = ALIGN(4);
        _data_end = .;   
    } >SYSRAM

    .bss : {
        . = ALIGN(4);
        _bss_start = .;
        *(.bss)
        *(.bss*)
        *(COMMON)
        . = ALIGN(4);
        _bss_end = .;
    } >SYSRAM

	.heap (NOLOAD):
	{
		. = ALIGN(8);
		_sheap = .;
		. += __HEAP_SIZE;
		_eheap = .;
		. = ALIGN(256);
		_ram_aligned_end = .;
	} > SYSRAM


	_user_stack_end = __STACK_START;
	_user_stack_start = _user_stack_end - 0x1000;

	_svc_stack_end = _user_stack_start;
	_svc_stack_start = _svc_stack_end - 0x100;

	_irq_stack_end = _svc_stack_start;
	_irq_stack_start = _irq_stack_end - 0x100;

	_fiq_stack_end = _irq_stack_start;
	_fiq_stack_start = _fiq_stack_end - 0x100;

  /* Remove information from the compiler libraries */
  /DISCARD/ :
  {
    libc.a ( * )
    libm.a ( * )
    libgcc.a ( * )
  }

	.ARM.attributes 0 : { *(.ARM.attributes) }

}
