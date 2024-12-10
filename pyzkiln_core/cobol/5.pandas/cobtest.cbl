      * Copyright IBM Corp. 2024.
       IDENTIFICATION DIVISION.
       PROGRAM-ID. "PANDASREAD".
       DATA DIVISION.
       WORKING-STORAGE SECTION.
       01 pandas-name         PIC U(16) VALUE Z'pandas'.
       01 pandas-module-ptr   USAGE POINTER.

       01 read-name           PIC U(16) VALUE Z'read_csv'.
       01 read-csv-func       USAGE POINTER.
       01 read-csv-args       USAGE POINTER.

       01 builtin-ptr         USAGE POINTER.
       01 print-name          PIC U(16) VALUE Z'print'.
       01 print-function      USAGE POINTER.
       01 print-args          USAGE POINTER.

       01 csv-path            PIC U(16) VALUE Z'./test.csv'.
       01 csv-path-ptr        USAGE POINTER.
       01 pandas-csv-data-ptr USAGE POINTER.

       PROCEDURE DIVISION.
       MAIN-PARAGRAPH.
      ** This program gives an example of how to import and call Python
      *  packages, specifically using pandas. This program loads up a
      *  ISO8859-1 encoded csv file named test.csv using Pandas, and
      *  prints out its content. This is equivalent to the following
      *  Python script:
      *  
      *  import pandas
      *  csv_data = pandas.read_csv('test.csv')
      *  print(csv_data)

      ** Initialize the Python interpreter
           CALL "Py_Initialize"

      ** Import the pandas module
           CALL "PyImport_ImportModule" USING
               pandas-name
               RETURNING pandas-module-ptr
           END-CALL

           IF pandas-module-ptr EQUAL null
               DISPLAY u'Failed to import pandas'
               GOBACK
           END-IF

      ** Get read_csv function from pandas
           CALL "PyObject_GetAttrString" USING
               BY VALUE pandas-module-ptr
               BY REFERENCE read-name
               RETURNING read-csv-func
           END-CALL

      ** Create a tuple for calling read_csv - pandas.read_csv(path)
           CALL "PyTuple_New" USING
               BY VALUE 1
               RETURNING read-csv-args
           END-CALL

      ** Get a Python UTF-8 Object for the path to the csv file
           CALL "PyUnicode_FromString" USING
               BY REFERENCE csv-path
               RETURNING csv-path-ptr
           END-CALL

      ** Add the path to our tuple
           CALL "PyTuple_SetItem" USING
               BY VALUE read-csv-args
               BY VALUE 0
               BY VALUE csv-path-ptr
           END-CALL

      ** Call pandas.read_csv
           CALL "PyObject_CallObject" USING
               BY VALUE read-csv-func
               BY VALUE read-csv-args
               RETURNING pandas-csv-data-ptr
           END-CALL

      ** Get Pythons builtin print function
           CALL "PyEval_GetBuiltins"
               RETURNING builtin-ptr
           END-CALL

           CALL "PyDict_GetItemString" USING
               BY VALUE builtin-ptr
               BY REFERENCE print-name
               RETURNING print-function
           END-CALL

      ** Create new tuple to call print with
           CALL "PyTuple_New" USING
               BY VALUE 1
               RETURNING print-args
           END-CALL

      ** Add our pandas csv object 
           CALL "PyTuple_SetItem" USING
               BY VALUE print-args
               BY VALUE 0
               BY VALUE pandas-csv-data-ptr
           END-CALL

      ** Call print(csv)
           CALL "PyObject_CallObject" USING
               BY VALUE print-function
               BY VALUE print-args
           END-CALL

      ** Shut down the Python interpreter
           CALL "Py_Finalize"
           STOP RUN.
