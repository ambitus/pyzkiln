#ifndef __TRANSCODE_H__
#define __TRANSCODE_H__
//
// Definitions for transcoding from one text encoding to another
//
// Author: Joe Bostian
// Copyright Contributors to the Ambitus Project.
// SPDX-License-Identifier: Apache-2.0 
//
#include <iconv.h>

#include "log.h"
#include "common_types.h"

// Coded Character Set Identifiers (CCSIDs) for all of the string encodings
// we wupport.  See the z/OS Unicode Services User's Guide and Reference,
// Appendix A for a complete list of IDs.
#define CCSID_IBM-037    0x00037
#define CCSID_EBCDIC     0x00037
#define CCSID_ISO8859-1  0x00819
#define CCSID_ASCII      0x00819

#define ENC_ASCII   "ISO8859-1"
#define ENC_EBCDIC  "IBM-037"

iconv_t tc_init(CCSID, CCSID, LOGGER_T *);
void    tc_term(iconv_t, LOGGER_T *);
RC      tc_transcode(iconv_t, char *, size_t, char *, size_t, LOGGER_T *pLog);
char   *tc_e2a_alloc(iconv_t, char *, int, char *, int, LOGGER_T *);
RC      tc_e2a(char *, char *, size_t, LOGGER_T *);
RC      tc_a2e(char *, char *, size_t, LOGGER_T *);

#endif