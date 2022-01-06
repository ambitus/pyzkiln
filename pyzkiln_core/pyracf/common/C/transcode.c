//
// Transcoding services
//
// Utilities and convenience functions that support the conversion of strings
// from one encoding to another.
//
// Author: Joe Bostian
// Copyright Contributors to the Ambitus Project.
// SPDX-License-Identifier: Apache-2.0
//
#include <iconv.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#include "transcode.h"
#include "common_types.h"
#include "log.h"

// Local method prototypes.
RC conv_encoding(CCSID, CCSID, char *, size_t, char *, size_t, LOGGER_T *);


// Wrappers for iconv functions to transcode strings.  Use these methods for 
// conversions of multiple strings to avoid the overhead of multiple iconv
// open/close.
iconv_t tc_init(CCSID ccsid_to, CCSID ccsid_from, LOGGER_T *pLog)
   {
    iconv_t cd;
    char enc_to[8];
    char enc_from[8];
    RC rc = SUCCESS;

    sprintf(enc_to, "%05x", ccsid_to);
    sprintf(enc_from, "%05x", ccsid_from);
    // log_debug(pLog, "Open transcode descriptor from %s, to %s", enc_from, enc_to);
    cd = iconv_open(enc_to, enc_from);

    if (cd == (iconv_t)(-1))
        {
        int e = errno;
        log_error(pLog, "Cannot open conversion descriptor from %s to %s", enc_from, enc_to);
        log_error(pLog, "   errno: %08d", e);
        log_error(pLog, "   %s", strerror(e));
        rc = FAILURE;
       }

    return cd;
   }                                   // tc_init

void tc_term(iconv_t cd, LOGGER_T *pLog)
   {
    RC rc = (RC)iconv_close(cd);

    if (rc != SUCCESS)
       {
        int e = errno;
        log_warning(pLog, "Cannot close iconv conversion descriptor");
        log_warning(pLog, "   errno: %08d", e);
        log_warning(pLog, "   %s", strerror(e));
        rc = WARNING;
       }

   }                                   // tc_term

RC tc_transcode(iconv_t cd,
                char *pFrom_str, size_t lFrom_str, 
                char *pTo_str, size_t lTo_str,
                LOGGER_T *pLog)
   {
    // All of the input to iconv needs to be a mutable value.  We can't just
    // pass the address of the input arguments.
    char *pFrom  = pFrom_str;
    char *pTo    = pTo_str;
    size_t lFrom = lFrom_str;
    size_t lTo   = lTo_str;
    size_t n;
    RC rc = SUCCESS;

    // iconv is not a string function, so it only converts the exact number of 
    // characters we tell it to.  Null terminate the string.
    // if (iconv(cd, &pFrom, &lFrom, &pTo, &lTo) >= 0)
    rc = iconv(cd, &pFrom, &lFrom, &pTo, &lTo);

    if (rc >= 0)
       pTo_str[lTo_str] = 0x00;

    else
       {
        int e = errno;
        log_error(pLog, "String transcode failed");
        log_error(pLog, "   input bytes left: %d", lFrom);
        log_error(pLog, "   errno: %08d", e);
        log_error(pLog, "   %s", strerror(e));
        rc = FAILURE;
       }    

    return rc;
   }                                   // tc_transcode

char *tc_e2a_alloc(iconv_t tc_e2a_cd, 
                   char *pEBC_str, int lEBC_str, 
                   char *pASC_str, int lASC_str, 
                   LOGGER_T *pLog)
   {
    char *pASC_new = pASC_str;

    if ((pASC_new == NULL) || (lEBC_str > lASC_str))
       {
        if (pASC_new != NULL)
           free(pASC_new);
        pASC_new = (char *)calloc(lEBC_str+1, sizeof(char));
       }

    if (tc_transcode(tc_e2a_cd, 
                     pEBC_str, lEBC_str,
                     &(pASC_new[0]), lEBC_str,
                     pLog) != 0)
       {
        if (pASC_new != NULL)
           {
            free(pASC_new);
            pASC_new = NULL;
           }
       }

    return pASC_new;
   }                                   // tc_e2a_alloc

// Convenience functions for transcoding single strings.  Make sure that
// the transcoding target string is >= in length to the source string.
RC tc_e2a(char *pEBC_str, char *pASC_str, size_t lStrs, LOGGER_T *pLog)
   {
    return conv_encoding(CCSID_ASCII, CCSID_EBCDIC,
                         pASC_str, lStrs, pEBC_str, lStrs, pLog);
   }                                   // tc_e2a

RC tc_a2e(char *pASC_str, char *pEBC_str, size_t lStrs, LOGGER_T *pLog)
   {
    return conv_encoding(CCSID_EBCDIC, CCSID_ASCII, 
                         pEBC_str, lStrs, pASC_str, lStrs, pLog);
   }                                   // tc_a2e

// Local utilities used by other methods.
RC conv_encoding(CCSID ccsid_to, CCSID ccsid_from,
                 char *pTo_str, size_t lTo_str, char *pFrom_str, size_t lFrom_str, 
                 LOGGER_T *pLog)
   {
    iconv_t cd;
    RC rc;

    cd = tc_init(ccsid_to, ccsid_from, pLog);

    if (cd >= 0)
       {
        rc = tc_transcode(cd, pFrom_str, lFrom_str, pTo_str, lTo_str, pLog);   
        tc_term(cd, pLog);
       }
    
    return(rc);
   }                                   // conv_encoding
