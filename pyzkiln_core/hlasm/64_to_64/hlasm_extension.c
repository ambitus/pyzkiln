#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <Python.h>

#pragma map(addfunc, "ADDFUNC") // links the function 'addfunc' to the symbol 'ADDFUNC'
extern int addfunc(int a, int b, int c, int d);

static PyObject *return_int(PyObject *self, PyObject *args){
    int a,b,c,d;

    if(!PyArg_ParseTuple(args, "iiii", &a, &b, &c, &d)){
        return NULL;
    }
    int res = addfunc(a,b,c,d);
    return Py_BuildValue("i", res);
}


// module method definitions
static PyMethodDef methods[] = {
    {"addition_HLASM", return_int, METH_VARARGS, "returns the addition of 4 integer arguments passed in"},
    {NULL, NULL, 0, NULL} // Sentinel (marks the end)
};

// module initialization function
static struct PyModuleDef hlasm_extension_module = {
    PyModuleDef_HEAD_INIT,  // macro used to initialize PyModuleDef
    "hlasm_extension",      // module name
    NULL,                   // module documentation
    -1,                     // module state (not global)
    methods                 // methods called
};

// module initialization PyInit_<module name>(void)
PyMODINIT_FUNC PyInit_hlasm_extension(void)
{
    return PyModule_Create(&hlasm_extension_module);
}
