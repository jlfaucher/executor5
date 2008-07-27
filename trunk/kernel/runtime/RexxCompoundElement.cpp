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
/****************************************************************************/
/* REXX Kernel                                                  RexxCompoundElement.c    */
/*                                                                          */
/* Primitive Compound Variable Element Class                                */
/*                                                                          */
/****************************************************************************/
#include "RexxCore.h"
#include "RexxCompoundElement.hpp"
#include "RexxActivity.hpp"

void RexxCompoundElement::live()
/******************************************************************************/
/* Function:  Normal garbage collection live marking                          */
/******************************************************************************/
{
  setUpMemoryMark
  memory_mark(this->variableValue);
  memory_mark(this->variable_name);
  memory_mark(this->dependents);
  memory_mark(this->parent);
  memory_mark(this->left);
  memory_mark(this->right);
  memory_mark(this->real_element);
  cleanUpMemoryMark
}

void RexxCompoundElement::liveGeneral()
/******************************************************************************/
/* Function:  Normal garbage collection live marking                          */
/******************************************************************************/
{
  setUpMemoryMarkGeneral
  memory_mark_general(this->variableValue);
  memory_mark_general(this->variable_name);
  memory_mark_general(this->dependents);
  memory_mark_general(this->parent);
  memory_mark_general(this->left);
  memory_mark_general(this->right);
  memory_mark_general(this->real_element);
  cleanUpMemoryMarkGeneral
}

void RexxCompoundElement::flatten(RexxEnvelope *envelope)
/******************************************************************************/
/* Function:  Flatten an object                                               */
/******************************************************************************/
{
  setUpFlatten(RexxCompoundElement)

   flatten_reference(newThis->variableValue, envelope);
   flatten_reference(newThis->variable_name, envelope);
   flatten_reference(newThis->dependents, envelope);
   flatten_reference(newThis->parent, envelope);
   flatten_reference(newThis->left, envelope);
   flatten_reference(newThis->right, envelope);
   flatten_reference(newThis->real_element, envelope);

  cleanUpFlatten
}


RexxCompoundElement *RexxMemory::newCompoundElement(
    RexxString *name)                  /* the name of the variable          */
/****************************************************************************/
/* Function:  Create a new REXX compound variable object                    */
/****************************************************************************/
{
                                       /* created variable object           */
  RexxCompoundElement *newObject;

                                       /* Get new object                    */
  newObject = (RexxCompoundElement *)new_object(sizeof(RexxCompoundElement));
                                       /* Give new object its behaviour     */
  BehaviourSet(newObject, TheCompoundElementBehaviour);
                                       /* set the virtual function table    */
  setVirtualFunctions(newObject, T_compound_element);
  /* Explicitly clear out the variable value, which is not cleared */
  /* by ClearObject call.  Note that we don't use OREF_SET here, as */
  /* this field is likely to be garbage. */
  newObject->variableValue = OREF_NULL;
  ClearObject(newObject);              /* clear everything out              */
  newObject->variable_name = name;     /* fill in the name                  */

  return newObject;                    /* return the new object             */
}
