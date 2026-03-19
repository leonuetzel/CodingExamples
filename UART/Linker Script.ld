ENTRY(EXCEPTION_RESET)



MEMORY
{
	
	FLASH_BANK_1		(rx)	: ORIGIN = 0x08000000, LENGTH = 1024K
	FLASH_BANK_2		(rx)	: ORIGIN = 0x08100000, LENGTH = 1024K
	
	SRAM_ITCM				(rwx)	: ORIGIN = 0x00000000, LENGTH = 64K
	SRAM_DTCM				(rwx)	: ORIGIN = 0x20000000, LENGTH = 128K
  SRAM_AXI				(rwx)	: ORIGIN = 0x24000000, LENGTH = 512K
	SRAM_1					(rwx)	: ORIGIN = 0x30000000, LENGTH = 128K
	SRAM_2					(rwx)	: ORIGIN = 0x30020000, LENGTH = 128K
	SRAM_3					(rwx)	: ORIGIN = 0x30040000, LENGTH = 32K
	SRAM_4					(rwx)	: ORIGIN = 0x38000000, LENGTH = 64K
	SRAM_BACKUP			(rwx)	: ORIGIN = 0x38800000, LENGTH = 4K
}



__memory_FLASH_BANK_1_start__		= ORIGIN(FLASH_BANK_1);
__memory_FLASH_BANK_1_size__		= LENGTH(FLASH_BANK_1);
__memory_FLASH_BANK_2_start__		= ORIGIN(FLASH_BANK_2);
__memory_FLASH_BANK_2_size__		= LENGTH(FLASH_BANK_2);
__memory_SRAM_ITCM_start__			= ORIGIN(SRAM_ITCM);
__memory_SRAM_ITCM_size__				= LENGTH(SRAM_ITCM);
__memory_SRAM_DTCM_start__			= ORIGIN(SRAM_DTCM);
__memory_SRAM_DTCM_size__				= LENGTH(SRAM_DTCM);
__memory_SRAM_AXI_start__				= ORIGIN(SRAM_AXI);
__memory_SRAM_AXI_size__				= LENGTH(SRAM_AXI);
__memory_SRAM_1_start__					= ORIGIN(SRAM_1);
__memory_SRAM_1_size__					= LENGTH(SRAM_1);
__memory_SRAM_2_start__					= ORIGIN(SRAM_2);
__memory_SRAM_2_size__					= LENGTH(SRAM_2);
__memory_SRAM_3_start__					= ORIGIN(SRAM_3);
__memory_SRAM_3_size__					= LENGTH(SRAM_3);
__memory_SRAM_4_start__					= ORIGIN(SRAM_4);
__memory_SRAM_4_size__					= LENGTH(SRAM_4);
__memory_SRAM_BACKUP_start__		= ORIGIN(SRAM_BACKUP);
__memory_SRAM_BACKUP_size__			= LENGTH(SRAM_BACKUP);





/*				CUSTOMIZE YOUR OS				*/
/*	Every Value's Unit is a Byte	*/

/*	Heap Settings	*/

__cmos_heap_start__							= __memory_SRAM_AXI_start__;
__cmos_heap_size__							= __memory_SRAM_AXI_size__;
__cmos_heap_blockSize__					= 32;


/*	Stack Settings	*/

__cmos_stack_size__							= 65536;
__cmos_stack_blockSize__				= 4096;

__cmos_mainStackSize__					= 4096;


/*	Threads	*/

__cmos_numberOfThreads__				= 16;









/*																		*/
/*	DO NOT CHANGE ANYTHING UNDER HERE	*/
/*																		*/









/*	OS specific Settings	*/
__cmos_initial_stackpointer__ 	= __memory_SRAM_AXI_start__ + __memory_SRAM_AXI_size__;
__cmos_heap_blockNumber__				= __cmos_heap_size__ / __cmos_heap_blockSize__;
__cmos_stack_blockNumber__			= __cmos_stack_size__ / __cmos_stack_blockSize__;







SECTIONS
{
	/*
		EXCEPTION_VECTOR and INTERRUPT_VECTOR
		
		Storage Static:		FLASH_BANK_1
		Storage Runtime:	FLASH_BANK_1
	*/
  .vectorTable 0x08000000:
  {
		KEEP(*(.exceptionVector))
    KEEP(*(.interruptVector))
    . = ALIGN(32);
  } >FLASH_BANK_1
	
	
	
	
	
	/*
		Null Pointer Catching
		
		Storage Static:		---
		Storage Runtime:	SRAM_ITCM
	*/
  .null 0x00000000:
  {
		. += 1;
    . = ALIGN(32);
  } >SRAM_ITCM
	
	
	
	
	
	/*
		Stack
		
		Storage Static:		---
		Storage Runtime:	SRAM_DTCM
	*/
  .stack (NOLOAD):
  {
		. = ALIGN(__cmos_stack_blockSize__);
		__cmos_stack_start__ = .;
		. += __cmos_stack_size__;
    . = ALIGN(32);
  } >SRAM_DTCM
	
	
	
	
	
	/*
		CODE_RAM
		
		Storage Static:		FLASH_BANK_1
		Storage Runtime:	SRAM_ITCM
	*/
	.code_ram :
  {
    . = ALIGN(32);
    *(.code_ram)
		*(.code_ram.*)
    . = ALIGN(32);
  } >SRAM_ITCM AT> FLASH_BANK_1
	
	__cmos_code_flash_start__ = LOADADDR(.code_ram);
	__cmos_code_flash_size__ = SIZEOF(.code_ram);
  __cmos_code_ram_start__ = ADDR(.code_ram);
	
	
	
	
	
	/*
		Stack Block Owner, Heap Block Owner, System Threads, Zero-initialized SRAM
		
		Storage Static:		---
		Storage Runtime:	SRAM_DTCM
	*/
	.sram_0 (NOLOAD):
  {
		. = ALIGN(32);
		__cmos_stack_blockOwner__ = .;
		. += __cmos_stack_blockNumber__;
		
		. = ALIGN(32);
		__cmos_heap_blockOwner__ = .;
		. += __cmos_heap_blockNumber__ / 4;
		
		. = ALIGN(32);
    *(.bss)
		*(.bss*)
    *(COMMON)
		
    . = ALIGN(32);
  } >SRAM_DTCM
	
	
	
	
	
	/*
		Static Initialization Code
		
		Storage Static:		FLASH_BANK_1
		Storage Runtime:	FLASH_BANK_1
	*/
	.constructors :
  {
	  . = ALIGN(32);
	  KEEP (*(.init_array*))
	  . = ALIGN(32);
  } >FLASH_BANK_1
	
	__cmos_constructor_start__ = LOADADDR(.constructors);
	__cmos_constructor_size__ = SIZEOF(.constructors);
	
	
	
	
	
	/*
		V-Tables
		
		Storage Static:		FLASH_BANK_1
		Storage Runtime:	FLASH_BANK_1
	*/
	.vtables :
  {
    . = ALIGN(32);
		*(.rodata._ZTV*)
    . = ALIGN(32);
  } >FLASH_BANK_1
	
	
	
	
	
	/*
		Code
		
		Storage Static:		FLASH_BANK_1
		Storage Runtime:	FLASH_BANK_1
	*/
	.code :
	{
		. = ALIGN(32);
		*(.text)
		*(.text*)
    . = ALIGN(32);
	} >FLASH_BANK_1
	
	
	
	
	
	/*
		DATA_CONST
		
		Storage Static:		FLASH_BANK_1
		Storage Runtime:	FLASH_BANK_1
	*/
	.const :
	{
		. = ALIGN(32);
		*(.data_const)
		*(.data_const.*)
    . = ALIGN(32);
	} >FLASH_BANK_1
	
	
	
	
	
	/*
		Data
		
		Storage Static:		FLASH_BANK_1
		Storage Runtime:	SRAM_DTCM
	*/
  .data_readonly :
  {
		. = ALIGN(32);
    *(.rodata*)
    . = ALIGN(32);
  } >FLASH_BANK_1
	
	
	
	.data :
  {
    . = ALIGN(32);
    *(.data)
		*(.data.*)
    . = ALIGN(32);
  } >SRAM_DTCM AT> FLASH_BANK_1
	
	__cmos_data_flash_start__ = LOADADDR(.data);
	__cmos_data_flash_size__ = SIZEOF(.data);
  __cmos_data_ram_start__ = ADDR(.data);
	
	
	
	
	
	/*
		Heap
		
		Storage Static:		---
		Storage Runtime:	SRAM_AXI
	*/
	.heap (NOLOAD):
	{
		. = ALIGN(32);
		. += __cmos_heap_size__;
		. = ALIGN(32);
	} >SRAM_AXI
	
	__cmos_heap_start__ = ADDR(.heap);
	__cmos_heap_size__ = SIZEOF(.heap);
	
	
	
	
	
	/*
		DATA_DMA
		
		Storage Static:		---
		Storage Runtime:	SRAM_1
	*/
	.dma (NOLOAD):
  {
		. = ALIGN(32);
		*(.data_dma)
	  *(.data_dma*)
  } >SRAM_1
	
	
	
	

  /DISCARD/ :
  {
    libc.a ( * )
    libm.a ( * )
    libgcc.a ( * )
		libc_nano.a ( * )
		*(.fini_array)
		*(.ARM.attributes)
  }
}