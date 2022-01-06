"""
  pxtr.py - Python interface to the RACF R_admin Profile extract functions

  Author: Joe Bostian
  Copyright Contributors to the Ambitus Project.
  SPDX-License-Identifier: Apache-2.0
"""
import json
import ctypes as C
import os

import r_admin


# Input to the profile extract functions of R_admin.  All the extract functions
# share this set of call parameters.  These map into the input/output parameter
# list of IRRSEQ00.
class Pxtr:
    def __init__(self, racf=None, radmin=None, func_type=None):
        self.func_type = func_type

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

        self.parms = {}
        self.parms['func_type'] = func_type
        self.parms['prof_name'] = ''
        self.parms['class_name'] = ''
        self.parms['flags'] = 0x00000000

        self.racf.log.debug('Pxtr init')
        if self.func_type is not None:
            self.racf.log.debug('    func_type: (0x%02x)' % self.func_type)
        return

    def set_func_type(self, func_type):
        self.parms['func_type'] = func_type
        return

    def set_prof_name(self, prof_name):
        self.parms['prof_name'] = prof_name
        return

    def set_class_name(self, class_name):
        self.parms['class_name'] = class_name
        return

    def set_list_of_stuff(self, list_of_stuff):
        self.parms['list_of_stuff'] = list_of_stuff
        return

    def set_bypass_cmd_proc(self, flag=1):
        if flag == 1:
            self.parms['flags'] |= r_admin.F_BYPASS_CMD_PROC
        else:
            self.parms['flags'] &= (~-r_admin.F_BYPASS_CMD_PROC)
        return

    def set_ext_base_seg_only(self, flag=1):
        if flag == 1:
            self.parms['flags'] |= r_admin.F_EXT_BASE_SEG_ONLY
        else:
            self.parms['flags'] &= (~-r_admin.F_EXT_BASE_SEG_ONLY)
        return

    def set_enf_facility_chk(self, flag=1):
        if flag == 1:
            self.parms['flags'] |= r_admin.F_ENF_FACILITY_CHK
        else:
            self.parms['flags'] &= (~-r_admin.F_ENF_FACILITY_CHK)
        return

    def set_generic_res_req(self, flag=1):
        if flag == 1:
            self.parms['flags'] |= r_admin.F_GENERIC_RES_REQ
        else:
            self.parms['flags'] &= (~-r_admin.F_GENERIC_RES_REQ)
        return

    def set_upcase_input_name(self, flag=1):
        if flag == 1:
            self.parms['flags'] |= r_admin.F_UPCASE_INPUT_NAME
        else:
            self.parms['flags'] &= (~-r_admin.F_UPCASE_INPUT_NAME)
        return

    def set_ret_only_prof_name(self, flag=1):
        if flag == 1:
            self.parms['flags'] |= r_admin.F_RET_ONLY_PROF_NAME
        else:
            self.parms['flags'] &= (~-r_admin.F_RET_ONLY_PROF_NAME)
        return

    def get_func_type(self):
        return self.parms['func_type']

    def get_prof_name(self):
        return self.parms['prof_name']

    def get_class_name(self):
        return self.parms['class_name']

    def get_bypass_cmd_proc(self):
        if self.parms['flags'] & r_admin.F_BYPASS_CMD_PROC:
            return 1
        else:
            return 0

    def get_ext_base_seg_only(self):
        if self.parms['flags'] & r_admin.F_EXT_BASE_SEG_ONLY:
            return 1
        else:
            return 0

    def get_enf_facility_chk(self):
        if self.parms['flags'] & r_admin.F_ENF_FACILITY_CHK:
            return 1
        else:
            return 0

    def get_generic_res_req(self):
        if self.parms['flags'] & r_admin.F_GENERIC_RES_REQ:
            return 1
        else:
            return 0

    def get_upcase_input_name(self):
        if self.parms['flags'] & r_admin.F_UPCASE_INPUT_NAME:
            return 1
        else:
            return 0

    def get_ret_only_prof_name(self):
        if self.parms['flags'] & r_admin.F_RET_ONLY_PROF_NAME:
            return 1
        else:
            return 0

    def run(self):
        self.racf.log.debug('Pxtr run')
        self.racf.log.debug('    Call data file: %s' %
                            (self.racf.request_df.get_name()))
        self.racf.log.debug('    Return data file: %s' %
                            (self.racf.results_df.get_name()))

        # Collect any parms from the parent function (R_admin) that the user
        # may have set, and assemble them into the input parameter json file
        # that gets passed to the C code.
        call_parms = self.radmin.bld_request()
        call_parms = call_parms + '            "func":\n'
        call_parms = call_parms + json.dumps(self.parms, indent=16) + '\n'
        call_parms = call_parms + '        }\n'
        call_parms = call_parms + '    }\n'
        call_parms = call_parms + '}\n'
        self.racf.log.debug('    parms built, write to %s' %
                            (self.racf.request_df.get_name()))

        # Write the parms to the request data file.  We're using this like a
        # pipe, but are using a regular file instead to avoid inherent
        # limitations on the length of data being passed.  Requests to RACF
        # aren't generally that long, but results from RACF can be very verbose.
        self.racf.request_df.open('w')
        self.racf.request_df.write(call_parms)
        self.racf.request_df.close()

        # Call the C interface to the profile extract function of the R_admin
        # service.  Pass in the name of the request and results files.
        self.racf.libracf.r_admin.restype = C.c_int
        self.racf.libracf.r_admin.argtypes = [C.c_char_p, C.c_char_p, C.c_int]
        request_fn = C.c_char_p(bytes(self.racf.request_df.get_name(),
                                'ISO8859-1'))
        results_fn = C.c_char_p(bytes(self.racf.results_df.get_name(),
                                'ISO8859-1'))
        f_debug = C.c_int(self.racf.get_debug())
        rc = self.racf.libracf.r_admin(request_fn, results_fn, f_debug)

        # Read and parse the results to return to the caller.
        return self.racf.get_results()

    def show(self):
        self.racf.log.debug('Pxtr - extract parms function:')
        self.racf.log.debug('   name: 0x%02x' % self.parms['svc_func'])
        self.racf.log.debug('   prof_name: %s' % self.parms['prof_name'])
        self.racf.log.debug('   class_name: %s' % self.parms['class_name'])
        self.racf.log.debug('   flags: %08x' % self.parms['flags'])
        return


# class R_admin_xtr_user:
#     def __init__(self, svc_func=None, prof_name='', ):
#         self.call_parms = R_admin_xtr_call_parms(function)
#         return
