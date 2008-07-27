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
/* REXX Kernel                                      MessageInstruction.hpp    */
/*                                                                            */
/* Primitive Parse Message Instruction Class Definitions                      */
/*                                                                            */
/******************************************************************************/
#ifndef Included_RexxInstructionMessage
#define Included_RexxInstructionMessage

#include "RexxInstruction.hpp"

#define message_i_double  0x01         /* message is the double form        */

class RexxInstructionMessage : public RexxInstruction {
 public:
  inline void *operator new(size_t size, void *ptr) {return ptr;}
  inline void operator delete(void *) { }
  inline void operator delete(void *, void *) { }

  RexxInstructionMessage(RexxExpressionMessage *);
  RexxInstructionMessage(RexxExpressionMessage *, RexxObject *);
  inline RexxInstructionMessage(RESTORETYPE restoreType) { ; };
  void live(size_t);
  void liveGeneral(int reason);
  void flatten(RexxEnvelope *);
  void execute(RexxActivation *, RexxExpressionStack *);

  RexxString *name;                    /* name of the message               */
  RexxObject *target;                  /* target subexpression              */
  RexxObject *super;                   /* super class target                */
  size_t      argumentCount;           // number of arguments
  RexxObject *arguments[1];            /* list of argument subexpressions   */
};
#endif
