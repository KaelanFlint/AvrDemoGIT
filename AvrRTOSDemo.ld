ENTRY(cstart)

MEMORY
{
	text (rx): org = 0x00000000, len = 0x8000
	data (rw!x):	org = 0x00800000, len = 0x900
	eeprom (r):		org = 0x00810000, len = 0x400
	fuse (r):		org = 0x00820000, len = 0x400
	lock (r):		org = 0x00830000, len = 0x400
	signature (r)	org = 0x00840000, len = 0x400
}

SECTIONS
{
	.text :
	{
	
	} > text
	.vectors :
	{
		*(APPLVECT)
		. = ALIGN(8) ;
	} > text =0
	
	
  . = 0x10000;
  .text : { *(.text) }
  . = 0x8000000;
  .data : { *(.data) }
  .bss : { *(.bss) }
}