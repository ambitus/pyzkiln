      *Copyright IBM Corp. 2024.
       IDENTIFICATION DIVISION.                                   
       PROGRAM-ID. 'COBTEST'.                                      
       ENVIRONMENT DIVISION.                                      
       CONFIGURATION SECTION.                                     
       INPUT-OUTPUT SECTION.                                      
       FILE-CONTROL.                                              
       DATA DIVISION.                                             
       FILE SECTION.                                              
       WORKING-STORAGE SECTION.                                   
       LINKAGE SECTION.                                           
       01 PARAM1                    PIC S9(9) USAGE IS BINARY. 
       01 PARAM2                    PIC S9(9) USAGE IS BINARY. 
       01 RETCODE                   PIC S9(9) USAGE IS BINARY. 
       PROCEDURE DIVISION USING BY VALUE PARAM1 
                          BY VALUE PARAM2
                          RETURNING RETCODE.                      
           ADD PARAM1 to RETCODE
           ADD PARAM2 to RETCODE
           GOBACK.
