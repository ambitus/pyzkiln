//
// json_prs.c - json parser.  This is a simple json parser built from the spec at 
//           https://www.json.org/json-en.html.  All input is provided in 1
//           contiguous buffer, and the output is a list of key-value pairs 
//           in memory that gets passed to the Racf interface code.  See keyval.c
//           to see how this parser interacts with the key-value list manager.
//
// Author: Joe Bostian
// Copyright Contributors to the Ambitus Project.
// SPDX-License-Identifier: Apache-2.0 
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdarg.h>
#include <errno.h>

#include "json.h"
#include "keyval.h"
#include "fileio.h"
#include "transcode.h"
#include "log.h"

#define ENTER  0x00
#define EXIT   0x01


// Types of data as identified in the value state of the json parser.  This is
// important with creating the in-memory key-value list that represents the input
// Json string.
#define TVAL_NONE   0x00
#define TVAL_OBJECT 0x01
#define TVAL_ARRAY  0x02
#define TVAL_STRING 0x03
#define TVAL_NUMBER 0x04
#define TVAL_CONST  0x05


// Json parsing state machine methods
int value(JSKV_CTL_T *);
int object(JSKV_CTL_T *);
int members(JSKV_CTL_T *);
int member(JSKV_CTL_T *);
int array(JSKV_CTL_T *);
int elements(JSKV_CTL_T *);
int element(JSKV_CTL_T *);
int string(JSKV_CTL_T *);
int characters(JSKV_CTL_T *);
int character(JSKV_CTL_T *);
int escape(JSKV_CTL_T *);
int hex(JSKV_CTL_T *);
int number(JSKV_CTL_T *);
int integer(JSKV_CTL_T *);
int digits(JSKV_CTL_T *);
int digit(JSKV_CTL_T *);
int onenine(JSKV_CTL_T *);
int fraction(JSKV_CTL_T *);
int exponent(JSKV_CTL_T *);
int sign(JSKV_CTL_T *);
int ws(JSKV_CTL_T *);

// Init and term
JSKV_CTL_T *jsonp_init(char *, LOGGER_T *);
void        jsonp_term(JSKV_CTL_T *);

// Utilities
void end_of_string(JSKV_CTL_T *);
char *read_parms_file(JSKV_CTL_T *, char *);
void trace(JSKV_CTL_T *, char *, int);
void trace_pinky(JSKV_CTL_T *pJKCtl);


//
// Mainline code
//
KV_CTL_T *json_to_kv(char *pJson_str, LOGGER_T *pLog)
   {
    JSKV_CTL_T *pJKCtl = jsonp_init(pJson_str, pLog);
    KV_CTL_T   *pKVCtl = NULL;

    if ((pJKCtl != NULL) && (pJson_str != NULL))
       {
        log_set_name(pJKCtl->pLog, "json_prs");
        log_debug(pLog, "%s", pJson_str);
            
        // Build a key-value list from the json string.  This is an implementation 
        // of the json grammar that can be found at https://www.json.org/json-en.html.
        if (element(pJKCtl) == SUCCESS)
           pKVCtl = pJKCtl->pKVCtl;
       }

    jsonp_term(pJKCtl);
    return pKVCtl;
   }                                   // json_to_kv

//
// State machine methods for parsing the input json document.
//
int value(JSKV_CTL_T *pJKCtl)
   {
    int rc = SUCCESS;

    trace(pJKCtl, "value", ENTER);

    switch(CH(index))
      {
       case '{':
         rc = object(pJKCtl);
         break;
      case '[':
         rc = array(pJKCtl);
         break;
      case '"':
         rc = string(pJKCtl);
         rc = kv_add_value(pJKCtl->pKVCtl, PCH(pinky), ICH(pinky)+1, CCSID_ASCII, VAL_TYPE_TXT);
         break;
      case '-':
      case '0':
      case '1':
      case '2':
      case '3':
      case '4':
      case '5':
      case '6':
      case '7':
      case '8':
         rc = number(pJKCtl);
         rc = kv_add_value(pJKCtl->pKVCtl, PCH(pinky), ICH(pinky)+1, CCSID_ASCII, VAL_TYPE_NUM);
         break;
      case 't':
         if ((ICH(index) >= 4) && (!strncmp(PCH(index), "true", 4)))
            {
             kv_add_value(pJKCtl->pKVCtl, "true", 4, CCSID_ASCII, VAL_TYPE_TXT);
             MOVE_INDEX_FINGER(4);
            }
         break;
      case 'f':
         if ((ICH(index) >= 5) && (!strncmp(PCH(index), "false", 5)))
            {
             kv_add_value(pJKCtl->pKVCtl, "false", 5, CCSID_ASCII, VAL_TYPE_TXT);
             MOVE_INDEX_FINGER(5);
            }
         break;
      case 'n':
         if ((ICH(index) >= 4) && (!strncmp(PCH(index), "null", 4)))
            {
             kv_add_value(pJKCtl->pKVCtl, "null", 4, CCSID_ASCII, VAL_TYPE_TXT);
             MOVE_INDEX_FINGER(4);
            }
         break;
      default:
         fprintf(stderr, "Error - json parse (value), unrecognized input.\n");
         fprintf(stderr, "        text: %s,  offset: %d\n", PCH(index), ICH(index));
         rc = FAILURE;
         break;
     }

    trace(pJKCtl, "value", EXIT);
    return rc;
   }                                   // value

int object(JSKV_CTL_T *pJKCtl)
   {
    int rc = SUCCESS;

    trace(pJKCtl, "object", ENTER);

    if (CH(index) == '{')
       {
        kv_nest(pJKCtl->pKVCtl);
        MOVE_INDEX_FINGER(1);
        rc = ws(pJKCtl);

        // Empty object
        if (CH(index) == '}')
           {
            kv_unnest(pJKCtl->pKVCtl);
            MOVE_INDEX_FINGER(1);
           }

        else
           {
            rc = members(pJKCtl);
            if (CH(index) == '}')
               {
                kv_unnest(pJKCtl->pKVCtl);
                MOVE_INDEX_FINGER(1);
               }

           }

       }

    else
       {
        fprintf(stderr, "Error - json parse (object), unrecognized input.\n");
        fprintf(stderr, "        text: %s,  offset: %d\n", PCH(index), ICH(index));
        rc = FAILURE;
       }
   
    trace(pJKCtl, "object", EXIT);
    return rc;
   }                                   // object

int members(JSKV_CTL_T *pJKCtl)
   {
    int rc;

    trace(pJKCtl, "members", ENTER);
    rc = member(pJKCtl);

    // Avoid recursion here because member lists could get pretty long.
    while((rc == SUCCESS) && (CH(index) == ','))
       {
        MOVE_INDEX_FINGER(1);
        rc = member(pJKCtl);
       }

    trace(pJKCtl, "members", EXIT);
    return rc;
   }                                   // members

int member(JSKV_CTL_T *pJKCtl)
   {
    int rc;

    trace(pJKCtl, "member", ENTER);

    rc = ws(pJKCtl);
    if (rc != SUCCESS)
       return rc;

    rc = string(pJKCtl);
    if (rc != SUCCESS)
       return rc;

    rc = ws(pJKCtl);
    if (rc != SUCCESS)
       return rc;

    if (CH(index) == ':')
       {
        MOVE_INDEX_FINGER(1);
        rc = kv_add(pJKCtl->pKVCtl, PCH(pinky), ICH(pinky)+1, CCSID_ASCII, VAL_DIM_NONE);
        rc = element(pJKCtl);
       }

    else
       {
        fprintf(stderr, "Error - json parse (member), expecting \":\".\n");
        fprintf(stderr, "        text: %s,  offset: %d\n", PCH(index), ICH(index));
        rc = FAILURE;
       }

    trace(pJKCtl, "member", EXIT);
    return rc;
   }                                   // member

int array(JSKV_CTL_T *pJKCtl)
   {
    int rc = SUCCESS;

    trace(pJKCtl, "array", ENTER);

    if (CH(index) == '[')
      {
       MOVE_INDEX_FINGER(1);
       // if (isspace(CH(index)))
       //   rc = ws(pJKCtl);
       // else
          rc = elements(pJKCtl);

       if (CH(index) == ']')
          {
           MOVE_INDEX_FINGER(1);
          }

       else
          {
           fprintf(stderr, "Error - json parse (array), expecting \"]\".\n");
           fprintf(stderr, "        text: %s,  offset: %d\n", PCH(index), ICH(index));
           rc = FAILURE;
          }

      }

    else
      {
       fprintf(stderr, "Error - json parse (array), expecting \"[\".\n");
       fprintf(stderr, "        text: %s,  offset: %d\n", PCH(index), ICH(index));
       rc = FAILURE;
      }

    trace(pJKCtl, "members", EXIT);
    return rc;
   }                                   // array

int elements(JSKV_CTL_T *pJKCtl)
   {
    int rc;

    trace(pJKCtl, "elements", ENTER);
    rc = element(pJKCtl);

    // Avoid recursion here because element lists could get pretty long.
    while((rc == SUCCESS) && (CH(index) == ','))
       {
        MOVE_INDEX_FINGER(1);
        rc = element(pJKCtl);
       }

    trace(pJKCtl, "elements", EXIT);
    return rc;
   }                                   // elements

int element(JSKV_CTL_T *pJKCtl)
   {
    int rc;

    trace(pJKCtl, "element", ENTER);
    rc = ws(pJKCtl);
    if (rc != SUCCESS)
       return rc;

    rc = value(pJKCtl);
    if (rc != SUCCESS)
       return rc;

    rc = ws(pJKCtl);
    // kv_print(pJKCtl->pKVCtl);
    trace(pJKCtl, "element", EXIT);
    return rc;
   }                                   // element

int string(JSKV_CTL_T *pJKCtl)
   {
    int rc = SUCCESS;

    trace(pJKCtl, "string", ENTER);

    if (CH(index) == '"')
       {
        MOVE_INDEX_FINGER(1);
        POINT_FINGER(pinky, PCH(index)+ICH(index), ICH(index));
        trace_pinky(pJKCtl);
        rc = characters(pJKCtl);

        if (rc == SUCCESS)
           {

            if (CH(index) == '"')
               {
                POINT_FINGER(pinky, PCH(pinky), (ICH(index))-(ICH(pinky))-1);
                MOVE_INDEX_FINGER(1);
                trace_pinky(pJKCtl);
               }

            else
              {
               fprintf(stderr, "Error - json parse (string), expecting \".\n");
               fprintf(stderr, "        text: %s,  offset: %d\n", PCH(index), ICH(index));
               rc = FAILURE;
              }

           }

       }

    else
      {
       fprintf(stderr, "Error - json parse (string), expecting \".\n");
       fprintf(stderr, "        text: %s,  offset: %d\n", PCH(index), ICH(index));
       rc = FAILURE;
      }
   
    trace(pJKCtl, "string", EXIT);
    return rc;
   }                                   // string

int characters(JSKV_CTL_T *pJKCtl)
   {
    int rc = SUCCESS;

    trace(pJKCtl, "characters", ENTER);

    // Null character string.
    if (CH(index) == '"')
       return rc;

    while ((rc == SUCCESS) && (CH(index) != '"'))
       rc = character(pJKCtl);

    trace(pJKCtl, "characters", EXIT);
    return rc;
   }                                   // characters

int character(JSKV_CTL_T *pJKCtl)
   {
    int rc = SUCCESS;

    trace(pJKCtl, "character", ENTER);

    if (CH(index) == '\\')
       {
        MOVE_INDEX_FINGER(1);
        rc = escape(pJKCtl);
       }

    else
       {
        if ((CH(index) != '"') &&
            (((int)(CH(index)) >=0x0020) && ((int)(CH(index)) <= 0x10FFFF)))
           {
            MOVE_INDEX_FINGER(1);
           }

        else
          {
           fprintf(stderr, "Error - json parse (character), unrecognized character.\n");
           fprintf(stderr, "        text: %08x,  offset: %d\n", PCH(index), ICH(index));
           rc = FAILURE;
          }

       }

    trace(pJKCtl, "character", EXIT);
    return rc;
   }                                   // character

int escape(JSKV_CTL_T *pJKCtl)
   {
    int rc = SUCCESS;

    trace(pJKCtl, "escape", ENTER);

    switch(CH(index))
      {
       case '"':
       case '\\':
       case '/':
       case 'b':
       case 'f':
       case 'n':
       case 'r':
       case 't':
          MOVE_INDEX_FINGER(1);
          break;
       case 'u':
          MOVE_INDEX_FINGER(1);  

          rc = hex(pJKCtl);
          if (rc == SUCCESS)
             rc = hex(pJKCtl);
             if (rc == SUCCESS)
                rc = hex(pJKCtl);
                if (rc == SUCCESS)
                   rc = hex(pJKCtl);
          break;
       default:
          fprintf(stderr, "Error - json parse (escape), unrecognized input.\n");
          fprintf(stderr, "        text: %08x,  offset: %d\n", PCH(index), ICH(index));
          rc = FAILURE;
          break;
      }

    trace(pJKCtl, "escape", EXIT);
    return rc;
   }                                   // escape

int hex(JSKV_CTL_T *pJKCtl)
   {
    int rc = SUCCESS;

    trace(pJKCtl, "hex", ENTER);
    return rc;

    if (isxdigit(CH(index)))
       {
        MOVE_INDEX_FINGER(1);
       }

    else
       {
        fprintf(stderr, "Error - json parse (hex), unrecognized input.\n");
        fprintf(stderr, "        text: %s,  offset: %d\n", PCH(index), ICH(index));
        rc = FAILURE;
       }

    trace(pJKCtl, "hex", EXIT);
    return rc;
   }                                   // hex 

int number(JSKV_CTL_T *pJKCtl)
   {
    int rc;

    trace(pJKCtl, "number", ENTER);

    rc = integer(pJKCtl);
    if (rc == SUCCESS)
       rc = fraction(pJKCtl);
       if (rc == SUCCESS)
          rc = exponent(pJKCtl);

    trace(pJKCtl, "number", EXIT);
    return rc;
   }                                   // number

int integer(JSKV_CTL_T *pJKCtl)
   {
    int rc = SUCCESS;

    trace(pJKCtl, "integer", ENTER);

    POINT_FINGER(pinky, PCH(index)+ICH(index), ICH(index));

    if (CH(index) == '-')
       {
        MOVE_INDEX_FINGER(1);
       }

    if (isdigit(CH(index)))
       {
         // Distinguish between a single digit and strings of digits.  If the
         // first digit is a zero, there must only be 1 digit.
         if (CH(index) == '0')
            rc = digit(pJKCtl);

         else
            {
             rc = onenine(pJKCtl);
             if ((rc == SUCCESS) && (isdigit(CH(index))))
                rc = digits(pJKCtl);
            }

       }

    else
       {
        fprintf(stderr, "Error - json parse (integer), expected a digit.\n");
        fprintf(stderr, "        text: %s,  offset: %d\n", PCH(index), ICH(index));
        rc = FAILURE;
       }

    POINT_FINGER(pinky, PCH(pinky), (ICH(index))-(ICH(pinky))-1);
    trace(pJKCtl, "integer", EXIT);
    return rc;
   }                                   // integer

int digits(JSKV_CTL_T *pJKCtl)
   {
    int rc;

    trace(pJKCtl, "digits", ENTER);

    rc = digit(pJKCtl);
    while(isdigit(CH(index)))
       rc = digit(pJKCtl);

    trace(pJKCtl, "digits", EXIT);
    return rc;
   }                                   // digits

int digit(JSKV_CTL_T *pJKCtl)
   {
    int rc = SUCCESS;

    trace(pJKCtl, "digit", ENTER);

    if (isdigit(CH(index)))
       {
        MOVE_INDEX_FINGER(1);
       }

    else
       {
        fprintf(stderr, "Error - json parse (digit), expected a digit.\n");
        fprintf(stderr, "        line: %d,  offset: %d\n", pJKCtl->nLines, ICH(index)-pJKCtl->iChar);
        rc = FAILURE;
       }

    trace(pJKCtl, "digit", EXIT);
    return rc;
   }                                   // digit

int onenine(JSKV_CTL_T *pJKCtl)
   {
    int rc = SUCCESS;

    trace(pJKCtl, "onenine", ENTER);

    switch(CH(index))
      {
       case '1':
       case '2':
       case '3':
       case '4':
       case '5':
       case '6':
       case '7':
       case '8':
       case '9':
          MOVE_INDEX_FINGER(1);
          break;
       default:
          fprintf(stderr, "Error - json parse (onenine), expected a digit (1-9).\n");
          fprintf(stderr, "        text: %s,  offset: %d\n", PCH(index), ICH(index));
          rc = FAILURE;
          break;
      }

    trace(pJKCtl, "onenine", EXIT);
    return rc;
   }                                   // onenine

int fraction(JSKV_CTL_T *pJKCtl)
   {
    trace(pJKCtl, "fraction", ENTER);

    if (CH(index) == '.')
      {
       MOVE_INDEX_FINGER(1);
      }

    trace(pJKCtl, "fraction", EXIT);
    return SUCCESS;  
   }                                   // fraction

int exponent(JSKV_CTL_T *pJKCtl)
   {
    trace(pJKCtl, "exponent", ENTER);

    if ((CH(index) == 'e') || (CH(index) == 'E'))
      {
       MOVE_INDEX_FINGER(1);
      }

    trace(pJKCtl, "exponent", EXIT);
    return SUCCESS;   
   }                                   // exponent

int sign(JSKV_CTL_T *pJKCtl)
   {
    trace(pJKCtl, "sign", ENTER);

    if ((CH(index) == '+') || (CH(index) == '-'))
      {
       MOVE_INDEX_FINGER(1);
      }

    trace(pJKCtl, "sign", EXIT);
    return SUCCESS;  
   }                                   // sign

int ws(JSKV_CTL_T *pJKCtl)
   {
    trace(pJKCtl, "ws", ENTER);

    while(isspace(CH(index)))
       {
        // If this is a newline, update our PD info to bump the line count.
        if (CH(index) == ASCII_LF)
           {
            pJKCtl->nLines++;
            pJKCtl->iChar = ICH(index);
           }

        MOVE_INDEX_FINGER(1);
       }

    trace(pJKCtl, "ws", EXIT);
    return SUCCESS;
   }                                   // ws

void end_of_string(JSKV_CTL_T *pJKCtl)
   {
    log_error(pJKCtl->pLog, "Error - scanned past end of json string.");
    exit(FAILURE);
   }                                   // end_of_string


//
// Init and term
//
JSKV_CTL_T *jsonp_init(char *pJson_str, LOGGER_T *pLog)
   {
    JSKV_CTL_T *pJKCtl = (JSKV_CTL_T *)calloc(1,sizeof(JSKV_CTL_T));

    if (pJKCtl != NULL)
       {
        pJKCtl->pJson_str = pJson_str;
        pJKCtl->lJson_str = strlen(pJson_str);
        pJKCtl->nBytes_left  = pJKCtl->lJson_str;
        pJKCtl->index.p = pJKCtl->pJson_str;
        pJKCtl->index.i = 0;
        pJKCtl->nLines = 1;
        pJKCtl->pLog = pLog;

        // Allocate and initialize a key-value area that represents the parsed
        // json input.  Base the initial size of this area on the input string in
        // the same way that we have the json parsing control area.
        pJKCtl->pKVCtl = kv_init(pJKCtl->pLog);

        if (pJKCtl->pKVCtl == NULL)
           {
            free(pJKCtl);
            pJKCtl = NULL;
           }

       }

    else
       log_error(pLog, "Can't allocate space for Json parse/gen control");
    return pJKCtl;
   }                                   // jsonp_init

void jsonp_term(JSKV_CTL_T *pJKCtl)
   {
  
    if (pJKCtl->pJson_str != NULL)
       {
        free(pJKCtl->pJson_str);
        pJKCtl->pJson_str = NULL;
       }

    // Do not free up the key-value list associated with this json.  That
    // gets returned to the caller.
    free(pJKCtl);
    return;
   }                                   // jsonp_term



//
// Trace methods for Debugging
//
void trace(JSKV_CTL_T *pJKCtl, char *state, int enter_exit)
   {
    // Enable this trace to see the operation of the parsing state machine.
    return;

    if (CH(index) == ASCII_TAB)
       {
        if (enter_exit == ENTER)
           log_debug(pJKCtl->pLog, ">>> [%12s]: CH: TAB, ICH %d", state, ICH(index));
        else
           log_debug(pJKCtl->pLog, "<<< [%12s]: CH: TAB, ICH %d", state, ICH(index));
       }

    else
    if (CH(index) == ASCII_LF)
       {
        if (enter_exit == ENTER)
           log_debug(pJKCtl->pLog, ">>> [%12s]: CH: NL,  ICH %d", state, ICH(index));
        else
           log_debug(pJKCtl->pLog, "<<< [%12s]: CH: NL,  ICH %d", state, ICH(index));
       }

    else
       {
        if (enter_exit == ENTER)
           log_debug(pJKCtl->pLog, ">>> [%12s]: CH: %c,   ICH %d", state, CH(index), ICH(index));
        else
           log_debug(pJKCtl->pLog, "<<< [%12s]: CH: %c,   ICH %d", state, CH(index), ICH(index));
       }

    return;
   }                                   // trace

void trace_pinky(JSKV_CTL_T *pJKCtl)
   {
    // Enable this trace to see how the finger moves through the input text.
    return;

    // The pinky finger is initialized to the beginning of a string or number, and then
    // updated when the end of that string or number is reached.  Include the parsed 
    // text representing the string or number in the trace output if the end of the text
    // is known.  When the index (i) of the pinky finger is less than the (i) of the 
    // index finger, then we know where the end of the text is.
    if (ICH(pinky) == ICH(index))
       log_debug(pJKCtl->pLog, "    pinky, p: %08x, i: %d", pJKCtl->pinky.p, pJKCtl->pinky.i);
            
    else    
       {
        char str[256];
        int l = (pJKCtl->pinky.i < sizeof(str)-1) ? (pJKCtl->pinky.i)+1 : sizeof(str)-1;
        
        strncpy(str, pJKCtl->pinky.p, l);
        str[l] = 0x00;
        log_debug(pJKCtl->pLog, "    (%s) p: %08x, i: %d", str, pJKCtl->pinky.p, pJKCtl->pinky.i);
       }

    return;
   }                                   // trace_pinky
