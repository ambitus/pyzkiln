//
// r_admin.c - the C entrypoint for the R_admin interface of py_racf
//
// This is the main entry point to the C code that communicates between
// Python and RACF.  This code initializes the global resources needed for
// all RACF calls, and routes control to the code specific to the callable
// service requested by the caller.
//
// There are 52 RACF callable services that each have their own set of
// sub-functions.  Once control is routed to the proper service handler,
// another level of routing is often performed to get to the specific 
// sub-function that the caller requests.
//
// Author: Joe Bostian
// Copyright Contributors to the Ambitus Project.
// SPDX-License-Identifier: Apache-2.0
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <iconv.h>

#include "r_admin.h"
#include "pxtr.h"
#include "keyval.h"
#include "json.h"
#include "fileio.h"
#include "transcode.h"

#pragma export(r_admin)


// Local prototypes
int            ra_get_func_type(R_ADMIN_CTL_T *, KV_CTL_T *);
KV_CTL_T      *ra_run_function(R_ADMIN_CTL_T *, int);
R_ADMIN_CTL_T *ra_init(char *, char *, FLAG);
R_ADMIN_CTL_T *ra_term(R_ADMIN_CTL_T *);


// -----------------------------------------------------------------------
// Mainline code - return int.  No RC type in the Python code.
// -----------------------------------------------------------------------
int r_admin(char *pFile_name_req, char *pFile_name_res, int fDebug)
   {
    RC rc = SUCCESS;
    R_ADMIN_CTL_T *pRACtl = ra_init(pFile_name_req, pFile_name_res, (FLAG)fDebug);

    if (pRACtl != NULL)
       {
        char *pJson_str_req = NULL;
        char *pJson_str_res = NULL;

        // Process request
        if ((pJson_str_req = read_file(pFile_name_req, pRACtl->pLog)) != NULL)
           {
            log_debug(pRACtl->pLog, "Build key-value list for request");
            pRACtl->pKVCtl_req = json_to_kv(pJson_str_req, pRACtl->pLog);
            log_set_name(pRACtl->pLog, "R_admin");

            if (pRACtl->pKVCtl_req != NULL)
               {
                // kv_print(pRACtl->pKVCtl_req);
                log_debug(pRACtl->pLog, "Get the function type requested");
                pRACtl->iFunc_type = ra_get_func_type(pRACtl, pRACtl->pKVCtl_req);

                if (pRACtl->iFunc_type > ADMIN_FUNC_NONE)
                   {
                    log_debug(pRACtl->pLog, "Perform RACF runction");
                    pRACtl->pKVCtl_res = ra_run_function(pRACtl, pRACtl->iFunc_type);
                   }

               }

           }

        // Process response
        if (pRACtl->pKVCtl_res != NULL)
           {
            log_debug(pRACtl->pLog, "Build json for results");
            pJson_str_res = json_from_kv(pRACtl->pKVCtl_res, pRACtl->pLog);
            log_set_name(pRACtl->pLog, "R_admin");
            if (pJson_str_res != NULL)
               rc = write_file(pFile_name_res, pJson_str_res, pRACtl->pLog);
           }

        else
           rc = FAILURE;

        if (pJson_str_req != NULL)
           free(pJson_str_req);
        if (pJson_str_res != NULL)
           free(pJson_str_res);
       }

    else
       rc = FAILURE;
    return (int)rc;
   }                                   // r_admin


// -----------------------------------------------------------------------
// Local subroutines
// -----------------------------------------------------------------------
KV_CTL_T *ra_run_function(R_ADMIN_CTL_T *pRACtl, int iFunc_type)
   {
    KV_CTL_T *pKVCtl_res = NULL;

    // Process based on the function requested, and the authorization required.
    if (((pRACtl->iFunc_type >= ADMIN_ADD_USER)  && (pRACtl->iFunc_type <= ADMIN_LST_USER)) ||
        ((pRACtl->iFunc_type >= ADMIN_ADD_GROUP) && (pRACtl->iFunc_type <= ADMIN_ALT_SETR)))
       {
        pRACtl->iFunc_grp = ADMIN_GRP_UPDATE;
        // rc = update_main(iFunc_type, profile_name, &ractl);
       }

    else if (((pRACtl->iFunc_type >= ADMIN_XTR_USER) && (pRACtl->iFunc_type <= ADMIN_XTR_CONNECT)) ||
             (pRACtl->iFunc_type == ADMIN_XTR_RESOURCE) ||
             (pRACtl->iFunc_type == ADMIN_XTR_NEXT_RESOURCE))
       {
        pRACtl->iFunc_grp = ADMIN_GRP_PXTR;
        log_debug(pRACtl->pLog, "Calling pxtr_run, iFunc_type: %d", pRACtl->iFunc_type);
        pKVCtl_res = pxtr_run(pRACtl, pRACtl->pLog);
       }

    else if (pRACtl->iFunc_type == ADMIN_XTR_PWENV)
       {
        pRACtl->iFunc_grp = ADMIN_GRP_PWXTR;
        // rc = pwxtr_main(iFunc_type, profile_name, &ractl);
       }

    else if (pRACtl->iFunc_type == ADMIN_XTR_PPENV)
       {
        pRACtl->iFunc_grp = ADMIN_GRP_PPXTR;
        // rc = ppxtr_main(iFunc_type, profile_name, &ractl);
       }

    else if (pRACtl->iFunc_type == ADMIN_XTR_SETR)
       {
        pRACtl->iFunc_grp = ADMIN_GRP_SRXTR;
        // rc = srxtr_main(iFunc_type, profile_name, &ractl);
       }

    else if (pRACtl->iFunc_type == ADMIN_UNL_SETR)
       {
        pRACtl->iFunc_grp = ADMIN_GRP_SRUNL;
        // rc = srunl_main(iFunc_type, profile_name, &ractl);
       }

    else if (pRACtl->iFunc_type == ADMIN_RUN_CMD)
       {
        pRACtl->iFunc_grp = ADMIN_GRP_CMDRUN;
        // rc = cmdrun_main(iFunc_type, profile_name, &ractl);
       }

    else
      log_error(pRACtl->pLog, "Function code %d not recognized\n", pRACtl->iFunc_type);

    log_set_name(pRACtl->pLog, "R_admin");
    return pKVCtl_res;
   }                                   // ra_run_function

int ra_get_func_type(R_ADMIN_CTL_T *pRACtl, KV_CTL_T *pKVCtl_req)
   {
    int iFunc_type = ADMIN_FUNC_NONE;
    KV_T     *pKV;
    KVV_T *pKVVal;

    // Get the function type from the input user request.
    pKV = kv_get(pRACtl->pKVCtl_req, pRACtl->pKVCtl_req->pKV_head, "racf", 1, KEY_REQUIRED);
    if (pKV != NULL)
       {
        pKV = kv_get(pRACtl->pKVCtl_req, pKV, "func_type", 1, KEY_REQUIRED);

        if (pKV != NULL)
           {
            pKVVal = kvv_get(pRACtl->pKVCtl_req, pKV, VAL_TYPE_NUM);

            if ((pKV != NULL) && (pKVVal->pVal != NULL))
               {
                log_info(pRACtl->pLog, "Retrieved key %s, value %s", pKV->pKey, pKVVal->pVal);
                iFunc_type = atoi(pKVVal->pVal);
               }

           }

       }

    return iFunc_type;
   }

R_ADMIN_CTL_T *ra_init(char *fnRequest, char *fnResults, FLAG fDebug)
   {
    R_ADMIN_CTL_T *pRACtl = calloc(1, sizeof(R_ADMIN_CTL_T)+
                                      (strlen(fnRequest)+1) +
                                      (strlen(fnResults)+1));

    if (pRACtl != NULL)
       {
        pRACtl->fDebug = fDebug;
        pRACtl->pLog = logger_init(fDebug, "R_admin");

        if (pRACtl->pLog != NULL)
           {
            BYTE *pFN = ((BYTE *)pRACtl)+sizeof(R_ADMIN_CTL_T);

            // Validate and save the request and results file names.
            if (fnRequest != NULL)
               {
                pRACtl->pFName_req = pFN;
                strcpy(pRACtl->pFName_req, fnRequest);
                pFN += strlen(fnRequest);

                if (fnResults != NULL)
                   {
                    pRACtl->pFName_res = pFN;
                    strcpy(pRACtl->pFName_res, fnResults);
                   }

                else
                   log_error(pRACtl->pLog, "No results file specified.");
               }

            else
               log_error(pRACtl->pLog, "No request file specified.");
           }

       }

   if ((pRACtl == NULL) || (pRACtl->pLog == NULL) ||
       (pRACtl->pFName_req == NULL) || (pRACtl->pFName_res == NULL))
      pRACtl = ra_term(pRACtl);

    return pRACtl;
   }                                   // ra_init

R_ADMIN_CTL_T *ra_term(R_ADMIN_CTL_T *pRACtl)
   {
    if (pRACtl != NULL)
       {
        if (pRACtl->pKVCtl_req != NULL)
           {
            kv_term(pRACtl->pKVCtl_req);
            pRACtl->pKVCtl_req = NULL;
           }

        if (pRACtl->pKVCtl_res != NULL)
           {
            kv_term(pRACtl->pKVCtl_res);
            pRACtl->pKVCtl_res = NULL;
           }

        if (pRACtl->pLog != NULL)
           {
            logger_term(pRACtl->pLog);
            pRACtl->pLog = NULL;
           }

        free(pRACtl);
       }
    
    return NULL;
   }                                   // ra_term

// Getters and setters
KV_CTL_T *ra_get_kvctl(R_ADMIN_CTL_T * pRACtl, FLAG fType)
   {
    if (fType == KV_REQ)
       return pRACtl->pKVCtl_req;
    else
       return pRACtl->pKVCtl_res;
   }                                   // ra_get_kvctl

void ra_set_kvctl(R_ADMIN_CTL_T *pRACtl, KV_CTL_T *pKVCtl, FLAG fType)
   {
    if (fType == KV_REQ)
       pRACtl->pKVCtl_req = pKVCtl;
    else
       pRACtl->pKVCtl_res = pKVCtl;
    return;
   }                                   // ra_set_kvctl