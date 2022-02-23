*---------------------------------------------------------------------*
* Glue code to link from python to racf radmin interface (IRRSEQ00)   *
*---------------------------------------------------------------------*
         SYSSTATE AMODE64=YES,ARCHLVL=OSREL,OSREL=SYSSTATE

CALADMN  TITLE 'Glue code to link from Python to RACF'
         EJECT ,

*---------------------------------------------------------------------*
* Establish an LE environment with correct addressability             *
*---------------------------------------------------------------------*
CALADMN  CELQPRLG DSASIZE=DSASIZ,     Use calculated size of DSA       *
               BASEREG=NONE,          Use relative addressing          *
               PARMWRDS=3,            This routine needs 3 parms       *
               PARMREG=11,            Location of callers parms        *
               ENTNAME=callRadmin,    Special entry point name         *
               PSECT=CALADMN1         Psect name to use

callRadmin ALIAS C'callRadmin'
         USING AUTOSTG,R4             Our DSA
         USING MYPARMS,R11            Parameters supplied to us
         EJECT ,

*---------------------------------------------------------------------*
* Keep a copy of our environment across function calls                *
*---------------------------------------------------------------------*
SAVEENV  DS    0H
         LGR   R10,R5
         USING CALADMN1,R10
         EJECT ,

*---------------------------------------------------------------------*
* Check the supplied parameters for pre-allocated storage.            *
*---------------------------------------------------------------------*
PARMCHEK DS    0H
         LTGR  R12,R1
         LGR   R8,R1
         EJECT ,

*---------------------------------------------------------------------*
* Grab some storage to build a 31-bit stack                           *
*---------------------------------------------------------------------*
GETSTOR  DS    0H
         XGR   R1,R1
         AGHI  R1,STOR31L             Size of data to obtain for us
         STG   R1,PARM1               Parm for call to malloc31

         LMG   R5,R6,MALL31FD         Setup function descriptor
         BASR  R7,R6                  Call function
         NOPR  0

CEEWSA   LOCTR
C_WSA64  CATTR DEFLOAD,RMODE(64),PART(CALADMN1)
MALLOC31 ALIAS C'__malloc31'
MALLOC31 AMODE 64
MALLOC31 XATTR LINKAGE(XPLINK),SCOPE(IMPORT),REF(CODE)
MALL31FD DC    RD(MALLOC31)
         DC    VD(MALLOC31)
CALADMN  LOCTR

         DS    0H
         LGR   R5,R10                 Restore our environment
         LTR   R3,R3                  Was call successful?
         JZ    NOMALL31               Terminate if no storage

         LGR   R12,R3                 Return from malloc into R12
         EJECT ,

*---------------------------------------------------------------------*
* Build a fully-working stack                                         *
*---------------------------------------------------------------------*
SKIPMALL DS    0H
         USING STOR31,R12             Address dedicated 31-bit storage
         LGR   R1,R8

         LA    R13,SAVEAREA           Point R13 at the save area ...
         XC    SAVEAREA,SAVEAREA      ... and clear
         XC    IRRSRC,IRRSRC          Clear RC placeholder
         EJECT ,

*---------------------------------------------------------------------*
* Call the IRRSEQ00 program in the correct AMODE.                     *
*                                                                     *
* Save the high halves of R2 through R14 because the RACF Admin       *
* interface doesn't seem to preserve the top of 64-bit regs for       *
* the caller.                                                         *
*---------------------------------------------------------------------*
CALLPROG DS    0H
         LARL  R15,=V(IRRSEQ00)       Entry point to racf admin routine
         L     R15,0(0,R15)
         SAM31
         STMH  R2,R14,SAVEHIGH        Save the high halves
         BALR  R14,R15                Off we go ...
         LMH   R2,R14,SAVEHIGH        Restore the high halves
         SAM64
         ST    R15,IRRSRC             Save the return code
         EJECT ,

*---------------------------------------------------------------------*
* Release our 31-bit storage                                          *
*---------------------------------------------------------------------*
RELEASE  DS    0H
         STG   R12,PARM1              Pointer to data to free

         LMG   R5,R6,FREE_FD          Setup function descriptor
         BASR  R7,R6                  Call function
         NOPR  0

CEEWSA   LOCTR
C_WSA64  CATTR DEFLOAD,RMODE(64),PART(CALADMN1)
FREE     ALIAS C'free'
FREE     AMODE 64
FREE     XATTR LINKAGE(XPLINK),SCOPE(IMPORT),REF(CODE)
FREE_FD  DC    RD(FREE)
         DC    VD(FREE)
CALADMN  LOCTR

SKIPFREE DS    0H
         DROP  R12                    31-bit storage now gone
         LGR   R5,R10                 Restore our environment
         EJECT ,

*---------------------------------------------------------------------*
* Set return code from called routine                                 *
*---------------------------------------------------------------------*
         DS    0H
         L     R3,IRRSRC              Restore RC from the racf call
         J     RETURN                 All finished
         EJECT ,

*---------------------------------------------------------------------*
* Failure has occurred allocating 31-bit storage                      *
*---------------------------------------------------------------------*
NOMALL31 DS    0H
         LHI   R3,-2
         J     RETURN
         EJECT ,

*---------------------------------------------------------------------*
* Set return code and terminate                                       *
* CELQEPLG will restore R4 through R15.                               *
*---------------------------------------------------------------------*
RETURN   DS    0H
         CELQEPLG ,
         EJECT ,

         DROP  R4                     No longer need DSA
         DROP  R10                    No longer need environment
         DROP  R11                    No longer need our parms

*---------------------------------------------------------------------*
* Dynamic storage                                                     *
*---------------------------------------------------------------------*
AUTOSTG  DSECT ,
         CEEDSA SECTYPE=XPLINK        Map CEE Dynamic Save Area
*
*------- Parameters used when calling other XPLINK routines
*
PARMLIST DS    0D
PARM1    DS    AD
PARM2    DS    AD
PARM3    DS    AD
PARM4    DS    AD
PARM5    DS    AD
PARM6    DS    AD
PARM7    DS    AD
PARM8    DS    AD
*
*------- Local storage
*
IRRSRC   DS    FD
*
DSASIZ   EQU   *-PARMLIST+CEEDSAHPSZ  Length of DSA required
         EJECT ,

*---------------------------------------------------------------------*
* Parameter list supplied to us on entry.                             *
* Don't write to these locations - it isn't our storage to use.       *
*---------------------------------------------------------------------*
MYPARMS  DSECT ,
         DS    0FD
DATAPTR  DS    AD
FUNCID   DS    FD
DSAPTR   DS    AD
         EJECT ,

*---------------------------------------------------------------------*
* Storage we need to specifically acquire in 31-bit                   *
*---------------------------------------------------------------------*
STOR31   DSECT ,
         DS    0F
*------- Save area
SAVEAREA DS    18F                    Save area for called routine
SAVEHIGH DS    13F                    R2-R14 high half save area

STOR31L  EQU   *-STOR31
         EJECT ,


*---------------------------------------------------------------------*
* Includes                                                            *
*---------------------------------------------------------------------*
         PUSH  PRINT
         PRINT OFF

         CVT   DSECT=YES              Map CVT
         IRRPCOMP ,                   Parm list & func code mappings
         IRRPRXTW ,                   Extract result area mapping
         CEECAA ,                     Map LE Common Anchor Area
         REGDEFS ,                    Register equates

         POP   PRINT
         EJECT ,

*---------------------------------------------------------------------*
* End of program                                                      *
*---------------------------------------------------------------------*
         END   CALADMN
