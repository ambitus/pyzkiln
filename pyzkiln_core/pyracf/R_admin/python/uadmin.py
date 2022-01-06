"""
  uadmin.py - Python interface to the RACF R_admin user administration functions

  Author: Joe Bostian
  Copyright Contributors to the Ambitus Project.
  SPDX-License-Identifier: Apache-2.0
"""
import json

import py_racf
import r_admin


# Input to the profile extract functions of R_admin.  All the extract functions
# share this set of call parameters.  These map into the input/output parameter
# list of IRRSEQ00.  
class Uadmin:
    def __init__(self, racf=None, radmin=None, svc_func=None):
        self.svc_func = svc_func
        return
    
    def set_function(self, svc_func=None):
        if svc_func == None:
            return None
        elif func == r_admin.ADMIN_ADD_USER:
            self.fgrp = r_admin.ADMIN_GRP_UADMIN
            self.func = func
            return