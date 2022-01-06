# Makefile variables
BASE=$(PYRACF_HOME)
NAME=libracf
LIB=$(NAME).so
LIB_X=$(NAME).x

# Platform commands
CC=c89
RM=rm -f

# Global arguments
LIB_LARGS=-Wl,"DLL,XPLINK,LP64"

# Object archives for C builts code
LIB_CSECTS=$(BASE)/R_admin/C/r_admin.o
LIB_ARCHS=$(BASE)/common/C/common.a $(BASE)/R_admin/C/r_admin_c.a $(BASE)/R_admin/asm/r_admin_a.a


# Build all
all:
	@$(MAKE) -c $(BASE)/common/C all
	@$(MAKE) -c $(BASE)/R_admin/C all
	@$(MAKE) -c $(BASE)/R_admin/asm all

	@echo "--- Linking $(NAME)"
	${CC} $(LIB_LARGS) -o $(LIB) $(LIB_CSECTS) $(LIB_ARCHS)

clean:
	@$(MAKE) -c $(BASE)/common/C clean
	@$(MAKE) -c $(BASE)/R_admin/C clean
	@$(MAKE) -c $(BASE)/R_admin/asm clean
	$(RM) $(LIB) $(LIB_X)
	$(RM) CEE*
