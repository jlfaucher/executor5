/*----------------------------------------------------------------------------*/
/*                                                                            */
/* Copyright (c) 1995, 2004 IBM Corporation. All rights reserved.             */
/* Copyright (c) 2005-2019 Rexx Language Association. All rights reserved.    */
/*                                                                            */
/* This program and the accompanying materials are made available under       */
/* the terms of the Common Public License v1.0 which accompanies this         */
/* distribution. A copy is also available at the following address:           */
/* http://www.oorexx.org/license.html                                         */
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
/******************************************************************************/
/* REXX Kernel                                                                */
/*                                                                            */
/* Translates a program and saves it in an output file                        */
/*                                                                            */
/* Common code for unix-based systems                                         */
/*                                                                            */
/******************************************************************************/
#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <limits.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#if defined( HAVE_FEATURES_H )
# include <features.h>
#endif

#include "rexx.h"
#include "RexxInternalApis.h"          /* Get private REXXAPI API's         */
#include "RexxErrorCodes.h"

#define BUFFERLEN         256           /* Length of message bufs used        */

void DisplayError(int msgid)           /* simplified catalog access@MAE004M */
{
    // retrieve the message from the central catalog
    const char *message = RexxGetErrorMessage(msgid);

    printf("%s\n", message);    /* print the message                 */
}

int main (int argc, char **argv)
{
    bool silent = false;
    int silentp;
    char *ptr;
    /* check for /s option               */
    for (silentp = 1; silentp < argc; silentp++)
    {
        if (argv[silentp][0] == '-' &&
            (argv[silentp][1] == 's' || argv[silentp][1] == 'S'))
        {
            silent = true;
            break;
        }
    }
    if (!silent)                       /* display version and copyright     */
    {
        ptr = RexxGetVersionInformation();
        printf("%s\n", ptr);
        RexxFreeMemory(ptr);
    }
    /* Check validity of arguments       */
    if (argc < 2 || argc > 4 ||          /* # args exceeding bounds           */
        (silent && argc==2) ||             /* -s is the first argument          */
        (silent && (silentp + 1 < argc)) ||  /* -s is not the last argument       */
        (!silent && argc==4))           /* 3 arguments, but no /s            */
    {
        if (argc > 2)
        {
            DisplayError((int)Error_REXXC_cmd_parm_incorrect);
        }
        DisplayError((int) Error_REXXC_wrongNrArg_unix);
        DisplayError((int) Error_REXXC_SynCheckInfo);
        exit(-1);                          /* terminate with an error           */
    }                                    /* end additions                     */
    /* modified control logic            */
    if ((argc==4 && silent) || (argc==3 && !silent))
    {
        if (strcmp(argv[1], argv[2]) == 0)
        {
            DisplayError((int)Error_REXXC_outDifferent);
            exit(-2);                        /* terminate with an error           */
        }
        /* translate and save the output     */
        return RexxTranslateProgram(argv[1], argv[2], NULL);
    }
    else                                 /* just doing syntax check           */
    {
        return RexxTranslateProgram(argv[1], NULL, NULL);
    }
}
