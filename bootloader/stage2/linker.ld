OUTPUT_FORMAT("binary")
ENTRY(start)
SECTIONS
{
  . = 0x0500;
  
  end = .; _end = .; __end = .;
}