# Makefile variables
BASE=$(PYRACF_HOME)

ARCH_NAME=r_admin_c.a

DEPS=irrpcomp.h r_admin.h

MAIN_SRC=r_admin.c
MAIN_OBJ=r_admin.o

# Platform commands
CC=c89
RM=rm -f
AR=ar -r

# Global arguments
CARGS=-c -Wc,"LANGLVL(EXTENDED),SSCOMM,XPLINK,LP64,ASCII,GOFF"
MAIN_CARGS= -Wc,"CSECT(r_admin)" 
INCS=-I. -I$(BASE)/R_admin/C/pxtr -I$(BASE)/common/C

all: start $(MAIN_OBJ) pxtr_func build_archive

start:
	@echo "--- Make r_admin service"

$(MAIN_OBJ): $(MAIN_SRC) $(DEPS)
	$(CC) $(CARGS) $(MAIN_CARGS) $(INCS) -o $(MAIN_OBJ) $*.c

$(DEPS):

pxtr_func:
	@$(MAKE) -c $(BASE)/R_admin/C/pxtr all

build_archive:
	# Get the common utility methods to include here.
	cp $(BASE)/common/C/*.o .
	$(AR) $(ARCH_NAME) *.o

clean:
	@echo "--- Clean R_admin service"
	$(RM) $(ARCH_NAME) *.o
	$(RM) CEE*
	@$(MAKE) -c $(BASE)/R_admin/C/pxtr clean
