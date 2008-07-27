/*----------------------------------------------------------------------------*/
/*                                                                            */
/* Copyright (c) 1995, 2004 IBM Corporation. All rights reserved.             */
/* Copyright (c) 2005-2006 Rexx Language Association. All rights reserved.    */
/*                                                                            */
/* This program and the accompanying materials are made available under       */
/* the terms of the Common Public License v1.0 which accompanies this         */
/* distribution. A copy is also available at the following address:           */
/* http://www.oorexx.org/license.html                          */
/*                                                                            */
/* Redistribution and use in source and binary forms, with or                 */
/* without modification, are permitted provided that the following            */
/* conditions are met:                                                        */
/*                                                                            */
/* Redistributions of source code must retain the above copyright             */
/* notice, this list of conditions and the following disclaimer.              */
/* Redistributions in binary form must reproduce the above copyright          */
/* notice, this list of conditions and the following disclaimer in            */
/* the documentation and/or other materials provided with the distribution.   */
/*                                                                            */
/* Neither the name of Rexx Language Association nor the names                */
/* of its contributors may be used to endorse or promote products             */
/* derived from this software without specific prior written permission.      */
/*                                                                            */
/* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS        */
/* "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT          */
/* LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS          */
/* FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT   */
/* OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,      */
/* SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED   */
/* TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,        */
/* OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY     */
/* OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING    */
/* NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS         */
/* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.               */
/*                                                                            */
/*----------------------------------------------------------------------------*/
/*********************************************************************/
/*                                                                   */
/*  File Name:          REXX.C                                   */
/*                                                                   */
/*  Description:        Provides a sample call to the REXX           */
/*                      interpreter, passing in an environment name, */
/*                      a file name, and a single argument string.   */
/*                                                                   */
/*  Entry Points:       main - main entry point                      */
/*                                                                   */
/*  Input:              None                                         */
/*                                                                   */
/*  Output:             returns 0 in all cases.                      */
/*                                                                   */
/*********************************************************************/


#define INCL_REXXSAA
#include <windows.h>
#include <rexx.h>                           /* needed for RexxStart()     */
#include <malloc.h>
#include <stdio.h>                          /* needed for printf()        */
#include <string.h>                         /* needed for strlen()        */

extern "C" {
BOOL   APIENTRY RexxInitialize (void);
}

                                         /* Global inducator */
extern  _declspec(dllimport) BOOL RexxStartedByApplication;
extern  _declspec(dllimport) BOOL ProcessRestoreImage;
extern  _declspec(dllimport) BOOL ProcessSaveImage;
extern  _declspec(dllimport) HANDLE RexxTerminated;           /* Termination complete semaphore.   */

//
//  Prototypes
//
int __cdecl main(int argc, char *argv[]);  /* main entry point           */
LONG APIENTRY MY_IOEXIT( LONG ExitNumber, LONG Subfunction, PEXIT ParmBlock);

extern _declspec(dllimport) PCHAR RexxGetVersionInformation(void);

#include "ArgumentParser.h"  /* defines getArguments and freeArguments */

//
//  MAIN program
//
int __cdecl main(int argc, char *argv[])
{
//  RXSYSEXIT exit_list[9];              /* Exit list array                 */
  LONG     rexxrc = 0;                 /* return code from rexx             */
  INT   i;                             /* loop counter                      */
  LONG  rc;                            /* actually running program RC       */
  PCHAR program_name;                  /* name to run                       */
  CHAR  arg_buffer[8192];              /* starting argument buffer          */
  char *cp;                            /* option character pointer          */
  RXSTRING arguments;                  /* rexxstart argument                */
  ULONG argcount;
  RXSTRING rxretbuf;                   // program return buffer

  RexxStartedByApplication = FALSE;    /* Call NOT from internal            */
  rc = 0;                              /* set default return                */

   /*
    * Convert the input array into a single string for the Object REXX
    * argument string. Initialize the RXSTRING variable to point to this
    * string. Keep the string null terminated so we can print it for debug.
    * First argument is name of the REXX program
    * Next argument(s) are parameters to be passed
   */

  arg_buffer[0] = '\0';                /* default to no argument string     */
  program_name = NULL;                 /* no program to run yet             */

  for (i = 1; i < argc; i++) {         /* loop through the arguments        */
                                       /* is this an option switch?         */
    if ((*(cp=*(argv+i)) == '-' || *cp == '/'))
      switch (*++cp) {
        case 'i': case 'I':            /* image build                       */
          ProcessRestoreImage = FALSE; /* hit the startup flags             */
          ProcessSaveImage = TRUE;     /* say this is a save image          */
          break;

        case 'v': case 'V':            /* version display */
            {
                PCHAR ptr = RexxGetVersionInformation();
                if (ptr) {
                    printf(ptr, "Interpreter");
                    GlobalFree(ptr);
                }
            }
            RexxShutDownAPI();        /* unload RXAPI.EXE */
            return 0;

        default:                       /* ignore other switches             */
          break;
      }
    else {                             /* convert into an argument string   */
      if (program_name == NULL)        /* no name yet?                      */
      {
        program_name = argv[i];        /* program is first non-option       */
        break;      /* end parsing after program_name has been resolved */
      }
      else {                           /* part of the argument string       */
        if (arg_buffer[0] != '\0')     /* not the first one?                */
          strcat(arg_buffer, " ");     /* add an blank                      */
        strcat(arg_buffer, argv[i]);   /* add this to the argument string   */
      }
    }
  }

  if (ProcessSaveImage) {              /* save an image?                    */
                                       /* This is a Saveimage ...           */
    RexxInitialize();                  /* do normal REXX init               */
    /* RexxTerminate();                   now immediately terminate         */
  }
  else if (program_name == NULL) {
                                       /* give a simple error message       */
    #undef printf
    printf("Syntax: REXX ProgramName [parameter_1....parameter_n]\n");
    /* try to unload the orexx memory manager */
    RexxShutDownAPI();
    return -1;
  }
  else {                               /* real program execution            */
    strcpy(arg_buffer, GetCommandLine());
    getArguments(NULL, arg_buffer, &argcount, &arguments);
    rxretbuf.strlength = 0L;           /* initialize return to empty*/

    #ifdef REXXC_DEBUG
    printf("program_name = %s\n", program_name);
    printf("argv 0 = %s\n", argv[0]);
    printf("argv 1 = %s\n", argv[1]);
    printf("argv 2 = %s\n", argv[2]);
    printf("argument.strptr = %s\n", argument.strptr);
    printf("argument.strlenth = %lu\n", argument.strlength);
    #endif


   /* Here we call the interpreter.  We don't really need to use     */
   /* all the casts in this call; they just help illustrate          */
   /* the data types used.                                           */
   rc=REXXSTART((LONG)       argcount,      /* number of arguments   */
                (PRXSTRING)  &arguments,     /* array of arguments   */
                (PSZ)        program_name,  /* name of REXX file     */
                (PRXSTRING)  0,             /* No INSTORE used       */
                (PSZ)        "CMD",         /* Command env. name     */
                (LONG)       RXCOMMAND,     /* Code for how invoked  */
//                (PRXSYSEXIT) exit_list,     /* No EXITs on this call */
                NULL,
                (PSHORT)     &rexxrc,       /* Rexx program output   */
                (PRXSTRING)  &rxretbuf );   /* Rexx program output   */

   /* wait until all activities did finish so no activity will be canceled */
   RexxWaitForTermination();

   /* free for DB2 2.1.1 version */
//   #undef free
//   if (rc==0) free(rxretbuf.strptr);        /* Release storage only if*/
                        /* rexx procedure executed*/
   if ((rc==0) && rxretbuf.strptr) GlobalFree(rxretbuf.strptr);        /* Release storage only if*/
   freeArguments(NULL, &arguments);

//   RexxDeregisterExit("MY_IOC",NULL);     // remove the exit in exe exit list
  }
  /* try to unload the orexx memory manager */
  RexxShutDownAPI();
                                             // return interpeter or
 return rc ? rc : rexxrc;                    // rexx program return cd
}


/*
LONG APIENTRY MY_IOEXIT(
     LONG ExitNumber,
     LONG Subfunction,
     PEXIT parmblock)
{
   RXSIOSAY_PARM *sparm ;
   RXSIOTRC_PARM *tparm ;
   RXSIOTRD_PARM *rparm ;
   RXSIODTR_PARM *dparm ;

   _setmode( _fileno( stdout ), _O_BINARY );
   switch (Subfunction) {
   case RXSIOSAY:
      sparm = ( RXSIOSAY_PARM * )parmblock ;
      printf("%s\n",sparm->rxsio_string.strptr);
      break;
   case RXSIOTRC:
      tparm = ( RXSIOTRC_PARM * )parmblock ;
      printf("%s\n",tparm->rxsio_string.strptr);
      break;
   case RXSIOTRD:
      rparm = (RXSIOTRD_PARM * )parmblock ;
      gets(rparm->rxsiotrd_retc.strptr);
      rparm->rxsiotrd_retc.strlength=strlen(rparm->rxsiotrd_retc.strptr);
      break;
   case RXSIODTR:
      dparm = (RXSIODTR_PARM * )parmblock ;
      gets(dparm->rxsiodtr_retc.strptr);
      dparm->rxsiodtr_retc.strlength=strlen(dparm->rxsiodtr_retc.strptr);
      break;
   default:
      break;
   } // endswitch

   return RXEXIT_HANDLED;

}
*/


