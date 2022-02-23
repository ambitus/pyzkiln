# Makefile variables
BASE=$(PYRACF_HOME)

ARCH_NAME=r_admin_a.a

DEPS=REGDEFS

SRCS=callRadmin.s
OBJS=callRadmin.o

# Platform commands
AS=as
RM=rm -f
AR=ar -r

# Global arguments
# Use -a=xxxxxxxx.lst to generate a listing
AARGS=-mGOFF 
INCS=-I.

all: start $(OBJS) build_archive

start:
	@echo "--- Make r_admin_a"

$(OBJS): $(SRCS) $(DEPS)
	$(AS) $(AARGS) $(INCS) -o $*.o $*.s

build_archive: $(OBJS)
	$(AR) $(ARCH_NAME) $(OBJS)

clean:
	@echo "--- Clean R_admin asm"
	$(RM) $(ARCH_NAME) *.o
	$(RM) CEE*
