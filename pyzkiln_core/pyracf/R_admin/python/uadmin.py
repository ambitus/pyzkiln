"""
  uadmin.py - Python interface to the RACF R_admin user administration functions

  Author: Joe Bostian
  Copyright Contributors to the Ambitus Project.
  SPDX-License-Identifier: Apache-2.0
"""
import json
import re

import py_racf
import r_admin


# Input to the profile extract functions of R_admin.  All the extract functions
# share this set of call parameters.  These map into the input/output parameter
# list of IRRSEQ00.  
class Uadmin:
    def __init__(self, racf=None, radmin=None, func_type=None):
        print('Uadmin constructor')
        if racf is not None:
            self.racf = racf
        else:
            print('Error - missing ancestor object')
            raise Exception
        if radmin is not None:
            self.radmin = radmin
        else:
            print('Error - missing ancestor object')
            raise Exception
        self.set_function(self, func_type)
        self.racf.log.debug('Uadmin init')
        if self.func_type is not None:
            self.racf.log.debug('    func_type: (0x%02x)' % self.func_type)
        return

    def set_function(self, func_type):
        if func_type == None:
            return None
        elif func_type == r_admin.ADMIN_ADD_USER:
            self.func_type = r_admin.ADMIN_ADD_USER
            self.fgrp = r_admin.ADMIN_GRP_UADMIN
        return
        

    def set_user_traits(traits, password):
        self.parms = traits
        if 'username' not in traits:
            print('Error - must provide "username" as dictionary key')
            raise Exception
        verify_username(traits['username'])
        self.username = traits['username']
        verify_password(password)
        self.password = password
        print(self.username)
        print(self.password)
        return

    def verify_username(username):
        if len(username) < 1 or len(username) > 8:
            print('Error - username length must be 1 to 8 characters')
            raise Exception
        if re.fullmatch(r'[a-zA-Z0-9$@#]{1,8}',username) is None:
            print('Error - username does not adhere to syntax rules')
            raise Exception
        return
    
    def verify_password(password):
        pass 