OUTPUT_FORMAT("elf32-littlearm", "elf32-littlearm", "elf32-littlearm")
OUTPUT_ARCH(arm)

MEMORY
{
    RAM (rx) : ORIGIN = 0xD0000000, LENGTH =   0x00001000
    INIT (rx) : ORIGIN = 0xD0001000, LENGTH =   0x00001000
}

__HEAP_SIZE = 0x40000;

SECTIONS
{
    .text : {
		KEEP (*(.ismain))
		*(.text)
        *(.text*)

					/* Todo: check if we need the next 3 lines */
		*(.glue_7)         /* glue arm to thumb code */
		*(.glue_7t)        /* glue thumb to arm code */
		/* *(.eh_frame) */

		KEEP (*(.init)) /* libc ctors */
		KEEP (*(.fini)) /* libc dtors */

        . = ALIGN(8);
     } > RAM

	/* .rodata sections (constants, strings, etc.) */
	.rodata :
	{
		. = ALIGN(8);
		*(.rodata)         
		*(.rodata*) 
		. = ALIGN(8);
	} > RAM

	/* used for unwinding (probably not used, but is ignored if your app doens't use exceptions */
	/* .ARM.extab	 : { *(.ARM.extab* .gnu.linkonce.armextab.*) } >ROM */
	/* .ARM : { */
	/* 	__exidx_start = .; */
	/* 	*(.ARM.exidx*) */
	/* 	__exidx_end = .; */
	/* } >ROM */

	.preinit_array :
	{
		. = ALIGN(8);
		PROVIDE_HIDDEN (__preinit_array_start = .);
		KEEP (*(.preinit_array*))
		PROVIDE_HIDDEN (__preinit_array_end = .);
		. = ALIGN(8);
	} > INIT

	.init_array :
	{
		. = ALIGN(8);
		PROVIDE_HIDDEN (__init_array_start = .);
		KEEP (*(SORT(.init_array.*)))
		KEEP (*(.init_array*))
		KEEP (*(.got))
		PROVIDE_HIDDEN (__init_array_end = .);
		. = ALIGN(8);
	} > INIT

	.fini_array :
	{
		. = ALIGN(8);
		PROVIDE_HIDDEN (__fini_array_start = .);
		KEEP (*(SORT(.fini_array.*)))
		KEEP (*(.fini_array*))
		PROVIDE_HIDDEN (__fini_array_end = .);
		. = ALIGN(8);
	} > INIT


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
        . = ALIGN(8);
        _bss_start = .;
        *(.bss)
        *(.bss*)   /* required for some malloc calls */
		*(COMMON)  /* required for libc, such as __lock___atexit_recursive_mutex */
        . = ALIGN(8);
        _bss_end = .;
		. = ALIGN(256);
		_ram_aligned_end = .;
    } > RAM


  /* Remove information from the compiler libraries */
  /DISCARD/ :
  {
    libc.a ( * )
    libm.a ( * )
    libgcc.a ( * )
  }

	.ARM.attributes 0 : { *(.ARM.attributes) }
}
