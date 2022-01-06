#ifndef __FILEIO_H__
#define __FILEIO_H__
//
// fileio.h - Definitions for file I/O methods.
//
// Author: Joe Bostian
// Copyright Contributors to the Ambitus Project.
// SPDX-License-Identifier: Apache-2.0 
//
#include "common_types.h"
#include "log.h"

// File I/O methods
char *read_file(char *, LOGGER_T *);
RC    write_file(char *, char *, LOGGER_T *);

#endif