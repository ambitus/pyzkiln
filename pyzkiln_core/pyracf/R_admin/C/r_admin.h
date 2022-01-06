#ifndef __R_ADMIN_H__
#define __R_ADMIN_H__
//
// r_admin.h - data shared between routines that implement the R_admin service call.
//
// Author: Joe Bostian
// Copyright Contributors to the Ambitus Project.
// SPDX-License-Identifier: Apache-2.0
//
#include "irrpcomp.h"
#include "keyval.h"
#include "log.h"
#include "common_types.h"

// Estimated ratio of JSON form vs address/offset/length.
const float JSON_BUFFER_RATIO = 1.25;

const FLAG KV_REQ = 0x00;
const FLAG KV_RES = 0x01;

// The main py_racf C API control block.  This is shared between modules for holding
// global information.
typedef struct R_ADMIN_CTL {
  int       iFunc_type;
  int       iFunc_grp;
  char     *pFName_req;                // request
  KV_CTL_T *pKVCtl_req;
  char     *pFName_res;                // result
  KV_CTL_T *pKVCtl_res;
  FLAG      fDebug;
  LOGGER_T *pLog;                      // area for logging in C

  // Return status from assembler call.  Use for FFDC.
  RC  SAF_rc;
  RC  RACF_rc;
  RSN RACF_rsn;

  // JSON generation state.
  char *p_jbuf;
  char *p_jbuf_finger;
  int   l_jbuf;
  int   l_jbuf_free;
  int   n_jbuf_lines;

 } R_ADMIN_CTL_T;

// Getters and setters for key-value lists representing requests and results
KV_CTL_T *ra_get_kvctl(R_ADMIN_CTL_T *, FLAG);
void      ra_set_kvctl(R_ADMIN_CTL_T *, KV_CTL_T *, FLAG);
#endif