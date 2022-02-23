#ifndef __IRRPCOMP_H__
#define __IRRPCOMP_H__
//
// irrpcomp.h - A C version of the irrpcomp assembler interface that is documented
//              in the RACF callable serveices guide.
//
// Author: Joe Bostian
// Copyright Contributors to the Ambitus Project.
// SPDX-License-Identifier: Apache-2.0
//

#include "common_types.h"


// R_admin function groups.
//
// These groups represent collections of function codes below.  Each of these 
// groups have common authorization requirements.
//
const BYTE ADMIN_GRP_UPDATE = 0x01;     // update functions
const BYTE ADMIN_GRP_PXTR   = 0x02;     // profile extract functions
const BYTE ADMIN_GRP_PWXTR  = 0x03;     // PKCS 7 password envelope extract
const BYTE ADMIN_GRP_PPXTR  = 0x04;     // PKCS 7 passphrase envelope extract
const BYTE ADMIN_GRP_SRXTR  = 0x05;     // setropts extract
const BYTE ADMIN_GRP_SRUNL  = 0x06;     // setropts unload
const BYTE ADMIN_GRP_CMDRUN = 0x07;     // run RACF command

// R_admin function codes.  
//
// Each of these represent a specific function that a caller can request.
//
const BYTE ADMIN_FUNC_NONE         = 0x00;

const BYTE ADMIN_ADD_USER          = 0x01;  // Add a user to the RACF database
const BYTE ADMIN_DEL_USER          = 0x02;  // Delete a user from the RACF database
const BYTE ADMIN_ALT_USER          = 0x03;  // Alter a user's RACF profile
const BYTE ADMIN_LST_USER          = 0x04;  // List the contents of a users RACF profile

const BYTE ADMIN_RUN_CMD           = 0x05;  // Run a RACF command image

const BYTE ADMIN_ADD_GROUP         = 0x06;  // Add a group to the RACF database
const BYTE ADMIN_DEL_GROUP         = 0x07;  // Delete a group from the RACF database
const BYTE ADMIN_ALT_GROUP         = 0x08;  // Alter a group's RACF profile
const BYTE ADMIN_LST_GROUP         = 0x09;  // List the contents of a group's RACF profile

const BYTE ADMIN_CONNECT           = 0x0A;  // Connect a single user to a RACF group
const BYTE ADMIN_REMOVE            = 0x0B;  // Remove a single user from a RACF group

const BYTE ADMIN_ADD_GENRES        = 0x0C;  // Add a general resource profile to the RACF database
const BYTE ADMIN_DEL_GENRES        = 0x0D;  // Delete a general resource profile from the RACF database
const BYTE ADMIN_ALT_GENRES        = 0x0E;  // Alter a general resource's RACF profile
const BYTE ADMIN_LST_GENRES        = 0x0F;  // List the contents of a general resource's RACF profile

const BYTE ADMIN_ADD_DS            = 0x10;  // Add a data set profile to the RACF database
const BYTE ADMIN_DEL_DS            = 0x11;  // Delete a data set profile from the RACF database
const BYTE ADMIN_ALT_DS            = 0x12;  // Alter a data set's RACF profile
const BYTE ADMIN_LST_DS            = 0x13;  // List the contents of a data set's RACF profile

const BYTE ADMIN_PERMIT            = 0x14;  // Permit a user or group to a RACF profile

const BYTE ADMIN_ALT_SETR          = 0x15;  // Alter SETROPTS information
const BYTE ADMIN_XTR_SETR          = 0x16;  // Extract SETROPTS information in R_admin format
const BYTE ADMIN_UNL_SETR          = 0x17;  // Extract SETROPTS information in SMF unload format

const BYTE ADMIN_XTR_PWENV         = 0x18;  // Extract PKCS #7 encrypted password envelope
const BYTE ADMIN_XTR_USER          = 0x19;  // Extract a user profile
const BYTE ADMIN_XTR_NEXT_USER     = 0x1A;  // Extract the next user profile
const BYTE ADMIN_XTR_GROUP         = 0x1B;  // Extract a group profile
const BYTE ADMIN_XTR_NEXT_GROUP    = 0x1C;  // Extract the next group profile
const BYTE ADMIN_XTR_CONNECT       = 0x1D;  // Extract connection information for a user and group
const BYTE ADMIN_XTR_PPENV         = 0x1E;  // Extract PKCS #7 encrypted password phrase envelope
const BYTE ADMIN_XTR_RESOURCE      = 0x1F;  // Extract a general resource profile
const BYTE ADMIN_XTR_NEXT_RESOURCE = 0x20;  // extract the next general resource profile


// -----------------------------------------------------------------------------
// User administration functions
// -----------------------------------------------------------------------------
// User administration and password envelope retrieval parameter list.  This parmlist 
// is used for these functions:
//    - ADMIN_ADD_USER
//    - ADMIN_DEL_USER
//    - ADMIN_ALT_USER
//    - ADMIN_LST_USER
//    - ADMIN_XTR_PPENV
//    - ADMIN_XTR_PWENV
// This struct is exactly 14 bytes long.
typedef struct R_ADMIN_UADM_PARMS {
   BYTE   l_userid;              // length of the userid
   char   userid[8];             // upper case userid
   BYTE   rsv_1;                 // reserved
   USHORT off_seg_1;             // offset to first segment
   USHORT n_segs;                // number of segments
                                 // start of first segment
   } R_ADMIN_UADM_PARMS_T;


// -----------------------------------------------------------------------------
// Profile extract functions
// -----------------------------------------------------------------------------
// Profile extract parameter list.  This parmlist is used for these functions:
//    - ADMIN_XTR_USER 
//    - ADMIN_XTR_NEXT_USER 
//    - ADMIN_XTR_GROUP 
//    - ADMIN_XTR_NEXT_GROUP 
//    - ADMIN_XTR_CONNECT 
//    - ADMIN_XTR_RESOURCE 
//    - ADMIN_XTR_NEXT_RESOURCE 
// This struct is exactly 60 bytes long.
typedef struct R_ADMIN_PXTR_PARMS {
   char  eyecatcher[4];          // 'PXTR'
   int   lOutbuf;                // output buffer length
   BYTE  subpool;                // subpool of output buffer
   BYTE  version;                // parameter list version
   BYTE  rsv_1[2];               // reserved
   char  class_name[8];          // class name - upper case, blank pad
   int   lProf_name;             // length of profile name
   BYTE  rsv_2[8];               // reserved
   BYTE  rsv_3[4];               // reserved
   UINT  flags;                  // see flag constants below
   int   nSegments;              // number of segments
   BYTE  rsv_4[16];              // reserved
                                 // start of profile name 
   } R_ADMIN_PXTR_PARMS_T;

// Parameter list flags.  See RACF callable services for details.
const UINT f_bypass_cmd_proc     = 0x80000000;  // bypass command processor (IN)
const UINT f_ext_base_seg_only   = 0x40000000;  // extract base segment only (IN)
const UINT f_enf_facility_chk    = 0x20000000;  // enforce facility class check (IN)
const UINT f_generic_res_req     = 0x10000000;  // generic resoure request (IN/OUT)
const UINT f_upcase_input_name   = 0x08000000;  // fold input name to upper case (IN)
const UINT f_ret_only_prof_name  = 0x04000000;  // return only the profile name (IN)

// Segment descriptor
typedef struct R_ADMIN_SDESC {
   char name[8];                 // segment name, upper case, blank padded
   UINT flags;                   // 
   int  nFields;                 // number of fields
   BYTE rsv_1[4];                // reserved
   int  off_fdesc_1;             // offset to first field descriptor
   BYTE rsv_2[16];               // reserved
                                 // start of next segment descriptor
   } R_ADMIN_SDESC_T;

// Field descriptor
// The field data and repeat group values are stored in the same locations.
// Define both names for the values stored there.
typedef union FDATA_LEN_RPT {
   int l_fld_data;               // length of field data or ...
   int n_repeat_grps;            // number of repeat groups
   } FDATA_LEN_RPT_T;

typedef union FDATA_OFF_RPT {
   int off_fld_data;             // offset to field data or ...
   int n_repeat_elems;           // number of elems in repeat field hdrs
   } FDATA_OFF_RPT_T;

typedef struct R_ADMIN_FDESC {
   char             name[8];     // field name, upper case, blank padded
   USHORT           type;        //
   BYTE             rsv_1[2];    // reserved
   UINT             flags;       //
   FDATA_LEN_RPT_T  len_rpt;     //
   BYTE             rsv_2[4];    // reserved
   FDATA_OFF_RPT_T  off_rpt;     // 
   BYTE             rsv_3[16];   // reserved
                                 // start of next field descriptor
   } R_ADMIN_FDESC_T;

// Field types
const USHORT t_mbr_repeat_group  = 0x8000;  // member of a repeat group
const USHORT t_reserved          = 0x4000;  // reserved
const USHORT t_boolean_field     = 0x2000;  // flag (boolean) field
const USHORT t_repeat_field_hdr  = 0x1000;  // repeat field header

// Field descriptor flags
const UINT f_boolean_field  = 0x80000000;   // value of a boolean field
const UINT f_output_only    = 0x40000000;   // output-only field
   

// -----------------------------------------------------------------------------
// Other definitions
// -----------------------------------------------------------------------------
// Profile name.  Profile names can represent:
//   - users or groups (8 characters)
//   - user.group connections (17 characters)
//   - general resources (up to 246 characters)
// Make this buffer large enough to include a null terminator.
#define MAX_PROF_NAME_LEN  247

typedef struct PROF_NAME {
   char name[MAX_PROF_NAME_LEN];
   char term;                    // string terminator
   } PROF_NAME_T;


// C linkage to the R_admin interface module IRRSEQ00
#pragma linkage(IRRSEQ00, OS)
int IRRSEQ00(char *, int *, int *, int *, int *, int *, int *, char *, void *, void *, void *, char *, char **, ...);

#endif
