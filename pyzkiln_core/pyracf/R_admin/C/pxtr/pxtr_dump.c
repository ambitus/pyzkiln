//
// Profile extract output methods
//
// These are primarily debug routines for dumping out both the
// args and parms for calling RACF, and the output RACF returns.
//
// Author: Joe Bostian
// Copyright Contributors to the Ambitus Project.
// SPDX-License-Identifier: Apache-2.0 
//
#include <stdio.h>
#include <string.h>

#include "r_admin.h"
#include "pxtr.h"
#include "irrpcomp.h"

// Local prototypes
void pxtr_print_segments(R_ADMIN_SDESC_T *, int, BYTE *, LOGGER_T *);
void pxtr_print_fields(R_ADMIN_FDESC_T *, int, BYTE *, LOGGER_T *);
void pxtr_dump_segments(R_ADMIN_SDESC_T *, int, LOGGER_T *);
void pxtr_dump_fields(R_ADMIN_FDESC_T *, int, LOGGER_T *);
void pxtr_dump_args_parms(PXTR_CTL_T *, LOGGER_T *);


//
// Formatted print of profile extract function control blocks.
// This includes the returned parameters control block and all 
// associated segments and fields.  Note that this outputs a
// human-readable for of the output.  See the dump routines below
// for raw output of the ouptut from R_admin.
//
void pxtr_print(R_ADMIN_PXTR_PARMS_T *pParms, LOGGER_T *pLog)
   {                                   // pxtr_print_output
    char eyecatcher[5];                // vars for null-terminating strings
    char class_name[9];
    PROF_NAME_T prof_name;
    BYTE *finger;                      // current memory location

    memset(eyecatcher, 0, sizeof(eyecatcher));
    strncpy(eyecatcher, pParms->eyecatcher, sizeof(pParms->eyecatcher));
    memset(class_name, 0, sizeof(class_name));
    strncpy(class_name, pParms->class_name, sizeof(pParms->class_name));

    printf("Profile extract parms (%08x)\n", pParms);
    printf("   eyecatcher: %s\n",eyecatcher);
    printf("   class name: %s\n",class_name);
    printf("   outbuf length: %d\n",pParms->lOutbuf);
    printf("   subpool: %d\n",pParms->subpool);
    printf("   version: %d\n",pParms->version);
    printf("   class name: %s\n",class_name);
    printf("   profile name length: %d\n",pParms->lProf_name);
    printf("   flags: %08x\n",pParms->flags);
    printf("   num of segments: %0d\n",pParms->nSegments);

    finger = (BYTE *)pParms + sizeof(R_ADMIN_PXTR_PARMS_T);
    memset(&prof_name, 0, sizeof(PROF_NAME_T));
    strncpy((&prof_name)->name, finger, pParms->lProf_name);
    printf("Profile name: %s\n",prof_name.name);

    // Print all of the segments and associated fields.
    finger += pParms->lProf_name;
    pxtr_print_segments((R_ADMIN_SDESC_T *)finger, pParms->nSegments, (BYTE *)pParms, pLog);
   }                                   // pxtr_print_output

void pxtr_print_segments(R_ADMIN_SDESC_T *p_sdesc, int nSegments, BYTE *pParms, LOGGER_T *pLog)
   {                                   // pxtr_print_segments
    int i_seg = 1;
    char seg_name[9];                  // var for null-terminating strings
    R_ADMIN_SDESC_T *p_seg = p_sdesc;

    while(i_seg <= nSegments)
      {
       BYTE *finger = (BYTE *)p_seg + p_seg->off_fdesc_1;

       memset(seg_name, 0, sizeof(seg_name));
       strncpy(seg_name, p_seg->name, sizeof(p_seg->name));

       printf("Segment %d\n", i_seg);
       printf("   name:             %s\n",seg_name);
       printf("   flags:            %08x\n",p_seg->flags);
       printf("   num fields:       %d\n",p_seg->nFields);
       printf("   off field desc 1: %d\n",p_seg->off_fdesc_1);

       // If this is the last segment, then fields follow immediately, 
       // otherwise, they are at the offset in this segment descriptor.
       if (i_seg <= nSegments)
          finger = (BYTE *)p_seg + sizeof(R_ADMIN_SDESC_T);
       else
          finger = (BYTE *)p_seg + p_seg->off_fdesc_1;
       pxtr_print_fields((R_ADMIN_FDESC_T *)finger, p_seg->nFields, pParms, pLog);

       i_seg++;
       p_seg++;
      }

   }                                   // pxtr_print_segments

void pxtr_print_fields(R_ADMIN_FDESC_T *p_fdesc, int nFields, BYTE *pParms, LOGGER_T *pLog)
   {                                   // pxtr_print_fields
    int i_fld = 1;
    char fld_name[9];                  // var for null-terminating strings
    R_ADMIN_FDESC_T *p_fld = p_fdesc;

    while(i_fld <= nFields)
      {
       memset(fld_name, 0, sizeof(fld_name));
       strncpy(fld_name, p_fld->name, sizeof(p_fld->name));

       printf("Field %d (R_ADMIN_FDESC_T)\n", i_fld);
       printf("   name:  %s\n",fld_name);

       printf("   type: (%04x)  ",p_fld->type);
       if (p_fld->type & t_boolean_field)
         printf("  boolean");
       else
         printf("  character");
       if (p_fld->type & t_mbr_repeat_group)
         printf(", repeat group member ");
       if (p_fld->type & t_repeat_field_hdr)
         printf(", repeat field header ");
       printf("\n");

       printf("   flags: (%08x)",p_fld->flags);
       if (p_fld->flags & f_output_only)
          printf("    output only");
       printf("\n");

       if (p_fld->type & t_boolean_field)
         {                              // boolean field type
          if (p_fld->flags & f_boolean_field)
            printf("     TRUE\n");
          else
            printf("     FALSE\n");
         }                             // boolean field type

       else
         {                             // character field

          if (!(p_fld->type & t_repeat_field_hdr))
            {                          // single value field
             char content[1025];       // null-terminated string
             int l_content = sizeof(content);

             // Null-terminate, and clip the size of the content if necessary.
             memset(content, 0, sizeof(content));
             if (p_fld->len_rpt.l_fld_data < sizeof(content))
               l_content = p_fld->len_rpt.l_fld_data;
             strncpy(content, ((char *)pParms)+p_fld->off_rpt.off_fld_data, l_content);
             printf("   content: %s\n", content);
            }                          // single value field

          else
            {                          // repeating field
             printf("   num repeat grps:  %d\n",p_fld->len_rpt.n_repeat_grps);
             printf("   num repeat elems: %d\n",p_fld->off_rpt.n_repeat_elems);
            }                          // repeating field

         }                             // character field

       i_fld++;
       p_fld++;
      }

   }                                   // pxtr_print_fields


// Raw dump of the profile extract function control blocks.
// This includes the returned parameters control block and all 
// associated segments and fields.  Note that this outputs a
// complete unformatted view of the output from R_admin.
void pxtr_dump(R_ADMIN_PXTR_PARMS_T *pParms, LOGGER_T *pLog)
   {                                   // pxtr_dump_output
    char eyecatcher[5];                // vars for null-terminating strings
    char class_name[9];
    PROF_NAME_T prof_name;
    BYTE *finger;                      // current memory location

    memset(eyecatcher, 0, sizeof(eyecatcher));
    strncpy(eyecatcher, pParms->eyecatcher, sizeof(pParms->eyecatcher));
    memset(class_name, 0, sizeof(class_name));
    strncpy(class_name, pParms->class_name, sizeof(pParms->class_name));

    log_debug(pLog, "Profile extract parms (%08x)", pParms);
    log_debug(pLog, "   +0 eyecatcher:   %s",eyecatcher);
    log_debug(pLog, "   +4 lOutbuf:     %d",pParms->lOutbuf);
    log_debug(pLog, "   +8 subpool:      %d",pParms->subpool);
    log_debug(pLog, "   +9 version:      %d",pParms->version);
    log_debug(pLog, "   +A reserved");
    log_debug(pLog, "   +C class_name:   %s",class_name);
    log_debug(pLog, "  +14 lProf_name:   %d",pParms->lProf_name);
    log_debug(pLog, "  +18 reserved");
    log_debug(pLog, "  +20 reserved");
    log_debug(pLog, "  +24 flags:        %08x",pParms->flags);
    log_debug(pLog, "  +28 nSegments:   %0d",pParms->nSegments);
    log_debug(pLog, "  +2C reserved");

    finger = (BYTE *)pParms + sizeof(R_ADMIN_PXTR_PARMS_T);
    memset(&prof_name, 0, sizeof(PROF_NAME_T));
    strncpy((&prof_name)->name, finger, pParms->lProf_name);
    log_debug(pLog, "  +3C profile name: |%s|\n",prof_name.name);

    dump_mem(pParms, 80, CCSID_EBCDIC, pLog);

    // Print all of the segments and associated fields.
    // finger += pParms->lProf_name;
    // pxtr_dump_segments((R_ADMIN_SDESC_T	*)finger, pParms->nSegments, pLog);
   }                                   // pxtr_dump_output

void pxtr_dump_segments(R_ADMIN_SDESC_T *p_sdesc, int nSegments, LOGGER_T *pLog)
   {                                   // pxtr_dump_segments
    int i_seg = 1;
    char seg_name[9];                  // var for null-terminating strings
    R_ADMIN_SDESC_T *p_seg = p_sdesc;

    while(i_seg <= nSegments)
      {
       BYTE *finger = (BYTE *)p_seg + p_seg->off_fdesc_1;

       memset(seg_name, 0, sizeof(seg_name));
       strncpy(seg_name, p_seg->name, sizeof(p_seg->name));

       printf("Segment %d (R_ADMIN_SDESC_T)\n", i_seg);
       printf("   +0 name:        %s\n",seg_name);
       printf("   +8 flags:       %08x\n",p_seg->flags);
       printf("   +C nFields:    %d\n",p_seg->nFields);
       printf("  +10 reserved\n");
       printf("  +14 off_fdesc_1: %d\n",p_seg->off_fdesc_1);
       printf("  +18 reserved\n");

       // If this is the last segment, then fields follow immediately, 
       // otherwise, they are at the offset in this segment descriptor.
       if (i_seg <= nSegments)
          finger = (BYTE *)p_seg + sizeof(R_ADMIN_SDESC_T);
       else
          finger = (BYTE *)p_seg + p_seg->off_fdesc_1;
       pxtr_dump_fields((R_ADMIN_FDESC_T *)finger, p_seg->nFields, pLog);

       i_seg++;
       p_seg++;
      }

   }                                   // pxtr_dump_segments

void pxtr_dump_fields(R_ADMIN_FDESC_T *p_fdesc, int nFields, LOGGER_T *pLog)
   {                                   // pxtr_dump_fields
    int i_fld = 1;
    char fld_name[9];                  // var for null-terminating strings
    R_ADMIN_FDESC_T *p_fld = p_fdesc;

    while(i_fld <= nFields)
      {
       memset(fld_name, 0, sizeof(fld_name));
       strncpy(fld_name, p_fld->name, sizeof(p_fld->name));

       printf("Field %d (R_ADMIN_FDESC_T)\n", i_fld);
       printf("   +0 name:          %s\n",fld_name);
       printf("   +8 type:          %04x\n",p_fld->type);
       printf("   +A reserved\n");                        
       printf("   +C flags:         %08x\n",p_fld->flags);

       if (!(p_fld->type & t_repeat_field_hdr))
         printf("  +10 l_fld_data: %d\n",p_fld->len_rpt.l_fld_data);
       else
         printf("  +10 n_repeat_grps: %d\n",p_fld->len_rpt.n_repeat_grps);

       printf("  +14 reserved\n");

       if (!(p_fld->type & t_repeat_field_hdr))
         printf("  +18 off_fld_data:   %d\n",p_fld->off_rpt.off_fld_data);
       else
         printf("  +18 n_repeat_elems: %d\n",p_fld->off_rpt.n_repeat_elems);

       printf("  +1C reserved\n");

       i_fld++;
       p_fld++;
      }

   }                                   // pxtr_dump_fields

void pxtr_dump_args_parms(PXTR_CTL_T *pPXTRCtl, LOGGER_T *pLog)
   {
    UNDERBAR_ARG_AREA_T * __ptr32 p31 = pPXTRCtl->pP31Area;

    log_debug(pLog, "---------------------------------------------");
    log_debug(pLog, "Args (%08x), p31: %08x:", &(p31->args), p31);
    log_debug(pLog, "  RACF_work_area p: %08x,  l: %d", &(p31->args.RACF_work_area), L_RACF_WORK_AREA);
    log_debug(pLog, "  SAF_rc,   ALET: %08x,  p: %08x", &(p31->args.ALET_SAF_rc), &(p31->args.SAF_rc));
    log_debug(pLog, "  RACF_rc,  ALET: %08x,  p: %08x", &(p31->args.ALET_RACF_rc), &(p31->args.RACF_rc));
    log_debug(pLog, "  RACF_rsn, ALET: %08x,  p: %08x", &(p31->args.ALET_RACF_rsn), &(p31->args.RACF_rsn));
    log_debug(pLog, "  func_code: %d,  p: %08x\n", p31->args.func_code, &(p31->args.func_code));

    pxtr_dump(&(p31->args.pxtr_parms), pLog);
   
    // log_debug(pLog, "  prof_name (%08x): %s", &(p31->args.prof_name.name), p31->args.prof_name.name);
    log_debug(pLog, "  ACEE (%08x): %08x", &(p31->args.ACEE), p31->args.ACEE);
    log_debug(pLog, "  outbuf_subpool (%08x): %d", &(p31->args.outbuf_subpool), p31->args.outbuf_subpool);
    log_debug(pLog, "  pOutbuf (%08x): %08x\n", &(p31->args.pOutbuf), p31->args.pOutbuf);

    log_debug(pLog, "Arg list (%08x):", &(pPXTRCtl->pP31Area->arg_list));
    log_debug(pLog, "  pWork_area:     %08x", p31->arg_list.pWork_area);
    log_debug(pLog, "  pALET_SAF_rc:   %08x,  pSAF_rc:   %08x", p31->arg_list.pALET_SAF_rc, p31->arg_list.pSAF_rc);
    log_debug(pLog, "  pALET_RACF_rc:  %08x,  pRACF_rc:  %08x", p31->arg_list.pALET_RACF_rc, p31->arg_list.pRACF_rc);
    log_debug(pLog, "  pALET_RACF_rsn: %08x,  pRACF_rsn: %08x", p31->arg_list.pALET_RACF_rsn, p31->arg_list.pRACF_rsn);
    log_debug(pLog, "  pFunc_code:  %08x", p31->arg_list.pFunc_code);
    log_debug(pLog, "  pPXTR_parms: %08x", p31->arg_list.pPXTR_parms);
    log_debug(pLog, "  pProf_name:  %08x", p31->arg_list.pProf_name);
    log_debug(pLog, "  pACEE:       %08x", p31->arg_list.pACEE);
    log_debug(pLog, "  pOutbuf_subpool:  %08x", p31->arg_list.pOutbuf_subpool);
    log_debug(pLog, "  ppOutbuf:  %08x", p31->arg_list.ppOutbuf);
    log_debug(pLog, "---------------------------------------------");
    return;
   }