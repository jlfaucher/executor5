/*----------------------------------------------------------------------------*/
/*                                                                            */
/* Copyright (c) 1995, 2004 IBM Corporation. All rights reserved.             */
/* Copyright (c) 2005-2014 Rexx Language Association. All rights reserved.    */
/*                                                                            */
/* This program and the accompanying materials are made available under       */
/* the terms of the Common Public License v1.0 which accompanies this         */
/* distribution. A copy is also available at the following address:           */
/* http://www.oorexx.org/license.html                                         */
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
#ifndef ProgramMetaData_Included
#define ProgramMetaData_Included

#include "SystemInterpreter.hpp"
#include "LanguageLevel.hpp"

class BufferClass;

class ProgramMetaData
{
public:
    void *operator new (size_t size, BufferClass *buff);
    void operator delete (void *p) { SystemInterpreter::releaseResultMemory(p); }

    ProgramMetaData(LanguageLevel, BufferClass *);
    ProgramMetaData(LanguageLevel, size_t);

    size_t getDataSize();
    size_t getHeaderSize();
    BufferClass *extractBufferData();
    char *getImageData();
    size_t getImageSize() { return imageSize; }
    bool validate(bool &);
    void write(FILE *handle, BufferClass *program);
    BufferClass *read(RexxString *name, FILE *handle);

protected:
    enum
    {
        MAGICNUMBER = 11111,           // remains constant from release-to-release
        METAVERSION = 41               // gets updated when internal form changes
    };


    char fileTag[16];                  // special header for file tagging
    unsigned short magicNumber;        // special tag to indicate good meta data
    unsigned short imageVersion;       // version identifier for validity
    unsigned short wordSize;           // size of a word
    unsigned short bigEndian;          // true if this is a big-endian platform
    LanguageLevel requiredLevel;       // required language level for execution
    size_t         imageSize;          // size of the image
    char           imageData[4];       // the copied image data
};

#endif
