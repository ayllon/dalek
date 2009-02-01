/* Null descriptor */
.word 0x00, 0x00, 0x00, 0x00

/* code descriptor (0x8) */
.word	0x0FFFF		/* limit low   */
.word	0x00000		/* base low    */
.byte	0x00		/* base middle */
.byte	0b10011010	/* access      */
.byte	0b11001111	/* granularity */
.byte	0x00 		/* base high   */

/* data descriptor (0x10) */
.word	0x0FFFF		/* limit low (Same as code) */
.word	0x00000		/* base low                 */
.byte	0x00		/* base middle              */
.byte	0b10010010	/* access                   */
.byte	0b11001111	/* granularity              */
.byte	0x00		/* base high                */
