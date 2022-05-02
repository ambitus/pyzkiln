#
# Call like this:  python3 test_py_racf
# Environmentals:
#   PY_RACF_HOME=<location of py_racf root>
#   PYTHONPATH=$PYTHONPATH:$PY_RACF_HOME:PY_RACF_HOME/common/python:$PY_RACF_HOME/R_admin/python
#   LIBPATH=$LIBPATH:$PYRACF_HOME
#
import py_racf
from py_racf import Racf
from r_admin import ADMIN_ADD_USER

import pprint


if __name__ == '__main__':

    # Get my own profile
    racf = Racf()
    racf.set_debug(1)
    racf.init_svc(py_racf.R_ADMIN)
    racf.svc.init_func(ADMIN_ADD_USER)
    traits = {}
    traits['username'] = 'user1'
    password = 'password'
    racf.svc.func.set_user_traits(traits, password)
    # racf.term()
    # del(racf)

    # Add a new user - test out an authorized function
    # racf = Racf(py_racf.R_ADMIN, r_admin.ADMIN_ADD_USER)
    # Fill in details for the call
    # racf.svc.func.run()
    # print('%s' % racf.get_results())
    # del(racf)
