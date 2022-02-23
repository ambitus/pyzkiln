//
// Memory dump methods for debugging
//
// Author: Joe Bostian
// Copyright Contributors to the Ambitus Project.
// SPDX-License-Identifier: Apache-2.0 
//
#include <stdio.h>

#include "dump.h"
#include "transcode.h"
#include "log.h"
#include "common_types.h"

// Local methods
void fmt_EBC_line(UINT **, int *, char *, int);
// void fmt_ASC_line(BYTE **, char *, int *);
void gen_readable_text(char *, char *, int, const char *);


void dump_mem(BYTE *pMem, int lMem, CCSID ccsid, LOGGER_T *pLog)
   {
    UINT *pWord = (UINT *)pMem;
    char  output_line[LINE_LEN];
    char  readable_text[LINE_LEN];
    int   iByte = 0;

    log_debug(pLog, "--- Memory, (%08x), %d bytes --------------------------------", pMem, lMem);

    while(iByte < lMem)
       {
        int nBytes = lMem - iByte;

        // Format a line of output, showing printable values in the requested encoding.
        switch(ccsid)
           {
            case CCSID_EBCDIC:
               gen_readable_text(&(readable_text[0]), (char *)pWord, 
                                  nBytes, &(EBC_MASK[0]));
               break;
            case CCSID_ASCII:
               // fmt_ASC_line(&finger, &(line[0]), &nBytes);
               break;
            default:
               break;
           } 

         if (nBytes >= BYTES_PER_LINE)
           {
            sprintf(output_line, "+%04x  %08x %08x %08x %08x  %s", 
                   iByte, *pWord, *(pWord+1), *(pWord+2), *(pWord+3), readable_text);
            pWord += sizeof(UINT);
            iByte += BYTES_PER_LINE;
           }

        else
           {
            char hex_text[(BYTES_PER_LINE*2)+4];
            int  iHex_text=0;
            int  iByte_byte = iByte;
            int  iWord_break = 0;

            while(iByte_byte < lMem)
               {

                // Break the hex text at word boundaries.
                if ((iWord_break > 0) && (iWord_break%sizeof(UINT) == 0))
                   {
                    hex_text[iHex_text] = ' ';
                    if (iHex_text < lMem)
                       iHex_text++;
                   }

                sprintf(&(hex_text[iHex_text]), "%02x", pMem[iByte_byte]);
                iHex_text += 2;
                iByte_byte++;
                iWord_break++;
               }

            hex_text[iHex_text] = 0x00;
            sprintf(output_line, "+%04x  %s   %s", iByte, hex_text, readable_text);
            iByte = lMem;              // done
           }

        log_debug(pLog, "%s", output_line);
       }

    log_debug(pLog, "-----------------------------------------------------------------");
    return;
   }                                   // dump_mem

void gen_readable_text(char *readable_text, char *pWord, int nBytes_left, const char *mask)
    {
     char text[LINE_LEN];
     int  iText = 0;
     int  nBytes = (nBytes_left < BYTES_PER_LINE) ? nBytes_left : BYTES_PER_LINE;
     char pad[LINE_LEN];
     int  nPad = BYTES_PER_LINE - nBytes;
     int  iWord_break = 0;

     // Pad the readable text with the appropriate amount of space, depending on the
     // number of bytes being formatted.
     pad[0] = 0x00;
     for(int iPad=0; iPad<nPad; iPad++)
        strcat(pad, "  ");

     // Add in a space for each word break in the loop below;
     nPad = (BYTES_PER_LINE/sizeof(UINT)) - (nBytes/sizeof(UINT)) - 1;
     for(int iPad=0; iPad<nPad; iPad++)
        strcat(pad, " ");

     // Format the hex input as readable text according to the encoding-specific mask
     // provided.
     for(int iByte=0; iByte<nBytes; iByte++)
        {

         // Break the printable text at word boundaries.
         if ((iWord_break > 0) && (iWord_break%sizeof(UINT) == 0))
            {
             text[iText] = ' ';
             if (iByte < nBytes-1)
                iText++;
            }

         text[iText] = mask[pWord[iByte]];
         iText++;
         iWord_break++;
        }

     text[iText] = 0x00;

     // Assemble the readable text to return from the components built here.
     sprintf(readable_text, "%s|%s|", pad, text);
     return;
    }