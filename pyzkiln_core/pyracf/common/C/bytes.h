#ifndef __STRING_H__
#define __STRING_H__
//
// Definitions for byte array string services
//
// Author: Joe Bostian
// Copyright Contributors to the Ambitus Project.
// SPDX-License-Identifier: Apache-2.0 
//
#include "common_types.h"

#define NO_TRIM  0x00
#define TRIM     0x01

#define NO_FOLD  0x00
#define FOLD     0x01

void bytes_to_str(char *, BYTE *, int, FLAG, FLAG);

#endif