#ifndef __JSON_H__
#define __JSON_H__
//
// json.h - Definitions for parsing and generating json strings to/from a 
//          key-value pair list.
//
#include "keyval.h"
#include "log.h"
#include "common_types.h"

// Maximum indentation for generated Json.
#define MAX_INDENT  96

// Character constants
#define ASCII_TAB  0x09
#define ASCII_LF   0x0A

#define JSON_STR_REALLOC_RATIO  0.25

// Macros for navigating the json string.  Inline where possible and avoid
// call overhead.
// pJKCtl->nBytes_left--;
#define MOVE_INDEX_FINGER(nBytes) \
    pJKCtl->index.i += nBytes; \
    pJKCtl->nBytes_left -= nBytes; \
    if (pJKCtl->nBytes_left < 0) \
       end_of_string(pJKCtl);

#define MOVE_FINGER(finger, nBytes) \
    pJKCtl->finger.i += nBytes;

#define POINT_FINGER(finger, at_p, at_i) \
    pJKCtl->finger.p = at_p; \
    pJKCtl->finger.i = at_i;

#define CH(finger)   (pJKCtl->finger.p[pJKCtl->finger.i])
#define PCH(finger)  (pJKCtl->finger.p)
#define ICH(finger)  (pJKCtl->finger.i)

typedef struct FINGER {
    char *p;
    int  i;
    BYTE tVal;
   } FINGER_T;

#define IS_ROOM(lStr) \
    ((pJKCtl->nBytes_left > lStr) ? TRUE : FALSE)

// The Parse/Gen Control block.  This contains the state of the parse, and 
// pointers to both the json string, and the parameter list parsed from it.
typedef struct JSKV_CTL {
    int   lCtl;
    char *pJson_str;
    int   lJson_str;
    int   nBytes_left;
    int   iNest;
    char  indent_str[MAX_INDENT+1];
    FINGER_T index;
    FINGER_T pinky;
    KV_CTL_T *pKVCtl;
    LOGGER_T *pLog;

    // Values used for problem determination
    int nLines;
    int iChar;
   } JSKV_CTL_T; 

// Main entry points to the Json parser and generator.
KV_CTL_T *json_to_kv(char *, LOGGER_T *);
char     *json_from_kv(KV_CTL_T *, LOGGER_T *);

#endif