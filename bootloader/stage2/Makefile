include ../../Makefile.inc

TARGET   = ../../build/stage2.bin
OBJFILES = stage2.S.o main.o memory.o printf.o strings.o ports.o idt.S.o idt.o irq.o irq.S.o timer.o keyboard.o
LDSCRIPT = linker.ld
INCLUDE  = -I include/

BUILTIN_KEYMAP = keymaps/es_es.o

all: $(TARGET)

$(TARGET): $(OBJFILES) $(BUILTIN_KEYMAP)
	$(LD)  -T $(LDSCRIPT) -o $@ $^

%.S.o: %.S
	$(CXX) $(CFLAGS) $(INCLUDE) $(DEFINES) -c -o $@ $<
	
%.o: %.c
	$(CXX) $(CFLAGS) $(INCLUDE) $(DEFINES) -c -o $@ $<

clean:
	$(RM) *.o $(TARGET)
