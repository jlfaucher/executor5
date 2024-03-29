/*----------------------------------------------------------------------------*/
/*                                                                            */
/* Copyright (c) 2005-2021 Rexx Language Association. All rights reserved.    */
/*                                                                            */
/* This program and the accompanying materials are made available under       */
/* the terms of the Common Public License v1.0 which accompanies this         */
/* distribution. A copy is also available at the following address:           */
/* https://www.oorexx.org/license.html                                        */
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
/* Manage system-specific aspects of an interpreter instance.                 */
/*                                                                            */
/******************************************************************************/
#ifndef Included_SysInterpreterInstance_hpp
#define Included_SysInterpreterInstance_hpp

#include "FileNameBuffer.hpp"

class InterpreterInstance;
class RexxActivation;

/**
 * A class type that can be embedded within an InterpreterInstance
 * object to hold platform-specific state for an instance.
 */
class SysInterpreterInstance
{
public:
    // this class is embedded within an InterpreterInstance class, so
    // these are not virtual
    void live(size_t);
    void liveGeneral(MarkReason);

    void initialize(InterpreterInstance *i, RexxOption *options);
    void terminate();
    void registerCommandHandlers(InterpreterInstance *i);
    void setupProgram(RexxActivation *activation);
    inline bool concurrencyTrace() { return concurrencyTraceEnabled; }
    inline void concurrencyTrace(bool b) { concurrencyTraceEnabled = b; }

protected:
    void addSearchExtension(const char *name);

    InterpreterInstance *instance;       // backlink to our instance container
    bool  externalTraceEnabled;   // are we started with external trace on
    bool  concurrencyTraceEnabled;   // are we started with concurrency trace on
};


class SysSearchPath
{
public:
    SysSearchPath(const char *parent, const char *extension);
    ~SysSearchPath() { }

    inline void addPath(const char *p)
    {
        // add path only if not an empty string
        if (p != NULL && *p != '\0')
        {
            path.appendPathSeparator();
            path += p;
        }
    }

    FileNameBuffer path;  // the constructed path
};


#endif
