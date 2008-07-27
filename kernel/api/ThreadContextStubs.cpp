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
/* REXX API support                                                           */
/*                                                                            */
/* Stub functions for all APIs accessed via the NativeMethodContext           */
/*                                                                            */
/******************************************************************************/

#include "RexxCore.h"
#include "ContextApi.hpp"
#include "RexxActivity.hpp"
#include "StringClass.hpp"
#include "IntegerClass.hpp"
#include "BufferClass.hpp"
#include "SupplierClass.hpp"
#include "ArrayClass.hpp"
#include "DirectoryClass.hpp"
#include "TableClass.hpp"
#include "MethodClass.hpp"
#include "RoutineClass.hpp"
#include "PackageClass.hpp"
#include "Interpreter.hpp"
#include "InterpreterInstance.hpp"
#include "SystemInterpreter.hpp"
#include "PointerClass.hpp"
#include "ActivityManager.hpp"
#include "RexxStartDispatcher.hpp"
#include "PackageManager.hpp"

BEGIN_EXTERN_C()

void RexxEntry DetachThread(RexxThreadContext *c)
{
    // we do this one without grabbing the lock because we're going away.
    ApiContext context(c, false);
    try
    {
        context.activity->detachThread();
    }
    catch (RexxNativeActivation *)
    {
    }
}


/**
 * API stub for raising a halt condition on a thread.
 */
void RexxEntry HaltThread(RexxThreadContext *c)
{
    ApiContext context(c);
    try
    {
        context.activity->halt(OREF_NULL);
    }
    catch (RexxNativeActivation *)
    {
    }
}


void RexxEntry SetThreadTrace(RexxThreadContext *c, logical_t setting)
{
    ApiContext context(c);
    try
    {
        context.activity->setTrace(setting != 0);
    }
    catch (RexxNativeActivation *)
    {
    }
}


RexxObjectPtr RexxEntry RequestGlobalReference(RexxThreadContext *c, RexxObjectPtr o)
{
    ApiContext context(c);
    try
    {
        context.activity->getInstance()->addGlobalReference((RexxObject *)o);
        return o;
    }
    catch (RexxNativeActivation *)
    {
    }
    return NULLOBJECT;
}


void RexxEntry ReleaseGlobalReference(RexxThreadContext *c, RexxObjectPtr o)
{
    ApiContext context(c);
    try
    {
        context.activity->getInstance()->removeGlobalReference((RexxObject *)o);
    }
    catch (RexxNativeActivation *)
    {
    }
}


void RexxEntry ReleaseLocalReference(RexxThreadContext *c, RexxObjectPtr o)
{
    ApiContext context(c);
    try
    {
        context.context->removeLocalReference((RexxObject *)o);
    }
    catch (RexxNativeActivation *)
    {
    }
}

//NB:  The name "SendMessage" has a conflict with a Windows API, so this name differs from
// the call vector version.
RexxObjectPtr RexxEntry SendMessageArray(RexxThreadContext *c, RexxObjectPtr o, CSTRING m, RexxArrayObject a)
{
    ApiContext context(c);
    try
    {
        return context.ret(((RexxObject *)o)->sendMessage(new_upper_string(m), (RexxArray *)a));
    }
    catch (RexxNativeActivation *)
    {
    }
    return NULLOBJECT;
}


RexxObjectPtr RexxEntry SendMessage0(RexxThreadContext *c, RexxObjectPtr o, CSTRING m)
{
    ApiContext context(c);
    try
    {
        return context.ret(((RexxObject *)o)->sendMessage(new_upper_string(m)));
    }
    catch (RexxNativeActivation *)
    {
    }
    return NULLOBJECT;
}


RexxObjectPtr RexxEntry SendMessage1(RexxThreadContext *c, RexxObjectPtr o, CSTRING m, RexxObjectPtr a1)
{
    ApiContext context(c);
    try
    {
        return context.ret(((RexxObject *)o)->sendMessage(new_upper_string(m), (RexxObject *)a1));
    }
    catch (RexxNativeActivation *)
    {
    }
    return NULLOBJECT;
}

RexxObjectPtr RexxEntry SendMessage2(RexxThreadContext *c, RexxObjectPtr o, CSTRING m, RexxObjectPtr a1, RexxObjectPtr a2)
{
    ApiContext context(c);
    try
    {
        return context.ret(((RexxObject *)o)->sendMessage(new_upper_string(m), (RexxObject *)a1, (RexxObject *)a2));
    }
    catch (RexxNativeActivation *)
    {
    }
    return NULLOBJECT;
}

RexxDirectoryObject RexxEntry GetLocalEnvironment(RexxThreadContext *c)
{
    ApiContext context(c);
    try
    {
        return (RexxDirectoryObject)ActivityManager::localEnvironment;
    }
    catch (RexxNativeActivation *)
    {
    }
    return NULLOBJECT;
}

RexxDirectoryObject RexxEntry GetGlobalEnvironment(RexxThreadContext *c)
{
    ApiContext context(c);
    try
    {
        return (RexxDirectoryObject)TheEnvironment;
    }
    catch (RexxNativeActivation *)
    {

    }
    return NULLOBJECT;
}


logical_t RexxEntry IsSameType(RexxThreadContext *c, RexxObjectPtr o1, RexxObjectPtr o2)
{
    ApiContext context(c);
    try
    {
        return ((RexxObject *)o1)->isSameType((RexxObject *)o2);
    }
    catch (RexxNativeActivation *)
    {
    }
    return 0;
}


logical_t RexxEntry IsInstanceOf(RexxThreadContext *c, RexxObjectPtr o, RexxClassObject cl)
{
    ApiContext context(c);
    try
    {
        return ((RexxObject *)o)->isInstanceOf((RexxClass *)cl);
    }
    catch (RexxNativeActivation *)
    {
    }
    return 0;
}


logical_t RexxEntry HasMethod(RexxThreadContext *c, RexxObjectPtr o, CSTRING n)
{
    ApiContext context(c);
    try
    {
        // convert the name to a string instance, and check the environments.
        return ((RexxObject *)o)->hasMethod(new_upper_string(n)) == TheTrueObject;

    }
    catch (RexxNativeActivation *)
    {
    }
    return 0;
}


RexxPackageObject RexxEntry LoadPackage(RexxThreadContext *c, CSTRING n)
{
    ApiContext context(c);
    try
    {
        ProtectedObject package;
        RexxString *name = new_string(n);
        RexxString *resolvedName = context.activity->getInstance()->resolveProgramName(name, OREF_NULL, OREF_NULL);

        // convert the name to a string instance, and check the environments.
        return (RexxPackageObject)context.ret(PackageManager::loadRequires(context.activity, name, resolvedName, package));
    }
    catch (RexxNativeActivation *)
    {
    }
    return NULLOBJECT;
}


RexxPackageObject RexxEntry LoadPackageFromData(RexxThreadContext *c, CSTRING n, CSTRING d, size_t l)
{
    ApiContext context(c);
    try
    {
        ProtectedObject package;
        return (RexxPackageObject)PackageManager::loadRequires(context.activity, new_string(n), d, l, package);
    }
    catch (RexxNativeActivation *)
    {
    }
    return NULLOBJECT;
}


RexxClassObject RexxEntry FindClass(RexxThreadContext *c, CSTRING n)
{
    ApiContext context(c);
    try
    {
        // convert the name to a string instance, and check the environments.
        RexxString *name = new_upper_string(n);
        return (RexxClassObject)context.ret(context.context->findClass(name));

    }
    catch (RexxNativeActivation *)
    {
    }
    return NULLOBJECT;
}


RexxClassObject RexxEntry FindClassFromPackage(RexxThreadContext *c, RexxPackageObject m, CSTRING n)
{
    ApiContext context(c);
    try
    {
        // convert the name to a string instance, and check the environments.
        RexxString *name = new_upper_string(n);
        return (RexxClassObject)context.ret(((PackageClass *)m)->findClass(name));

    }
    catch (RexxNativeActivation *)
    {
    }
    return NULLOBJECT;
}


RexxDirectoryObject RexxEntry GetPackageRoutines(RexxThreadContext *c, RexxPackageObject m)
{
    ApiContext context(c);
    try
    {
        return (RexxDirectoryObject)context.ret(((PackageClass *)m)->getRoutines());
    }
    catch (RexxNativeActivation *)
    {
    }
    return NULLOBJECT;
}


RexxDirectoryObject RexxEntry GetPackagePublicRoutines(RexxThreadContext *c, RexxPackageObject m)
{
    ApiContext context(c);
    try
    {
        return (RexxDirectoryObject)context.ret(((PackageClass *)m)->getPublicRoutines());
    }
    catch (RexxNativeActivation *)
    {
    }
    return NULLOBJECT;
}

RexxDirectoryObject RexxEntry GetPackageClasses(RexxThreadContext *c, RexxPackageObject m)
{
    ApiContext context(c);
    try
    {
        return (RexxDirectoryObject)context.ret(((PackageClass *)m)->getClasses());
    }
    catch (RexxNativeActivation *)
    {
    }
    return NULLOBJECT;
}

RexxDirectoryObject RexxEntry GetPackagePublicClasses(RexxThreadContext *c, RexxPackageObject m)
{
    ApiContext context(c);
    try
    {
        return (RexxDirectoryObject)context.ret(((PackageClass *)m)->getPublicClasses());
    }
    catch (RexxNativeActivation *)
    {
    }
    return NULLOBJECT;
}

RexxDirectoryObject RexxEntry GetPackageMethods(RexxThreadContext *c, RexxPackageObject m)
{
    ApiContext context(c);
    try
    {
        return (RexxDirectoryObject)context.ret(((PackageClass *)m)->getClasses());
    }
    catch (RexxNativeActivation *)
    {
    }
    return NULLOBJECT;
}


RexxObjectPtr RexxEntry CallRoutine(RexxThreadContext *c, RexxRoutineObject r, RexxArrayObject a)
{
    ApiContext context(c);
    try
    {
        CallRoutineDispatcher dispatcher((RoutineClass *)r, (RexxArray *)a);
        context.activity->run(dispatcher);
        return (RexxObjectPtr)dispatcher.result;
    }
    catch (RexxNativeActivation *)
    {
    }
    return NULLOBJECT;
}


RexxObjectPtr RexxEntry CallProgram(RexxThreadContext *c, const char * p, RexxArrayObject a)
{
    ApiContext context(c);
    try
    {
        CallProgramDispatcher dispatcher(p, (RexxArray *)a);
        context.activity->run(dispatcher);
        return (RexxObjectPtr)dispatcher.result;
    }
    catch (RexxNativeActivation *)
    {
    }
    return NULLOBJECT;
}


RexxMethodObject RexxEntry NewMethod(RexxThreadContext *c, CSTRING name, CSTRING source, stringsize_t length)
{
    ApiContext context(c);
    try
    {
        // convert the name to a string instance, and check the environments.
        return (RexxMethodObject)context.ret(new RexxMethod(new_string(name), source, length));
    }
    catch (RexxNativeActivation *)
    {
    }
    return NULLOBJECT;
}


RexxRoutineObject RexxEntry NewRoutine(RexxThreadContext *c, CSTRING name, CSTRING source, stringsize_t length)
{
    ApiContext context(c);
    try
    {
        // convert the name to a string instance, and check the environments.
        return (RexxRoutineObject)context.ret(new RoutineClass(new_string(name), source, length));
    }
    catch (RexxNativeActivation *)
    {
    }
    return NULLOBJECT;
}


logical_t RexxEntry IsRoutine(RexxThreadContext *c, RexxObjectPtr o)
{
    ApiContext context(c);
    try
    {
        // convert the name to a string instance, and check the environments.
        return (logical_t)((RexxObject *)o)->isInstanceOf(TheRoutineClass);
    }
    catch (RexxNativeActivation *)
    {
    }
    return false;
}


logical_t RexxEntry IsMethod(RexxThreadContext *c, RexxObjectPtr o)
{
    ApiContext context(c);
    try
    {
        // convert the name to a string instance, and check the environments.
        return (logical_t)((RexxObject *)o)->isInstanceOf(TheMethodClass);
    }
    catch (RexxNativeActivation *)
    {
    }
    return false;
}


RexxPackageObject RexxEntry GetRoutinePackage(RexxThreadContext *c, RexxRoutineObject o)
{
    ApiContext context(c);
    try
    {
        // convert the name to a string instance, and check the environments.
        return (RexxPackageObject)((RoutineClass *)o)->getPackage();
    }
    catch (RexxNativeActivation *)
    {
    }
    return false;
}


RexxPackageObject RexxEntry GetMethodPackage(RexxThreadContext *c, RexxMethodObject o)
{
    ApiContext context(c);
    try
    {
        // convert the name to a string instance, and check the environments.
        return (RexxPackageObject)((RexxMethod *)o)->getPackage();
    }
    catch (RexxNativeActivation *)
    {
    }
    return false;
}



RexxBufferObject RexxEntry SaveRoutine(RexxThreadContext *c, RexxRoutineObject m)
{
    ApiContext context(c);
    try
    {
        return (RexxBufferObject)context.ret(((RoutineClass *)m)->save());
    }
    catch (RexxNativeActivation *)
    {
    }
    return NULLOBJECT;
}

RexxObjectPtr RexxEntry NewObject(RexxThreadContext *c)
{
    ApiContext context(c);
    try
    {
        return context.ret(new (TheObjectClass) RexxObject());
    }
    catch (RexxNativeActivation *)
    {
    }
    return NULLOBJECT;
}

POINTER RexxEntry ObjectToCSelf(RexxThreadContext *c, RexxObjectPtr o)
{
    ApiContext context(c);
    try
    {
        // ask the object to figure this out
        return ((RexxObject *)o)->getCSelf();
    }
    catch (RexxNativeActivation *)
    {
    }
    return NULL;
}


RexxObjectPtr RexxEntry NumberToObject(RexxThreadContext *c, wholenumber_t n)
{
    ApiContext context(c);
    try
    {
        return context.ret(Numerics::wholenumberToObject((wholenumber_t)n));
    }
    catch (RexxNativeActivation *)
    {
    }
    return NULLOBJECT;
}

RexxObjectPtr RexxEntry UintptrToObject(RexxThreadContext *c, uintptr_t n)
{
    ApiContext context(c);
    try
    {
        return context.ret(Numerics::ptrToObject(n));
    }
    catch (RexxNativeActivation *)
    {
    }
    return NULLOBJECT;
}


RexxObjectPtr RexxEntry ValueToObject(RexxThreadContext *c, ValueDescriptor *d)
{
    ApiContext context(c);
    try
    {
        return (RexxObjectPtr)context.ret(context.context->valueToObject(d));
    }
    catch (RexxNativeActivation *)
    {
        context.context->setConditionInfo(OREF_NULL);
    }
    return NULLOBJECT;
}


RexxArrayObject RexxEntry ValuesToObject(RexxThreadContext *c, ValueDescriptor *d, size_t count)
{
    ApiContext context(c);
    try
    {
        return (RexxArrayObject)context.ret(context.context->valuesToObject(d, count));
    }
    catch (RexxNativeActivation *)
    {
        context.context->setConditionInfo(OREF_NULL);
    }
    return NULLOBJECT;
}


logical_t RexxEntry ObjectToValue(RexxThreadContext *c, RexxObjectPtr o, ValueDescriptor *d)
{
    ApiContext context(c);
    try
    {
        return context.context->objectToValue((RexxObject *)o, d);
    }
    catch (RexxNativeActivation *)
    {
        // some conversion failures result in an exception...cancel that, and
        // just return FALSE;
        context.context->setConditionInfo(OREF_NULL);
    }
    return false;
}

RexxObjectPtr RexxEntry UnsignedNumberToObject(RexxThreadContext *c, stringsize_t n)
{
    ApiContext context(c);
    try
    {
        return context.ret(Numerics::stringsizeToObject((stringsize_t)n));
    }
    catch (RexxNativeActivation *)
    {
    }
    return NULLOBJECT;
}


logical_t RexxEntry ObjectToNumber(RexxThreadContext *c, RexxObjectPtr o, wholenumber_t *n)
{
    ApiContext context(c);
    try
    {
        wholenumber_t temp;
        // this uses the entire value range
        if (Numerics::objectToWholeNumber((RexxObject *)o, temp, SSIZE_MAX, SSIZE_MIN))
        {
            *n = (wholenumber_t)temp;
            return true;
        }
        return false;
    }
    catch (RexxNativeActivation *)
    {
    }
    return 0;
}


logical_t RexxEntry ObjectToUnsignedNumber(RexxThreadContext * c, RexxObjectPtr o, stringsize_t * n)
{
    ApiContext context(c);
    try
    {
        stringsize_t temp;
        // this uses the entire value range
        if (Numerics::objectToStringSize((RexxObject *)o, temp, SIZE_MAX))
        {
            *n = (stringsize_t)temp;
            return true;
        }
        return false;
    }
    catch (RexxNativeActivation *)
    {
    }
    return 0;
}

RexxObjectPtr RexxEntry Int64ToObject(RexxThreadContext *c, int64_t n)
{
    ApiContext context(c);
    try
    {
        return context.ret(Numerics::int64ToObject(n));
    }
    catch (RexxNativeActivation *)
    {
    }
    return NULLOBJECT;
}

RexxObjectPtr RexxEntry UnsignedInt64ToObject(RexxThreadContext * c, uint64_t n)
{
    ApiContext context(c);
    try
    {
        return context.ret(Numerics::uint64ToObject(n));
    }
    catch (RexxNativeActivation *)
    {
    }
    return NULLOBJECT;
}

logical_t RexxEntry ObjectToInt64(RexxThreadContext *c, RexxObjectPtr o, int64_t * n)
{
    ApiContext context(c);
    try
    {
        // this uses the entire value range
        return Numerics::objectToInt64((RexxObject *)o, *n);
    }
    catch (RexxNativeActivation *)
    {
    }
    return 0;
}

logical_t RexxEntry ObjectToUnsignedInt64(RexxThreadContext *c, RexxObjectPtr o, uint64_t *n)
{
    ApiContext context(c);
    try
    {
        // this uses the entire value range
        return Numerics::objectToUnsignedInt64((RexxObject *)o, *n);
    }
    catch (RexxNativeActivation *)
    {
    }
    return 0;
}

logical_t RexxEntry ObjectToUintptr(RexxThreadContext * c, RexxObjectPtr o, uintptr_t * n)
{
    ApiContext context(c);
    try
    {
        // this uses the entire value range
        return Numerics::objectToUintptr((RexxObject *)o, *n);
    }
    catch (RexxNativeActivation *)
    {
    }
    return 0;
}

RexxObjectPtr RexxEntry DoubleToObject(RexxThreadContext *c, double n)
{
    ApiContext context(c);
    try
    {
        return context.ret(new_numberstringFromDouble(n));
    }
    catch (RexxNativeActivation *)
    {
    }
    return NULLOBJECT;
}

RexxObjectPtr RexxEntry DoubleToObjectWithPrecision(RexxThreadContext *c, double n, size_t precision)
{
    ApiContext context(c);
    try
    {
        return context.ret(new_string(n, precision));
    }
    catch (RexxNativeActivation *)
    {
    }
    return NULLOBJECT;
}

logical_t RexxEntry ObjectToDouble(RexxThreadContext *c, RexxObjectPtr o, double *n)
{
    ApiContext context(c);
    try
    {
        return ((RexxObject *)o)->doubleValue(*n);
    }
    catch (RexxNativeActivation *)
    {
    }
    return 0;
}

RexxStringObject RexxEntry ObjectToString(RexxThreadContext *c, RexxObjectPtr o)
{
    ApiContext context(c);
    try
    {
        return (RexxStringObject)context.ret(REQUEST_STRING((RexxObject *)o));
    }
    catch (RexxNativeActivation *)
    {
    }
    return NULLOBJECT;
}

CSTRING RexxEntry ObjectToStringValue(RexxThreadContext *c, RexxObjectPtr o)
{
    ApiContext context(c);
    try
    {
        RexxString *temp = REQUEST_STRING((RexxObject *)o);
        context.ret(temp);
        return (CSTRING)temp->getStringData();
    }
    catch (RexxNativeActivation *)
    {
    }
    return NULL;
}

size_t RexxEntry StringGet(RexxThreadContext *c, RexxStringObject s, size_t o, POINTER r, size_t l)
{
    ApiContext context(c);
    try
    {
        RexxString *temp = (RexxString *)s;
        return temp->get(o, (char *)r, l);
    }
    catch (RexxNativeActivation *)
    {
    }
    return 0;
}

size_t RexxEntry StringLength(RexxThreadContext *c, RexxStringObject s)
{
    ApiContext context(c);
    try
    {
        RexxString *temp = (RexxString *)s;
        return temp->getLength();
    }
    catch (RexxNativeActivation *)
    {
    }
    return 0;
}

CSTRING RexxEntry StringData(RexxThreadContext *c, RexxStringObject s)
{
    ApiContext context(c);
    try
    {
        RexxString *temp = (RexxString *)s;
        return (CSTRING)temp->getStringData();
    }
    catch (RexxNativeActivation *)
    {
    }
    return NULL;
}

RexxStringObject RexxEntry NewString(RexxThreadContext *c, CSTRING s, size_t l)
{
    ApiContext context(c);
    try
    {
        return (RexxStringObject)context.ret(new_string(s, (stringsize_t)l));
    }
    catch (RexxNativeActivation *)
    {
    }
    return NULLOBJECT;
}

RexxStringObject RexxEntry NewStringFromAsciiz(RexxThreadContext *c, CSTRING s)
{
    ApiContext context(c);
    try
    {
        return (RexxStringObject)context.ret(new_string((const char *)s));
    }
    catch (RexxNativeActivation *)
    {
    }
    return NULLOBJECT;
}

RexxStringObject RexxEntry StringUpper(RexxThreadContext *c, RexxStringObject s)
{
    ApiContext context(c);
    try
    {
        RexxString *temp = (RexxString *)s;
        return (RexxStringObject)context.ret(temp->upper());
    }
    catch (RexxNativeActivation *)
    {
    }
    return NULLOBJECT;
}

RexxStringObject RexxEntry StringLower(RexxThreadContext *c, RexxStringObject s)
{
    ApiContext context(c);
    try
    {
        RexxString *temp = (RexxString *)s;
        return (RexxStringObject)context.ret(temp->lower());
    }
    catch (RexxNativeActivation *)
    {
    }
    return NULLOBJECT;
}

logical_t RexxEntry IsString(RexxThreadContext *c, RexxObjectPtr o)
{
    ApiContext context(c);
    try
    {
        return isString((RexxObject *)o);
    }
    catch (RexxNativeActivation *)
    {
    }
    return 0;
}

RexxBufferStringObject RexxEntry NewBufferString(RexxThreadContext * c, size_t l)
{
    ApiContext context(c);
    try
    {
        return (RexxBufferStringObject)context.ret(raw_string((stringsize_t)l));
    }
    catch (RexxNativeActivation *)
    {
    }
    return NULLOBJECT;
}

size_t  RexxEntry BufferStringLength(RexxThreadContext *c, RexxBufferStringObject s)
{
    ApiContext context(c);
    try
    {
        RexxString *temp = (RexxString *)s;
        return temp->getLength();
    }
    catch (RexxNativeActivation *)
    {
    }
    return 0;
}

POINTER RexxEntry BufferStringData(RexxThreadContext *c, RexxBufferStringObject s)
{
    ApiContext context(c);
    try
    {
        RexxString *temp = (RexxString *)s;
        return (POINTER)temp->getWritableData();
    }
    catch (RexxNativeActivation *)
    {
    }
    return NULL;
}

RexxStringObject RexxEntry FinishBufferString(RexxThreadContext *c, RexxBufferStringObject s, size_t l)
{
    ApiContext context(c);
    try
    {
        RexxString *temp = (RexxString *)s;
        temp->finish(l);
        return s;
    }
    catch (RexxNativeActivation *)
    {
    }
    return NULL;
}

void  RexxEntry TablePut(RexxThreadContext *c, RexxTableObject t, RexxObjectPtr o, RexxObjectPtr i)
{
    ApiContext context(c);
    try
    {
        ((RexxTable *)t)->put((RexxObject *)o, (RexxObject *)i);
    }
    catch (RexxNativeActivation *)
    {
    }
}

RexxObjectPtr RexxEntry TableGet(RexxThreadContext *c, RexxTableObject t, RexxObjectPtr i)
{
    ApiContext context(c);
    try
    {
        return context.ret(((RexxTable *)t)->get((RexxObject *)i));
    }
    catch (RexxNativeActivation *)
    {
    }
    return OREF_NULL;
}

RexxObjectPtr RexxEntry TableRemove(RexxThreadContext *c, RexxTableObject t, RexxObjectPtr i)
{
    ApiContext context(c);
    try
    {
        return context.ret(((RexxTable *)t)->remove((RexxObject *)i));
    }
    catch (RexxNativeActivation *)
    {
    }
    return OREF_NULL;
}

RexxTableObject RexxEntry NewTable(RexxThreadContext *c)
{
    ApiContext context(c);
    try
    {
        return (RexxTableObject)context.ret(new_table());
    }
    catch (RexxNativeActivation *)
    {
    }
    return OREF_NULL;
}

logical_t RexxEntry IsTable(RexxThreadContext *c, RexxObjectPtr o)
{
    ApiContext context(c);
    try
    {
        return isOfClass(Table, (RexxObject *)o);
    }
    catch (RexxNativeActivation *)
    {
    }
    return false;
}

void  RexxEntry DirectoryPut(RexxThreadContext *c, RexxDirectoryObject t, RexxObjectPtr o, CSTRING i)
{
    ApiContext context(c);
    try
    {
        ((RexxDirectory *)t)->put((RexxObject *)o, new_string(i));
    }
    catch (RexxNativeActivation *)
    {
    }
}

RexxObjectPtr RexxEntry DirectoryAt(RexxThreadContext *c, RexxDirectoryObject t, CSTRING i)
{
    ApiContext context(c);
    try
    {
        return context.ret(((RexxDirectory *)t)->at(new_string(i)));
    }
    catch (RexxNativeActivation *)
    {
    }
    return OREF_NULL;
}

RexxObjectPtr RexxEntry DirectoryRemove(RexxThreadContext *c, RexxDirectoryObject t, CSTRING i)
{
    ApiContext context(c);
    try
    {
        return context.ret(((RexxDirectory *)t)->remove(new_string(i)));
    }
    catch (RexxNativeActivation *)
    {
    }
    return OREF_NULL;
}

RexxDirectoryObject RexxEntry NewDirectory(RexxThreadContext *c)
{
    ApiContext context(c);
    try
    {
        return (RexxDirectoryObject)context.ret(new_directory());
    }
    catch (RexxNativeActivation *)
    {
    }
    return OREF_NULL;
}

logical_t RexxEntry IsDirectory(RexxThreadContext *c, RexxObjectPtr o)
{
    ApiContext context(c);
    try
    {
        return isOfClass(Directory, (RexxObject *)o);
    }
    catch (RexxNativeActivation *)
    {
    }
    return false;
}

RexxObjectPtr RexxEntry ArrayAt(RexxThreadContext *c, RexxArrayObject a, size_t i)
{
    ApiContext context(c);
    try
    {
        return (RexxObjectPtr)context.ret(((RexxArray *)a)->getApi(i));
    }
    catch (RexxNativeActivation *)
    {
    }
    return OREF_NULL;
}

logical_t RexxEntry ArrayHasIndex(RexxThreadContext *c, RexxArrayObject a, size_t i)
{
    ApiContext context(c);
    try
    {
        return ((RexxArray *)a)->hasIndexApi(i);
    }
    catch (RexxNativeActivation *)
    {
    }
    return false;
}

void RexxEntry ArrayPut(RexxThreadContext *c, RexxArrayObject a, RexxObjectPtr o, size_t i)
{
    ApiContext context(c);
    try
    {
        ((RexxArray *)a)->putApi((RexxObject *)o, i);
    }
    catch (RexxNativeActivation *)
    {
    }
}


size_t RexxEntry ArrayAppend(RexxThreadContext *c, RexxArrayObject a, RexxObjectPtr o)
{
    ApiContext context(c);
    try
    {
        return ((RexxArray *)a)->append((RexxObject *)o);
    }
    catch (RexxNativeActivation *)
    {
        return 0;
    }
}


size_t RexxEntry ArrayAppendString(RexxThreadContext *c, RexxArrayObject a, CSTRING s, size_t l)
{
    ApiContext context(c);
    try
    {
        RexxString *str = new_string(s, (stringsize_t)l);
        return ((RexxArray *)a)->append(str);
    }
    catch (RexxNativeActivation *)
    {
        return 0;
    }
}


size_t RexxEntry ArraySize(RexxThreadContext *c, RexxArrayObject a)
{
    ApiContext context(c);
    try
    {
        return ((RexxArray *)a)->size();
    }
    catch (RexxNativeActivation *)
    {
    }
    return 0;
}

wholenumber_t RexxEntry ArrayDimension(RexxThreadContext *c, RexxArrayObject a)
{
    ApiContext context(c);
    try
    {
        return ((RexxArray *)a)->getDimension();
    }
    catch (RexxNativeActivation *)
    {
    }
    return 0;
}

RexxArrayObject RexxEntry NewArray(RexxThreadContext *c, size_t s)
{
    ApiContext context(c);
    try
    {
        return (RexxArrayObject)context.ret(new_array(s));
    }
    catch (RexxNativeActivation *)
    {
    }
    return NULLOBJECT;
}

RexxArrayObject RexxEntry ArrayOfOne(RexxThreadContext *c, RexxObjectPtr o)
{
    ApiContext context(c);
    try
    {
        return (RexxArrayObject)context.ret(new_array((RexxObject *)o));
    }
    catch (RexxNativeActivation *)
    {
    }
    return NULLOBJECT;
}

RexxArrayObject RexxEntry ArrayOfTwo(RexxThreadContext *c, RexxObjectPtr o1, RexxObjectPtr o2)
{
    ApiContext context(c);
    try
    {
        return (RexxArrayObject)context.ret(new_array((RexxObject *)o1, (RexxObject *)o2));
    }
    catch (RexxNativeActivation *)
    {
    }
    return NULLOBJECT;
}


logical_t RexxEntry IsArray(RexxThreadContext *c, RexxObjectPtr o)
{
    ApiContext context(c);
    try
    {
        return isArray((RexxObject *)o);
    }
    catch (RexxNativeActivation *)
    {
    }
    return false;
}

POINTER RexxEntry BufferData(RexxThreadContext *c, RexxBufferObject b)
{
    ApiContext context(c);
    try
    {
        return (POINTER)((RexxBuffer *)b)->getData();
    }
    catch (RexxNativeActivation *)
    {
    }
    return NULL;
}

size_t RexxEntry BufferLength(RexxThreadContext *c, RexxBufferObject b)
{
    ApiContext context(c);
    try
    {
        return ((RexxBuffer *)b)->getLength();
    }
    catch (RexxNativeActivation *)
    {
    }
    return 0;
}

RexxBufferObject RexxEntry NewBuffer(RexxThreadContext *c, size_t l)
{
    ApiContext context(c);
    try
    {
        return (RexxBufferObject)context.ret((RexxObject *)new_buffer(l));
    }
    catch (RexxNativeActivation *)
    {
    }
    return NULLOBJECT;
}

logical_t RexxEntry IsBuffer(RexxThreadContext *c, RexxObjectPtr o)
{
    ApiContext context(c);
    try
    {
        return isOfClass(Buffer, (RexxObject *)o);
    }
    catch (RexxNativeActivation *)
    {
    }
    return false;
}

wholenumber_t RexxEntry IntegerValue(RexxThreadContext *c, RexxIntegerObject o)
{
    ApiContext context(c);
    try
    {
        return ((RexxInteger *)o)->wholeNumber();
    }
    catch (RexxNativeActivation *)
    {
    }
    return 0;
}

RexxIntegerObject RexxEntry NewInteger(RexxThreadContext *c, wholenumber_t n)
{
    ApiContext context(c);
    try
    {
        return (RexxIntegerObject)context.ret(new_integer(n));
    }
    catch (RexxNativeActivation *)
    {
    }
    return NULLOBJECT;
}

logical_t RexxEntry IsInteger(RexxThreadContext *c, RexxObjectPtr o)
{
    ApiContext context(c);
    try
    {
        return isInteger((RexxObject *)o);
    }
    catch (RexxNativeActivation *)
    {
    }
    return false;
}

POINTER RexxEntry PointerValue(RexxThreadContext *c, RexxPointerObject o)
{
    ApiContext context(c);
    try
    {
        return (POINTER)((RexxPointer *)o)->pointer();
    }
    catch (RexxNativeActivation *)
    {
    }
    return NULL;
}

RexxPointerObject RexxEntry NewPointer(RexxThreadContext *c, POINTER p)
{
    ApiContext context(c);
    try
    {
        return (RexxPointerObject)context.ret(new_pointer(p));
    }
    catch (RexxNativeActivation *)
    {
    }
    return NULLOBJECT;
}

logical_t RexxEntry IsPointer(RexxThreadContext *c, RexxObjectPtr o)
{
    ApiContext context(c);
    try
    {
        return isOfClass(Pointer, (RexxObject *)o);
    }
    catch (RexxNativeActivation *)
    {
    }
    return false;
}

RexxObjectPtr RexxEntry SupplierValue(RexxThreadContext *c, RexxSupplierObject o)
{
    ApiContext context(c);
    try
    {
        return context.ret(((RexxSupplier *)o)->value());
    }
    catch (RexxNativeActivation *)
    {
    }
    return NULLOBJECT;
}

RexxObjectPtr RexxEntry SupplierIndex(RexxThreadContext *c, RexxSupplierObject o)
{
    ApiContext context(c);
    try
    {
        return context.ret(((RexxSupplier *)o)->index());
    }
    catch (RexxNativeActivation *)
    {
    }
    return NULLOBJECT;
}

logical_t RexxEntry SupplierAvailable(RexxThreadContext *c, RexxSupplierObject o)
{
    ApiContext context(c);
    try
    {
        return ((RexxSupplier *)o)->available() == TheTrueObject;
    }
    catch (RexxNativeActivation *)
    {
    }
    return false;
}

void RexxEntry SupplierNext(RexxThreadContext *c, RexxSupplierObject o)
{
    ApiContext context(c);
    try
    {
        ((RexxSupplier *)o)->next();
    }
    catch (RexxNativeActivation *)
    {
    }
}

RexxSupplierObject RexxEntry NewSupplier(RexxThreadContext *c, RexxArrayObject values, RexxArrayObject names)
{
    ApiContext context(c);
    try
    {
        return (RexxSupplierObject)context.ret(new_supplier((RexxArray *)values, (RexxArray *)names));
    }
    catch (RexxNativeActivation *)
    {
    }
    return NULLOBJECT;
}

RexxStemObject RexxEntry NewStem(RexxThreadContext *c, CSTRING name)
{
    ApiContext context(c);
    try
    {
        if (name == NULL)
        {
            return (RexxStemObject)context.ret(new RexxStem(OREF_NULL));
        }
        else
        {
            return (RexxStemObject)context.ret(new RexxStem(new_string(name)));
        }

    }
    catch (RexxNativeActivation *)
    {
    }
    return NULLOBJECT;
}

void RexxEntry SetStemElement(RexxThreadContext *c, RexxStemObject s, CSTRING n, RexxObjectPtr v)
{
    ApiContext context(c);
    try
    {
        ((RexxStem *)s)->setElement(n, (RexxObject *)v);
    }
    catch (RexxNativeActivation *)
    {
    }
}

RexxObjectPtr RexxEntry GetStemElement(RexxThreadContext *c, RexxStemObject s, CSTRING n)
{
    ApiContext context(c);
    try
    {
        return context.ret(((RexxStem *)s)->getElement(n));
    }
    catch (RexxNativeActivation *)
    {
    }
    return NULLOBJECT;
}

void RexxEntry DropStemElement(RexxThreadContext *c, RexxStemObject s, CSTRING n)
{
    ApiContext context(c);
    try
    {
        ((RexxStem *)s)->dropElement(n);
    }
    catch (RexxNativeActivation *)
    {
    }
}

void RexxEntry SetStemArrayElement(RexxThreadContext *c, RexxStemObject s, size_t i, RexxObjectPtr v)
{
    ApiContext context(c);
    try
    {
        ((RexxStem *)s)->setElement((size_t )i, (RexxObject *)v);
    }
    catch (RexxNativeActivation *)
    {
    }
}

RexxObjectPtr RexxEntry GetStemArrayElement(RexxThreadContext *c, RexxStemObject s, size_t i)
{
    ApiContext context(c);
    try
    {
        return context.ret(((RexxStem *)s)->getElement((size_t)i));
    }
    catch (RexxNativeActivation *)
    {
    }
    return NULLOBJECT;
}

void RexxEntry DropStemArrayElement(RexxThreadContext *c, RexxStemObject s, size_t i)
{
    ApiContext context(c);
    try
    {
        ((RexxStem *)s)->dropElement((size_t)i);
    }
    catch (RexxNativeActivation *)
    {
    }
}

RexxSupplierObject RexxEntry GetAllStemElements(RexxThreadContext *c, RexxStemObject s)
{
    ApiContext context(c);
    try
    {
        return (RexxSupplierObject)context.ret(((RexxStem *)s)->supplier());
    }
    catch (RexxNativeActivation *)
    {
    }
    return NULLOBJECT;
}

RexxObjectPtr RexxEntry GetStemValue(RexxThreadContext *c, RexxStemObject s)
{
    ApiContext context(c);
    try
    {
        return context.ret(((RexxStem *)s)->getStemValue());
    }
    catch (RexxNativeActivation *)
    {
    }
    return NULLOBJECT;
}

logical_t RexxEntry IsStem(RexxThreadContext *c, RexxObjectPtr o)
{
    ApiContext context(c);
    try
    {
        return isOfClass(Stem, (RexxObject *)o);
    }
    catch (RexxNativeActivation *)
    {
    }
    return false;
}

void RexxEntry RaiseException0(RexxThreadContext *c, size_t n)
{
    ApiContext context(c);
    try
    {
        reportException((wholenumber_t)n);
    }
    catch (RexxNativeActivation *)
    {
    }
}

void RexxEntry RaiseException1(RexxThreadContext *c, size_t n, RexxObjectPtr o1)
{
    ApiContext context(c);
    try
    {
        reportException((wholenumber_t)n, (RexxObject *)o1);
    }
    catch (RexxNativeActivation *)
    {
    }
}

void RexxEntry RaiseException2(RexxThreadContext *c, size_t n, RexxObjectPtr o1, RexxObjectPtr o2)
{
    ApiContext context(c);
    try
    {
        reportException((wholenumber_t)n, (RexxObject *)o1, (RexxObject *)o2);
    }
    catch (RexxNativeActivation *)
    {
    }
}

void RexxEntry RaiseExceptionArray(RexxThreadContext *c, size_t n, RexxArrayObject a)
{
    ApiContext context(c);
    try
    {
        reportException((wholenumber_t)n, (RexxArray *)a);
    }
    catch (RexxNativeActivation *)
    {
    }
}

void RexxEntry RaiseCondition(RexxThreadContext *c, CSTRING name, CSTRING desc, RexxArrayObject add, RexxObjectPtr result)
{
    ApiContext context(c);
    try
    {
        context.activity->raiseCondition(new_upper_string(name), OREF_NULL, new_string(desc),
            (RexxArray *)add, (RexxObject *)result, OREF_NULL);
    }
    catch (RexxNativeActivation *)
    {
    }
}

logical_t RexxEntry CheckCondition(RexxThreadContext *c)
{
    ApiContext context(c);
    try
    {
        return context.context->getConditionInfo() != OREF_NULL;
    }
    catch (RexxNativeActivation *)
    {
    }
    return false;
}

RexxDirectoryObject RexxEntry GetConditionInfo(RexxThreadContext *c)
{
    ApiContext context(c);
    try
    {
        return (RexxDirectoryObject)context.context->getConditionInfo();
    }
    catch (RexxNativeActivation *)
    {
    }
    return OREF_NULL;
}


void RexxEntry DecodeConditionInfo(RexxThreadContext *c, RexxDirectoryObject d, RexxCondition *cd)
{
    ApiContext context(c);
    try
    {
        Interpreter::decodeConditionData((RexxDirectory *)d, cd);
    }
    catch (RexxNativeActivation *)
    {
    }
}

void RexxEntry ClearCondition(RexxThreadContext *c)
{
    ApiContext context(c);
    try
    {
        context.context->setConditionInfo(OREF_NULL);
    }
    catch (RexxNativeActivation *)
    {
    }
}

END_EXTERN_C()

RexxThreadInterface RexxActivity::threadContextFunctions =
{
    THREAD_INTERFACE_VERSION,
    DetachThread,
    HaltThread,
    SetThreadTrace,
    RequestGlobalReference,
    ReleaseGlobalReference,
    ReleaseLocalReference,

    SendMessageArray,
    SendMessage0,
    SendMessage1,
    SendMessage2,

    GetLocalEnvironment,
    GetGlobalEnvironment,

    IsSameType,
    IsInstanceOf,
    HasMethod,
    LoadPackage,
    LoadPackageFromData,
    FindClass,
    FindClassFromPackage,
    GetPackageRoutines,
    GetPackagePublicRoutines,
    GetPackageClasses,
    GetPackagePublicClasses,
    GetPackageMethods,
    CallRoutine,
    CallProgram,

    NewMethod,
    NewRoutine,
    IsRoutine,
    IsMethod,
    GetRoutinePackage,
    GetMethodPackage,
    SaveRoutine,

    NewObject,
    ObjectToCSelf,
    NumberToObject,
    UintptrToObject,
    ValueToObject,
    ValuesToObject,
    ObjectToValue,
    UnsignedNumberToObject,
    ObjectToNumber,
    ObjectToUnsignedNumber,
    Int64ToObject,
    UnsignedInt64ToObject,
    ObjectToInt64,
    ObjectToUnsignedInt64,
    ObjectToUintptr,
    DoubleToObject,
    DoubleToObjectWithPrecision,
    ObjectToDouble,

    ObjectToString,
    ObjectToStringValue,
    StringGet,
    StringLength,
    StringData,
    NewString,
    NewStringFromAsciiz,
    StringUpper,
    StringLower,
    IsString,

    NewBufferString,
    BufferStringLength,
    BufferStringData,
    FinishBufferString,

    TablePut,
    TableGet,
    TableRemove,
    NewTable,
    IsTable,

    DirectoryPut,
    DirectoryAt,
    DirectoryRemove,
    NewDirectory,
    IsDirectory,

    ArrayAt,
    ArrayHasIndex,
    ArrayPut,
    ArrayAppend,
    ArrayAppendString,
    ArraySize,
    ArrayDimension,
    NewArray,
    ArrayOfOne,
    ArrayOfTwo,
    IsArray,

    BufferData,
    BufferLength,
    NewBuffer,
    IsBuffer,

    IntegerValue,
    NewInteger,
    IsInteger,

    PointerValue,
    NewPointer,
    IsPointer,

    SupplierValue,
    SupplierIndex,
    SupplierAvailable,
    SupplierNext,
    NewSupplier,

    NewStem,
    SetStemElement,
    GetStemElement,
    DropStemElement,
    SetStemArrayElement,
    GetStemArrayElement,
    DropStemArrayElement,
    GetAllStemElements,
    GetStemValue,
    IsStem,

    RaiseException0,
    RaiseException1,
    RaiseException2,
    RaiseExceptionArray,
    RaiseCondition,
    CheckCondition,
    GetConditionInfo,
    DecodeConditionInfo,
    ClearCondition,

    OREF_NULL,
    OREF_NULL,
    OREF_NULL,
    OREF_NULL
};
