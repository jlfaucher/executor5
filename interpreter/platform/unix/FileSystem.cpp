/*----------------------------------------------------------------------------*/
/*                                                                            */
/* Copyright (c) 1995, 2004 IBM Corporation. All rights reserved.             */
/* Copyright (c) 2005-2019 Rexx Language Association. All rights reserved.    */
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
/* REXX Unix support                                                          */
/*                                                                            */
/* Unix specific file related routines.                                       */
/*                                                                            */
/******************************************************************************/
#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "RexxCore.h"
#include "StringClass.hpp"
#include "BufferClass.hpp"
#include "ProtectedObject.hpp"
#include "Interpreter.hpp"
#include "SystemInterpreter.hpp"
#include "InterpreterInstance.hpp"
#include "SysFileSystem.hpp"
#include "ActivityManager.hpp"
#include "SysProcess.hpp"

#include <string.h>
#include <stdio.h>
#include <stddef.h>
#include <sys/stat.h>
#include <limits.h>

#if defined( HAVE_SYS_FILIO_H )
# include <sys/filio.h>
#endif

#include <sys/ioctl.h>
#include <sys/types.h>
#include <unistd.h>
#include "SysProcess.hpp"

#ifdef HAVE_STROPTS_H
# include <stropts.h>
#endif

/**
 * Resolve a program for intial loading or a subroutine call.
 *
 * @param _name      The target name.  This can be fully qualified, or a simple name
 *                   without an extension.
 * @param _parentDir The directory of the file of our calling program.  The first place
 *                   we'll look is in the same directory as the program.
 * @param _parentExtension
 *                   The extension our calling program has.  If there is an extension,
 *                   we'll use that version first before trying any of the default
 *                   extensions.
 *
 * @return A string version of the file name, if found.  Returns OREF_NULL if
 *         the program cannot be found.
 */
RexxString* SysInterpreterInstance::resolveProgramName(RexxString *_name, RexxString *_parentDir, RexxString *_parentExtension)
{
    char resolvedName[PATH_MAX + 3];    // finally resolved name

    const char *name = _name->getStringData();
    const char *parentDir = _parentDir == OREF_NULL ? NULL : _parentDir->getStringData();
    const char *parentExtension = _parentExtension == OREF_NULL ? NULL : _parentExtension->getStringData();
    const char *pathExtension = instance->searchPath == OREF_NULL ? NULL : instance->searchPath->getStringData();

    SysSearchPath searchPath(parentDir, pathExtension);

    // if the file already has an extension, this dramatically reduces the number
    // of searches we need to make.
    if (SysFileSystem::hasExtension(name))
    {
        if (SysFileSystem::searchName(name, searchPath.path, NULL, resolvedName))
        {
            return new_string(resolvedName);
        }
        return OREF_NULL;
    }

    // if we have a parent extension provided, use that in preference to any default searches
    if (parentExtension != NULL)
    {
        if (SysFileSystem::searchName(name, searchPath.path, parentExtension, resolvedName))
        {
            return new_string(resolvedName);
        }

    }

    // ok, now time to try each of the individual extensions along the way.
    for (size_t i = 1; i <= instance->searchExtensions->items(); i++)
    {
        RexxString *ext = (RexxString *)instance->searchExtensions->get(i);

        if (SysFileSystem::searchName(name, searchPath.path, ext->getStringData(), resolvedName))
        {
            return new_string(resolvedName);
        }
    }

    // The file may purposefully have no extension.
    if (SysFileSystem::searchName(name, searchPath.path, NULL, resolvedName))
    {
        return new_string(resolvedName);
    }

    return OREF_NULL;
}


/**
 * Load the image file into storage
 *
 * @param imageBuffer
 *                  The returned image buffer
 * @param imageSize The returned image size.
 */
void SystemInterpreter::loadImage(char *&imageBuffer, size_t &imageSize)
{
    char fullname[PATH_MAX + 2];    // finally resolved name

    // first try for a colocated image file
    const char *installLocation =  SysProcess::getExecutableLocation();
    if (installLocation != NULL)
    {
        snprintf(fullname, sizeof(fullname), "%s%s", installLocation, BASEIMAGE);
        if (loadImage(imageBuffer, imageSize, fullname))
        {
            return;
        }
    }

    // colocation failed, try the current directory next
    if (loadImage(imageBuffer, imageSize, BASEIMAGE))
    {
        return;
    }

    // Now try to locate the file on the path if that fails
    if (SysFileSystem::primitiveSearchName(BASEIMAGE, getenv("PATH"), NULL, fullname))
    {
        if (loadImage(imageBuffer, imageSize, fullname))
        {
            return;
        }
    }

#ifdef ORX_CATDIR
    // last attempt, we might have a statically defined location to use. This is
    // bad form really, but we might as well make one more attempt
    strcpy(fullname, ORX_CATDIR"/rexx.img");
    if (loadImage(imageBuffer, imageSize, fullname))
    {
        return;
    }
#endif
    Interpreter::logicError("no startup image");   /* open failure                      */
}


/**
 * Load the image file into storage
 *
 * @param imageBuffer
 *                  The returned image buffer
 * @param imageSize The returned image size.
 */
bool SystemInterpreter::loadImage(char *&imageBuffer, size_t &imageSize, const char *imageFile)
{
    // try first in the current directory
    FILE *image = fopen(imageFile, "rb");
    // if not found, then try a path search
    if (image == NULL)
    {
        return false;
    }

    /* Read in the size of the image     */
    if (!fread(&imageSize, 1, sizeof(size_t), image))
    {
        return false;
    }

    /* Create new segment for image      */
    imageBuffer = (char *)memoryObject.allocateImageBuffer(imageSize);
    /* Create an object the size of the  */
    /* image. We will be overwriting the */
    /* object header.                    */
    /* read in the image, store the      */
    /* the size read                     */
    if (!(imageSize = fread(imageBuffer, 1, imageSize, image)))
    {
        Interpreter::logicError("could not read in the image");
    }
    fclose(image);                       /* and close the file                */
    return true;
}



BufferClass *SystemInterpreter::readProgram(const char *file_name)
/*******************************************************************/
/* Function:  Read a program into a buffer                         */
/*******************************************************************/
{
    FILE    *handle;                     /* open file access handle           */
    size_t   buffersize;                 /* size of read buffer               */
    size_t   readSize;                   // actual bytes read
    {
        handle = fopen(file_name, "rb");     /* open as a binary file             */
        if (handle == NULL)
        {                 /* open error?                       */
            return OREF_NULL;                  /* return nothing                    */
        }

        fseek(handle, 0, SEEK_END);          /* seek to the file end              */
        buffersize = ftell(handle);          /* get the file size                 */
        fseek(handle, 0, SEEK_SET);          /* seek back to the file beginning   */
    }
    BufferClass *buffer = new_buffer(buffersize);     /* get a buffer object               */
    ProtectedObject p(buffer);
    {
        UnsafeBlock releaser;

        readSize = fread(buffer->getData(), 1, buffersize, handle);
        fclose(handle);                      /* close the file                    */
    }
    if (readSize < buffersize) // read error?
    {
        return OREF_NULL;                  /* return nothing                    */
    }
    return buffer;                       /* return the program buffer         */
}

RexxString *SystemInterpreter::qualifyFileSystemName(
  RexxString * name)                   /* stream information block          */
/*******************************************************************/
/* Function:  Qualify a stream name for this system                */
/*******************************************************************/
{
    char nameBuffer[SysFileSystem::MaximumFileNameBuffer];

    /* clear out the block               */
    memset(nameBuffer, 0, sizeof(nameBuffer));
    SysFileSystem::qualifyStreamName(name->getStringData(), nameBuffer, sizeof(nameBuffer)); /* expand the full name              */
    /* get the qualified file name       */
    return new_string(nameBuffer);
}




