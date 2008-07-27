/*----------------------------------------------------------------------------*/
/*                                                                            */
/* Copyright (c) 1995, 2004 IBM Corporation. All rights reserved.             */
/* Copyright (c) 2005-2006 Rexx Language Association. All rights reserved.    */
/*                                                                            */
/* This program and the accompanying materials are made available under       */
/* the terms of the Common Public License v1.0 which accompanies this         */
/* distribution. A copy is also available at the following address:           */
/* http://www.ibm.com/developerworks/oss/CPLv1.0.htm                          */
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
/* REXX Kernel                                             RexxContext.hpp    */
/*                                                                            */
/* Primitive Rexx execution context definition                                */
/*                                                                            */
/******************************************************************************/
#ifndef Included_RexxContext
#define Included_RexxContext

#include "ObjectClass.hpp"

class RexxActivation;
class PackageClass;


class RexxContext : public RexxObject
{
public:
    inline void *operator new(size_t, void *ptr) { return ptr; }
    inline void  operator delete(void *, void *) { ; }
    void *operator new(size_t);
    inline void  operator delete(void *) { ; }

    void live(size_t);
    void liveGeneral(int reason);
    void flatten(RexxEnvelope*);

    RexxContext(RexxActivation *);
    inline RexxContext(RESTORETYPE restoreType) { ; };

    PackageClass *getPackage();
    RexxObject *getDigits();
    RexxObject *getFuzz();
    RexxObject *getForm();
    RexxObject *getVariables();
    RexxObject *getExecutable();
    RexxObject *getArgs();
    RexxObject *getCondition();
    RexxObject *copyRexx();

    RexxObject *newRexx(RexxObject **args, size_t argc);

    inline void detach() { activation = OREF_NULL; }

    static void createInstance();
    static RexxClass *classInstance;   // singleton class instance

protected:
    void checkValid();

    RexxActivation *activation;
};


#endif

