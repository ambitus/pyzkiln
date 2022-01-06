#ifndef __KV_LIST_H__
#define __KV_LIST_H__
//
// kvlist.h - Definitions for building a list of key/value pairs parsed
//            from a json file.
//
// Author: Joe Bostian
// Copyright Contributors to the Ambitus Project.
// SPDX-License-Identifier: Apache-2.0 
//
#include "transcode.h"
#include "log.h"
#include "common_types.h"

#define L_INIT_KVLIST  2048
#define KVLIST_REALLOC_MULT  1.25

#define KEY_OPTIONAL   0x00
#define KEY_REQUIRED   0x01

// Dimension and type of the value for the KV pair.  A KV pair with no key,
// value, dimension, or type indicates the end of a nesting.
#define VAL_DIM_NONE   0x00
#define VAL_DIM_SCALAR 0x01
#define VAL_DIM_VECTOR 0x02
#define VAL_DIM_NEST   0x04

#define VAL_TYPE_NONE  0x00
#define VAL_TYPE_ANY   0x01
#define VAL_TYPE_TXT   0x02
#define VAL_TYPE_NUM   0x03
#define VAL_TYPE_BOOL  0x04

#define NOT_LAST_VAL  0x00
#define LAST_VAL      0x01

// Values associated with a key.  A key may have a single scalar value, or an
// array of values associated with it.  Since we don't know how many values there
// are at the time they key is parsed from the input, implement the value list as
// a linked list. 
typedef struct KVV {
    char  *pVal;
    int    lVal;
    BYTE   val_type;
    struct KVV *pNext;
    int    lAlloc;
   } KVV_T;

// A key-value pair.  These pairs are implemented as a list of linked entries that
// are allocated from the KV area that starts with the control area.
typedef struct KV {
    char *pKey;
    int   lKey;
    int   nVals;
    KVV_T *pKVVal_head;
    KVV_T *pKVVal_tail;
    int  iNest;
    BYTE val_dim;
    struct KV *pPrev;
    struct KV *pNext;
    int lAlloc;
   } KV_T;

// Key-value control area.
typedef struct KV_CTL {
    BYTE     *pOffset_base;
    KV_T     *pKV_head;
    KV_T     *pKV_tail;
    int       nKV_nesting;
    int       lKV_list;
    iconv_t tc_a2e_cd;
    iconv_t tc_e2a_cd;
    LOGGER_T *pLog;
   } KV_CTL_T;


// Key-value methods.
KV_CTL_T *kv_init();
void      kv_term(KV_CTL_T *);
RC        kv_add(KV_CTL_T *, BYTE *, int, CCSID, BYTE);
RC        kv_add_value(KV_CTL_T *, BYTE *, int, CCSID, BYTE);
void      kv_nest(KV_CTL_T *);
void      kv_unnest(KV_CTL_T *);
KV_T     *kv_get(KV_CTL_T *, KV_T *, char *, int, FLAG);
char     *kv_get_key(KV_T *);
KVV_T *kvv_get(KV_CTL_T *, KV_T *, BYTE);
char     *kv_get_value_val(KVV_T *);
KV_T     *kv_get_next(KV_T *);
KV_T     *kv_get_list(KV_CTL_T *);
int       kv_get_nesting(KV_CTL_T *);
int       kv_get_list_len(KV_CTL_T *);
RC        kv_set_val_dim(KV_CTL_T *, BYTE);
FLAG      kv_is_key(KV_T *, char *);
KV_T     *kv_alloc(KV_CTL_T *, int);
KVV_T *kv_alloc_val(KV_CTL_T *, int);
void      kv_print(KV_CTL_T *);
void      kv_key_print(KV_CTL_T *, KV_T *);
void      kvv_print(KV_CTL_T *, KV_T *);

#endif