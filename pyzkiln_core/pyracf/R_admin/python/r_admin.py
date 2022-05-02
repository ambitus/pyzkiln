"""
  py_racf_r_admin.py - Python interface to the RACF R_admin service

  Author: Joe Bostian
  Copyright Contributors to the Ambitus Project.
  SPDX-License-Identifier: Apache-2.0
"""
import py_racf
from pxtr import Pxtr
from uadmin import Uadmin 


# R_admin flags.
#
# See RACF callable services for the details of the functional behavior that
# These flags represe.
F_BYPASS_CMD_PROC = 0x80000000      # bypass cmd processor (IN)
F_EXT_BASE_SEG_ONLY = 0x40000000    # extract base seg only (IN)
F_ENF_FACILITY_CHK = 0x20000000     # enf facility class check (IN)
F_GENERIC_RES_REQ = 0x10000000      # generic resoure request (IN/OUT)
F_UPCASE_INPUT_NAME = 0x08000000    # fold input name to upper (IN)
F_RET_ONLY_PROF_NAME = 0x04000000   # return only profile name (IN)


# R_admin function groups.
#
# R_admin function codes grouped by argument and parameter type.  All
# functions in any one of these given groups share an interface.  Although
# these groups are part of the RACF documentation, they aren't a formal
# part of the API.
ADMIN_GRP_UADMIN = 0x01   # user administration/update
ADMIN_GRP_PWXTR = 0x02    # PKCS 7 pwd & passphrase envelope extract
ADMIN_GRP_PXTR = 0x03     # profile extract
ADMIN_GRP_CMDRUN = 0x04   # run RACF command
ADMIN_GRP_GADMIN = 0x05   # RACF group administration/update
ADMIN_GRP_GCONN = 0x06    # RACF group connect admin
ADMIN_GRP_GENRES = 0x07   # general resource profile admin
ADMIN_GRP_SETRAD = 0x08   # SETROPTS administration
ADMIN_GRP_SETRRP = 0x09   # SETROPTS reporting
ADMIN_GRP_RRSF = 0x0A     # RRSF settings and node definition


# R_admin function types (codes).
#
# Each of these represent a specific function that a caller can request.
ADMIN_ADD_USER = 0x01     # Add a user to the RACF db
ADMIN_DEL_USER = 0x02     # Delete a user from the RACF db
ADMIN_ALT_USER = 0x03     # Alter a user's RACF profile
ADMIN_LST_USER = 0x04     # List contents of users RACF profile

ADMIN_RUN_CMD = 0x05      # Run a RACF command image

ADMIN_ADD_GROUP = 0x06    # Add a group to the RACF database
ADMIN_DEL_GROUP = 0x07    # Delete a group from the RACF database
ADMIN_ALT_GROUP = 0x08    # Alter a group's RACF profile
ADMIN_LST_GROUP = 0x09    # List contents of a group's RACF profile

ADMIN_CONNECT = 0x0A      # Connect a single user to a RACF group
ADMIN_REMOVE = 0x0B       # Remove a single user from a RACF group

ADMIN_ADD_GENRES = 0x0C   # Add general resource profile to RACF
ADMIN_DEL_GENRES = 0x0D   # Del general resource profile from RACF
ADMIN_ALT_GENRES = 0x0E   # Alter a general resource's RACF profile
ADMIN_LST_GENRES = 0x0F   # List contents gen resource's RACF prof

ADMIN_ADD_DS = 0x10       # Add a data set profile to RACF
ADMIN_DEL_DS = 0x11       # Delete a data set profile from RACF
ADMIN_ALT_DS = 0x12       # Alter a data set's RACF profile
ADMIN_LST_DS = 0x13       # List contents of data set's RACF prof

ADMIN_PERMIT = 0x14       # Permit a user or group to a RACF prof

ADMIN_ALT_SETR = 0x15     # Alter SETROPTS information
ADMIN_XTR_SETR = 0x16     # Extr SETROPTS info in R_admin format
ADMIN_UNL_SETR = 0x17     # Extr SETROPTS info in SMF unload format

ADMIN_XTR_PWENV = 0x18           # Extr PKCS #7 encrypted pw env
ADMIN_XTR_USER = 0x19            # Extr a user profile
ADMIN_XTR_NEXT_USER = 0x1A       # Extr the next user profile
ADMIN_XTR_GROUP = 0x1B           # Extr a group profile
ADMIN_XTR_NEXT_GROUP = 0x1C      # Extr the next group profile
ADMIN_XTR_CONNECT = 0x1D         # Extr connection info user, grp
ADMIN_XTR_PPENV = 0x1E           # Extr PKCS #7 encr pwd phrase env
ADMIN_XTR_RESOURCE = 0x1F        # Extr a general resource profile
ADMIN_XTR_NEXT_RESOURCE = 0x20   # Extr next general resource prof
ADMIN_XTR_RRSF = 0x21            # Extr RRSF settings, node def inf
ADMIN_XTR_DATASET = 0x22         # Extr a dataset profile
ADMIN_XTR_NEXT_DATASET = 0x23    # Extr the next dataset profile


# Radmin - the main R_admin class
#
# This routed control to the specific R_admin function requested.
class Radmin:
    def __init__(self, racf=None, func_type=None):
        if racf is not None:
            self.racf = racf
        else:
            print('Error - missing ancestor object')
            raise Exception

        self.svc_type = py_racf.R_ADMIN
        self.func_type = func_type
        self.func = None
        self.fgrp = None

        self.racf.log.debug('R_admin init')
        self.racf.log.debug('    svc_type: (0x%02x)' % self.svc_type)
        if self.func_type is not None:
            self.racf.log.debug('    func_type: (0x%02x)' % self.func_type)

        # Init function-specific resources for this request
        if func_type is not None:
            self.func = self.init_func(func_type)
        return

    def init_func(self, func_type=None):
        if func_type == ADMIN_ADD_USER:
            self.fgrp = ADMIN_GRP_UADMIN
            self.func = Uadmin(self.racf, self, func_type)
            return
        elif func_type == ADMIN_DEL_USER:
            self.fgrp = ADMIN_GRP_UADMIN
            return None
        elif func_type == ADMIN_ALT_USER:
            self.fgrp = ADMIN_GRP_UADMIN
            return None
        elif func_type == ADMIN_LST_USER:
            self.fgrp = ADMIN_GRP_UADMIN
            return None
        elif func_type == ADMIN_RUN_CMD:
            self.fgrp = ADMIN_GRP_CMDRUN
            return None
        elif func_type == ADMIN_ADD_GROUP:
            self.fgrp = ADMIN_GRP_GADMIN
            return None
        elif func_type == ADMIN_DEL_GROUP:
            self.fgrp = ADMIN_GRP_GADMIN
            return None
        elif func_type == ADMIN_ALT_GROUP:
            self.fgrp = ADMIN_GRP_GADMIN
            return None
        elif func_type == ADMIN_LST_GROUP:
            self.fgrp = ADMIN_GRP_GADMIN
            return None
        elif func_type == ADMIN_CONNECT:
            self.fgrp = ADMIN_GRP_GCONN
            return None
        elif func_type == ADMIN_REMOVE:
            self.fgrp = ADMIN_GRP_GCONN
            return None
        elif func_type == ADMIN_ADD_GENRES:
            self.fgrp = ADMIN_GRP_GENRES
            return None
        elif func_type == ADMIN_DEL_GENRES:
            self.fgrp = ADMIN_GRP_GENRES
            return None
        elif func_type == ADMIN_ALT_GENRES:
            self.fgrp = ADMIN_GRP_GENRES
            return None
        elif func_type == ADMIN_LST_GENRES:
            self.fgrp = ADMIN_GRP_GENRES
            return None
        elif func_type == ADMIN_ADD_DS:
            self.fgrp = ADMIN_GRP_GENRES
            return None
        elif func_type == ADMIN_DEL_DS:
            self.fgrp = ADMIN_GRP_GENRES
            return None
        elif func_type == ADMIN_ALT_DS:
            self.fgrp = ADMIN_GRP_GENRES
            return None
        elif func_type == ADMIN_LST_DS:
            self.fgrp = ADMIN_GRP_GENRES
            return None
        elif func_type == ADMIN_PERMIT:
            self.fgrp = ADMIN_GRP_GENRES
            return None
        elif func_type == ADMIN_ALT_SETR:
            self.fgrp = ADMIN_GRP_SETRAD
            return None
        elif func_type == ADMIN_XTR_SETR:
            self.fgrp = ADMIN_GRP_SETRRP
            return None
        elif func_type == ADMIN_UNL_SETR:
            self.fgrp = ADMIN_GRP_SETRRP
            return None
        elif func_type == ADMIN_XTR_PWENV:
            self.fgrp = ADMIN_GRP_PWXTR
            return None
        elif func_type == ADMIN_XTR_USER:
            self.fgrp = ADMIN_GRP_PXTR
            self.func = Pxtr(self.racf, self, func_type)
            return
        elif func_type == ADMIN_XTR_NEXT_USER:
            self.fgrp = ADMIN_GRP_PXTR
            return None
        elif func_type == ADMIN_XTR_GROUP:
            self.fgrp = ADMIN_GRP_PXTR
            return None
        elif func_type == ADMIN_XTR_NEXT_GROUP:
            self.fgrp = ADMIN_GRP_PXTR
            return None
        elif func_type == ADMIN_XTR_CONNECT:
            self.fgrp = ADMIN_GRP_PXTR
            return None
        elif func_type == ADMIN_XTR_PPENV:
            self.fgrp = ADMIN_GRP_PWXTR
            return None
        elif func_type == ADMIN_XTR_RESOURCE:
            self.fgrp = ADMIN_GRP_PXTR
            return None
        elif func_type == ADMIN_XTR_NEXT_RESOURCE:
            self.fgrp = ADMIN_GRP_PXTR
            return None
        elif func_type == ADMIN_XTR_RRSF:
            self.fgrp = ADMIN_GRP_RRSF
            return None
        elif func_type == ADMIN_XTR_DATASET:
            self.fgrp = ADMIN_GRP_PXTR
            return None
        elif func_type == ADMIN_XTR_NEXT_DATASET:
            self.fgrp = ADMIN_GRP_PXTR
            return None
        else:
            self.log.warning('No Radmin function specified')
            return None

        return None

    def set_func_type(self, func_type=None):
        if func_type is not None:
            self.func_type = func_type
            self.func = self.init_func(func_type)
        return

    def bld_request(self):
        # Collect any parms from the Racf parent class that the user may have
        # set, and assemble them into the input parameter json string that
        # gets passed on to the service code.
        self.racf.log.debug('    R_admin run')
        call_parms = self.racf.bld_request()
        call_parms = call_parms + '        "svc": {\n'
        call_parms = call_parms + '            "svc_type": '
        call_parms = call_parms + str(self.svc_type) + ',\n'
        return call_parms

    def show(self):
        print('R_admin - RACF admin service:')
        print('   func: %d' % self.func)
        print('   fgrp: %d' % self.fgrp)
        return
