#ifndef __LOGGER_H__
#define __LOGGER_H__
//
// logger.h - Definitions for building the C logger.
//
// Author: Joe Bostian
// Copyright Contributors to the Ambitus Project.
// SPDX-License-Identifier: Apache-2.0 
//
#include "common_types.h"

#define L_FMT_BUF  2048
#define L_LOG_NAME 32

#define LOG_NONE      0x00
#define LOG_DEBUG     0x01
#define LOG_INFO      0x02
#define LOG_WARNING   0x03
#define LOG_ERROR     0x04
#define LOG_CRITICAL  0x05
#define N_LOG_TYPES  6
#define L_LOG_TYPE   16

typedef struct LOGGER {
    FLAG fDebug;
    char log_name[L_LOG_NAME];
    char *log_types[N_LOG_TYPES]; 
   } LOGGER_T;

// Getters and setters.
void log_set_debug(LOGGER_T *, FLAG);
void log_set_name(LOGGER_T *, char *);

// Logger methods
LOGGER_T *logger_init(FLAG, char *);
void      logger_term(LOGGER_T *);
void      log_log(LOGGER_T *, BYTE, char *);
void      log_debug(LOGGER_T *, const char *, ...);
void      log_info(LOGGER_T *, const char *, ...);
void      log_warning(LOGGER_T *, const char *, ...);
void      log_error(LOGGER_T *, const char *, ...);
void      log_critical(LOGGER_T *, const char *, ...);

#endif