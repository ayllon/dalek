/*
 * version.c
 *
 */
#include <printf.h>
#include <cli.h>

uint8 version(uint8 argn, const char **argv)
{
  printf("\nDAleK Boot Loader (Stage 2). Version 0.0.1\n");
  printf("==========================================\n");
  printf("\n2009 (c) Alejandro Alvarez Ayllon\n");
  printf("Licensed under the GPL license.\n\n");
  return 0;
}

