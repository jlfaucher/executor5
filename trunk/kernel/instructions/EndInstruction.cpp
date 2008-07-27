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
/* REXX Translator                                              EndInstruction.c      */
/*                                                                            */
/* Primitive End Parse Class                                                  */
/*                                                                            */
/******************************************************************************/
#include <stdlib.h>
#include "RexxCore.h"
#include "StringClass.hpp"
#include "RexxActivation.hpp"
#include "EndInstruction.hpp"
#include "DoInstruction.hpp"
#include "DoBlock.hpp"

RexxInstructionEnd::RexxInstructionEnd(
    RexxString *name)                  /* The END instruction name          */
/****************************************************************************/
/* Function:  Set the name of an END instruction                            */
/****************************************************************************/
{
  OrefSet(this, this->name, name);
}

void RexxInstructionEnd::live()
/******************************************************************************/
/* Function:  Normal garbage collection live marking                          */
/******************************************************************************/
{
  setUpMemoryMark
  memory_mark(this->nextInstruction);  /* must be first one marked          */
  memory_mark(this->name);
  cleanUpMemoryMark
}

void RexxInstructionEnd::liveGeneral()
/******************************************************************************/
/* Function:  Generalized object marking                                      */
/******************************************************************************/
{
  setUpMemoryMarkGeneral
                                       /* must be first one marked          */
  memory_mark_general(this->nextInstruction);
  memory_mark_general(this->name);
  cleanUpMemoryMarkGeneral
}

void RexxInstructionEnd::flatten(RexxEnvelope *envelope)
/******************************************************************************/
/* Function:  Flatten an object                                               */
/******************************************************************************/
{
  setUpFlatten(RexxInstructionEnd)

  flatten_reference(newThis->nextInstruction, envelope);
  flatten_reference(newThis->name, envelope);

  cleanUpFlatten
}

void RexxInstructionEnd::execute(
    RexxActivation      *context,      /* current activation context        */
    RexxExpressionStack *stack )       /* evaluation stack                  */
/****************************************************************************/
/* Function:  Execute a REXX END instruction                                */
/****************************************************************************/
{
  RexxDoBlock       *doBlock;          /* target DO block                   */

  context->unindent();                 /* remove indentation                */
  context->traceInstruction(this);     /* trace if necessary                */
  if (context->blockNest == 0)         /* no possible blocks?               */
                                       /* this is an error                  */
    report_exception(Error_Unexpected_end_nodo);

  switch (this->getStyle()) {          /* process each loop type            */

    case LOOP_BLOCK:                   /* is this a loop?                   */
      doBlock = context->topBlock();   /* get the top DO block              */
                                       /* reset the indentation             */
      context->setIndent(doBlock->getIndent());
                                       /* pass on the reexecution           */
      doBlock->getParent()->reExecute(context, stack, doBlock);
      break;

    case SELECT_BLOCK:                 /* END of a select block             */
                                       /* looking for a WHEN match          */
                                       /* this is an error                  */
      report_exception(Error_When_expected_nootherwise);
      break;

    default:                           /* all others                        */
      context->removeBlock();          /* just step back next level         */
      break;
  }
}

