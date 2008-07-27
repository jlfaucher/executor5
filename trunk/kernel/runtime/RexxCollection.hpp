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
/* REXX Kernel                                                  RexxCollection.hpp    */
/*                                                                            */
/* Primitive HashTableCollection Class                                        */
/*                                                                            */
/******************************************************************************/
#ifndef Included_RexxHashTableCollection
#define Included_RexxHashTableCollection

#include "RexxHashTable.hpp"

 class RexxHashTableCollection : public RexxObject {
   public:
    void         live();
    void         liveGeneral();
    void         flatten(RexxEnvelope *);
    RexxObject * unflatten(RexxEnvelope *);
    RexxObject * makeProxy(RexxEnvelope *);
    RexxObject * copy();
    RexxArray  * makeArray();

    virtual RexxObject *mergeItem(RexxObject *, RexxObject *);

    RexxObject   *copyValues(long depth);
    RexxObject   *removeRexx(RexxObject *);
    RexxObject   *getRexx(RexxObject *);
    RexxObject   *put(RexxObject *, RexxObject *);
    RexxObject   *add(RexxObject *, RexxObject *);
    RexxObject   *allAt(RexxObject *);
    RexxObject   *hasIndex(RexxObject *);
    RexxSupplier *supplier();
    RexxObject   *merge(RexxHashTableCollection *);

    inline long          items() { return this->contents->totalEntries(); };
    inline long          first() { return this->contents->first(); };
    inline long          next(long pos) { return this->contents->next(pos);};
    inline RexxObject   *value(long pos) {return this->contents->value(pos); };
    inline RexxObject   *index(long pos) {return this->contents->index(pos); };
    inline BOOL          available(long pos) {return this->contents->available(pos); };
    inline RexxObject   *remove(RexxObject *key) {return this->contents->remove(key); };
    inline RexxObject   *get(RexxObject *key) {return this->contents->get(key); };

    RexxHashTable *contents;           /* collection associated hash table  */
 };
 #endif
