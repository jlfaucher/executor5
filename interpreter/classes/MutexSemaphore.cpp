/*----------------------------------------------------------------------------*/
/*                                                                            */
/* Copyright (c) 1995, 2004 IBM Corporation. All rights reserved.             */
/* Copyright (c) 2005-2020 Rexx Language Association. All rights reserved.    */
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
/*                                                                            */
/* MutexSemaphore class                                                       */
/*                                                                            */
/******************************************************************************/
#include "RexxCore.h"
#include "MutexSemaphore.hpp"
#include "MethodArguments.hpp"
#include "ProtectedObject.hpp"
#include "ActivityManager.hpp"
#include "PackageClass.hpp"


RexxClass *MutexSemaphoreClass::classInstance = OREF_NULL;   // singleton class instance

/**
 * Perform class bootstrap activities.
 */
void MutexSemaphoreClass::createInstance()
{
    CLASS_CREATE(MutexSemaphore);
}

/**
 * Allocate memory for a mutex semaphore object.
 *
 * @param size   The size of the object.
 *
 * @return A new object configured for a mutex semaphore object.
 */
void* MutexSemaphoreClass::operator new(size_t size)
{
    RexxInternalObject *newObject = new_object(size, T_MutexSemaphore);
    return (void *)newObject;
}


/**
 * Default constructor for the MutexSemaphore
 */
MutexSemaphoreClass::MutexSemaphoreClass() : semaphore("MutexSemaphoreClass::semaphoe")
{
    // initialize the semaphore
    semaphore.create();
    nestCount = 0;
}


/**
 * Default new method for creating an object from Rexx
 * code.  This is the version inherited by all subclasses
 * of the Object class.
 *
 * @param arguments Pointer to the arguments of the new() method.  These
 *                  are passed on to the init method.
 * @param argCount  The count of the arguments.
 *
 * @return An initialized object instance.  If this is a subclass,
 *         the object will have all of the subclass behaviour.
 */
RexxObject *MutexSemaphoreClass::newRexx(RexxObject **arguments, size_t argCount)
{
    // this class is defined on the object class, but this is actually attached
    // to a class object instance.  Therefore, any use of the this pointer
    // will be touching the wrong data.  Use the classThis pointer for calling
    // any methods on this object from this method.
    RexxClass *classThis = (RexxClass *)this;

    Protected<RexxObject> newObj =  new MutexSemaphoreClass;

    // handle Rexx class completion
    classThis->completeNewObject(newObj, arguments, argCount);

    return newObj;
}


/**
 * Do cleanup of the semaphore object.
 *
 * @return Always returns NULL.
 */
RexxObject* MutexSemaphoreClass::close()
{
    semaphore.close();
    if (nestCount > 0)
    {
        // tell the current activity this is no longer needed
        ActivityManager::currentActivity->removeMutex(this);
    }
    nestCount = 0;
    return OREF_NULL;
}


/**
 * Handle nesting of the semaphore, including letting the current owner
 * know we are the current holders of the lock. If the activity terminates
 * with us holding the lock, this will be released during cleanup.
 */
void MutexSemaphoreClass::handleNesting()
{
    nestCount++;
    // if this is the first acquisition, let the current activity know we
    // are holding this.
    if (nestCount == 1)
    {
        // tell the current activity to register this mutex
        ActivityManager::currentActivity->addMutex(this);
    }
}


/**
 * Acquire the mutex.
 * @param t  An optional timeout in seconds, either String or TimeSpan.
 *           Returns immediately if the timeout is zero,
 *           waits indefinitely if it's negative.
 *           Default is -1.
 *
 * @return true if this was successfully acquired,
 *         false if we timed out.
 */
RexxObject* MutexSemaphoreClass::request(RexxObject *t, const char *ds, int di)
{
    wholenumber_t timeout;

    if (t == OREF_NULL)
    {
        timeout = -1;
    }
    else
    {
        RexxObject *seconds;

        // we allow timeout to be a TimeSpan or a String object
        RexxClass *TimeSpan = TheRexxPackage->findClass(GlobalNames::TIMESPAN);
        if (t->isInstanceOf(TimeSpan))
        {
            ProtectedObject p;
            seconds = t->sendMessage(GlobalNames::TOTALSECONDS, p);
        }
        else
        {
            seconds = t;
        }
        double secs = floatingPointArgument(seconds, "timeout");
        // wait() is uint32, so we can wait for 4294967 seconds at most
        if (secs >= 0 && secs <= 4294967)
        {
            // we need milliseconds (the product will always fit a unint32)
            timeout = (wholenumber_t)(secs * 1000.0);
        }
        else
        {
            // negative or too large, we just wait forever
            timeout = -1;
        }
    }

    bool acquired;
    if (timeout < 0)
    {
        UnsafeBlock releaser;
        acquired = semaphore.request(ds, di);
    }
    else if (timeout == 0)
    {
        acquired = semaphore.requestImmediate(ds, di);
    }
    else
    {
        UnsafeBlock releaser;

        acquired = semaphore.request((uint32_t)timeout, ds, di);
    }

    // only do the nesting thing if we got the lock
    if (acquired)
    {
        handleNesting();
    }

    return booleanObject(acquired);
}


/**
 * Release the mutex semaphore
 *
 * @return true if this was successfully released, false if we were not the semaphore owners.
 */
RexxObject* MutexSemaphoreClass::release(const char *ds, int di)
{
    // don't even attempt this if this isn't held.
    if (nestCount == 0)
    {
        return TheFalseObject;
    }

    // if this thread really owns this mutex, this will succeed
    bool released = semaphore.release(ds, di);
    if (released)
    {
        nestCount--;
        if (nestCount == 0)
        {
            // tell the current activity this is no longer needed
            ActivityManager::currentActivity->removeMutex(this);
        }
    }
    return booleanObject(released);
}


/**
 * Force this semaphore to release all of it's locks on thread termination.
 */
void MutexSemaphoreClass::forceLockRelease(const char *ds, int di)
{
    while (nestCount > 0)
    {
        semaphore.release(ds, di);
        nestCount--;
    }
}
