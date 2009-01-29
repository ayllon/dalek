/*
 * stage1.s
 *
 * First stage boot loader
 * This file looks for and loads the second stage boot loader
 * It depends on the file system (this one is for FAT12/16),
 * in the future I plan to write for others.
 */
	
.text
.global start
	
start:
	cli
	hlt

/* Magic */
	.org 0x1FE
	.word 0xAA55
