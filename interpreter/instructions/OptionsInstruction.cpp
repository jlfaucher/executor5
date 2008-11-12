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
/******************************************************************************/
/* REXX Translator                                                            */
/*                                                                            */
/* Primitive Options Parse Class                                              */
/*                                                                            */
/******************************************************************************/
#include <stdlib.h>
#include "RexxCore.h"
#include "StringClass.hpp"
#include "RexxActivation.hpp"
#include "OptionsInstruction.hpp"

RexxInstructionOptions::RexxInstructionOptions(
    RexxObject *_expression)            /* associated expression object      */
/******************************************************************************/
/* Initialize a REXX OPTION instruction                                       */
/******************************************************************************/
{
  OrefSet(this, this->expression, _expression);
}

void RexxInstructionOptions::execute(
    RexxActivation      *context,      /* current activation context        */
    RexxExpressionStack *stack)        /* evaluation stack                  */
/******************************************************************************/
/* Function:  Execute a REXX OPTIONS instruction                              */
/******************************************************************************/
{
    RexxObject *value;                   /* output value                      */
    RexxString *stringVal;               /* string version of the value       */
    size_t i;                            /* loop counter                      */
    RexxString *word;                    /* current word                      */

    context->traceInstruction(this);     /* trace if necessary                */
                                         /* get the expression value          */
    value = this->expression->evaluate(context, stack);
    stringVal = REQUEST_STRING(value);   /* get the string version            */
    context->traceResult(value);         /* trace the output value            */
    for (i = 1; ;i++)                  /* now process each word             */
    {
        /* get the next word                 */
        word = (RexxString *)(stringVal->word(new_integer(i)));
        if (word->getLength() == 0)        /* get the length of the word        */
        {
            break;                           /* if length of word = 0 then stop   */
        }

#ifdef _DEBUG
        if (word->strCaselessCompare("DUMPMEMORY"))
        {
            memoryObject.dumpEnable = true;
            memoryObject.dump();
        }
#endif
    }
    context->pauseInstruction();         /* do debug pause if necessary       */
}

