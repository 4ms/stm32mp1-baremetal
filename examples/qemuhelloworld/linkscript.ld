ENTRY(_Reset)
SECTIONS
{
 . = 0x10000;
 .startup . : { startup.o(.text) }
 .text : { *(.text) }
 .data : { *(.data) }
 .bss : { 
	__bss_start__ = .;
	*(.bss COMMON)
	__bss_end__ = .;
  }
  . = ALIGN(8);

  __exidx_start = .;
  .ARM.exidx   : { *(.ARM.exidx* .gnu.linkonce.armexidx.*) }
  __exidx_end = .;
  . = ALIGN(8);
 
 . = . + 0x10000; /* 256kB of stack memory */
 stack_top = .;
 end = .;
}
