"""
  r_admin_fff.py - Python interface to the RACF R_admin service

  Author: Joe Bostian
  Copyright Contributors to the Ambitus Project.
  SPDX-License-Identifier: Apache-2.0
"""


# R_admin flags.
#
# See RACF callable services for the details of the functional behavior that
# These flags represe.
class RaFlags:
    def __init__(self):
        self.F_BYPASS_CMD_PROC = 0x80000000      # bypass cmd processor (IN)
        self.F_EXT_BASE_SEG_ONLY = 0x40000000    # extract base seg only (IN)
        self.F_ENF_FACILITY_CHK = 0x20000000     # enf facility class chk (IN)
        self.F_GENERIC_RES_REQ = 0x10000000      # generic resoure req (IN/OUT)
        self.F_UPCASE_INPUT_NAME = 0x08000000    # fold input name to UC (IN)
        self.F_RET_ONLY_PROF_NAME = 0x04000000   # return only prof name (IN)
        return


# R_admin function groups.
#
# R_admin function codes grouped by argument and parameter type.  All
# functions in any one of these given groups share an interface.  Although
# these groups are part of the RACF documentation, they aren't a formal
# part of the API.
class RaFGrps:
    def __init__(self):
        self.ADMIN_GRP_UADMIN = 0x01  # user administration/update
        self.ADMIN_GRP_PWXTR = 0x02   # PKCS 7 pwd & passphrase envelope extr
        self.ADMIN_GRP_PXTR = 0x03    # profile extract
        self.ADMIN_GRP_CMDRUN = 0x04  # run RACF command
        self.ADMIN_GRP_GADMIN = 0x05  # RACF group administration/update
        self.ADMIN_GRP_GCONN = 0x06   # RACF group connect admin
        self.ADMIN_GRP_GENRES = 0x07  # general resource profile admin
        self.ADMIN_GRP_SETRAD = 0x08  # SETROPTS administration
        self.ADMIN_GRP_SETRRP = 0x09  # SETROPTS reporting
        self.ADMIN_GRP_RRSF = 0x0A    # RRSF settings and node def
        return


# R_admin function types (codes).
#
# Each of these represent a specific function that a caller can request.
class RaFTypes:
    def __init__(self):
        self.ADMIN_ADD_USER = 0x01    # Add a user to the RACF db
        self.ADMIN_DEL_USER = 0x02    # Delete a user from the RACF db
        self.ADMIN_ALT_USER = 0x03    # Alter a user's RACF profile
        self.ADMIN_LST_USER = 0x04    # List contents of users RACF profile

        self.ADMIN_RUN_CMD = 0x05     # Run a RACF command image

        self.ADMIN_ADD_GROUP = 0x06   # Add a group to the RACF database
        self.ADMIN_DEL_GROUP = 0x07   # Delete a group from the RACF database
        self.ADMIN_ALT_GROUP = 0x08   # Alter a group's RACF profile
        self.ADMIN_LST_GROUP = 0x09   # List contents of a group's RACF profile

        self.ADMIN_CONNECT = 0x0A     # Connect a single user to a RACF group
        self.ADMIN_REMOVE = 0x0B      # Remove a single user from a RACF group

        self.ADMIN_ADD_GENRES = 0x0C  # Add general resource profile to RACF
        self.ADMIN_DEL_GENRES = 0x0D  # Del general resource profile from RACF
        self.ADMIN_ALT_GENRES = 0x0E  # Alter a general resource's RACF profile
        self.ADMIN_LST_GENRES = 0x0F  # List contents gen resource's RACF prof

        self.ADMIN_ADD_DS = 0x10      # Add a data set profile to RACF
        self.ADMIN_DEL_DS = 0x11      # Delete a data set profile from RACF
        self.ADMIN_ALT_DS = 0x12      # Alter a data set's RACF profile
        self.ADMIN_LST_DS = 0x13      # List contents of data set's RACF prof

        self.ADMIN_PERMIT = 0x14      # Permit a user or group to a RACF prof

        self.ADMIN_ALT_SETR = 0x15    # Alter SETROPTS information
        self.ADMIN_XTR_SETR = 0x16    # Extr SETROPTS info in R_admin format
        self.ADMIN_UNL_SETR = 0x17    # Extr SETROPTS info in SMF unload format

        self.ADMIN_XTR_PWENV = 0x18          # Extr PKCS #7 encrypted pw env
        self.ADMIN_XTR_USER = 0x19           # Extr a user profile
        self.ADMIN_XTR_NEXT_USER = 0x1A      # Extr the next user profile
        self.ADMIN_XTR_GROUP = 0x1B          # Extr a group profile
        self.ADMIN_XTR_NEXT_GROUP = 0x1C     # Extr the next group profile
        self.ADMIN_XTR_CONNECT = 0x1D        # Extr connection info user, grp
        self.ADMIN_XTR_PPENV = 0x1E          # Extr PKCS #7 encr pwd phrase env
        self.ADMIN_XTR_RESOURCE = 0x1F       # Extr a general resource profile
        self.ADMIN_XTR_NEXT_RESOURCE = 0x20  # Extr next general resource prof
        self.ADMIN_XTR_RRSF = 0x21           # Extr RRSF settings, node def inf
        self.ADMIN_XTR_DATASET = 0x22        # Extr a dataset profile
        self.ADMIN_XTR_NEXT_DATASET = 0x23   # Extr the next dataset profile
        return
