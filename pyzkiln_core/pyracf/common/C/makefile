# Makefile variables
BASE=$(PYRACF_HOME)

ARCH_NAME=common.a

DEPS=json.h fileio.h transcode.h keyval.h log.h dump.h bytes.h common_types.h 

SRCS=json_prs.c json_gen.c fileio.c transcode.c keyval.c log.c dump.c bytes.c
OBJS=json_prs.o json_gen.o fileio.otranscode.o keyval.o log.o dump.o bytes.o

# Platform commands
CC=c89
RM=rm -f
AR=ar -r

# Global arguments
CARGS=-c -Wc,"LANGLVL(EXTENDED),SSCOMM,XPLINK,LP64,ASCII,GOFF"
INCS=-I.

all: start $(OBJS) build_archive

start:
	@echo "--- Make common C"

$(OBJS): $(SRCS) $(DEPS)
	$(CC) $(CARGS) $(INCS) -o $*.o $*.c

# These objects are included in the archives for the individual services that
# have CSECTs.  Create an archive here as well for convenience.
build_archive: $(OBJS)
	$(AR) $(ARCH_NAME) *.o

$(DEPS):

clean:
	@echo "--- Clean common C"
	$(RM) $(ARCH_NAME) *.o
	$(RM) CEE*
