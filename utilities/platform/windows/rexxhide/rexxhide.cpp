/*----------------------------------------------------------------------------*/
/*                                                                            */
/* Copyright (c) 1995, 2004 IBM Corporation. All rights reserved.             */
/* Copyright (c) 2005-2009 Rexx Language Association. All rights reserved.    */
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
/*  File Name:          REXXHIDE.C                                   */
/*                                                                   */
/*  Calling REXX without creating any console                        */
/*********************************************************************/

#include <windows.h>
#include <oorexxapi.h>                      /* needed for RexxStart()     */
#include <malloc.h>
#include <string.h>                         /* needed for strlen()        */
#include <stdio.h>
#include <io.h>

#include "ArgumentParser.h"  /* defines getArguments and freeArguments */

//
//  MAIN program
//
int WINAPI WinMain(
    HINSTANCE hInstance,	// handle to current instance
    HINSTANCE hPrevInstance,	// handle to previous instance
    LPSTR lpCmdLine,	// pointer to command line
    int nCmdShow)
{
    int32_t rc;                          /* actually running program RC       */
    const char *program_name;            /* name to run                       */
    CHAR  arg_buffer[1024];              /* starting argument buffer         */
    CONSTRXSTRING arguments;             /* rexxstart argument                */
    size_t argcount;

    rc = 0;                              /* set default return                */

    strcpy(arg_buffer, lpCmdLine);
    getArguments(&program_name, arg_buffer, &argcount, &arguments);

    if (program_name == NULL)
    {
        /* give a simple error message       */
        MessageBox(NULL, "Syntax: REXXHIDE ProgramName [parameter_1....parameter_n]\n", "Wrong Arguments", MB_OK | MB_ICONHAND);
        return -1;
    }
    else                               /* real program execution            */
    {
        RexxInstance        *pgmInst;
        RexxThreadContext   *pgmThrdInst;
        RexxArrayObject      rxargs, rxcargs;
        RexxDirectoryObject  dir;
        RexxObjectPtr        result;
        int i;
        int argc;                            // parsed count of arguments
        PCHAR *argv;                         // parsed out arguments

        // parse the arguments into argv/argc format
        argv = CommandLineToArgvA(lpCmdLine, &argc);

        RexxCreateInterpreter(&pgmInst, &pgmThrdInst, NULL);
        // configure the traditional single argument string
        rxargs = pgmThrdInst->NewArray(1);
        pgmThrdInst->ArrayPut(rxargs,
                              pgmThrdInst->NewString(arguments.strptr, arguments.strlength), 1);
        // set up the C args into the .local environment
        dir = (RexxDirectoryObject)pgmThrdInst->GetLocalEnvironment();
        rxcargs = pgmThrdInst->NewArray(argc - 2);
        for (i = 2; i < argc; i++) {
            pgmThrdInst->ArrayPut(rxcargs,
                                  pgmThrdInst->NewStringFromAsciiz(argv[i]),
                                  i - 1);
        }

        pgmThrdInst->DirectoryPut(dir, rxcargs, "SYSCARGS");

        LocalFree(argv);        // released the parsed argguments
        // call the interpreter
        result = pgmThrdInst->CallProgram(program_name, rxargs);
        rc = 0;
        if (result != NULL) {
            pgmThrdInst->ObjectToInt32(result, &rc);
        }

        if (rc < 0)
        {
            sprintf(arg_buffer, "Open Object Rexx program execution failure: rc = %d",rc);
            MessageBox(NULL, arg_buffer, "Execution Error", MB_OK | MB_ICONHAND);
        }
    }
    // return interpeter or
    return rc;                                  // rexx program return cd
}




