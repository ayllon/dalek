/* Null descriptor */
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
