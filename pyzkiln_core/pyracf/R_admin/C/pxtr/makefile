# Makefile variables
BASE=$(PYRACF_HOME)

DEPS=irrpcomp.h r_admin.h pxtr.h

SRCS=pxtr.c pxtr_kv.c pxtr_dump.c
OBJS=pxtr.o pxtr_kv.o pxtr_dump.o

# Platform commands
CC=c89
RM=rm -f
CP=cp

# Global arguments
CARGS=-c -Wc,"LANGLVL(EXTENDED),SSCOMM,XPLINK,LP64,ASCII,GOFF"
INCS=-I. -I$(BASE)/R_admin/C -I$(BASE)/common/C

all: start $(OBJS) copy_objs

start:
	@echo "--- Make pxtr function"

$(OBJS): $(SRCS) $(DEPS)
	$(CC) $(CARGS) $(INCS) -o $*.o $*.c

$(DEPS):

# Copy all of the built objects for this function back up to the parent
# directory so they can included in the RACF service archive.
copy_objs: $(OBJS)
	$(CP) *.o ..

clean:
	@echo "--- Clean pxtr function"
	$(RM) *.o
	$(RM) CEE*
