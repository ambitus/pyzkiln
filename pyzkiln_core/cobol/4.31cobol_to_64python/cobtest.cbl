      *Copyright IBM Corp. 2024.
       IDENTIFICATION DIVISION.
       PROGRAM-ID. "COBTEST".
       DATA DIVISION.
       WORKING-STORAGE SECTION.
       77  PGM-NAME                   PICTURE X(13).
       LINKAGE SECTION.
       PROCEDURE DIVISION.
      *Dynamically call our 64-bit COBOL program
           MOVE "DYCALLEE" to PGM-NAME.
           CALL PGM-NAME.
           STOP RUN.
