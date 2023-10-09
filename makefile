TARGET := example
EXTENSION := c
CC := gcc

INCLUDEDIR := include
OBJDIR := object
SRCDIR := source
BINDIR := binary
LIBDIR := lib
ASMDIR := asm

SRCFILES := $(shell find $(SRCDIR) -type f \( -iname "*.$(EXTENSION)" \) -exec basename \{} \;)
HEADERFILES := $(shell find $(INCLUDEDIR) -type f \( -iname "*.h" \) -exec basename \{} \;)
OBJFILES := $(SRCFILES:%.$(EXTENSION)=%.o)

MACROS := MAKE
BASEFLAGS := $(addprefix -D ,$(MACROS))
DEBUGFLAGS := $(BASEFLAGS) -g -Wall
RELEASEFLAGS := $(BASEFLAGS) -O2

clean:
	rm -rf $(OBJDIR)
	rm -rf $(BINDIR)
	rm -rf $(LIBDIR)

#setup directory
setup:
	mkdir -p $(OBJDIR)
	mkdir -p $(BINDIR)
	mkdir -p $(LIBDIR)

# Shared
data.o:
	$(CC) $(DEBUGFLAGS) -c $(SRCDIR)/data.c -o $(OBJDIR)/data.o -I $(INCLUDEDIR)

entry.o:
	$(CC) $(DEBUGFLAGS) -c $(SRCDIR)/entry.c -o $(OBJDIR)/entry.o -I $(INCLUDEDIR)

list.o:
	$(CC) $(DEBUGFLAGS) -c $(SRCDIR)/list.c -o $(OBJDIR)/list.o -I $(INCLUDEDIR)

table.o:
	$(CC) $(DEBUGFLAGS) -c $(SRCDIR)/table.c -o $(OBJDIR)/table.o -I $(INCLUDEDIR)


#---------------TESTs------------------------------------

test_data:
	$(CC) $(DEBUGFLAGS) -o $(OBJDIR)/data.o -c $(SRCDIR)/data.c -I $(INCLUDEDIR) && $(CC) $(DEBUGFLAGS) tests/test_data.c -o $(BINDIR)/test_data $(OBJDIR)/data.o -I $(INCLUDEDIR)

test_data_run: test_data
	./$(BINDIR)/test_data

test_entry: test_data_run
	$(CC) $(DEBUGFLAGS) -o $(OBJDIR)/entry.o -c $(SRCDIR)/entry.c -I $(INCLUDEDIR) && $(CC) $(DEBUGFLAGS) tests/test_entry.c -o $(BINDIR)/test_entry $(OBJDIR)/data.o $(OBJDIR)/entry.o -I $(INCLUDEDIR)

test_entry_run: test_entry
	./$(BINDIR)/test_entry

test_list: test_entry_run
	$(CC) $(DEBUGFLAGS) -o $(OBJDIR)/list.o -c $(SRCDIR)/list.c -I $(INCLUDEDIR) && $(CC) $(DEBUGFLAGS) tests/test_list.c -o $(BINDIR)/test_list $(OBJDIR)/data.o $(OBJDIR)/entry.o $(OBJDIR)/list.o -I $(INCLUDEDIR)

test_list_run: test_list
	./$(BINDIR)/test_list

test_table: test_list_run
	$(CC) $(DEBUGFLAGS) -o $(OBJDIR)/table.o -c $(SRCDIR)/table.c -I $(INCLUDEDIR) && $(CC) $(DEBUGFLAGS) tests/test_table.c -o $(BINDIR)/test_table $(OBJDIR)/data.o $(OBJDIR)/entry.o $(OBJDIR)/list.o $(OBJDIR)/table.o -I $(INCLUDEDIR)

test_table_run: test_table
	./$(BINDIR)/test_table