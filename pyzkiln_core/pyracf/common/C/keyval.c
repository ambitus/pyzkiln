//
// kvlist.c - key-value list management.  These are all of the methods for 
//            managing a list of key-value pairs that represent the input to
//            the RACF api, and the output from it.  All keys and values in
//            this list are encoded in ASCII.
//
// Author: Joe Bostian
// Copyright Contributors to the Ambitus Project.
// SPDX-License-Identifier: Apache-2.0 
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdarg.h>
#include <errno.h>

#include "keyval.h"
#include "bytes.h"
#include "log.h"
#include "dump.h"
#include "common_types.h"


//
// Init and term.
//
KV_CTL_T *kv_init(LOGGER_T *pLog)
   {
    KV_CTL_T *pKVCtl = (KV_CTL_T *)calloc(1,sizeof(KV_CTL_T));

    if (pKVCtl != NULL)
       {
        pKVCtl->tc_a2e_cd = tc_init(CCSID_EBCDIC, CCSID_ASCII, pLog);
        pKVCtl->tc_e2a_cd = tc_init(CCSID_ASCII, CCSID_EBCDIC, pLog);
        pKVCtl->nKV_nesting = 1;
        pKVCtl->pLog = pLog;       
       }

    else
       log_error(pLog, "KV control allocate failed\n");

    return pKVCtl;
   }                                   // kv_init

void kv_term(KV_CTL_T *pKVCtl)
   {
    KV_T *pKV = pKVCtl->pKV_head;
    KV_T *pKV_next;
    
    while(pKV != NULL)
       {
        KVV_T *pVal = pKV->pKVVal_head;
        KVV_T *pVal_next;

        while(pVal != NULL)
           {
            pVal_next = pVal->pNext;
            memset(pVal, 0, (sizeof(KVV_T)+pVal->lAlloc));
            free(pVal);
            pVal = pVal_next;
           }

        pKV_next = pKV->pNext;
        memset(pKV, 0, (sizeof(KV_T)+pKV->lAlloc));
        free(pKV);
        pKV = pKV_next;
       }

    return;
   }                                   // kv_term

//
// Add keys and values
//
// All KV pairs have at least a key, and optionally a value.  Add a KV 
// pair to the list, with a key.
RC kv_add(KV_CTL_T *pKVCtl, BYTE *pKey, int lKey, CCSID key_enc, BYTE val_dim)
   {
    KV_T *pKV = NULL;
    char *pASCKey;                     // ASCII key
    char *pTCKey = NULL;               // transcoded key, if needed

    // Add a regular KV with a key.
    if (pKey != NULL)
       {
        // Allocate space for the key-value pair and the key text, plus a
        // null terminator.
        pKV = kv_alloc(pKVCtl, lKey);
        if (pKV != NULL)
           pKV->pKey = (char *)pKV + sizeof(KV_T);
        else 
           return FAILURE;

        // If the key is not already in ASCII, transcode it.
        if (key_enc == CCSID_ASCII)
           pASCKey = pKey;

        else
           {
            // Use calloc to ensure null termination of the key string.
            pTCKey = (char *)calloc((size_t)(lKey+1), sizeof(char));

            // TODO: expand this section to handle non-EBCDIC input encodings,
            // and non-ASCII output encodings for the key.
            if (tc_transcode(pKVCtl->tc_e2a_cd, 
                             pKey, lKey, pTCKey, lKey, 
                             pKVCtl->pLog) == SUCCESS)
               pASCKey = pTCKey;
           }

        bytes_to_str(pKV->pKey, pASCKey, lKey, TRIM, FOLD);
        pKV->lKey = lKey;
        pKV->iNest = pKVCtl->nKV_nesting;
        pKV->val_dim = val_dim;
        // log_debug(pKVCtl->pLog, "     Add key (%s), lKey: %d", pKV->pKey, pKV->lKey);
       }

    // Add a NULL KV that indicates the end of a nest.
    else
       {
        log_debug(pKVCtl->pLog, "Adding NULL KV to end nesting.\n");
        pKV = kv_alloc(pKVCtl, 0);
        if (pKV == NULL)
           return FAILURE;
        pKV->iNest = pKVCtl->nKV_nesting;
       }

    // Now add this to the key-value chain.
    if (pKVCtl->pKV_head == NULL)
       {
        pKVCtl->pKV_head = pKV;
        pKVCtl->pKV_tail = pKV;
       }

    else
       {
        pKV->pPrev = pKVCtl->pKV_tail;
        pKVCtl->pKV_tail->pNext = pKV;
        pKVCtl->pKV_tail = pKV;
       }

    if (pTCKey != NULL)
       free(pTCKey);
    return SUCCESS;
   }                                   // kv_add

// Add a value to the current KV pair.  
RC kv_add_value(KV_CTL_T *pKVCtl, BYTE *pVal, int lVal, CCSID val_enc, BYTE val_type)
   {
    KV_T *pKV = pKVCtl->pKV_tail;
    KVV_T *pKVV = NULL;
    char *pASCVal;                     // ASCII value
    char *pTCVal = NULL;               // transcoded value, if needed

    if (pVal != NULL)
       {
        // Allocate space for the value struct and the value text, plus a
        // null terminator.
        pKVV = kv_alloc_val(pKVCtl, lVal);
        if (pKVV != NULL)
           pKVV->pVal = (char *)pKVV + sizeof(KVV_T);
        else
           return FAILURE;
       }

    else
       {
        log_error(pKVCtl->pLog, "No value specified to add.\n");
        return FAILURE;
       }

    //  If the value is not already in ASCII, transcode it. 
    if (val_enc == CCSID_ASCII)
       pASCVal = pVal;

    else
       {
        // Use calloc to ensure null termination of the value string.
        pTCVal = (char *)calloc((size_t)(lVal+1), sizeof(char));

        // TODO: expand this section to handle non-EBCDIC input encodings,
        // and non-ASCII output encodings for the key.
        if (tc_transcode(pKVCtl->tc_e2a_cd, 
                         pVal, lVal, pTCVal, lVal, 
                         pKVCtl->pLog) == SUCCESS)
           pASCVal = pTCVal;
       }

    bytes_to_str(pKVV->pVal, pASCVal, lVal, NO_FOLD, NO_FOLD);
    pKVV->lVal = lVal;
    pKVV->val_type = val_type;

    // Update the KV pair with this new value.  If this is the first value
    // for this key, mark it as a scalar value.
    if (pKV->pKVVal_head == NULL)
       {
        kv_set_val_dim(pKVCtl, VAL_DIM_SCALAR);
        pKV->pKVVal_head = pKVV;
        pKV->pKVVal_tail = pKVV;
       }

    else
       {
        kv_set_val_dim(pKVCtl, VAL_DIM_VECTOR);
        pKV->pKVVal_tail->pNext = pKVV;
        pKV->pKVVal_tail = pKVV;
       }

    pKV->nVals++;

    if (pTCVal != NULL)
       free(pTCVal);
    return SUCCESS;
   }                                   // kv_add_value

void kv_nest(KV_CTL_T *pKVCtl)
   {
    // The current key value is nested.
    kv_set_val_dim(pKVCtl, VAL_DIM_NEST);
    pKVCtl->nKV_nesting++;
    // log_debug(pKVCtl->pLog, "    Nest keyval: %d", pKVCtl->nKV_nesting);
    return;
   }                                   // kv_nest

void kv_unnest(KV_CTL_T *pKVCtl)
   {
    pKVCtl->nKV_nesting--;
    // log_debug(pKVCtl->pLog, "    Un-nest keyval: %d", pKVCtl->nKV_nesting);
    return;
   }                                   // kv_unnest


//
// Getters and setters
//
KV_T *kv_get(KV_CTL_T *pKVCtl, KV_T *pKV, char *pKey_name, int iInstance, FLAG fRequired)
   {
    KV_T *p = pKV;
    int i = 1;

    // Start at the KV pair passed in, and look forward through the list for
    // the instance required.  If we are looking for a required KV instance, and 
    // we don't find it, flag an error.
    while((p != NULL) && (!kv_is_key(p,pKey_name)) && (i<=iInstance))
       {
        if (kv_is_key(p,pKey_name))
           i++;
        p = kv_get_next(p);
       }

    // Note that if the key name is found, but it isn't the ith requested instance,
    // we will reach the end of the list, and p will be NULL.
    if (p != NULL)
       {
        if (!kv_is_key(p, pKey_name))
           p = NULL;
       }

    else
       {
        if (fRequired == KEY_REQUIRED)
           log_error(pKVCtl->pLog, "Required keyvalue (%s) not found", pKey_name);
       }

    return p;
   }                                   // kv_get

// Get the key associated with a KV pair.
char *kv_get_key(KV_T *pKV)
   {
    return pKV->pKey;
   }                                   // kv_get_key

// Get the value dimension for a KV pair.
BYTE kv_get_val_dim(KV_T *pKV)
   {
    return pKV->val_dim;
   }                                   // kv_get_val_dim

// Get a scalar value associated with a key.  The input pKV should point to the
// key for the value to fetch.
KVV_T *kvv_get(KV_CTL_T *pKVCtl, KV_T *pKV, BYTE val_type)
   {
    KVV_T *pKVVal = NULL;

    // First, make sure we have a KV pair with a scalar value.
    if ((pKV != NULL) && (pKV->val_dim & VAL_DIM_SCALAR))
       pKVVal = pKV->pKVVal_head;

    else
       {
        if (pKV == NULL)
           log_info(pKVCtl->pLog, "(kvv_get), Attempt to fetch NULL KV pair");
        else
           log_error(pKVCtl->pLog,  "(kvv_get), Value for key (%s) is not scalar", pKV->pKey);
       }

    // Now make sure the value is the right type, and has an actual value.
    if (pKVVal != NULL)
       {

        if ((val_type != VAL_TYPE_ANY) && (pKVVal->val_type != val_type))
           {
            log_error(pKVCtl->pLog,  "(kvv_get), Expected val_type %d, got %d", val_type, pKVVal->val_type);
            pKVVal = NULL;
           }

        if (pKVVal->pVal == NULL)
           {
            log_error(pKVCtl->pLog, "(kvv_get), NULL value in pKVVal");
            pKVVal = NULL;
           }

       }

    return pKVVal;
   }                                   // kvv_get

char *kv_get_value_val(KVV_T *pKVVal)
   {
    return pKVVal->pVal;
   }

KV_T *kv_get_next(KV_T *pKV)
   {
    return pKV->pNext;
   }                                   // kv_get_next

KV_T *kv_get_list(KV_CTL_T *pKVCtl)
   {
    return pKVCtl->pKV_head;
   }                                   // kv_get_list

int kv_get_nesting(KV_CTL_T *pKVCtl)
   {
    return pKVCtl->nKV_nesting;
   }                                   // kv_get_nesting

int kv_get_inest(KV_T *pKV)
   {
    return pKV->iNest;
   }                                   // kv_get_nesting

int kv_get_list_len(KV_CTL_T *pKVCtl)
   {
    return pKVCtl->lKV_list;
   }                                   // kv_get_list_len

RC kv_set_val_dim(KV_CTL_T *pKVCtl, BYTE val_dim)
   {
    RC rc = SUCCESS;

    if (pKVCtl->pKV_tail != NULL)
       {

        switch(val_dim)
           {
            case VAL_DIM_NONE:
               ; // log_debug(pKVCtl->pLog, "    Set value dimension to NONE");
               break;
           case VAL_DIM_SCALAR:
               ; // log_debug(pKVCtl->pLog, "    Set value dimension to SCALAR");
               break; 
            case VAL_DIM_VECTOR:
               ; // log_debug(pKVCtl->pLog, "    Set value dimension to VECTOR");
               break;
            case VAL_DIM_NEST:
               ; //log_debug(pKVCtl->pLog, "    Set value dimension to NEST");
               break;
            default:
               log_error(pKVCtl->pLog, "(kv_set_val_dim), Unrecognized value dimension (%02x)", val_dim);
               rc = FAILURE;
           }

        if (rc == SUCCESS)
           pKVCtl->pKV_tail->val_dim |= val_dim;
       }

    return rc;
   }                                   // kvv_set_dim

FLAG kv_is_key(KV_T *pKV, char *pKey_name)
   {
    FLAG fMatch = FALSE;

    if ((pKV != NULL) && (pKV->pKey != NULL) && (pKey_name != NULL) && (!strcmp(pKV->pKey, pKey_name)))
       fMatch = TRUE;

    return fMatch;
   }                                   // kv_is_key


//
// Utilities
//
KV_T *kv_alloc(KV_CTL_T *pKVCtl, int lKey)
   {
    KV_T *pKV;
    int lAlloc = sizeof(KV_T) + lKey;

    lAlloc = lAlloc + 24 - (lAlloc % 16);
    pKV = (KV_T *)calloc(1,lAlloc);

    if (pKV != NULL)
       {
        pKV->lAlloc = lAlloc;
        pKVCtl->lKV_list += lAlloc;
       }

    else
       {
        log_error(pKVCtl->pLog, "key-value allocate failed.\n");
        log_error(pKVCtl->pLog, "        Key len: %d\n", lKey);
       }

    return pKV;
   }                                   // kv_alloc

KVV_T *kv_alloc_val(KV_CTL_T *pKVCtl, int lVal)
   {
    KVV_T *pKVV;
    int lAlloc = sizeof(KVV_T) + lVal;

    lAlloc = lAlloc + 24 - (lAlloc % 16);
    pKVV = (KVV_T *)calloc(1,lAlloc);

    if (pKVV != NULL)
       pKVV->lAlloc = lAlloc;

    else
       {
        log_error(pKVCtl->pLog, "Value allocate failed.\n");
        log_error(pKVCtl->pLog, "        Value len: %d\n", lVal);
       }

    return pKVV;
   }                                   // kv_alloc_val

void kv_print(KV_CTL_T *pKVCtl)
   {
    KV_T *pKV = pKVCtl->pKV_head;

    log_info(pKVCtl->pLog, "--------------------------------");
    log_info(pKVCtl->pLog, "keyval list:");
    // log_debug(pKVCtl->pLog, "pKVCtl: %08x", pKVCtl);
    // log_debug(pKVCtl->pLog, "pKV_head: %08x,  pKV_tail: %08x", pKVCtl->pKV_head,  pKVCtl->pKV_tail);
    // log_debug(pKVCtl->pLog, "nKV_nesting: %d", pKVCtl->nKV_nesting);

    while(pKV != NULL)
       {
        kv_key_print(pKVCtl, pKV);
        if (pKV->nVals > 0)
           kvv_print(pKVCtl, pKV);
        pKV = pKV->pNext;
       }

    log_info(pKVCtl->pLog, "--------------------------------");
   }                                   // kv_print

void kv_key_print(KV_CTL_T *pKVCtl, KV_T *pKV)
   {

    if (pKV != NULL)
       {
        log_info(pKVCtl->pLog, "Key: %s, nVals: %d, iNest: %d, val dim: %02x", 
        pKV->pKey, pKV->nVals, pKV->iNest, pKV->val_dim);

        /*
        log_info(pKVCtl->pLog, "Key: %s, pKV: %08x, ", pKV->pKey, pKV);
        log_info(pKVCtl->pLog, "   nVals: %d, iNest: %d, val dim: %d", 
                 pKV->nVals, pKV->iNest, pKV->val_dim);

        switch(pKV->val_dim)
           {
            case VAL_DIM_SCALAR:
               log_info(pKVCtl->pLog, "   scalar dimension");
               break;
            case VAL_DIM_VECTOR:
               log_info(pKVCtl->pLog, "   vector dimension");
               break;
            case VAL_DIM_NEST:
               log_info(pKVCtl->pLog, "   nested member dimension");
               break;
            default:
               log_error(pKVCtl->pLog, "   unknown value dimension");
           }
        */ 
       }

   }                                   // kv_key_print

void kvv_print(KV_CTL_T *pKVCtl, KV_T *pKV)
   {
    KVV_T *pKVVal = pKV->pKVVal_head;

    while(pKVVal != NULL)
       {
        int iVal = 1;
        log_info(pKVCtl->pLog, "   value %d: |%s|, len: %d, type: %d", 
                 iVal, pKVVal->pVal, pKVVal->lVal, pKVVal->val_type);
        /*
        switch(pKVVal->val_type)
           {
            case VAL_TYPE_TXT:
               log_info(pKVCtl->pLog, "   text value");
               break;
            case VAL_TYPE_NUM:
               log_info(pKVCtl->pLog, "   numeric value");
               break;
            default:
               log_info(pKVCtl->pLog, "   unknown value type");
           }
        */
        iVal++;
        pKVVal = pKVVal->pNext;
       }

    return;
   }                                   // kvv_print