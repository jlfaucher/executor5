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
/* REXX Translator                                              ExpressionCompoundVariable.c     */
/*                                                                            */
/* Primitive Translator Expression Parsing Compound Variable Reference Class  */
/*                                                                            */
/******************************************************************************/
#include <stdlib.h>
#include "RexxCore.h"
#include "StringClass.hpp"
#include "QueueClass.hpp"
#include "StemClass.hpp"
#include "RexxVariableDictionary.hpp"
#include "RexxActivation.hpp"
#include "RexxActivity.hpp"
#include "ExpressionCompoundVariable.hpp"
#include "ExpressionVariable.hpp"
#include "RexxVariable.hpp"

RexxCompoundVariable::RexxCompoundVariable(
    RexxString * stem,                 /* stem retriever                    */
    LONG         stemIndex,            /* stem lookaside index              */
    RexxQueue  * tails,                /* list of tails                     */
    LONG         TailCount)            /* count of tails                    */
/******************************************************************************/
/* Function:  Complete compound variable initialization                       */
/******************************************************************************/
{
  ClearObject(this);                   /* initialize the object             */
  this->tailCount= TailCount;          /* set the count (and hash value)    */
  OrefSet(this, this->stem, stem);     /* save the associate value          */
  this->index = stemIndex;             /* set the stem index                */

  while (TailCount > 0) {              /* loop through the variable list    */
                                       /* copying each variable             */
    OrefSet(this, this->tails[--TailCount], tails->pop());
  }
}

RexxObject * build(
    RexxString * variable_name,         /* full variable name of compound    */
    BOOL direct )                       /* this is direct access             */
/******************************************************************************/
/* Function:  Build a dynamically created compound variable                   */
/******************************************************************************/
{
  RexxString *   stem;                 /* stem part of compound variable    */
  RexxString *   tail;                 /* tail section string value         */
  RexxQueue  *   tails;                /* tail elements                     */
  RexxObject *   tailPart;             /* tail element retriever            */
  INT     position;                    /* scan position within compound name*/
  INT     start;                       /* starting scan position            */
  INT     length;                      /* length of tail section            */

  length = variable_name->length;      /* get the string length             */
  position = 0;                        /* start scanning at first character */
                                       /* scan to the first period          */
  while (variable_name->getChar(position) != '.') {
    position++;                        /* step to the next character        */
    length--;                          /* reduce the length also            */
  }
                                       /* extract the stem part             */
  stem = variable_name->extract(0, position + 1);
  save(stem);                          /* lock the stem part                */
                                       /* processing to decompose the name  */
                                       /* into its component parts          */

  tails = new_queue();                 /* get a new list for the tails      */
  save(tails);                         /* protect the stem name             */
  position++;                          /* step past previous period         */
  length--;                            /* adjust the length                 */
  if (direct == TRUE) {                /* direct access?                    */
                                       /* extract the tail part             */
    tail = variable_name->extract(position, length);
    tails->push(tail);                 /* add to the tail piece list        */
  }
  else {
    while (length > 0) {               /* process rest of the variable      */
      start = position;                /* save the start position           */
                                       /* scan for the next period          */
      while (length > 0 && variable_name->getChar(position) != '.') {
        position++;                    /* step to the next character        */
        length--;                      /* reduce the length also            */
      }
                                       /* extract the tail part             */
      tail = variable_name->extract(start, position - start);
                                       /* have a null tail piece or         */
                                       /* section begin with a digit?       */
      /* CHM - defect 87: change index start to 0 and compare for range     */
      /* ASCII '0' to '9' to recognize a digit                              */
      if (tail->length == 0 || (tail->getChar(0) >= '0' && tail->getChar(0) <= '9'))
        tailPart = (RexxObject *)tail; /* this is a literal piece           */
      else {
                                       /* create a new variable retriever   */
        tailPart = (RexxObject *)new RexxParseVariable(tail, 0);
      }
      tails->push(tailPart);           /* add to the tail piece list        */
      position++;                      /* step past previous period         */
      length--;                        /* adjust the length                 */
    }
                                       /* have a trailing period?           */
    if (variable_name->getChar(position - 1) == '.')
      tails->push(OREF_NULLSTRING);    /* add to the tail piece list        */
  }
  discard_hold(stem);                  /* release the stem                  */
  discard_hold(tails);                 /* and the tails                     */
                                       /* create and return a new compound  */
  return (RexxObject *)new (tails->getSize()) RexxCompoundVariable(stem, 0, tails, tails->getSize());
}

void RexxCompoundVariable::live()
/******************************************************************************/
/* Function:  Normal garbage collection live marking                          */
/******************************************************************************/
{
  INT  i;                              /* loop counter                      */
  INT  count;                          /* argument count                    */

  setUpMemoryMark
  for (i = 0, count = this->tailCount; i < count; i++)
    memory_mark(this->tails[i]);
  memory_mark(this->stem);
  cleanUpMemoryMark
}

void RexxCompoundVariable::liveGeneral()
/******************************************************************************/
/* Function:  Generalized object marking                                      */
/******************************************************************************/
{
  INT  i;                              /* loop counter                      */
  INT  count;                          /* argument count                    */

  setUpMemoryMarkGeneral
  for (i = 0, count = this->tailCount; i < count; i++)
    memory_mark_general(this->tails[i]);
  memory_mark_general(this->stem);
  cleanUpMemoryMarkGeneral
}

void RexxCompoundVariable::flatten(RexxEnvelope *envelope)
/******************************************************************************/
/* Function:  Flatten an object                                               */
/******************************************************************************/
{

  INT  i;                              /* loop counter                      */
  INT  count;                          /* argument count                    */

  setUpFlatten(RexxCompoundVariable)

  flatten_reference(newThis->stem, envelope);
  for (i = 0, count = this->tailCount; i < count; i++)
    flatten_reference(newThis->tails[i], envelope);

  cleanUpFlatten
}

RexxObject * RexxCompoundVariable::evaluate(
    RexxActivation      *context,      /* current activation context        */
    RexxExpressionStack *stack )       /* evaluation stack                  */
/******************************************************************************/
/* Function:  Evaluate a REXX compound variable                               */
/******************************************************************************/
{
  RexxObject   *value;                 /* final variable value              */

                                       /* and ask it for the value          */
  value = context->evaluateLocalCompoundVariable(stem, index, &tails[0], tailCount);

  stack->push(value);                  /* place on the evaluation stack     */
  return value;                        /* return the located variable       */
}

RexxObject  *RexxCompoundVariable::getValue(
  RexxVariableDictionary *dictionary)  /* current activation dictionary     */
/******************************************************************************/
/* Function:  Direct value retrieve of a compound variable                    */
/******************************************************************************/
{
                                       /* resolve the tail element          */
  return dictionary->getCompoundVariableValue(stem, &tails[0], tailCount);
}

RexxObject  *RexxCompoundVariable::getValue(
  RexxActivation *context)             /* current activation dictionary     */
/******************************************************************************/
/* Function:  Direct value retrieve of a compound variable                    */
/******************************************************************************/
{
                                       /* resolve the tail element          */
  return context->getLocalCompoundVariableValue(stem, index, &tails[0], tailCount);
}


void RexxCompoundVariable::set(
  RexxActivation *context,             /* current activation context        */
  RexxObject *value )                  /* new value to assign               */
/******************************************************************************/
/* Function:  Assign a new value to a compound variable                       */
/******************************************************************************/
{
                                       /* the dictionary manages all of these details */
  context->setLocalCompoundVariable(stem, index, &tails[0], tailCount, value);
}


void RexxCompoundVariable::set(
  RexxVariableDictionary *dictionary,  /* an object variable dictionary     */
  RexxObject *value )                  /* new value to assign               */
/******************************************************************************/
/* Function:  Assign a new value to a compound variable                       */
/******************************************************************************/
{
                                       /* the dictionary manages all of these details */
  dictionary->setCompoundVariable(stem, &tails[0], tailCount, value);
}


BOOL RexxCompoundVariable::exists(
  RexxActivation *context)             /* current execution context         */
/******************************************************************************/
/* Function:  Check to see if a compound variable exists in a directory       */
/******************************************************************************/
{
                                       /* retrieve the variable value, and  */
                                       /* see it really exists              */
  return context->localCompoundVariableExists(stem, index, &tails[0], tailCount);
}

void RexxCompoundVariable::assign(
    RexxActivation *context,           /* current activation context        */
    RexxObject     *value )            /* new value to assign               */
/******************************************************************************/
/* Function:  Assign a value to a compound variable                           */
/******************************************************************************/
{
    /* the context manages the assignment details */
    context->assignLocalCompoundVariable(stem, index, (RexxObject **)&tails[0], tailCount, value);
}

void RexxCompoundVariable::drop(
    RexxActivation *context)           /* current activation context        */
/******************************************************************************/
/* Function:  Drop a compound variable                                        */
/******************************************************************************/
{
                                       /* set the compound value            */
  context->dropLocalCompoundVariable(stem, index, &tails[0], tailCount);
}


void RexxCompoundVariable::procedureExpose(
  RexxActivation      *context,        /* current activation context        */
  RexxActivation      *parent,         /* the parent activation context     */
  RexxExpressionStack *stack)          /* current evaluation stack          */
/******************************************************************************/
/* Function:  Expose a compound variable                                      */
/******************************************************************************/
{
    /* first get (and possible create) the compound variable in the */
    /* parent context. */
    RexxCompoundElement *variable = parent->exposeLocalCompoundVariable(stem, index, (RexxObject **)&tails[0], tailCount);
    /* get the stem index from the current level.  This may end up */
    /* creating the stem that holds the exposed value. */
    RexxStem *stem_table = context->getLocalStem(stem, index);
    /* have the stem expose this */
    stem_table->expose(variable);
    /* trace resolved compound name */
    context->traceCompoundName(stem, variable->getName());
}


void RexxCompoundVariable::expose(
  RexxActivation      *context,        /* current activation context        */
  RexxExpressionStack *stack,          /* current evaluation stack          */
                                       /* variable scope we're exposing from*/
  RexxVariableDictionary *object_dictionary)
/******************************************************************************/
/* Function:  Expose a compound variable                                      */
/******************************************************************************/
{
    /* get the stem in the source dictionary */
    RexxStem *source_stem = object_dictionary->getStem(stem);
                                          /* new tail for compound             */
    RexxCompoundTail resolved_tail(context, &tails[0], tailCount);
    /* first get (and possible create) the compound variable in the */
    /* object context. */
    RexxCompoundElement *variable = source_stem->exposeCompoundVariable(&resolved_tail);
    /* get the stem index from the current level.  This may end up */
    /* creating the stem that holds the exposed value. */
    RexxStem *stem_table = context->getLocalStem(stem, index);
    /* have the stem expose this */
    stem_table->expose(variable);
    /* tracing intermediate values?      */
    if (context->tracingIntermediates()) {
        /* trace resolved compound name */
        context->traceCompoundName(stem, variable->getName());
    }
}


void RexxCompoundVariable::setGuard(
  RexxActivation *context )            /* current activation context        */
/******************************************************************************/
/* Function:  Set a guard wait in a compound variable                         */
/******************************************************************************/
{
  RexxCompoundElement *variable;       /* compound variable object          */

                                       /* get the variable item             */
  variable = context->getLocalCompoundVariable(stem, index, &tails[0], tailCount);
  variable->inform(CurrentActivity);   /* mark the variable entry           */
}

void RexxCompoundVariable::clearGuard(
  RexxActivation *context )            /* current activation context        */
/******************************************************************************/
/* Function:  Clear a guard wait on a compound variable                       */
/******************************************************************************/
{
  RexxCompoundElement *variable;       /* compound variable object          */

                                       /* get the variable item             */
  variable = context->getLocalCompoundVariable(stem, index, &tails[0], tailCount);
  variable->uninform(CurrentActivity); /* mark the variable entry           */
}

void * RexxCompoundVariable::operator new(size_t size,
    LONG tailCount)                    /* count of tails                    */
/******************************************************************************/
/* Function:  Create a new compound variable object                           */
/******************************************************************************/
{
  RexxObject * newObject;              /* newly created object              */
                                       /* Get new object                    */
  newObject = new_object(size + ((tailCount - 1) * sizeof(RexxObject *)));
                                       /* Give new object its behaviour     */
  BehaviourSet(newObject, TheCompoundVariableBehaviour);
  return newObject;                    /* return the new compound variable  */
}

