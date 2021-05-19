ENTRY(Reset_Handler)

_estack = 0x10040000;

_Min_Heap_Size  = 0x1000;
_Min_Stack_Size = 0x400;
__SHARED_PTR_SIZE = 256;

MEMORY
{
	m_interrupts (RX)  	: ORIGIN = 0x00000000, LENGTH = 0x00000440
	m_text       (RX)  	: ORIGIN = 0x10000000, LENGTH = 0x00020000
	m_data       (RW)  	: ORIGIN = 0x10020000, LENGTH = 0x00020000
	SHAREDMEM    (RW)	: ORIGIN = 0x10040000, LENGTH = (128K - __SHARED_PTR_SIZE) /* shared with ca7 */
}

_params_ptr = ORIGIN(SHAREDMEM) + LENGTH(SHAREDMEM);
/* 0x1005FF00: 256 Bytes of shared memory addresses = 64 pointers */

SECTIONS
{
  .isr_vector :
  {
    . = ALIGN(4);
    KEEP(*(.isr_vector))
    . = ALIGN(4);
  } > m_interrupts
 

  .text :
  {
    . = ALIGN(4);
    *(.text)           /* .text sections (code) */
    *(.text*)          /* .text* sections (code) */
    *(.glue_7)         /* glue arm to thumb code */
    *(.glue_7t)        /* glue thumb to arm code */
    *(.eh_frame)

    KEEP (*(.init))
    KEEP (*(.fini))

    . = ALIGN(4);
    _etext = .;        /* define a global symbols at end of code */
  } > m_text

  /* Constant data into ROM memory*/
  .rodata :
  {
    . = ALIGN(4);
    *(.rodata)         /* .rodata sections (constants, strings, etc.) */
    *(.rodata*)        /* .rodata* sections (constants, strings, etc.) */
    . = ALIGN(4);
  } > m_text

  .ARM.extab   : { 
  	. = ALIGN(4);
  	*(.ARM.extab* .gnu.linkonce.armextab.*)
  	. = ALIGN(4);
  } > m_text
  
  .ARM : {
    . = ALIGN(4);
    __exidx_start = .;
    *(.ARM.exidx*)
    __exidx_end = .;
    . = ALIGN(4);
  } > m_text

  .preinit_array     :
  {
    . = ALIGN(4);
    PROVIDE_HIDDEN (__preinit_array_start = .);
    KEEP (*(.preinit_array*))
    PROVIDE_HIDDEN (__preinit_array_end = .);
    . = ALIGN(4);
  } > m_text
  
  .init_array :
  {
    . = ALIGN(4);
    PROVIDE_HIDDEN (__init_array_start = .);
    KEEP (*(SORT(.init_array.*)))
    KEEP (*(.init_array*))
    PROVIDE_HIDDEN (__init_array_end = .);
    . = ALIGN(4);
  } > m_text
  
  .fini_array :
  {
    . = ALIGN(4);
    PROVIDE_HIDDEN (__fini_array_start = .);
    KEEP (*(SORT(.fini_array.*)))
    KEEP (*(.fini_array*))
    PROVIDE_HIDDEN (__fini_array_end = .);
    . = ALIGN(4);
  } > m_text

  /* Used by the startup to initialize data */
  __DATA_ROM = .;
  _sidata = LOADADDR(.data);

  /* Initialized data sections */
  .data :  AT(__DATA_ROM)
  {
    . = ALIGN(4);
    _sdata = .;        /* create a global symbol at data start */
    *(.data)           /* .data sections */
    *(.data*)          /* .data* sections */

    . = ALIGN(4);
    _edata = .;        /* define a global symbol at data end */
  } > m_data

  __DATA_END = __DATA_ROM + (_edata - _sdata);
  text_end = ORIGIN(m_text) + LENGTH(m_text);
  ASSERT(__DATA_END <= text_end, "region m_text overflowed with text and data")

  /* .resource_table : */
  /* { */
  /*   . = ALIGN(4); */
  /*   KEEP (*(.resource_table*)) */
  /*   . = ALIGN(4); */
  /* } > m_data */
  

  /* Uninitialized data section into RAM memory */
  . = ALIGN(4);
  .bss :
  {
    /* This is used by the startup in order to initialize the .bss secion */
    _sbss = .;         /* define a global symbol at bss start */
    __bss_start__ = _sbss;
    *(.bss)
    *(.bss*)
    *(COMMON)

    . = ALIGN(4);
    _ebss = .;         /* define a global symbol at bss end */
    __bss_end__ = _ebss;
  } > m_data

  /* User_heap_stack section, used to check that there is enough RAM left */
  ._user_heap_stack :
  {
    . = ALIGN(8);
    PROVIDE ( end = . );
    PROVIDE ( _end = . );
	_sheap = .;
    . = . + _Min_Heap_Size;
	_eheap = .;
    . = . + _Min_Stack_Size;
    . = ALIGN(8);
  } > m_data

  

  /* Remove information from the compiler libraries */
  /DISCARD/ :
  {
    libc.a ( * )
    libm.a ( * )
    libgcc.a ( * )
  }

  .ARM.attributes 0 : { *(.ARM.attributes) }

}
