include ../../Makefile.inc

TARGET   = ../../build/stage2.bin
OBJFILES = stage2.S.o main.o memory.o printf.o strings.o ports.o idt.S.o idt.o irq.o irq.S.o timer.o keyboard.o cli.o get.o
LDSCRIPT = linker.ld
INCLUDE  = -I include/

BUILTIN_KEYMAP = keymaps/es_es.o

COMMANDS_O = commands/*.o

all: $(TARGET)

# Helper
force_look:
	@true

$(TARGET): $(OBJFILES) $(BUILTIN_KEYMAP) $(COMMANDS_O)
	$(LD)  -T $(LDSCRIPT) -o $@ $^

%.S.o: %.S
	$(CXX) $(CFLAGS) $(INCLUDE) $(DEFINES) -c -o $@ $<
	
%.o: %.c
	$(CXX) $(CFLAGS) $(INCLUDE) $(DEFINES) -c -o $@ $<

$(COMMANDS_O): force_look
	-cd commands;for d in *.c; do ($(CXX) $(CFLAGS) -I ../include $(DEFINES) -c $$d); done

clean:
	$(RM) *.o $(TARGET)
