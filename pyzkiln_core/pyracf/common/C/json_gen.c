//
// json_gen.c - json generator.  Create a json text string from a key-value pair
//           list, and write it to a results file.  See keyval.c to see how 
//           this generator interacts with the key-value list manager.
//
// Author: Joe Bostian
// Copyright Contributors to the Ambitus Project.
// SPDX-License-Identifier: Apache-2.0 
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "json.h"
#include "keyval.h"
#include "fileio.h"
#include "transcode.h"
#include "log.h"

#define MAX_LINE_LEN  256

// Keyval processing
char *root_kv(JSKV_CTL_T *);

// Init and term
JSKV_CTL_T *jsong_init(KV_CTL_T *, LOGGER_T *);
void        jsong_term(JSKV_CTL_T *);

// Utilities
RC   append_kv_json(JSKV_CTL_T *, char *, char *, BYTE);
RC   realloc_json_str(JSKV_CTL_T *, size_t);
void js_nest(JSKV_CTL_T *);
void js_unnest(JSKV_CTL_T *);


//
// Mainline code
//
char *json_from_kv(KV_CTL_T *pKVCtl, LOGGER_T *pLog)
   {
    JSKV_CTL_T *pJKCtl = jsong_init(pKVCtl, pLog);
    char *pJson_str = NULL;
   
    log_set_name(pLog, "json_gen");

    if (pJKCtl != NULL)
       {
        // kv_print(pKVCtl);
        pJson_str = root_kv(pJKCtl);
       }

    jsong_term(pJKCtl);
    return pJson_str;
   }                                   // json_from_kv


//
// Keyval processing
//
char *root_kv(JSKV_CTL_T *pJKCtl)
   {
    KV_T *pKV = kv_get_list(pJKCtl->pKVCtl);
    RC    rc = append_str_json(pJKCtl, "{\n");

    js_nest(pJKCtl);

    // Loop through all of the KVs in the list
    while((pKV != NULL) && (rc == SUCCESS))
       {
        KVV_T *pKVVal = NULL;
        char  *pKey;
        char  *pVal   = NULL;
        BYTE  val_dim;

        pKey = kv_get_key(pKV);
        val_dim = kv_get_val_dim(pKV);

        // Handle scalar and nested values.
        if (val_dim & VAL_DIM_SCALAR)
           pKVVal = kvv_get(pJKCtl->pKVCtl, pKV, VAL_TYPE_ANY);
        if (pKVVal != NULL)
           pVal = kv_get_value_val(pKVVal);

        // Nest this KV in the Json output
        if (pJKCtl->iNest < kv_get_inest(pKV))
           js_nest(pJKCtl);
        else 
        if (pJKCtl->iNest > kv_get_inest(pKV))
           js_unnest(pJKCtl);

        // Build the json from this KV pair at the proper nesting.
        if ((rc = append_kv_json(pJKCtl, pKey, pVal, val_dim)) == SUCCESS)
           pKV = kv_get_next(pKV);
       }

    js_unnest(pJKCtl);

    if (rc == SUCCESS)
       rc = append_str_json(pJKCtl, "}\n");

    if (rc != SUCCESS)
       {
        free(pJKCtl->pJson_str);
        pJKCtl->pJson_str = NULL;
        pJKCtl->lJson_str = 0;
       }

    return pJKCtl->pJson_str;
   }                                   // root_kv


//
// Init and term
//
JSKV_CTL_T *jsong_init(KV_CTL_T *pKVCtl, LOGGER_T *pLog)
   {
    JSKV_CTL_T *pJKCtl = NULL;
    int lJson_str = kv_get_list_len(pKVCtl);

    // Use the total length of the key value list as an educated guess at
    // how long the associated Json string will be.  Allocate the parse/gen
    // control block and json string separately so we can free the CB, and 
    // return the Json string top the caller
    log_debug(pLog, "Length of KV list: %d", lJson_str);

    if (lJson_str > 0)
       {
        char *pJson_str = (char *)calloc(1,lJson_str);

        pJKCtl = (JSKV_CTL_T *)calloc(1,sizeof(JSKV_CTL_T));
        
        if ((pJKCtl != NULL) && (pJson_str != NULL))
           {
            pJKCtl->pKVCtl = pKVCtl;
            pJKCtl->pJson_str = pJson_str;
            pJKCtl->lJson_str = lJson_str;
            pJKCtl->nBytes_left = pJKCtl->lJson_str;
            for(int i=0; i<MAX_INDENT; i++)
               pJKCtl->indent_str[i] = ' ';
            pJKCtl->indent_str[0] = 0x00;
            pJKCtl->index.p = pJKCtl->pJson_str;
            pJKCtl->index.i = 0;
            pJKCtl->nLines = 1;
            pJKCtl->pLog = pLog;
           }

        else
           {
            if (pJson_str == NULL)
               log_error(pLog, "Can't allocate space for Json string");
            if (pJKCtl == NULL)
               log_error(pLog, "Can't allocate space for Json parse/gen control");
           }

       }

    else
       log_error(pLog, "Falied to determine key/value list length");
    return pJKCtl;
   }                                   // jsong_init

void jsong_term(JSKV_CTL_T *pJKCtl)
   {
    if (pJKCtl != NULL)
       free(pJKCtl);
    return;
   }                                   // jsong_term

//
// Utilities
//
RC append_kv_json(JSKV_CTL_T *pJKCtl, char *pKey, char *pVal, BYTE val_dim)
   {
    char line[MAX_LINE_LEN+1];
    size_t lKey = strlen(pKey);
    size_t lVal = strlen(pVal);
    size_t nIndent = pJKCtl->iNest * 3;
    size_t lLine = lKey + lVal + nIndent + 7;
    RC rc = SUCCESS;

    // Check that we won't won't overflow the Json string buffer, and
    // re-allocate first if necessary.
    if (!IS_ROOM(lLine))
       rc = realloc_json_str(pJKCtl, lLine);

    // Make sure we won't overflow the formatted line string buffer.
    if (lLine > MAX_LINE_LEN)
       {
        log_error(pJKCtl->pLog, "Json line string too long");
        rc = FAILURE;
       }

    if (rc == SUCCESS)
       {
        // Build json for a KV with a key.
        if ((pKey != NULL) || (pVal != NULL) || (val_dim != VAL_DIM_NONE))
           {
            if (val_dim & VAL_DIM_NEST)
               sprintf(line, "%s\"%s\": {\n", pJKCtl->indent_str, pKey);
            else
               sprintf(line, "%s\"%s\": \"%s\",\n", pJKCtl->indent_str, pKey, pVal);
           }

        // Build json for a NULL KV, indicating the end of a nest.
        else
           {
            int finger = strlen(pJKCtl->pJson_str)-1;

            // Json objects are bracketed with '{', and '}'.  Members of the object
            // are separated by commas, but the last member of the object should not
            // be followed by a comma.  See if the prior line ended in ",\n", and 
            // remove the comma if so.
            if ((finger > 1) && (!strncmp(&(pJKCtl->pJson_str[finger-1]),",\n",2)))
               {
                pJKCtl->pJson_str[finger-1] = '\n';
                pJKCtl->pJson_str[finger] = 0x00;
               }

            sprintf(line, "%s}\n", pJKCtl->indent_str);
           }

        strcat(pJKCtl->pJson_str, line);
        MOVE_INDEX_FINGER(strlen(line));
       }

    return rc;
   }                                   // append_kv_json

RC append_str_json(JSKV_CTL_T *pJKCtl, char *pStr)
   {
    char line[MAX_LINE_LEN+1];
    size_t nIndent = pJKCtl->iNest * 3;
    size_t lLine = strlen(pStr) + nIndent;
    RC rc = SUCCESS;

    // Check that we won't won't overflow the Json string buffer, and
    // re-allocate first if necessary.
    if (!IS_ROOM(lLine))
       rc = realloc_json_str(pJKCtl, lLine);

    // Make sure we won't overflow the formatted line string buffer.
    if (lLine > MAX_LINE_LEN)
       {
        log_error(pJKCtl->pLog, "Json line string too long");
        rc = FAILURE;
       }

    if (rc == SUCCESS)
       {
        sprintf(line, "%s%s", pJKCtl->indent_str, pStr);
        strcat(pJKCtl->pJson_str, line);
        MOVE_INDEX_FINGER(strlen(line));
       }

    return rc;
   }                                   // append_str_json

RC realloc_json_str(JSKV_CTL_T *pJKCtl, size_t lStr)
   {
     RC rc = SUCCESS;

     return rc;
   }                                   // realloc_json_str

void js_nest(JSKV_CTL_T *pJKCtl)
   {
    int nIndent = pJKCtl->iNest * 3;

    if (nIndent < MAX_INDENT)
       {
        pJKCtl->indent_str[nIndent] = ' ';
        nIndent += 3;
        pJKCtl->indent_str[nIndent] = 0x00;
        pJKCtl->iNest++;
       }

    return;
   }                                   // js_nest

void js_unnest(JSKV_CTL_T *pJKCtl)
   {
    int nIndent = pJKCtl->iNest * 3;

    if (nIndent >= 0)
       {
        pJKCtl->indent_str[nIndent] = ' ';
        nIndent -= 3;
        pJKCtl->indent_str[nIndent] = 0x00;
        pJKCtl->iNest--;
       }

    return;
   }                                   // js_unnest