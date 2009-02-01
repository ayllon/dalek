include ../../Makefile.inc

TARGET   = ../../build/stage2.bin
OBJFILES = stage2.S.o main.o memory.o printf.o strings.o ports.o idt.S.o idt.o irq.o irq.S.o timer.o keyboard.o
LDSCRIPT = linker.ld
INCLUDE  = -I include/

all: $(TARGET)

$(TARGET): $(OBJFILES)
	$(LD)  -T $(LDSCRIPT) -o $@ $^

%.S.o: %.S
	$(CXX) $(CFLAGS) $(INCLUDE) -c -o $@ $<
	
%.o: %.c
	$(CXX) $(CFLAGS) $(INCLUDE) -c -o $@ $<

clean:
	$(RM) *.o $(TARGET)
