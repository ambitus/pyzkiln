#include <stdlib.h>
#include <stdio.h>
#include <__le_cwi.h>
#include <unistd.h>
#include <signal.h>
#include <iconv.h>
#include <string.h>
#include <stdint.h>

#define MLENGTH    8    /*length of module name string */
#define FLENGTH    6    /*length of function name string */
#define ALENGTH    16   /*length of target program argument */


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
}RO31_cb;

/* Module name to load */
typedef struct RO31_module{
    int length;
    char module_name[MLENGTH];
}RO31_module;

/* Function name to query */
typedef struct RO31_function{
    int length;
    char function_name[FLENGTH];
}RO31_function;

/* Arguments of the target program, R1 will point to content */
/* when calling target program                               */
typedef struct RO31_args{
    int length;
    int in1;
    int in2;
    int in3;
    int in4;
}RO31_args;

int caller(int n1, int n2, int n3, int n4)
{
    RO31_cb*           RO31_info;
    RO31_args*         RO31_args_p;
    RO31_module*       RO31_module_p;
    RO31_function*     RO31_func_p;
    unsigned int       ret_value;
    unsigned int       buf_len;

    buf_len =  sizeof(RO31_cb) + MLENGTH + FLENGTH + ALENGTH + 12;
    /* Get below the bar storage */
    CELQGIPB(&buf_len,(void *)&RO31_info,&ret_value);


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

    RO31_module_p->length   = MLENGTH;
    RO31_func_p->length     = FLENGTH;
    RO31_args_p->length     = ALENGTH;

    RO31_args_p -> in1 = n1;
    RO31_args_p -> in2 = n2;
    RO31_args_p -> in3 = n3;
    RO31_args_p -> in4 = n4;

    iconv_t cd = iconv_open("IBM-1047", "UTF-8");

    /* Setting up iconv module name conversion from utf to ebcdic */
    char outbuf_mod[MLENGTH];
    char *inptr_mod = "CALL.dll";
    char *outptr_mod = outbuf_mod;
    size_t insize_mod = MLENGTH;
    size_t outsize_mod = MLENGTH;

    size_t mod_res = iconv(cd, &inptr_mod, &insize_mod, &outptr_mod, &outsize_mod);
    if(mod_res == (size_t)-1){
        perror("iconv");
        return 0;
    }
    strcpy((*RO31_module_p).module_name, outbuf_mod);

    /* Setting up iconv function name conversion from utf to ebcdic */
    char outbuf_func[FLENGTH];
    char *inptr_func = "CALLEE";
    char *outptr_func = outbuf_func;
    size_t insize_func = FLENGTH;
    size_t outsize_func = FLENGTH;

    size_t func_res = iconv(cd, &inptr_func, &insize_func, &outptr_func, &outsize_func);
    if(func_res == (size_t)-1){
        printf("iconv had an unsuccesful attempt converting string\n");
        return 0;
    }
    strcpy((*RO31_func_p).function_name, outbuf_func);

    iconv_close(cd);

    /* Call CEL4RO31()      */
    CEL4RO31((void*)RO31_info);

    /* Set return value     */
    ret_value = RO31_info->ret_gr_buffer[0];

    printf("Back to 64bit program, return value is %d\n",ret_value);
    printf("THIS IS THE RETCODE %u\n", RO31_info->retcode);

    return (int)ret_value;
}

#include <Python.h>

static PyObject *return_int(PyObject *self, PyObject *args){
    int n1,n2,n3,n4;

    if(!PyArg_ParseTuple(args, "iiii", &n1,&n2,&n3,&n4)){
        return NULL;
    }

    int c = caller(n1,n2,n3,n4);
    return Py_BuildValue("i", c);
}

// module method definitions
static PyMethodDef methods[] = {
    {"HLASM_int", return_int, METH_VARARGS, "returns sum of 4 inputs"},
    {NULL, NULL, 0, NULL} // Sentinel
};

// module initialization function
static struct PyModuleDef hlasm_extension_module = {
    PyModuleDef_HEAD_INIT,
    "hlasm_extension",  // module name
    NULL,               // module documentation
    -1,                 // module state (not global)
    methods             // methods called
};

// module initialization PyInit_<module name>(void)
PyMODINIT_FUNC PyInit_hlasm_extension(void)
{
    return PyModule_Create(&hlasm_extension_module);
}
