#ifndef __PXTR_H__
#define __PXTR_H__
//
// Data shared between routines that make up the profile extact functions
// of the R_admin service call.
//
// Note that live parsing editors don't generally understand the __ptr32
// qualifier on pointer declarations.  They will flag errors in the code that 
// the xl/C compiler on z/OS process correctly.
//
// Author: Joe Bostian
// Copyright Contributors to the Ambitus Project.
// SPDX-License-Identifier: Apache-2.0
//
#include <iconv.h>

#include "irrpcomp.h"
#include "r_admin.h"
#include "keyval.h"
#include "common_types.h"

// Scaffolding for VSCode - it doesn't understand __ptr32.  Remove this before
// building the code lest ye waken a dragon.
// #define __ptr32

// Constants
#define L_RACF_WORK_AREA 1024

// We have to marshall our arguments into a 31-bit area that the RACF 
// interface can use.  Not only do the arguments have to live in this
// 31-bit area "under the bar", the argument list has to be there as
// well.
typedef struct CALL_ARGS {
   char RACF_work_area[L_RACF_WORK_AREA];

   int  ALET_SAF_rc;                   // return and reason codes
   RC   SAF_rc;
   int  ALET_RACF_rc;
   RC   RACF_rc;
   int  ALET_RACF_rsn;
   RSN  RACF_rsn;

   BYTE func_code;                     // extract function to perform

   R_ADMIN_PXTR_PARMS_T pxtr_parms;     // extract function parm area

   PROF_NAME_T prof_name;              // name of profile to extract

   UINT ACEE;                          // output area for the service
   BYTE outbuf_subpool;

   char * __ptr32 pOutbuf;             // R_admin returns data here
   } CALL_ARGS_T;

// Normal OS linkage conventions require a list of pointers for the 
// argument list. This is what will be passed to RACF assembler service.
typedef struct CALL_ARGS_LIST {
   char * __ptr32 pWork_area;

   int *  __ptr32 pALET_SAF_rc;
   RC *   __ptr32 pSAF_rc;
   int *  __ptr32 pALET_RACF_rc;
   RC *   __ptr32 pRACF_rc;
   int *  __ptr32 pALET_RACF_rsn;
   RSN *  __ptr32 pRACF_rsn;

   BYTE * __ptr32 pFunc_code;

   R_ADMIN_PXTR_PARMS_T * __ptr32 pPXTR_parms;

   // PROF_NAME_T * __ptr32 pProf_name;
   char * __ptr32 pProf_name;

   UINT * __ptr32 pACEE;
   BYTE * __ptr32 pOutbuf_subpool;
   char * __ptr32 * __ptr32 ppOutbuf;
   } CALL_ARGS_LIST_T;

// A convenience method to group the allocation of all the required
// storage areas.
typedef struct UNDERBAR_ARG_AREA {
   CALL_ARGS_T args;
   CALL_ARGS_LIST_T arg_list;
   } UNDERBAR_ARG_AREA_T;

// The main profile extract anchor block.
typedef struct PXTR_CTL {
  UNDERBAR_ARG_AREA_T *pP31Area;
  int       lP31Area;
  LOGGER_T *pLog;
  } PXTR_CTL_T;

// Main profile extract method.
KV_CTL_T *pxtr_run(R_ADMIN_CTL_T *, LOGGER_T *);

// Profile extract to key-value list.
KV_CTL_T *results_to_kv(PXTR_CTL_T *, R_ADMIN_PXTR_PARMS_T *);

// Profile extract dump methods.
void pxtr_print(R_ADMIN_PXTR_PARMS_T *, LOGGER_T *);
void pxtr_dump(R_ADMIN_PXTR_PARMS_T *, LOGGER_T *);
void pxtr_dump_args_parms(PXTR_CTL_T *, LOGGER_T *);

// Glue code to call R_admin (IRRSEQ00).
int callRadmin(CALL_ARGS_LIST_T * __ptr32);

#endif