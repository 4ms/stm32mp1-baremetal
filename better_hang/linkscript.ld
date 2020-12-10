ENTRY(_Reset)

SECTIONS
{
    . = 0xC2000040;
    .text : { startup.o (.vector_table) *(.text) } 
    . = ALIGN(8);
}
