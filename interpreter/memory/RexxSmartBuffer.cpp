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
/* REXX Kernel                                           RexxSmartBuffer.c    */
/*                                                                            */
/* Primitive Smart Buffer Class                                               */
/*                                                                            */
/******************************************************************************/
#include <stdlib.h>
#include <string.h>
#include "RexxCore.h"
#include "BufferClass.hpp"
#include "RexxSmartBuffer.hpp"

RexxSmartBuffer::RexxSmartBuffer(size_t startSize)
/******************************************************************************/
/* Function:  Initialize a smart buffer object                                */
/******************************************************************************/
{
  this->clearObject();                 /* start out fresh                   */
                                       /* default initial buffersize, 1 page*/
                                       /* for buffer + data.                */
  OrefSet(this, this->buffer, (RexxBuffer *)new_buffer(startSize));
}

void RexxSmartBuffer::live(size_t liveMark)
/******************************************************************************/
/* Function:  Normal garbage collection live marking                          */
/******************************************************************************/
{
  memory_mark(this->buffer);
}
void RexxSmartBuffer::liveGeneral(int reason)
/******************************************************************************/
/* Function:  Generalized object marking                                      */
/******************************************************************************/
{
  memory_mark_general(this->buffer);
}

void RexxSmartBuffer::flatten(RexxEnvelope *envelope)
/******************************************************************************/
/* Function:  Flatten an object                                               */
/******************************************************************************/
{
  setUpFlatten(RexxSmartBuffer)

   flatten_reference(newThis->buffer, envelope);

  cleanUpFlatten
}

size_t RexxSmartBuffer::copyData(
    void *start,                       /* data to be copied                 */
    size_t length)                     /* length to copy                    */
/******************************************************************************/
/* Function:  Copy data into the buffer at the very end.                      */
/******************************************************************************/
{
    if (this->space() < length)          /* enough room in buffer for data?   */
    {
        /* not big enough, tell buffer to    */
        /* make itself bigger, and assign    */
        /* new buffer                        */
        OrefSet(this, this->buffer, this->buffer->expand(length));
    }
    this->buffer->copyData(this->current, (char *)start, length);

    size_t dataLoc = this->current;      /* save start location of copied data*/
                                         /* bump pointer to end of copied     */
                                         /* data, (prepare for next copy)     */
    this->current = this->current + length;
    return dataLoc;                      /* return location of copied data    */
}

void   *RexxSmartBuffer::operator new(size_t size)
/******************************************************************************/
/* Function:  Create a new translator object                                  */
/******************************************************************************/
{
    return new_object(size, T_SmartBuffer);        /* get storage for a new object      */
}

size_t RexxSmartBuffer::space()
/******************************************************************************/
/* Function:  Return the space remaining in the buffer                        */
/******************************************************************************/
{
  return this->buffer->getLength() - this->current;
}

