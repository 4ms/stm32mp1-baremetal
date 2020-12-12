OUTPUT_FORMAT("elf32-littlearm", "elf32-littlearm", "elf32-littlearm")
OUTPUT_ARCH(arm)
ENTRY(_Reset)

MEMORY
{
    ROM (rx) : ORIGIN = 0xC2000040, LENGTH = 1M
    RAM (rwx): ORIGIN = 0xC0200000, LENGTH = 1M
}

SECTIONS
{
    .text : {
        build/startup.o (.vector_table)
        *(.text*)
        *(.rodata*)
        . = ALIGN(8);
     } > ROM

    _text_end = .;

    .data : AT(ADDR(.text) + SIZEOF(.text))
    {
        . = ALIGN(8);
        _data_start = .;
        *(.data)
        . = ALIGN(8);
        _data_end = .;
    } > RAM

    .bss : {
        _bss_start = .;
        *(.bss)
        . = ALIGN(8);
        _bss_end = .;
    } > RAM

    _fiq_stack_start = ADDR(.bss) + SIZEOF(.bss);
    _fiq_stack_end = _fiq_stack_start + 0x1000; /* 64 KB */

    _irq_stack_start = _fiq_stack_end;
    _irq_stack_end = _irq_stack_start + 0x1000; /* 64 KB */

    _stack_start = _irq_stack_end;
    _stack_end = _stack_start + 0x1000; /* 64 KB */

    _irq_stack_size = _irq_stack_end - _irq_stack_start;
    _fiq_stack_size = _fiq_stack_end - _fiq_stack_start;
}
