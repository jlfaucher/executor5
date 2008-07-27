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
/* REXX Translator                                              ReplyInstruction.c    */
/*                                                                            */
/* Primitive Reply Parse Class                                                */
/*                                                                            */
/******************************************************************************/
#include <stdlib.h>
#include "RexxCore.h"
#include "RexxActivation.hpp"
#include "ReplyInstruction.hpp"

RexxInstructionReply::RexxInstructionReply(
    RexxObject *expression)
/******************************************************************************/
/* Function:  Initialize a REXX REPLY instruction                             */
/******************************************************************************/
{
                                       /* do the common initialization      */
 OrefSet(this, this->expression, expression);
}

void RexxInstructionReply::execute(
    RexxActivation      *context,      /* current activation context        */
    RexxExpressionStack *stack)        /* evaluation stack                  */
/******************************************************************************/
/* Function:  Execute a REXX REPLY instruction                                */
/******************************************************************************/
{
  RexxObject *result;                  /* expression result                 */

#ifdef NOTHREADSUPPORT
   report_exception1(Error_Execution_no_concurrency,
                  new_cstring("Concurrency not supported"));
#else

  context->traceInstruction(this);     /* trace if necessary                */
  if (!context->inMethod())            /* is this a method clause?          */
                                       /* raise an error                    */
    report_exception(Error_Translation_reply);
  if (this->expression != OREF_NULL) { /* given an expression value?        */
                                       /* evaluate the expression           */
    result = this->expression->evaluate(context, stack);
    context->traceResult(result);      /* trace if necessary                */
                                       /* get the expression value and tell */
    context->reply(result);            /* the activation to reply with it   */
  }
  else
    context->reply(OREF_NULL);         /* return with no value              */
  context->pauseInstruction();         /* do debug pause if necessary       */
#endif                                 // end of NOTHREADSUPPORT
}

