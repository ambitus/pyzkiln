#ifndef __COMMON_H__
#define __COMMON_H__

// Simple types
typedef unsigned char  BYTE;
typedef signed   short SSHORT;
typedef signed   int   SINT;
typedef unsigned short USHORT;
typedef unsigned int   UINT;
typedef unsigned short CCSID;

typedef int RC;
typedef int RSN;
typedef unsigned char FLAG;

// Return codes
const RC SUCCESS = 0x00000000;
const RC WARNING = 0x00000004;
const RC FAILURE = 0x00000008;

const FLAG FALSE = 0x00;
const FLAG TRUE  = 0x01;

const FLAG OFF = 0x00;
const FLAG ON  = 0x01;

#endif
