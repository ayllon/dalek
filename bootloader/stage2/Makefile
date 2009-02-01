include ../../Makefile.inc

TARGET   = ../../build/stage2.bin
OBJFILES = stage2.o main.o memory.o printf.o strings.o
LDSCRIPT = linker.ld
INCLUDE  = -I include/

all: $(TARGET)

$(TARGET): $(OBJFILES)
	$(LD)  -T $(LDSCRIPT) -o $@ $^

stage2.o: stage2.S
	$(CXX) $(CFLAGS) $(INCLUDE) -c -o $@ $<
	
%.o: %.c
	$(CXX) $(CFLAGS) $(INCLUDE) -c -o $@ $<

clean:
	$(RM) *.o $(TARGET)
