include ../../Makefile.inc

TARGET   = ../../build/stage1.bin
OBJFILES = stage1.o
LDSCRIPT = linker.ld


all: $(TARGET)

$(TARGET): $(OBJFILES)
	$(LD)  -T $(LDSCRIPT) -o $@ $^

%.o: %.s
	$(CXX) $(CFLAGS) -c -o $@ $<

clean:
	$(RM) *.o $(TARGET)
