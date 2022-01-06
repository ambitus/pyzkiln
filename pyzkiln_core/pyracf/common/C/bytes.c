//
// Byte array services
//
// String service analogs that work with byte array (pointer/length) 
// defined text.  In some cases, these will directly parallel standard
// library functions that normally work with NULL-terminated strings.
//
// Author: Joe Bostian
// Copyright Contributors to the Ambitus Project.
// SPDX-License-Identifier: Apache-2.0
//
#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>

#include "string.h"
#include "common_types.h"

// RACF returns text in byte arrays that are not delilmited by null 
// characters.  It returns buffers that have a fixed length, and strings
// shorter than this length will be padded with blanks.  Strip these
// blanks from the end and null-terminate the byte array to make it
// string-friendly.
void bytes_to_str(char *pStr, BYTE *pBytes, int nBytes, FLAG fTrim, FLAG fLower)
   {
    int i = nBytes-1;
    int j;

    // If requested, trim trailing whitespace.
    if (fTrim)
       {
        while((i >= 0) && (isspace(pBytes[i])>0))
           i--;
       }

    // Copy the bytes over to the string.
    j = i;
    if (fLower)
       {
        while(i >= 0)
           pStr[j--] = tolower(pBytes[i--]);
       }

    else
       {
        while(i >= 0)
           pStr[j--] = pBytes[i--];
       }

    return;
   }                                   // bytes_to_str