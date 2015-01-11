/**
Access Byte
===========
Present | Privilege (2) | 1 | Executable | Direction/Conforming | RW | Accessed
	- Present: 1 for valid selector
	- Privilege: Ring level
	- Executable: 1 for code
	- Direction/Conforming
		* For data
			+ 0 segment grows up, 1 down
		* For code
			+ 0 can only be executed from ring in Privilege
			+ 1 can be called from equal or lower privilege
	- RW
		* For data, 1 to allow write
		* For code, 1 to allow read
	- Accessed
		* Set to 1 by the CPU when accessed

Flags
=====
Granularity | Size | 0 | 0
	- Granularity: 0 for byte, 1 for page (4 KiB)
	- Size: 0 for 16 bits protected mode, 32 for 32 bits protected mode
*/

/* Null descriptor (0x00) */
.word 0x00, 0x00, 0x00, 0x00

/* code descriptor (0x8) */
.word	0xFFFF		/* limit low   */
.word	0x0000		/* base low    */
.byte	0x00		/* base middle */
.byte	0b10011010	/* access byte */
.byte	0b11001111	/* flag + limit */
.byte	0x00 		/* base high   */

/* data descriptor (0x10) */
.word	0xFFFF		/* limit low */
.word	0x0000		/* base low */
.byte	0x00		/* base middle */
.byte	0b10010010	/* access byte */
.byte	0b11001111	/* flag + limit */
.byte	0x00		/* base high */

/* real mode code descriptor (0x18) */
.word	0xFFFF      /* limit low */
.word	0x0000      /* base low */
.byte	0x00        /* base middle */
.byte   0b10011110  /* access byte */
.byte   0x00        /* flag + limit */
.byte   0x00        /* base high */

/* real mode data descriptor (0x20) */
.word	0xFFFF      /* limit low */
.word	0x0000      /* base low */
.byte	0x00        /* base middle */
.byte   0b10010010  /* access byte */
.byte   0x00        /* flag + limit */
.byte   0x00        /* base high */
