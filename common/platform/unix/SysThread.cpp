/*----------------------------------------------------------------------------*/
/*                                                                            */
/* Copyright (c) 1995, 2004 IBM Corporation. All rights reserved.             */
/* Copyright (c) 2005-2017 Rexx Language Association. All rights reserved.    */
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
/******************************************************************************/
/* REXX Kernel                                              SysThread.cpp     */
/*                                                                            */
/* Unix implementation of the SysThread class.                                */
/*                                                                            */
/******************************************************************************/

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "SysThread.hpp"
#include <stdio.h>
#include <unistd.h>

// attach an activity to an existing thread
void SysThread::attachThread()
{
    // initialize the thread basics
    _threadID = pthread_self();
    attached = true;      // we didn't create this one
}


void SysThread::setPriority(int priority)
{
    int schedpolicy;
    struct sched_param schedparam;

    pthread_getschedparam(_threadID,  &schedpolicy, &schedparam);

    /* Medium_priority(=100) is used for every new thread */
    schedparam.sched_priority = priority;
    pthread_setschedparam(_threadID, schedpolicy, &schedparam);
}


void SysThread::dispatch()
{
    // default dispatch returns immediately
}


char *SysThread::getStackBase()
{
   int32_t temp;
#pragma GCC diagnostic push
// avoid CLANG warning: address of stack memory associated with local variable returned
#pragma clang diagnostic ignored "-Wreturn-stack-address"
// avoid CLANG warning: unknown warning group '-Wreturn-local-addr', ignored
#pragma clang diagnostic ignored "-Wunknown-pragmas"
// avoid GCC warning: function returns address of local variable
#pragma GCC diagnostic ignored "-Wreturn-local-addr"
   return ((char *)(&temp)) - THREAD_STACK_SIZE;
#pragma GCC diagnostic pop
}


void SysThread::terminate()
{
    if (!attached && _threadID != 0)
    {
        pthread_detach(_threadID);
        _threadID = 0;
    }
}


void SysThread::startup()
{
    // this is a nop on Unix.
}


void SysThread::shutdown()
{
    // this is a nop on Unix.
}


void SysThread::yield()
{
    sched_yield();
}


bool SysThread::equals(SysThread &other)
{
    return pthread_equal(_threadID, other._threadID);
}


static void * call_thread_function(void *argument)
{
    ((SysThread *)argument)->dispatch();
    return NULL;
}


// create a new thread and attach to an activity
void SysThread::createThread()
{
    pthread_attr_t  newThreadAttr;
    int schedpolicy, maxpri, minpri;
    struct sched_param schedparam;

    attached = false;           // we own this thread

    // Create an attr block for Thread.
    pthread_attr_init(&newThreadAttr);
#if defined(LINUX) ||  defined(OPSYS_SUN) || defined(AIX)
    /* scheduling on two threads controlled by the result method of the */
    /* message object do not work properly without an enhanced priority */
    pthread_getschedparam(pthread_self(), &schedpolicy, &schedparam);

#if defined(AIX)
  // Starting with AIX 5.3 the priority for a thread created by
  // a non root user can not be higher then 59. The priority
  // of a user prog should not be higher then 59 (IBM AIX development).
  schedparam.sched_priority = 59;
#else
#   ifdef _POSIX_PRIORITY_SCHEDULING
        maxpri = sched_get_priority_max(schedpolicy);
        minpri = sched_get_priority_min(schedpolicy);
        schedparam.sched_priority = (minpri + maxpri) / 2;
#   endif
#endif

#if defined(OPSYS_SUN)
    /* PTHREAD_EXPLICIT_SCHED ==> use scheduling attributes of the new object */
    pthread_attr_setinheritsched(&newThreadAttr, PTHREAD_EXPLICIT_SCHED);

    /* Performance measurements show massive performance improvements > 50 % */
    /* using Round Robin scheduling instead of FIFO scheduling               */
    pthread_attr_setschedpolicy(&newThreadAttr, SCHED_RR);
#endif

#if defined(AIX)
    /* PTHREAD_EXPLICIT_SCHED ==> use scheduling attributes of the new object */
    pthread_attr_setinheritsched(&newThreadAttr, PTHREAD_EXPLICIT_SCHED);

    /* Each thread has an initial priority that is dynamically modified by   */
    /* the scheduler, according to the thread's activity; thread execution   */
    /* is time-sliced. On other systems, this scheduling policy may be       */
    /* different.                                                            */
    pthread_attr_setschedpolicy(&newThreadAttr, SCHED_OTHER);
#endif

    pthread_attr_setschedparam(&newThreadAttr, &schedparam);
#endif

    // Set the stack size.
    pthread_attr_setstacksize(&newThreadAttr, THREAD_STACK_SIZE);

    // Now create the thread
    int rc = pthread_create(&_threadID, &newThreadAttr, call_thread_function, (void *)this);
    if (rc != 0)
    {
        _threadID = 0;
        fprintf(stderr," *** ERROR: At SysThread(), createThread - RC = %d !\n", rc);
    }
    pthread_attr_destroy(&newThreadAttr);
    return;
}


// wait for the thread to terminatre
void SysThread::waitForTermination()
{
    if (!attached && _threadID != 0)
    {
        void *res;
        pthread_join(_threadID, &res);
        _threadID = 0;
    }
}

