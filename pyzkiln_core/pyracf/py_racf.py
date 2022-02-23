"""
  py_racf.py - Python interface to RACF

  Author: Joe Bostian
  Copyright Contributors to the Ambitus Project.
  SPDX-License-Identifier: Apache-2.0
"""
import os
import tempfile
import logging
import ctypes
import json

import r_admin

SUCCESS = 0x00000000
WARNING = 0x00000004
FAILURE = 0x00000008

# RACF callable services
CK_ACCESS = 0x01
CK_FILE_OWNER = 0x02
CK_IPC_ACCESS = 0x03
CK_OWNER_TWO_FILES = 0x04
CK_PRIV = 0x05
CK_PROCESS_OWNER = 0x06
CLEAR_SETID = 0x07
DELETEUSP = 0x08
GETGMAP = 0x09
GET_UID_GID_SUPGRPS = 0x10
GETUMAP = 0x11
INITACEE = 0x12
INITUSP = 0x13
MAKEFSP = 0x14
MAKEISP = 0x15
MAKE_ROOT_FSP = 0x16
QUERY_FILE_SECURITY_OPTIONS = 0x17
R_ADMIN = 0x18
R_AUDIT = 0x19
R_AUDITX = 0x20
R_CACHESERV = 0x21
R_CHAUDIT = 0x22
R_CHMOD = 0x23
R_CHOWN = 0x24
R_DATALIB = 0x25
R_DCEAUTH = 0x26
R_DCEINFO = 0x27
R_DCEKEY = 0x28
R_DCERUID = 0x29
R_EXEC = 0x30
R_FORK = 0x31
R_GENSEC = 0x32
R_GETGROUPS = 0x33
R_GETGROUPSBYNAME = 0x34
R_GETINFO = 0x35
R_IPC_CTL = 0x36
R_KERBINFO = 0x37
R_PGMSIGNVER = 0X38
R_PKISERV = 0x39
R_PROXYSERV = 0x40
R_PTRACE = 0x41
R_SETEGID = 0x42
R_SETEUID = 0x43
R_SETFACL = 0x44
R_SETFSECL = 0x45
R_SETGID = 0x46
R_SETUID = 0x47
R_TICKETSERV = 0x48
R_UMASK = 0x49
R_USERMAP = 0x50
R_WRITEPRIV = 0x51


# The main RACF Python nterface.
class Racf:
    def __init__(self, svc_type=None, func_type=None, f_debug=0):
        self.svc_type = svc_type
        self.func_type = func_type
        self.svc = None

        # Make sure the logs and data files are set up before initializing
        # the service-specific object
        self.log = Log(f_debug)
        self.request_df = DataFile()
        self.results_df = DataFile()
        self.results = {"key": "value"}
        self.f_debug = f_debug
        self.rc = SUCCESS

        # Resources for loading the C libraries for calling RACF
        self.pyracf_home = os.environ['PYRACF_HOME']
        self.libname = self.pyracf_home + '/' + 'libracf.so'
        self.libracf = None

        # self.log.warning('Racf init (%02x, %02x)', self.svc, self.func)
        self.log.debug('Racf init ...')
        self.log.debug('    Call data file: %s' % self.request_df.get_name())
        self.log.debug('    Return data file: %s' % self.results_df.get_name())

        # Fetch the C library for calling RACF
        ctypes.cdll.LoadLibrary(self.libname)
        self.libracf = ctypes.CDLL(self.libname)
        self.log.debug('    Fetched RACF library from: %s' % self.libname)

        # Init service-specific resources for this request
        if self.svc_type is not None:
            self.log.debug('    svc_type: %s' % self.svc_type)
            self.svc = self.init_svc(svc_type, func_type)

            # svc init will initialize the function, if the function type has
            # been provided.
            if self.func_type is not None:
                self.log.debug('    func_type: %s' % self.func_type)
        return

    def init_svc(self, svc_type=None, func_type=None):
        if svc_type == CK_ACCESS:
            return None
        elif svc_type == CK_FILE_OWNER:
            return None
        elif svc_type == CK_IPC_ACCESS:
            return None
        elif svc_type == CK_OWNER_TWO_FILES:
            return None
        elif svc_type == CK_PRIV:
            return None
        elif svc_type == CK_PROCESS_OWNER:
            return None
        elif svc_type == CLEAR_SETID:
            return None
        elif svc_type == DELETEUSP:
            return None
        elif svc_type == GETGMAP:
            return None
        elif svc_type == GET_UID_GID_SUPGRPS:
            return None
        elif svc_type == GETUMAP:
            return None
        elif svc_type == INITACEE:
            return None
        elif svc_type == INITUSP:
            return None
        elif svc_type == MAKEFSP:
            return None
        elif svc_type == MAKEISP:
            return None
        elif svc_type == MAKE_ROOT_FSP:
            return None
        elif svc_type == QUERY_FILE_SECURITY_OPTIONS:
            return None
        elif svc_type == R_ADMIN:
            self.svc = r_admin.Radmin(self, func_type)
            return
        elif svc_type == R_AUDIT:
            return None
        elif svc_type == R_AUDITX:
            return None
        elif svc_type == R_CACHESERV:
            return None
        elif svc_type == R_CHAUDIT:
            return None
        elif svc_type == R_CHMOD:
            return None
        elif svc_type == R_CHOWN:
            return None
        elif svc_type == R_DATALIB:
            return None
        elif svc_type == R_DCEAUTH:
            return None
        elif svc_type == R_DCEINFO:
            return None
        elif svc_type == R_DCEKEY:
            return None
        elif svc_type == R_DCERUID:
            return None
        elif svc_type == R_EXEC:
            return None
        elif svc_type == R_FORK:
            return None
        elif svc_type == R_GENSEC:
            return None
        elif svc_type == R_GETGROUPS:
            return None
        elif svc_type == R_GETGROUPSBYNAME:
            return None
        elif svc_type == R_GETINFO:
            return None
        elif svc_type == R_IPC_CTL:
            return None
        elif svc_type == R_KERBINFO:
            return None
        elif svc_type == R_PGMSIGNVER:
            return None
        elif svc_type == R_PKISERV:
            return None
        elif svc_type == R_PROXYSERV:
            return None
        elif svc_type == R_PTRACE:
            return None
        elif svc_type == R_SETEGID:
            return None
        elif svc_type == R_SETEUID:
            return None
        elif svc_type == R_SETFACL:
            return None
        elif svc_type == R_SETFSECL:
            return None
        elif svc_type == R_SETGID:
            return None
        elif svc_type == R_SETUID:
            return None
        elif svc_type == R_TICKETSERV:
            return None
        elif svc_type == R_UMASK:
            return None
        elif svc_type == R_USERMAP:
            return None
        elif svc_type == R_WRITEPRIV:
            return None
        else:
            self.log.warning('No RACF service specified')
            return None

        return None

    def get_svc_type(self):
        return self.svc_type

    def get_results(self):
        # Read the results of the RACF call from the results data file.
        self.results_df.open('r')
        self.results = json.loads(self.results_df.read())
        self.results_df.close()
        return self.results

    def get_f_debug(self):
        return self.f_debug

    def set_svc_type(self, svc_type=None):
        self.svc_type = svc_type
        return

    def set_debug(self, f_debug=0):
        self.f_debug = f_debug
        self.log.set_debug(f_debug)
        return

    def get_debug(self):
        return self.f_debug

    def bld_request(self):
        # Set up the top level of the Json doc string that contains all of the
        # parms to pass to the Racf function and service.  Include any settings
        # from the caller.  The Racf service will close the Json string before
        # making the call.
        # self.log.debug('Racf run')
        request = '{\n    "racf": {\n'
        request = request + '        "f_debug": ' + str(self.f_debug) + ',\n'
        return request

    def term(self):
        self.log.debug('RACF term ...')
        self.request_df.close()
        self.results_df.close()
        return


class Log:
    def __init__(self, f_debug=0):
        # Please try to keep these logger settings aligned with those of the
        # logger in the common C code (logger.c).
        self.f_debug = f_debug
        self.logger = logging.getLogger('py_RACF')
        self.logger.setLevel(logging.DEBUG)

        self.ch = logging.StreamHandler()
        self.ch.setLevel(logging.DEBUG)

        self.formatter = logging.Formatter('%(asctime)s [%(name)s] - %(levelname)s: %(message)s','%H:%M:%S')
        self.ch.setFormatter(self.formatter)

        self.logger.addHandler(self.ch)

    def debug(self, txt):
        if self.f_debug:
            self.logger.debug(txt)
        return

    def info(self, txt):
        self.logger.info(txt)
        return

    def warning(self, txt):
        self.logger.warning(txt)
        return

    def error(self, txt):
        self.logger.error(txt)
        return

    def critical(self, txt):
        self.logger.error(txt)
        return

    def set_debug(self, f_debug=0):
        self.f_debug = f_debug
        return


class DataFile:
    def __init__(self):
        self.file = None
        self.osfd, self.name = tempfile.mkstemp(prefix="racf_", suffix=".io", text=True)
        os.close(self.osfd)            # don't use OS level file descriptors
        return

    def open(self, mode):
        self.file = open(self.name, mode, encoding="ISO8859-1")
        return

    def close(self):
        self.file.close()
        return

    def read(self):
        return self.file.read()

    def write(self, txt):
        self.file.write(txt)
        return

    def get_name(self):
        return self.name
