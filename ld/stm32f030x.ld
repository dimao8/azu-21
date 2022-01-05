MEMORY
{
  FLASH (rx)  : ORIGIN = 0x08000000, LENGTH = 0x00004000
  RAM   (rwx) : ORIGIN = 0x20000000, LENGTH = 0x00001000
}

_end_stack = 0x20000FFF;

ENTRY(Reset_Handler)

SECTIONS
{
	. = ORIGIN(FLASH);

	.vectors :
	{
		*(.vectors)
	} > FLASH

	.text :
	{
		*(.text)
		*(.text*)
		*(.rodata)
		*(.rodata*)
		_end_text = .;
	} > FLASH

	.data :
	{
		_start_data = .;
		*(.data)
		_end_data = .;
	} > RAM AT > FLASH

	. = ALIGN(4);

	.bss :
	{
		_start_bss = .;
		*(.bss)
		_end_bss = .;
	} > RAM

	. = ALIGN(4);

	_start_stack = .;
}

_end = .;
PROVIDE(end = .);
