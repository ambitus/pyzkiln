//
// fileio.c - file I/O services.  These read and write methods are used to
//            manage I/O from the input request file, and to the output
//            retults file.
//
// Author: Joe Bostian
// Copyright Contributors to the Ambitus Project.
// SPDX-License-Identifier: Apache-2.0 
//
#define _POSIX_SOURCE
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "common_types.h"
#include "log.h"
#include "fileio.h"


// Read the contents of a file to a buffer.
char *read_file(char *pFile_name, LOGGER_T *pLog)
   {
    char *pFile_str = NULL;

    if (pFile_name != NULL)
      {
       int fd = open(pFile_name, O_RDONLY);

       if (fd >= 0)
          {
           struct stat file_info;

           if ((fstat(fd, &file_info) == 0) && (file_info.st_size > 0))
              {
               ssize_t lFile = file_info.st_size;
               ssize_t lFile_str = lFile + sizeof(char);
               pFile_str = (char *)calloc(lFile_str, sizeof(char));

               if (pFile_str != NULL)
                  {
                   ssize_t lRead = read(fd, (&(pFile_str[0])), lFile);

                   if (lRead == lFile)
                       log_debug(pLog, "Read %d chars from file (%s)", 
                                lFile, pFile_name);

                   else
                      {
                       int e = errno;
                       log_error(pLog, "File read failed.");
                       log_error(pLog, "   file: %s", pFile_name);
                       log_error(pLog, "   length read: %d, file length: %d", lRead, lFile);
                       log_error(pLog, "   errno: %08d", e);
                       log_error(pLog, "   %s", strerror(e));

                       free(pFile_str);
                       pFile_str = NULL;
                      }

                  }

               else
                  log_error(pLog, "Error - can't allocate %d bytes for file text.", lFile);
              }

           else
              {
               int e = errno;
               log_error(pLog, "Can't get info for file.");
               log_error(pLog, "   file: %s", pFile_name);
               log_error(pLog, "   errno: %08d", e);
               log_error(pLog, "   %s", strerror(e));
              }

          }

       else
          {
           int e = errno;
           log_error(pLog, "Can't open file.");
           log_error(pLog, "   File %s.", pFile_name);
           log_error(pLog, "   errno: %08d", e);
           log_error(pLog, "   %s", strerror(e));
          }

       if (fd >= 0)
          close(fd);
      }

    return pFile_str;
   }                                   // read_file


// Write the contents of a buffer to a file.
RC write_file(char *pFile_name, char * pFile_str, LOGGER_T *pLog)
   {
    RC rc = SUCCESS;

    if (pFile_name != NULL)
       {
        int fd = open(pFile_name, O_RDWR);

        if (fd >= 0)
           {

            if (pFile_str != NULL)
               {
                ssize_t lStr = strlen(pFile_str);
                ssize_t lWrite = write(fd, (&(pFile_str[0])), lStr);

                if (lWrite == lStr)
                   log_debug(pLog, "Wrote %d chars to file (%s)", strlen(pFile_str), pFile_name);

                else
                   {
                    int e = errno;
                    log_error(pLog, "File write failed.");
                    log_error(pLog, "   file: %s", pFile_name);
                    log_error(pLog, "   length written: %d, string length: %d", lWrite, lStr);
                    log_error(pLog, "   errno: %08d", e);
                    log_error(pLog, "   %s", strerror(e));

                    rc = FAILURE;
                   }

               }

           }

        else
           {
            int e = errno;
            log_error(pLog, "Can't open file.");
            log_error(pLog, "   File %s.", pFile_name);
            log_error(pLog, "   errno: %08d", e);
            log_error(pLog, "   %s", strerror(e));
           }

        if (fd >= 0)
           close(fd);
       }

    return rc;
   }                                   // write_file