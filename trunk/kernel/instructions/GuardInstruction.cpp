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
/* REXX Translator                                              GuardInstruction.c    */
/*                                                                            */
/* Primitive Guard Parse Class                                                */
/*                                                                            */
/******************************************************************************/
#include <stdlib.h>
#include "RexxCore.h"
#include "ArrayClass.hpp"
#include "RexxActivity.hpp"
#include "RexxActivation.hpp"
#include "GuardInstruction.hpp"
#include "ExpressionBaseVariable.hpp"

extern RexxActivity *CurrentActivity;  /* current running activity          */

RexxInstructionGuard::RexxInstructionGuard(
    RexxObject *expression,            /* guard expression                  */
    RexxArray  *variable_list,         /* list of variables to trigger on   */
    BOOL        on_off)                /* ON or OFF form                    */
/******************************************************************************/
/* Function:  Complete initialization of a GUARD instruction object           */
/******************************************************************************/
 {
  INT   i;                             /* loop counter                      */

                                       /* save the guard expression         */
  OrefSet(this, this->expression, expression);
  if (on_off)                          /* this the ON form?                 */
    i_flags |= guard_on_form;          /* turn on the flag                  */
  if (variable_list != OREF_NULL) {    /* got a guard expression?           */
                                       /* get the variable size             */
    guard_variable_count = variable_list->size();
                                       /* loop through the variable list    */
    for (i = 1; i <= guard_variable_count; i++)
                                       /* copying each variable             */
      OrefSet(this, this->variables[i-1], (RexxVariableBase *)(variable_list->get(i)));
  }
  else
    guard_variable_count = 0;          /* no extra variables                */
}

//extern RexxActivityCurrentActivity;  /* expose current activity object    */

void RexxInstructionGuard::execute(
    RexxActivation      *context,      /* current activation context        */
    RexxExpressionStack *stack )       /* evaluation stack                  */
/****************************************************************************/
/* Function:  Execute a REXX GUARD instruction                              */
/****************************************************************************/
{
  LONG        size;                    /* size of guard variables list      */
  LONG        i;                       /* loop counter                      */
  RexxObject *result;                  /* guard expression result           */

#ifdef NOTHREADSUPPORT
   report_exception1(Error_Execution_no_concurrency,
                  new_cstring("Concurrency not supported"));
#else

  context->traceInstruction(this);     /* trace if necessary                */
  if (!context->inMethod())            /* is this a method clause?          */
                                       /* raise an error                    */
    report_exception(Error_Translation_guard_guard);
                                       /* non-expression form?              */
  else if (this->expression == OREF_NULL) {

    if (!(i_flags&guard_on_form))      /* is this the OFF form?             */
      context->guardOff();             /* set unguarded status in activation*/
    else
      context->guardOn();              /* set guarded status in activation  */
  }
  else {
    size = guard_variable_count;       /* get variable list count           */
    for (i = 0; i < size; i++) {       /* loop through the variable list    */
                                       /* set a guard on each variable,     */
                                       /* counting the guards on each       */
                                       /* variable that is actually exposed */
      this->variables[i]->setGuard(context);
    }

    if (!(i_flags&guard_on_form))      /* is this the OFF form?             */
      context->guardOff();             /* set unguarded status in activation*/
    else
      context->guardOn();              /* set guarded status in activation  */

    CurrentActivity->guardSet();       /* initialize the guard sem          */
                                       /* get the expression value          */
    result = this->expression->evaluate(context, stack);
    context->traceResult(result);      /* trace if necessary                */
                                       /* do first evaluation without       */
                                       /* establishing any variable guards  */
                                       /* false on first attempt?           */
    if (!result->truthValue(Error_Logical_value_guard)) {
      do {                             /* need to loop until true           */
        stack->clear();                /* clear the expression stack        */

#ifdef NEWGUARD
		/* this code doesn't wait for the kernel or the scope so I can check */
		/* the result at once (no other threads can change it before) 		 */
        i = context->guardWait();       /* establish guards and wait         */
        result = this->expression->evaluate(context, stack);
		/* I checked the result, so get the kernel and the scope now */
		RequestKernelAccess(context->activity);
		context->guardWaitScope(i);
        CurrentActivity->guardSet();   /* initialize the guard sem          */
#else
        context->guardWait();       /* establish guards and wait         */
        CurrentActivity->guardSet();   /* initialize the guard sem          */
        result = this->expression->evaluate(context, stack);
#endif
        context->traceResult(result);  /* trace if necessary                */
                                       /* while this is still false         */
      } while (!result->truthValue(Error_Logical_value_guard));
    }
    for (i = 0; i < size; i++) {       /* loop through the variable list    */
                                       /* set a guard on each variable,     */
                                       /* counting the guards on each       */
                                       /* variable that is actually exposed */
      this->variables[i]->clearGuard(context);
    }
  }
#endif                                 // end of NOTHREADSUPPORT
}

void RexxInstructionGuard::live()
/******************************************************************************/
/* Function:  Normal garbage collection live marking                          */
/******************************************************************************/
{
  INT  i;                              /* loop counter                      */
  INT  count;                          /* argument count                    */

  setUpMemoryMark
  memory_mark(this->nextInstruction);  /* must be first one marked          */
  for (i = 0, count = guard_variable_count; i < count; i++)
    memory_mark(this->variables[i]);
  memory_mark(this->expression);
  cleanUpMemoryMark
}



void RexxInstructionGuard::liveGeneral()
/******************************************************************************/
/* Function:  Generalized object marking                                      */
/******************************************************************************/
{
  INT  i;                              /* loop counter                      */
  INT  count;                          /* argument count                    */

  setUpMemoryMarkGeneral
                                       /* must be first one marked          */
  memory_mark_general(this->nextInstruction);
  memory_mark_general(this->expression);
  for (i = 0, count = guard_variable_count; i < count; i++)
    memory_mark_general(this->variables[i]);
  cleanUpMemoryMarkGeneral
}

void RexxInstructionGuard::flatten(RexxEnvelope *envelope)
/******************************************************************************/
/* Function:  Flatten an object                                               */
/******************************************************************************/
{
  INT  i;                              /* loop counter                      */
  INT  count;                          /* argument count                    */

  setUpFlatten(RexxInstructionGuard)

  flatten_reference(newThis->nextInstruction, envelope);
  flatten_reference(newThis->expression, envelope);
  for (i = 0, count = guard_variable_count; i < count; i++)
    flatten_reference(newThis->variables[i], envelope);

  cleanUpFlatten
}

