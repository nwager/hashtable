CC:=gcc
IDIR:=include
SDIR:=src
LDIR:=lib
ODIR:=obj
OUTDIR:=out
TDIR:=test
LIBOUT:=libout
LLDIR:=../linkedlist

# make sure DEBUG is not automatically defined by your IDE
DEBUG:=1

LIBLL:=linkedlist
LIBLLFILE:=$(LDIR)/lib$(LIBLL).a

CFLAGS:=-Wall -Wpedantic -Wextra -Werror -g -I$(IDIR)
ifeq ($(DEBUG), 1)
	CFLAGS +=-DDEBUG
endif
LIBFLAG:=-L$(LDIR) -l$(LIBLL)

_DEPS:=hashtable.h linkedlist.h
DEPS:=$(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJS:=hashtable.o
OBJS:=$(patsubst %,$(ODIR)/%,$(_OBJS))

_OUT:=libhashtable.a
OUTPUT:=$(OUTDIR)/$(_OUT)

TOUTPUT:=$(OUTDIR)/test_hashtable.out

# targets

all: $(OUTPUT)

$(LIBLLFILE):
	./updatelib.sh $(LLDIR)

$(ODIR)/%.o: $(SDIR)/%.c $(DEPS)
	mkdir -p $(ODIR)
	$(CC) -c -o $@ $< $(CFLAGS)

$(OUTPUT): $(OBJS) $(LIBLLFILE)
	mkdir -p $(OUTDIR)
	mkdir -p $(OUTDIR)/tmp
	cd $(OUTDIR)/tmp && ar x ../../$(LIBLLFILE)
	ar cr $(OUTPUT) $(OBJS) $(OUTDIR)/tmp/*.o
	rm -rf $(OUTPUT)/tmp

test: $(OUTPUT) $(TDIR)/test_hashtable.c
	$(CC) -o $(TOUTPUT) $^ $(CFLAGS) $(LIBFLAG)

runtest: test
	# Running tests
	./$(TOUTPUT)
	# Tests passed

runleak: test
	# Running tests
	leaks --atExit -- $(TOUTPUT)
	# Tests passed

lib: $(OUTPUT)
	mkdir -p $(LIBOUT)
	cp $(OUTPUT) $(DEPS) $(LIBOUT)

.PHONY: clean

clean:
	rm -rf *.o *.a *~ $(ODIR) $(OUTDIR) $(TOUTPUT) $(LIBLLFILE) $(LIBOUT)