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
/*****************************************************************************/
/* REXX Windows Support                                                      */
/*                                                                           */
/* Main Windows interpreter control.  This is the preferred location for     */
/* all platform dependent global variables.                                  */
/* The interpreter does not instantiate an instance of this                  */
/* class, so most variables and methods should be static.                    */
/*                                                                           */
/*                                                                           */
/*****************************************************************************/

#include "RexxCore.h"
#include "SystemInterpreter.hpp"
#include "Interpreter.hpp"

#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <io.h>

class InterpreterInstance;


HINSTANCE SystemInterpreter::moduleHandle = 0;      // handle to the interpeter DLL

void SystemInterpreter::processStartup(HINSTANCE mod)
{
    moduleHandle = mod;
    // startup timeslice processing
    startTimeSlice();
    // now do the platform independent startup
    Interpreter::processStartup();
}


void SystemInterpreter::processShutdown()
{
    stopTimeSlice();              // shutdown the timer thread
    // now do the platform independent shutdown
    Interpreter::processShutdown();
}


void SystemInterpreter::startInterpreter()
{
}


void SystemInterpreter::terminateInterpreter()
{
}



void SystemInterpreter::live(size_t liveMark)
{
}

void SystemInterpreter::liveGeneral(int reason)
{
  if (!memoryObject.savingImage())
  {
  }
}


/**
 * Load a message from the system message resources.
 *
 * @param code   The error message identifier.
 * @param buffer The buffer for the returned message.
 * @param bufferLength
 *               The length of th message buffer.
 *
 * @return The success/failure indicator.
 */
bool SystemInterpreter::loadMessage(wholenumber_t code, char *buffer, size_t bufferLength)
{
    return LoadString(moduleHandle, (UINT)code, buffer, (int)bufferLength) != 0;
}

//TODO:  Add a system field in the instance class

void SystemInterpreter::initializeInstance(InterpreterInstance *instance)
{
    /* Because of using the stand-alone runtime library or when using different compilers,
       the std-streams of the calling program and the REXX.DLL might be located at different
       addresses and therefore _file might be -1. If so, std-streams are reassigned to the
       file standard handles returned by the system */
    if ((stdin->_file == -1) && (GetFileType(GetStdHandle(STD_INPUT_HANDLE)) != FILE_TYPE_UNKNOWN))
        *stdin = *_fdopen(_open_osfhandle((intptr_t)GetStdHandle(STD_INPUT_HANDLE),_O_RDONLY), "r");
    if ((stdout->_file == -1) && (GetFileType(GetStdHandle(STD_OUTPUT_HANDLE)) != FILE_TYPE_UNKNOWN))
        *stdout = *_fdopen(_open_osfhandle((intptr_t)GetStdHandle(STD_OUTPUT_HANDLE),_O_APPEND), "a");
    if ((stderr->_file == -1) && (GetFileType(GetStdHandle(STD_ERROR_HANDLE)) != FILE_TYPE_UNKNOWN))
        *stderr = *_fdopen(_open_osfhandle((intptr_t)GetStdHandle(STD_ERROR_HANDLE),_O_APPEND), "a");
}


void SystemInterpreter::terminateInstance(InterpreterInstance *instance)
{
}

