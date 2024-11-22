//Copyright IBM Corp. 2024.

#include <Python.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <__le_cwi.h>
#include <unistd.h>
#include <signal.h>
#include <iconv.h>
#include <string.h>

#define MLENGTH    10  /*length of module name string */
#define FLENGTH    7   /*length of function name string */
#define ALENGTH    8   /*length of target program argument */

/* Fixed length structure RO31_CB */
typedef struct RO31_cb{
    unsigned int version;
    unsigned int length;
    unsigned int flags;
    unsigned int off_module;
    unsigned int off_func;
    unsigned int off_args;
    unsigned int dll_handle;
    unsigned int func_desc;
    unsigned int ret_gr_buffer[5];
    unsigned int retcode;
} RO31_cb;

/* Module name to load */
typedef struct RO31_module{
    int length;
    char module_name[MLENGTH];
} RO31_module;

/* Function name to query */
typedef struct RO31_function{
    int length;
    char function_name[FLENGTH];
} RO31_function;

/* Arguments of the target program, R1 will point to content */
/* when calling target program                               */
typedef struct RO31_args{
    int length;
    int in1;
    int in2;
} RO31_args;

int call_cobtest_31bit(int n1, int n2)
{
    RO31_cb* RO31_info;
    RO31_args* RO31_args_p;
    RO31_module* RO31_module_p;
    RO31_function* RO31_func_p;
    unsigned int ret;
    unsigned int buf_len;

    buf_len =  sizeof(RO31_cb) + MLENGTH + FLENGTH + ALENGTH + 12;
    /* Get below the bar storage */
    CELQGIPB(&buf_len,(void *)&RO31_info, &ret);

    /* Init the RO31_INFO */
    (*RO31_info).version = 1;
    (*RO31_info).flags = 0xE0000000;
    (*RO31_info).off_module = sizeof(RO31_cb);
    (*RO31_info).off_func = (*RO31_info).off_module + MLENGTH + 4;
    (*RO31_info).off_args = (*RO31_info).off_func + FLENGTH + 4;
    (*RO31_info).length = sizeof(RO31_cb) + MLENGTH + FLENGTH + ALENGTH + 12;

    RO31_args_p = (RO31_args*)(((intptr_t)RO31_info)+(*RO31_info).off_args);
    RO31_module_p = (RO31_module*)(((intptr_t)RO31_info)+(*RO31_info).off_module);
    RO31_func_p = (RO31_function*)(((intptr_t)RO31_info)+(*RO31_info).off_func);

    RO31_module_p->length = MLENGTH;
    RO31_func_p->length = FLENGTH;
    RO31_args_p->length = ALENGTH;

    RO31_args_p->in1 = n1;
    RO31_args_p->in2 = n2;

    iconv_t cd = iconv_open("IBM-1047", "UTF-8");

    /* Setting up iconv module name conversion from utf to ebcdic */
    char outbuf_mod[MLENGTH];
    char *inptr_mod = "cobtest.so";
    char *outptr_mod = outbuf_mod;
    size_t insize_mod = MLENGTH;
    size_t outsize_mod = MLENGTH;

    size_t mod_res = iconv(cd, &inptr_mod, &insize_mod, &outptr_mod, &outsize_mod);
    if (mod_res == (size_t)-1) {
        perror("iconv");
        return 0;
    }
    strcpy((*RO31_module_p).module_name, outbuf_mod);

    /* Setting up iconv function name conversion from utf to ebcdic */
    char outbuf_func[FLENGTH];
    char *inptr_func = "COBTEST";
    char *outptr_func = outbuf_func;
    size_t insize_func = FLENGTH;
    size_t outsize_func = FLENGTH;

    size_t func_res = iconv(cd, &inptr_func, &insize_func, &outptr_func, &outsize_func);
    if (func_res == (size_t)-1) {
        printf("iconv had an unsuccesful attempt to convert function\n");
        return 0;
    }
    strcpy((*RO31_func_p).function_name, outbuf_func);

    iconv_close(cd);

    printf("calling\n");
    /* Call CEL4RO31()      */
    CEL4RO31((void*)RO31_info);

    /* Set return value     */
    ret = RO31_info->ret_gr_buffer[0];
    printf("called %d\n", ret);

    return (int)ret;
}

static PyObject *call_cobtest(PyObject *self, PyObject *args){
    int n1, n2;

    if(!PyArg_ParseTuple(args, "ii", &n1, &n2)){
        return NULL;
    }

    int ret = call_cobtest_31bit(n1, n2);
    return Py_BuildValue("i", ret);
}

// module method definitions
static PyMethodDef methods[] = {
    {"call_cobtest", call_cobtest, METH_VARARGS, "returns sum of 4 inputs"},
    {NULL, NULL, 0, NULL}
};

// module initialization function
static struct PyModuleDef callcobol_exension_module = {
    PyModuleDef_HEAD_INIT,
    "callcobol",
    NULL,
    -1,
    methods
};

// module initialization PyInit_<module name>(void)
PyMODINIT_FUNC PyInit_callcobol(void)
{
    return PyModule_Create(&callcobol_exension_module);
}

