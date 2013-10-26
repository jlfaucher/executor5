/*----------------------------------------------------------------------------*/
/*                                                                            */
/* Copyright (c) 1995, 2004 IBM Corporation. All rights reserved.             */
/* Copyright (c) 2005-2009 Rexx Language Association. All rights reserved.    */
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
/* REXX Windows Support                                          rexxutil.c   */
/*                                                                            */
/* Windows system utility functions                                           */
/*                                                                            */
/******************************************************************************/
/**********************************************************************
*                                                                     *
*   This program extends the REXX language by providing many          *
*   REXX external functions.                                          *
*   These functions are:                                              *
*       SysCls              -- Clear the screen in an OS/2 fullscreen *
*                              or windowed command prompt session.    *
*       SysCurPos           -- Set and/or Query the cursor position   *
*                              in an OS/2 fullscreen or windowed      *
*                              command prompt session.                *
*       SysCurState         -- Make the cursor visible or invisible   *
*                              in an OS/2 fullscreen or windowed      *
*                              command prompt session.                *
*       SysDriveInfo        -- Returns information about a specific   *
*                              drive.                                 *
*       SysDriveMap         -- Returns a list of the drives on the    *
*                              machine                                *
*       SysDropFuncs        -- Makes all functions in this package    *
*                              unknown to REXX.                       *
*       SysFileDelete       -- Deletes a file                         *
*       SysFileSearch       -- Searches for a file matching a given   *
*                              filespec.                              *
*       SysFileTree         -- Searches for files matching a given    *
*                              filespec, including files in           *
*                              subdirectories.                        *
*       SysGetKey           -- Returns one by of data indicating the  *
*                              key which was pressed,                 *
*                              in an OS/2 fullscreen or windowed      *
*                              command prompt session.                *
*       SysIni              -- Reads and/or updates entries in .INI   *
*                              files.                                 *
*       SysLoadFuncs        -- Makes all functions in this package    *
*                              known to REXX so REXX programs may     *
*                              call them.                             *
*       SysMkDir            -- Creates a directory                    *
*       SysWinVer           -- Returns the Win OS and Version number  *
*       SysVersion          -- Returns the OS and Version number      *
*       SysRmDir            -- Removes a directory                    *
*       SysSearchPath       -- Searches throught a specified path     *
*                              for a file.                            *
*       SysSleep            -- Suspends execution for a number of     *
*                              seconds and milliseconds.              *
*       SysTempFilename     -- Creates a unique filename              *
*       SysTextScreenRead   -- Reads characters from the screen,      *
*                              in an OS/2 fullscreen or windowed      *
*                              command prompt session.                *
*       SysTextScreenSize   -- Returns the size of the window in      *
*                              rows and columns,                      *
*                              in an OS/2 fullscreen or windowed      *
*                              command prompt session.                *
*       SysWaitNamedPipe    -- Wait on a named pipe.                  *
*       SysRegisterObjectClass -- Register a new object class         *
*       SysDeregisterObjectClass -- Remove class registration         *
*       SysQueryClassList   -- Get list of registered classes         *
*       SysCreateObject     -- Create an object instance              *
*       SysDestroyObject    -- Delete an object instance              *
*       SysSetObjectData    -- Change object settings data            *
*       SysBootDrive        -- Return the windows boot drive          *
*       SysSystemDirectory  -- Return the Windows system directory    *
*       SysQueryEAList      -- Return list of file EA names           *
*       SysWildCard         -- Perform file wild card editting        *
*       SysFileSystemType   -- Return drive file system type          *
*       SysVolumeLabel      -- Return the drive label                 *
*       SysAddFileHandle    -- Add file handles to a process          *
*       SysSetFileHandle    -- Set file handles for a process         *
*       SysCreateMutexSem   -- Create a Mutex semaphore               *
*       SysOpenMutexSem     -- Open a Mutex semaphore                 *
*       SysCloseMutexSem    -- Close a Mutex semaphore                *
*       SysRequestMutexSem  -- Request a Mutex semaphore              *
*       SysReleaseMutexSem  -- Release a Mutex semaphore              *
*       SysCreateEventSem   -- Create an Event semaphore              *
*       SysOpenEventSem     -- Open an Event semaphore                *
*       SysCloseEventSem    -- Close an Event semaphore               *
*       SysPostEventSem     -- Post an Event semaphore                *
*       SysPulseEventSem    -- Post and reset an Event semaphore      *
*       SysResetEventSem    -- Reset an Event semaphore               *
*       SysWaitEventSem     -- Wait on an Event semaphore             *
*       SysProcessType      -- Return type of process                 *
*       SysSetPriority      -- Set current thread priority            *
*       SysGetCollate       -- Get country/codepage collating sequence*
*       SysNationalLanguageCompare -- NLS strict compare              *
*       SysMapCase          -- NLS uppercasing                        *
*       SysSetProcessCodePage -- Set current code page                *
*       SysQueryProcessCodePage -- Get current code page              *
*       SysAddRexxMacro     -- Load program into macro space          *
*       SysDropRexxMacro    -- Drop program from macro space          *
*       SysReorderRexxMacro -- Reorder program in macro space         *
*       SysQueryRexxMacro   -- Query ordering of macro space program  *
*       SysClearRexxMacroSpace -- Remove all programs from macro space*
*       SysLoadRexxMacroSpace  -- Load a Rexx macro space             *
*       SysSaveRexxMacroSpace  -- Save a Rexx macro space             *
*       SysShutDownSystem   -- Shutdown the system                    *
*       SysSwitchSession    -- Switch to a named session              *
*       SysDropLibrary      -- Drop a function package                *
*       SysPi               -- Return Pi to given precision           *
*       SysSqrt             -- Calculate a square root                *
*       SysExp              -- Calculate an exponent                  *
*       SysLog              -- Return natural log of a number         *
*       SysLog10            -- Return log base 10 of a number         *
*       SysSinh             -- Hyperbolic sine function               *
*       SysCosh             -- Hyperbolic cosine function             *
*       SysTanh             -- Hyperbolic tangent function            *
*       SysPower            -- raise number to non-integer power      *
*       SysSin              -- Sine function                          *
*       SysCos              -- Cosine function                        *
*       SysTan              -- Tangent function                       *
*       SysCotan            -- Cotangent function                     *
*       SysArcSin           -- ArcSine function                       *
*       SysArcCos           -- ArcCosine function                     *
*       SysArcTan           -- ArcTangent function                    *
*       SysQueryProcess     -- Get information on current proc/thread *
*       SysDumpVariables    -- Dump current variables to a file       *
*       SysSetFileDateTime  -- Set the last modified date of a file   *
*       SysGetFileDateTime  -- Get the last modified date of a file   *
*       SysStemSort         -- sort a stem array                      *
*       SysStemDelete       -- delete items in a stem array           *
*       SysStemInsert       -- insert items into a stem array         *
*       SysStemCopy         -- copy items from one stem array to other*
*       SysUtilVersion      -- query version of REXXUTIL.DLL          *
*       SysWinFileEncrypt   -- Encrypt file on a W2K-NTFS             *
*       SysWinFileDecrypt   -- Decrypt file on a W2K-NTFS             *
*       SysGetErrortext     -- Retrieve textual desc. of error number *
*       SysWinGetDefaultPrinter -- retrieve default printer           *
*       SysWinGetPrinters   -- Obtain list of local printers          *
*       SysWinSetDefaultPrinter -- set the local default printer      *
*       SysFileCopy         -- Copy files on the file system          *
*       SysFileMove         -- Move / Rename files or directories     *
*       SysIsFile           -- Check for the existance of a file      *
*       SysIsFileDirectory  -- Check for the existance of a directory *
*       SysIsFileLink       -- Check for the existance of a link      *
*       SysIsFileCompressed   -- Check for a file to be compressed    *
*       SysIsFileEncrypted    -- Check for a file to be encrypted     *
*       SysIsFileNotContentIndexed -- Check if a file should be indexed *
*       SysIsFileOffline    -- Check if a file is offline             *
*       SysIsFileSparse     -- Check if a file is sparse              *
*       SysIsFileTemporary  -- Check if a file is temporary           *
*                                                                     *
**********************************************************************/

/* Include files */

#include "oorexxapi.h"
#include <memory.h>
#include <fcntl.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <conio.h>
#include <math.h>
#include <limits.h>
#include <shlwapi.h>

#define OM_WAKEUP (WM_USER+10)
VOID CALLBACK SleepTimerProc( HWND, UINT, UINT, DWORD);

/*********************************************************************/
/*  Various definitions used by various functions.                   */
/*********************************************************************/

#define MAX_LABEL      13              /* max label length (sdrvinfo)*/
#define MAX_DIGITS     9               /* max digits in numeric arg  */
#define MAX            264             /* temporary buffer length    */
#define IBUF_LEN       4096            /* Input buffer length        */
#define MAX_READ       0x10000         /* full segment of buffer     */
#define CH_EOF         0x1A            /* end of file marker         */
#define CH_CR          '\r'            /* carriage return character  */
#define CH_NL          '\n'            /* new line character         */
#define AllocFlag      PAG_COMMIT | PAG_WRITE  /* for DosAllocMem    */
#define RNDFACTOR      1664525L
#define MAX_ENVVAR     1024
#define MAX_LINE_LEN   4096            /* max line length            */

/*********************************************************************/
/*  Various definitions used by the math functions                   */
/*********************************************************************/
#define SINE        0                  /* trig function defines...   */
#define COSINE      3                  /* the ordering is important, */
#define TANGENT     1                  /* as these get transformed   */
#define COTANGENT   2                  /* depending on the angle     */
#define MAXTRIG     3                  /* value                      */
#define ARCSINE     0                  /* defines for arc trig       */
#define ARCCOSINE   1                  /* functions.  Ordering is    */
#define ARCTANGENT  2                  /* not as important here      */

#define pi  3.14159265358979323846l    /* pi value                   */

#define DEGREES    'D'                 /* degrees option             */
#define RADIANS    'R'                 /* radians option             */
#define GRADES     'G'                 /* grades option              */

#define DEFAULT_PRECISION  9           /* default precision to use   */
#define MAX_PRECISION     16           /* maximum available precision*/

/*********************************************************************/
/*  Defines used by SysDriveMap                                      */
/*********************************************************************/

#define  USED           0
#define  FREE           1
#define  CDROM          2
#define  REMOTE         3
#define  LOCAL          4
#define  RAMDISK        5
#define  REMOVABLE      6

/*********************************************************************/
/* Defines uses by SysTree                                           */
/*********************************************************************/

#define  RECURSE        0x0002
#define  DO_DIRS        0x0004
#define  DO_FILES       0x0008
#define  NAME_ONLY      0x0010
#define  EDITABLE_TIME  0x0020
#define  LONG_TIME      0x0040   /* long time format for SysFileTree */
#define  CASELESS       0x0080
#define  RXIGNORE       2              /* Ignore attributes entirely */
#define  AllAtts        FILE_NORMAL | FILE_READONLY | FILE_HIDDEN | \
FILE_SYSTEM | FILE_DIRECTORY | FILE_ARCHIVED
#define  AllFiles       FILE_NORMAL | FILE_READONLY | FILE_HIDDEN | \
FILE_SYSTEM | FILE_ARCHIVED
#define  AllDirs        FILE_READONLY | FILE_HIDDEN | \
FILE_SYSTEM | FILE_ARCHIVED | MUST_HAVE_DIRECTORY | FILE_DIRECTORY

/*********************************************************************/
/* Defines used by SysStemSort -- must match values in okstem.hpp    */
/*********************************************************************/
#define SORT_CASESENSITIVE 0
#define SORT_CASEIGNORE    1

#define SORT_ASCENDING 0
#define SORT_DECENDING 1

/*********************************************************************/
/* Define used for Unicode translation. Not present in early Windows */
/* SDKs.                                                             */
/*********************************************************************/
#ifndef WC_ERR_INVALID_CHARS
#define WC_ERR_INVALID_CHARS      0x00000080
#endif

// Defines for various SysFileTree buffer.
#define FNAMESPEC_BUF_EXTRA    8
#define FNAMESPEC_BUF_LEN      MAX_PATH + FNAMESPEC_BUF_EXTRA
#define FOUNDFILE_BUF_LEN      MAX_PATH
#define FILETIME_BUF_LEN       64
#define FILEATTR_BUF_LEN       16
#define FOUNDFILELINE_BUF_LEN  FOUNDFILE_BUF_LEN + FILETIME_BUF_LEN + FILEATTR_BUF_LEN


/*********************************************************************/
/* Structures used throughout REXXUTIL.C                             */
/*********************************************************************/

/*********************************************************************/
/* RxTree Structure used by GetLine, OpenFile and CloseFile          */
/*********************************************************************/
typedef struct _GetFileData {
  char *       buffer;                 /* file read buffer           */
  size_t       size;                   /* file size                  */
  size_t       data;                   /* data left in buffer        */
  size_t       residual;               /* size left to read          */
  char *       scan;                   /* current scan position      */
  HANDLE       handle;                 /* file handle                */
} GetFileData;


/*
 *  Data structure for SysFileTree.
 *
 *  Note that in Windows the MAX_PATH define includes the terminating null.
 */
typedef struct RxTreeData {
    size_t         count;                         // Number of found file lines
    RexxStemObject files;                         // Stem that holds results.
    char           fNameSpec[FNAMESPEC_BUF_LEN];  // File name portion of the search for file spec, may contain glob characters.
    char           foundFile[FOUNDFILE_BUF_LEN];  // Full path name of found file
    char           fileTime[FILETIME_BUF_LEN];    // Time and size of found file
    char           fileAttr[FILEATTR_BUF_LEN];    // File attribute string of found file
    char           foundFileLine[FOUNDFILELINE_BUF_LEN]; // Buffer for found file line, includes foundFile, fileTime, and fileAttr
    char          *dFNameSpec;                    // Starts out pointing at fNameSpec
    size_t         nFNameSpec;                    // CouNt of bytes in dFNameSpec buffer
} RXTREEDATA;

/*********************************************************************/
/* RxStemData                                                        */
/*   Structure which describes as generic                            */
/*   stem variable.                                                  */
/*********************************************************************/

typedef struct RxStemData {
    SHVBLOCK shvb;                     /* Request block for RxVar    */
    char ibuf[IBUF_LEN];               /* Input buffer               */
    char varname[MAX];                 /* Buffer for the variable    */
                                       /* name                       */
    char stemname[MAX];                /* Buffer for the variable    */
                                       /* name                       */
    size_t stemlen;                    /* Length of stem.            */
    size_t vlen;                       /* Length of variable value   */
    size_t j;                          /* Temp counter               */
    size_t tlong;                      /* Temp counter               */
    size_t count;                      /* Number of elements         */
                                       /* processed                  */
} RXSTEMDATA;


/*********************************************************************/
/* Saved character status                                            */
/*********************************************************************/
static   int   ExtendedFlag = 0;       /* extended character saved   */
static   char  ExtendedChar;           /* saved extended character   */

/*********************************************************************/
/* function pointer for GetDiskFreespaceEx for SysDriveInfo          */
/*********************************************************************/
typedef  BOOL (WINAPI *P_GDFSE)(LPCTSTR, PULARGE_INTEGER, PULARGE_INTEGER,
                               PULARGE_INTEGER);
static   P_GDFSE pGetDiskFreeSpaceEx = NULL;

/*********************************************************************/
/* Numeric Error Return Strings                                      */
/*********************************************************************/

#define  NO_UTIL_ERROR    "0"          /* No error whatsoever        */
#define  ERROR_NOMEM      "2"          /* Insufficient memory        */
#define  ERROR_FILEOPEN   "3"          /* Error opening text file    */

/*********************************************************************/
/* Alpha Numeric Return Strings                                      */
/*********************************************************************/

#define  ERROR_RETSTR   "ERROR:"

/*********************************************************************/
/* Numeric Return calls                                              */
/*********************************************************************/

#define  INVALID_ROUTINE 40            /* Raise Rexx error           */
#define  VALID_ROUTINE    0            /* Successful completion      */

/*********************************************************************/
/* Some useful macros                                                */
/*********************************************************************/

#define BUILDRXSTRING(t, s) { \
  strcpy((t)->strptr,(s));\
  (t)->strlength = strlen((s)); \
}

#define RETVAL(retc) { \
  retstr->strlength = strlen(itoa(retc, retstr->strptr,10)); \
  return VALID_ROUTINE; \
}

/*********************************************************************/
/****************  REXXUTIL Supporting Functions  ********************/
/****************  REXXUTIL Supporting Functions  ********************/
/****************  REXXUTIL Supporting Functions  ********************/
/*********************************************************************/

void inline outOfMemoryException(RexxThreadContext *c)
{
    c->RaiseException1(Rexx_Error_System_service_user_defined, c->String("failed to allocate memory"));
}

/**
 * <routineName> argument <argPos> must not be a null string
 *
 * SysFileTree argument 2 must not be a null string
 *
 * @param c      Threade context we are operating in.
 * @param fName  Routine name.
 * @param pos    Argument position.
 */
void inline nullStringException(RexxThreadContext *c, CSTRING fName, size_t pos)
{
    c->RaiseException2(Rexx_Error_Incorrect_call_null, c->String(fName), c->StringSize(pos));
}

inline void safeLocalFree(void *p)
{
    if (p != NULL)
    {
        LocalFree(p);
    }
}

/**
 * Raises an exception for an unrecoverable system API failure.
 *
 * @param c    Call context we are operating in.
 * @param api  System API name.
 * @param rc   Return code from calling the API.
 */
static void systemServiceExceptionCode(RexxThreadContext *c, CSTRING api, uint32_t rc)
{
    char buf[256] = {0};
    _snprintf(buf, sizeof(buf),
             "system API %s() failed; rc: %d last error code: %d", api, rc, GetLastError());

    c->RaiseException1(Rexx_Error_System_service_user_defined, c->String(buf));
}

/**
 * Tests if the the current operating system version meets the specified
 * requirements. Really a front end to VerifyVersionInfo().  See MSDN docs for
 * type and condition flags.
 *
 * @param major       OS major number.
 * @param minor       OS minor number.
 * @param sp          Service pack level.
 * @param type        Further refines the test.  See MSDN for all the flags, but
 *                    for example there is VER_NT_WORKSTATION to differentiate
 *                    between NT desktop and NT server.
 * @param condition   The test condition.  Typical flags would be VER_EQUAL or
 *                    VER_GREATER_EQUAL.
 *
 * @return True if the condition is met by the current operating system, or
 *         false if not.
 */
static bool isWindowsVersion(DWORD major, DWORD minor, unsigned int sp, unsigned int type, unsigned int condition)
{
    OSVERSIONINFOEX ver;
    DWORDLONG       mask = 0;
    DWORD           testForMask = VER_MAJORVERSION | VER_MINORVERSION;

    ZeroMemory(&ver, sizeof(OSVERSIONINFOEX));

    ver.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
    ver.dwMajorVersion = major;
    ver.dwMinorVersion = minor;

    VER_SET_CONDITION(mask, VER_MAJORVERSION, condition);
    VER_SET_CONDITION(mask, VER_MINORVERSION, condition);

    if ( condition != VER_EQUAL )
    {
        ver.wServicePackMajor = sp;
        testForMask |= VER_SERVICEPACKMAJOR;
        VER_SET_CONDITION(mask, VER_SERVICEPACKMAJOR, condition);
    }

    if ( type != 0 )
    {
        ver.wProductType = type;
        testForMask |= VER_PRODUCT_TYPE;
        VER_SET_CONDITION(mask, VER_PRODUCT_TYPE, condition);
    }

    if ( VerifyVersionInfo(&ver, testForMask, mask) )
    {
        return true;
    }
    else
    {
        return false;
    }
}

/********************************************************************
* Function:  string2size_t(string, number)                          *
*                                                                   *
* Purpose:   Validates and converts an ASCII-Z string from string   *
*            form to an unsigned long.  Returns false if the number *
*            is not valid, true if the number was successfully      *
*            converted.                                             *
*                                                                   *
* RC:        true - Good number converted                           *
*            false - Invalid number supplied.                       *
*********************************************************************/
bool string2size_t(
    const char *string,                  /* string to convert          */
    size_t *number)                      /* converted number           */
{
    size_t   accumulator;                /* converted number           */
    size_t   length;                     /* length of number           */

    length = strlen(string);             /* get length of string       */
    if (length == 0 ||                   /* if null string             */
        length > MAX_DIGITS + 1)         /* or too long                */
    {
        return false;                    /* not valid                  */
    }

    accumulator = 0;                     /* start with zero            */

    while (length)                       /* while more digits          */
    {
        if (!isdigit(*string))             /* not a digit?               */
        {
            return false;                    /* tell caller                */
        }
                                             /* add to accumulator         */
        accumulator = accumulator * 10 + (*string - '0');
        length--;                          /* reduce length              */
        string++;                          /* step pointer               */
    }
    *number = accumulator;               /* return the value           */
    return true;                         /* good number                */
}

inline bool isAtLeastVista(void)
{
    return isWindowsVersion(6, 0, 0, 0, VER_GREATER_EQUAL);
}

/*********************************************************************/
/*                                                                   */
/*   Subroutine Name:   memupper                                     */
/*                                                                   */
/*   Descriptive Name:  uppercase a memory location                  */
/*                                                                   */
/*   Entry Point:       memupper                                     */
/*                                                                   */
/*   Input:             memory to upper case                         */
/*                      length of memory location                    */
/*                                                                   */
/*********************************************************************/

void  memupper(
  char    *location,                   /* location to uppercase      */
  size_t   length)                     /* length to uppercase        */
{
  for (; length--; location++)         /* loop for entire string     */
                                       /* uppercase in place         */
    *location = toupper(*location);
}

bool ReadNextBuffer( GetFileData *filedata );

/********************************************************************
* Function:  OpenFile(file, filedata)                               *
*                                                                   *
* Purpose:   Prepares a file for reading.                           *
*                                                                   *
* RC:        0     - file was opened successfully                   *
*            1     - file open error occurred                       *
*********************************************************************/

bool MyOpenFile(
   const char  *file,                  /* file name                  */
   GetFileData *filedata )             /* global file information    */
{
   DWORD       dwSize;                 /* file status information    */

                                       /* try to open the file       */
  if ((filedata->handle = CreateFile(file, GENERIC_READ,
                            FILE_SHARE_READ | FILE_SHARE_WRITE, 0, OPEN_EXISTING,
                            FILE_FLAG_WRITE_THROUGH, 0))
                            == INVALID_HANDLE_VALUE)
    return true;                          /* return failure             */

                                       /* retrieve the file size     */
  dwSize = GetFileSize(filedata->handle, NULL);
                                       /* if GetFileSize failed or   */
                                       /* size=0                     */
  if (dwSize == 0xffffffff || !dwSize) {
    CloseHandle(filedata->handle);     /* close the file             */
    return true;                       /* and quit                   */
  }
  if (dwSize <= MAX_READ) {            /* less than a single buffer  */
    DWORD bytesRead;
                                       /* allocate buffer for file   */
    if (!(filedata->buffer = (char *)GlobalAlloc(GMEM_ZEROINIT |
                                         GMEM_FIXED, dwSize))) {
      CloseHandle(filedata->handle);   /* close the file             */
      return true;
    }
    filedata->size = dwSize;           /* save file size             */
    filedata->residual = 0;            /* no left over information   */
                                       /* read the file in           */
    if (!ReadFile(filedata->handle, filedata->buffer, dwSize, &bytesRead, NULL)) {
      GlobalFree(filedata->buffer);    /* free the buffer            */
      CloseHandle(filedata->handle);   /* close the file             */
      return true;
    }

    // Set scan to beginning of buffer, and data to number of bytes we have.
    filedata->scan = filedata->buffer;
    filedata->data = bytesRead;
  }
  else {                               /* need to read partial       */
                                       /* allocate buffer for read   */
    if (!(filedata->buffer = (char *)GlobalAlloc(GMEM_ZEROINIT |
                                         GMEM_FIXED, MAX_READ))) {
      CloseHandle(filedata->handle);   /* close the file             */
      return true;
    }

    filedata->size = dwSize;           /* save file size             */
                                       /* and set remainder          */
    filedata->residual = filedata->size;
                                       /* read the file in           */
    if (ReadNextBuffer(filedata)) {
      GlobalFree(filedata->buffer);    /* free the buffer            */
      CloseHandle(filedata->handle);   /* close the file             */
      return true;
    }
  }
  return false;                        /* file is opened             */
}

/********************************************************************
* Function:  CloseFile(filedata)                                    *
*                                                                   *
* Purpose:   Close a file                                           *
*********************************************************************/
void CloseFile(
   GetFileData *filedata )             /* global file information    */
{
  CloseHandle(filedata->handle);       /* close the file             */
  GlobalFree(filedata->buffer);        /* free the buffer            */
}

/**
 * Reads the next buffer of data.
 *
 * @param filedata  Global file information.
 *
 * @return  0, buffer was read.  1, an error occurred reading buffer.
 */
bool ReadNextBuffer(GetFileData *filedata)
{
    size_t size;
    DWORD  bytesRead;

    /* get size of this read      */
    size = min(MAX_READ, filedata->residual);

    /* read the file in           */
    if ( !ReadFile(filedata->handle, filedata->buffer, (DWORD)size, &bytesRead, NULL) )
    {
        return 1;
    }
    filedata->data = bytesRead;

    if ( filedata->data != size )
    {
        // Read less than requested, no residual.
        filedata->residual = 0;            /* no residual                */
    }
    else
    {
        // Residual is remainder.
        filedata->residual = filedata->residual - size;
    }

    /* don't check for EOF but read to real end of file     */
    //                                     /* look for a EOF mark        */
    //endptr = memchr(filedata->buffer, CH_EOF, filedata->data);
    //
    //if (endptr) {                        /* found an EOF mark          */
    //                                     /* set new length             */
    //  filedata->data = (ULONG)(endptr - filedata->buffer);
    //  filedata->residual = 0;            /* no residual                */
    //}

    // Set position to beginning.
    filedata->scan = filedata->buffer;
    return 0;
}

/********************************************************************
* Function:  GetLine(line, size, filedata)                          *
*                                                                   *
* Purpose:   Reads a line of data using buffered reads.  At end of  *
*            file, zero is returned to indicate nothing left.       *
*                                                                   *
* RC:        true -  line was read successfully                     *
*            false - end of file was reached                        *
*********************************************************************/

bool GetLine(
   char        *line,                  /* returned line              */
   size_t       size,                  /* size of line buffer        */
   GetFileData *filedata )             /* file handle                */
{
   char        *scan;                  /* current scan pointer       */
   size_t       length;                /* line length                */
   size_t       copylength;            /* copied length              */


  if (!(filedata->data)) {             /* if out of current buffer   */
    if (filedata->residual) {          /* may be another buffer      */
      ReadNextBuffer(filedata);        /* try to read one            */
      if (!filedata->data)             /* nothing more?              */
        return true;                   /* all done                   */
    }
    else
      return true;                     /* return EOF condition       */
  }
                                       /* look for a carriage return */
  scan = (char *)memchr(filedata->scan, CH_NL, filedata->data);
  if (scan) {                          /* found one                  */
                                       /* calculate the length       */
    length = scan - filedata->scan;
    copylength = min(length, size);    /* get length to copy         */
                                       /* copy over the data         */
    memcpy(line, filedata->scan, copylength);
    line[copylength] = '\0';           /* make into ASCIIZ string    */

    /* we don't want the CR character in the result string*/
    if ( line[copylength - 1] == CH_CR ) {
      line[copylength - 1] = '\0';
    } /* endif */

    filedata->data -= length + 1;      /* reduce the length          */
    filedata->scan = scan + 1;         /* set new scan point         */

    if (!filedata->data) {             /* all used up                */
      if (filedata->residual)          /* more to read               */
        ReadNextBuffer(filedata);      /* read the next buffer       */
    }
    return false;                        /* this worked ok           */
  }
  else                                   /* ran off the end          */
  {
    /* now we have scanned the whole buffer, but didn't find LF.         */
    /* we have two situation that can appear:                            */
    /* 1.) size > filedata->data ==> there is still room in the working  */
    /*     buffer, we can see whether we have scanned the whole file     */
    /*     --> ReadNextBuffer, or this was it, and we return             */
    /* 2.) size < filedata->buffer ==> we have scanned to the end of the */
    /*     buffer, more than what would fit into it, but still we        */
    /*     haven't had a hit. So copy all elements into the buffer       */
    /*     read the next buffer, GetLine to get the next LF              */
    /*     and return what was put into buffer. Be ALWAYS AWARE that     */
    /*     that buffer limits to 2047 bytes, and that we only return up  */
    /*     to 2047 bytes of a line. The rest of the line is not returned */
    /*     and not checked for search argument. Nevertheless, this       */
    /*     garantees, that the line counter (argument 'N') corresponds   */
    /*     with the input file                                           */

                                       /* get length to copy         */
    if (size > filedata->data)
    {
       copylength = filedata->data;    /* copy the rest into linebuffer */
                                       /* copy over the data         */
       memcpy(line, filedata->scan, copylength);
       line[copylength] = '\0';          /* make into ASCIIZ string  */

     /* all data should be read, filedata->data must be zero         */
       filedata->data -= copylength;
     /* scan should be at the end                                    */
       filedata->scan += copylength;     /* set new scan point       */

    /* if no more data to read in the file, return OK     */
       if (!filedata->residual)
          return false;
       else
          return GetLine(line + copylength, size - copylength, filedata);
    }
    else        /* the line is full, scan until LF found but no copy */
    {
       copylength = min(size, filedata->data);
                                         /* copy over the data       */
       memcpy(line, filedata->scan, copylength);
       line[copylength] = '\0';          /* make into ASCIIZ string  */

       filedata->data  = 0;            /* no data in buffer          */
       filedata->scan += filedata->data; /* set scan point to end    */

       if (filedata->residual)         /* more to read               */
       {
           ReadNextBuffer(filedata);   /* read the next buffer       */
           return GetLine(line + copylength, 0, filedata);
       }
       else
          return false;
    }
  }
}

/********************************************************************
* Function:  SetFileMode(file, attributes)                          *
*                                                                   *
* Purpose:   Change file attribute bits                             *
*            without PM.                                            *
*                                                                   *
* RC:        0    -  File attributes successfully changed           *
*            1    -  Unable to change attributes                    *
*********************************************************************/
bool SetFileMode(
  const char *file,                    /* file name                  */
  size_t   attr )                      /* new file attributes        */
{

  DWORD         dwfileattrib;          /* file attributes            */

                                       /* get the file status        */
  if ((dwfileattrib = GetFileAttributes(file)) != 0xffffffff) {
                                       /* if worked                  */
                                       /* set the attributes         */
    if ((dwfileattrib = SetFileAttributes(file, (DWORD)attr)) != 0)
      return false;   /* give back success flag     */
    else
      return true;
  } else
    return true;
}

/********************************************************************
* Function:  string2long(string, number)                            *
*                                                                   *
* Purpose:   Validates and converts an ASCII-Z string from string   *
*            form to an unsigned long.  Returns false if the number *
*            is not valid, true if the number was successfully      *
*            converted.                                             *
*                                                                   *
* RC:        true - Good number converted                           *
*            false - Invalid number supplied.                       *
*********************************************************************/

bool string2long(
  const char *string,
  int *number)
{
  int      accumulator;                /* converted number           */
  size_t   length;                     /* length of number           */
  int      sign;                       /* sign of number             */

  sign = 1;                            /* set default sign           */
  if (*string == '-') {                /* negative?                  */
    sign = -1;                         /* change sign                */
    string++;                          /* step past sign             */
  }

  length = strlen(string);             /* get length of string       */
  if (length == 0 ||                   /* if null string             */
      length > MAX_DIGITS)             /* or too long                */
    return false;                      /* not valid                  */

  accumulator = 0;                     /* start with zero            */

  while (length != 0) {                /* while more digits          */
    if (!isdigit(*string))             /* not a digit?               */
      return false;                    /* tell caller                */
                                       /* add to accumulator         */
    accumulator = accumulator * 10 + (*string - '0');
    length--;                          /* reduce length              */
    string++;                          /* step pointer               */
  }
  *number = accumulator * sign;        /* return the value           */
  return true;                         /* good number                */
}

/********************************************************************
* Function:  string2ulong(string, number)                           *
*                                                                   *
* Purpose:   Validates and converts an ASCII-Z string from string   *
*            form to an unsigned long.  Returns false if the number *
*            is not valid, true if the number was successfully      *
*            converted.                                             *
*                                                                   *
* RC:        true - Good number converted                           *
*            false - Invalid number supplied.                       *
*********************************************************************/

bool string2ulong(
  const char  *string,                 /* string to convert          */
  size_t *number)                      /* converted number           */
{
  size_t   accumulator;                /* converted number           */
  size_t   length;                     /* length of number           */

  length = strlen(string);             /* get length of string       */
  if (length == 0 ||                   /* if null string             */
      length > MAX_DIGITS + 1)         /* or too long                */
    return false;                      /* not valid                  */

  accumulator = 0;                     /* start with zero            */

  while (length) {                     /* while more digits          */
    if (!isdigit(*string))             /* not a digit?               */
      return false;                    /* tell caller                */
                                       /* add to accumulator         */
    accumulator = accumulator * 10 + (*string - '0');
    length--;                          /* reduce length              */
    string++;                          /* step pointer               */
  }
  *number = accumulator;               /* return the value           */
  return true;                         /* good number                */
}

/********************************************************************
* Function:  string2pointer(string)                                 *
*                                                                   *
* Purpose:   Validates and converts an ASCII-Z string from string   *
*            form to a pointer value.  Returns false if the number  *
*            is not valid, true if the number was successfully      *
*            converted.                                             *
*                                                                   *
* RC:        true - Good number converted                           *
*            false - Invalid number supplied.                       *
*********************************************************************/

bool string2pointer(
  const char *string,                  /* string to convert          */
  void  **pointer)                     /* converted number           */
{
  return sscanf(string, "%p", pointer) == 1;
}

/********************************************************************
* Function:  mystrstr(haystack, needle, hlen, nlen, sensitive)      *
*                                                                   *
* Purpose:   Determines if the string 'needle' is in the            *
*            string 'haystack' by returning it's position or        *
*            a NULL if not found.  The length of haystack and       *
*            needle are given by 'hlen' and 'nlen' respectively.    *
*                                                                   *
*            If 'sensitive' is true, then the search is case        *
*            sensitive, else it is case insensitive.                *
*                                                                   *
* RC:        num  - The pos where needle was found.                 *
*            NULL - needle not found.                               *
*                                                                   *
* Used By:   SysFileSearch()                                        *
*********************************************************************/

char *mystrstr(
  const char *haystack,
  const char *needle,
  size_t  hlen,
  size_t  nlen,
  bool    sensitive)
{
// TODO:  This can be made a LOT more efficient

  char line[MAX_LINE_LEN];
  char target[MAX_LINE_LEN];
  size_t p;
 /* Copy line  - Change nulls to spaces and uppercase if needed      */

  for (p = 0; p < hlen; p++)
  {
    if (haystack[p] == '\0')
      line[p] = ' ';
    else if (sensitive)
      line[p] = haystack[p];
    else line[p] = (char)toupper(haystack[p]);
  }
  line[p] = '\0';

 /* Copy target  - Change nulls to spaces and uppercase if needed    */

  for (p = 0; p < nlen; p++) {

    if (needle[p] == '\0')
      target[p] = ' ';
    else if (sensitive)
      target[p] = needle[p];
    else target[p] = (char)toupper(needle[p]);
  }
  target[p] = '\0';

  return strstr(line, target);
}

/*****************************************************************
* Function:  getpath(string, path, filename)                     *
*                                                                *
* Purpose:  This function gets the PATH and FILENAME of the file *
*           target contained in STRING.  The path will end with  *
*           the '\' char if a path is supplied.                  *
*                                                                *
*****************************************************************/

void getpath(
  char *string,
  char *path,
  char *filename)
{
  size_t len;                          /* length of filespec         */
  int    LastSlashPos;                 /* position of last slash     */
  char   szBuff[MAX_PATH];             /* used to save current dir   */
  char   drv[3];                       /* used to change dir         */
  size_t i=0;

  while (string[i] == ' ')
  {
      i++;        /* skip leading blanks        */
  }
  if (i > 0)
  {
      len = strlen(string);            /* Get length of full file    */
      if ((string[i] == '\\' || string[i] == '/') ||  /* if first after blank is \ */
          (string[i] == '.' &&
            ((i<len && (string[i+1] == '\\' || string[i+1] == '/')) ||  /* or .\ */
            (i+1<len && string[i+1] == '.' && (string[i+2] == '\\' || string[i+2] == '/')))) ||  /* or ..\ */
            (i<len && string[i+1] == ':'))  /* z: */
                string = &string[i];
  }

  if (!strcmp(string, "."))            /* period case?               */
    strcpy(string, "*.*");             /* make it a *.* request      */
  else if (!strcmp(string, ".."))      /* double period case?        */
    strcpy(string, "..\\*.*");         /* make it a ..\*.* request   */
  len = strlen(string);                /* Get length of full file    */
                                       /* spec                       */
  LastSlashPos = (int)len;             /* Get max pos of last '\'    */

    /* Step back through string until at begin or at '\' char        */

  while (string[LastSlashPos] != '\\' && string[LastSlashPos] != '/' && LastSlashPos >= 0)
    --LastSlashPos;
  if (LastSlashPos < 0) {              /* no backslash, may be drive */
    if (string[1] == ':') {
      len = MAX_PATH;                  /* set max length             */
                                       /* Save the current drive     */
                                       /* and path                   */
      GetCurrentDirectory(sizeof(szBuff), szBuff);
      /* just copy the drive letter and the colon, omit the rest */
      /* (necessary i.g. if "I:*" is used */
      memcpy(drv, string, 2);
      drv[2] = '\0';

      SetCurrentDirectory(drv);        /* change to specified drive  */
                                       /* Get current directory      */
      GetCurrentDirectory((DWORD)len, path);
      SetCurrentDirectory(szBuff);     /* go back to where we were   */
                                       /* need a trailing slash?     */
      if (path[strlen(path) - 1] != '\\')
        strcat(path, "\\");            /* add a trailing slash       */
      LastSlashPos = 1;                /* make drive the path        */
    }
    else {
                                       /* Get current directory      */
      GetCurrentDirectory(MAX_PATH, path);
                                       /* need a trailing slash?     */
      if (path[strlen(path) - 1] != '\\')
        strcat(path, "\\");            /* add a trailing slash       */
    }
  }
  else {                               /* have a path                */
    if (string[1] == ':') {            /* have a drive?              */
                                       /* copy over the path         */
      memcpy(path, string, LastSlashPos+1);
      path[LastSlashPos+1] = '\0';     /* make into an ASCII-Z string*/
    }
    else {
      char fpath[MAX_PATH];
      char drive[_MAX_DRIVE];
      char dir[_MAX_DIR];
      char fname[_MAX_FNAME];
      char ext[_MAX_EXT];
      char lastc;


      if (LastSlashPos == 0)  /* only one backslash at the beginning */
      {
          _fullpath(fpath, "\\", MAX_PATH);  /* Get full path        */
          strcat(fpath, &string[1]);
      }
      else
      {

          string[LastSlashPos] = '\0'; /* chop off the path          */
          _fullpath(fpath, string, MAX_PATH); /* Get full path       */
          string[LastSlashPos] = '\\'; /* put the slash back         */
          lastc = fpath[strlen(fpath)-1];
          if (lastc != '\\' && lastc != '/')
              strcat(fpath, &string[LastSlashPos]);
      }
      _splitpath( fpath, drive, dir, fname, ext );

      strcpy(path, drive);
      strcat(path, dir);

      if (!strlen(path)) {             /* invalid path?              */
                                       /* copy over the path         */
         memcpy(path, string, LastSlashPos+1);
         path[LastSlashPos+1] = '\0';  /* make into an ASCII-Z string*/
      }
                                       /* need a trailing slash?     */
      if (path[strlen(path) - 1] != '\\')
        strcat(path, "\\");            /* add a trailing slash       */
    }
  }

    /* Get file name from filespec (just after last '\')             */
  if (string[LastSlashPos+1])          /* have a real name?          */
                                       /* copy it over               */
    strcpy(filename, &string[LastSlashPos+1]);
  else
    strcpy(filename, "*.*");           /* just use wildcards         */
}


/****************************************************************
* Function: GetUniqueFileName(Template, Filler, file)           *
*                                                               *
* Purpose:  This function returns a unique temporary file name  *
*           given a template and a filler character.            *
*                                                               *
* Params:   CHAR* Template - The template.  Must contain at     *
*                            least one or more filler chars.    *
*                                                               *
*                            Example:  "C:\TEMP\FILE????.DAT    *
*                                                               *
*           CHAR Filler    - The character in the Template to   *
*                            be replaced with numbers.  For     *
*                            the above example, the filler char *
*                            would be '?'.                      *
*           CHAR* file     - file name produced (output)        *
*                                                               *
* Used By:  RxTempFileName()                                    *
****************************************************************/

VOID GetUniqueFileName(
  CHAR  *Template,
  CHAR   Filler,
  CHAR  *file)
{

  CHAR numstr[6];
  bool Unique = false;

  ULONG x,                             /* loop index                 */
        i,                             /*                            */
        j = 0,                         /* number of filler chars     */
                                       /* found                      */
        num,                           /* temporary random number    */
        start,                         /* first random number        */
        max = 1;                       /* maximum random number      */

  INT  seed;                           /* to get current time        */
  WIN32_FIND_DATA wfdFinfo;            /* Windows Find data struct   */
                                       /* Structure                  */
  SYSTEMTIME DT;                       /* The date and time structure*/
  UINT            fuErrorMode;         /* holds current file err mode*/
  HANDLE hSearch;                      /* handle of file if found    */

 /** Determine number of filler characters *                         */

  for (x = 0; Template[x] != 0; x++)

    if (Template[x] == Filler) {
      max = max *10;
      j++;
    }

 /** Return NULL string if less than 1 or greater than 4 *           */

  if (j == 0 || j > 5) {
    Unique = true;
    strcpy(file, "");
    return;
  }

 /** Get a random number in the appropriate range                    */

                                       /* Get the time               */
  GetSystemTime(&DT);                  /* via Windows                */

  seed = DT.wHour*60 + DT.wMinute;     /* convert to hundreths       */
  seed = seed*60 + DT.wSecond;
  seed = seed*100 + ( DT.wMilliseconds / (UINT)10 );
  seed = seed * RNDFACTOR + 1;
  num = (ULONG)seed % max;
  start = num;

 /** Do until a unique name is found                                 */

  while (!Unique) {

    /** Generate string which represents the number                  */

    switch (j) {
      case 1 :
        wsprintf(numstr, "%01u", num);
        break;
      case 2 :
        wsprintf(numstr, "%02u", num);
        break;
      case 3 :
        wsprintf(numstr, "%03u", num);
        break;
      case 4 :
        wsprintf(numstr, "%04u", num);
        break;
      case 5 :
        wsprintf(numstr, "%05u", num);
        break;
    }

    /** Subsitute filler characters with numeric string              */

    i = 0;

    for (x = 0; Template[x] != 0; x++)

      if (Template[x] == Filler)
        file[x] = numstr[i++];

      else
        file[x] = Template[x];
    file[x] = '\0';

    /** See if the file exists                                       */
                                       /* Disable Hard-Error popups  */
    fuErrorMode = SetErrorMode(SEM_NOOPENFILEERRORBOX);
    hSearch = FindFirstFile(file, &wfdFinfo);

    if (hSearch == INVALID_HANDLE_VALUE)/* file not found?           */
      Unique = true;                   /* got one                    */

    FindClose(hSearch);
    SetErrorMode(fuErrorMode);         /* Enable previous setting    */

    /** Make sure we are not wasting our time                        */

    num = (num+1)%max;

    if (num == start && !Unique) {
      Unique = true;
      strcpy(file, "");
    }
  }
}

/**********************************************************************
***             <<<<<< REXXUTIL Functions Follow >>>>>>>            ***
***             <<<<<< REXXUTIL Functions Follow >>>>>>>            ***
***             <<<<<< REXXUTIL Functions Follow >>>>>>>            ***
***             <<<<<< REXXUTIL Functions Follow >>>>>>>            ***
**********************************************************************/
/**********************************************************************
* Function:  SysCls                                                   *
*                                                                     *
* Syntax:    call SysCls                                              *
*                                                                     *
* Return:    NO_UTIL_ERROR - Successful.                              *
**********************************************************************/

size_t RexxEntry SysCls(const char *name, size_t numargs, CONSTRXSTRING args[], const char *queuename, PRXSTRING retstr)
{
  HANDLE hStdout;                      /* Handle to Standard Out     */
  DWORD dummy;
  COORD Home = {0, 0};                 /* Home coordinates on console*/
  CONSOLE_SCREEN_BUFFER_INFO csbiInfo; /* Console information        */

  if (numargs)                         /* arguments specified?       */
      return INVALID_ROUTINE;          /* raise the error            */

  hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
                                       /* if in character mode       */
  if (GetConsoleScreenBufferInfo(hStdout, &csbiInfo)) {
    FillConsoleOutputCharacter( hStdout, ' ',
                                csbiInfo.dwSize.X * csbiInfo.dwSize.Y,
                                Home, &dummy );
    SetConsoleCursorPosition(hStdout, Home);

  }
  BUILDRXSTRING(retstr, NO_UTIL_ERROR);/* pass back result           */
  return VALID_ROUTINE;                /* no error on call           */
}

/*************************************************************************
* Function:  SysCurPos - positions cursor in OS/2 session                *
*                                                                        *
* Syntax:    call SysCurPos [row, col]                                   *
*                                                                        *
* Params:    row   - row to place cursor on                              *
*            col   - column to place cursor on                           *
*                                                                        *
* Return:    row, col                                                    *
*************************************************************************/

size_t RexxEntry SysCurPos(const char *name, size_t numargs, CONSTRXSTRING args[], const char *queuename, PRXSTRING retstr)
{

  int    inrow;                        /* Row to change to           */
  int    incol;                        /* Col to change to           */
  COORD NewHome;                       /* Position to move cursor    */
  CONSOLE_SCREEN_BUFFER_INFO csbiInfo; /* Console information        */
  HANDLE hStdout;                      /* Handle to Standard Out     */

  BUILDRXSTRING(retstr, NO_UTIL_ERROR);/* set default result         */
                                       /* check arguments            */
  if ((numargs != 0 && numargs != 2))  /* wrong number?              */
    return INVALID_ROUTINE;            /* raise an error             */

                                       /* get handle to stdout       */
  hStdout = GetStdHandle(STD_OUTPUT_HANDLE);

                                       /* get current position, and  */
                                       /* continue only if in        */
                                       /* character mode             */
  if (GetConsoleScreenBufferInfo(hStdout, &csbiInfo)) {

    sprintf(retstr->strptr, "%d %d", csbiInfo.dwCursorPosition.Y,
              csbiInfo.dwCursorPosition.X);
    retstr->strlength = strlen(retstr->strptr);

    if (numargs != 0) {                /* reset position to given    */
      if (!RXVALIDSTRING(args[0]) ||   /* not real arguments give    */
          !RXVALIDSTRING(args[1]))
        return INVALID_ROUTINE;        /* raise an error             */
                                       /* convert row to binary      */
      if (!string2long(args[0].strptr, &inrow) || inrow < 0)
        return INVALID_ROUTINE;        /* return error               */
                                       /* convert row to binary      */
      if (!string2long(args[1].strptr, &incol) || incol < 0)
        return INVALID_ROUTINE;        /* return error               */

      NewHome.Y = (SHORT)inrow;        /* convert to short form      */
      NewHome.X = (SHORT)incol;        /* convert to short form      */
                                       /* Set the cursor position    */
      SetConsoleCursorPosition(hStdout, NewHome);
    }
  }

  return VALID_ROUTINE;                /* no error on call           */
}

/*************************************************************************
* Function:  SysCurState                                                 *
*                                                                        *
* Syntax:    call SysCurState state                                      *
*                                                                        *
* Params:    state - Either 'ON' or 'OFF'.                               *
*                                                                        *
* Return:    NO_UTIL_ERROR - Successful.                                 *
*************************************************************************/

size_t RexxEntry SysCurState(const char *name, size_t numargs, CONSTRXSTRING args[], const char *queuename, PRXSTRING retstr)
{

  CONSOLE_CURSOR_INFO CursorInfo;      /* info about cursor          */
  HANDLE hStdout;                      /* Handle to Standard Out     */

  BUILDRXSTRING(retstr, NO_UTIL_ERROR);
                                       /* validate the arguments     */
  if (numargs != 1)
    return INVALID_ROUTINE;
                                       /* get handle to stdout       */
  hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
                                       /* Get the cursor info        */
  GetConsoleCursorInfo(hStdout,&CursorInfo);
                                       /* Get state and validate     */
  if (_stricmp(args[0].strptr, "ON") == 0)
    CursorInfo.bVisible = true;
  else if (_stricmp(args[0].strptr, "OFF") == 0)
    CursorInfo.bVisible = FALSE;
  else
    return INVALID_ROUTINE;            /* Invalid state              */
                                       /* Set the cursor info        */
  SetConsoleCursorInfo(hStdout,&CursorInfo);
  return VALID_ROUTINE;                /* no error on call           */
}

/*************************************************************************
* Function:  SysDriveInfo                                                *
*                                                                        *
* Syntax:    call SysDriveInfo drive                                     *
*                                                                        *
* Params:    drive - 'C', 'D', 'E', etc.                                 *
*                                                                        *
* Return:    disk free total label                                       *
*************************************************************************/

size_t RexxEntry SysDriveInfo(const char *name, size_t numargs, CONSTRXSTRING args[], const char *queuename, PRXSTRING retstr)
{
  CHAR   chFileSysType[MAX_PATH],      /*  File system name          */
         chVolumeName[MAX_PATH],       /*  volume label              */
         chDriveLetter[4];             /*  drive_letter + : + \ + \0 */
  BOOL   bGVIrc;                       /* rc from GVI                */

                                       /* GetDiskFreeSpace calculations */
  DWORD  dwSectorsPerCluster, dwBytesPerSector;
  DWORD  dwFreeClusters, dwClusters;
  BOOL   bGDFSrc;                      /* GDFS rc                    */
  UINT   errorMode;

  DWORD dwgle;
  unsigned __int64 i64FreeBytesToCaller,
                   i64TotalBytes,
                   i64FreeBytes;

                                       /* validate arguments         */
  if (numargs != 1 ||
      args[0].strlength > 2 ||         /* no more than 2 chars       */
      args[0].strlength == 0)          /* at least 1                 */
    return INVALID_ROUTINE;

  const char *arg = args[0].strptr;    /* get argument pointer       */
                                       /* drive letter?              */
  if (strlen(arg) == 2 &&              /* if second letter isn't : bye */
      arg[1] != ':')
    return INVALID_ROUTINE;

  if (arg[0] < 'A' ||                  /* is it in range?            */
      arg[0] > 'z')
    return INVALID_ROUTINE;

   if (strlen(arg) == 1){              /* need to add a : if only the*/
     chDriveLetter[0]=arg[0];          /* letter was passed in       */
     chDriveLetter[1]=':';
     chDriveLetter[2]='\\';            /* need to add \ because of   */
     chDriveLetter[3]='\0';            /* bug in getvolumeinfo       */
   }
   else                           /* have <letter>: , just copy over */
   {
     strcpy(chDriveLetter, args[0].strptr);
     chDriveLetter[2]='\\';            /* need to add \ because of   */
     chDriveLetter[3]='\0';            /* bug in getvolumeinfo       */
   }

  /* try to load GetDiskFreeSpaceEx function pointer */
  if ( !pGetDiskFreeSpaceEx )
  {
    pGetDiskFreeSpaceEx = (P_GDFSE) GetProcAddress(GetModuleHandle("kernel32.dll"),
                                                   "GetDiskFreeSpaceExA");
  } /* endif */

  errorMode = SetErrorMode(SEM_FAILCRITICALERRORS);
                         /* get the volume name and file system type */
  bGVIrc=GetVolumeInformation(chDriveLetter,
                           chVolumeName,
                           (DWORD)MAX_PATH,
                           NULL,
                           NULL,
                           NULL,
                           chFileSysType,
                           (DWORD)MAX_PATH);

  dwgle=GetLastError();

  /* use appropriate function */
  if ( pGetDiskFreeSpaceEx )
  {
    bGDFSrc = pGetDiskFreeSpaceEx(chDriveLetter,
                                  (PULARGE_INTEGER) &i64FreeBytesToCaller,
                                  (PULARGE_INTEGER) &i64TotalBytes,
                                  (PULARGE_INTEGER) &i64FreeBytes);
  }
  else
  {
                              /* get the disk free space information */
    bGDFSrc=GetDiskFreeSpace( chDriveLetter,
                              &dwSectorsPerCluster,
                              &dwBytesPerSector,
                              &dwFreeClusters,
                              &dwClusters);

    /* force 64 bit maths */
    i64TotalBytes = (__int64)dwClusters * dwSectorsPerCluster * dwBytesPerSector;
    i64FreeBytes = (__int64)dwFreeClusters * dwSectorsPerCluster * dwBytesPerSector;
  } /* endif */

  dwgle=GetLastError();
  SetErrorMode(errorMode);

  if (bGVIrc && bGDFSrc) {

    /* use simplified display routine with 64 bit types */
    sprintf(retstr->strptr,            // drive free total label
            "%c%c  %-12I64u %-12I64u %-13s",
            chDriveLetter[0], chDriveLetter[1],
            i64FreeBytes, i64TotalBytes, chVolumeName);
                                       /* create return string       */
    retstr->strlength = strlen(retstr->strptr);
  }
  else
    retstr->strlength = 0;             /* return null string         */

  return VALID_ROUTINE;                /* no error on call           */
}

/*************************************************************************
* Function:  SysDriveMap                                                 *
*                                                                        *
* Syntax:    call SysDriveMap [drive] [,mode]                            *
*                                                                        *
* Params:    drive - 'C', 'D', 'E', etc.  The drive to start the search  *
*                     with.                                              *
*            mode  - Any of the following:  'FREE', 'USED', 'DETACHED',  *
*                                           'LOCAL', 'REMOTE'            *
*                                                                        *
* Return:    'A: B: C: D: ...'                                           *
*************************************************************************/

size_t RexxEntry SysDriveMap(const char *name, size_t numargs, CONSTRXSTRING args[], const char *queuename, PRXSTRING retstr)
{

  CHAR     temp[MAX];                  /* Entire drive map built here*/

  CHAR     tmpstr[MAX];                /* Single drive entries built */
                                       /* here                       */
  CHAR     DeviceName[4];              /* Device name or drive letter*/
                                       /* string                     */
  DWORD    DriveMap;                   /* Drive map                  */
  ULONG    Ordinal;                    /* Ordinal of entry in name   */
                                       /* list                       */
                                       /* required                   */
  ULONG    dnum;                       /* Disk num variable          */
  ULONG    start = 3;                  /* Initial disk num           */
  ULONG    Mode = USED;                /* Query mode USED, FREE,     */
                                       /* LOCAL, etc                 */
  LONG     rc;                         /* OS/2 return codes          */
  UINT     errorMode;

  Ordinal = (ULONG )0;

  temp[0] = '\0';

  if (numargs > 2)                     /* validate arguments         */
    return INVALID_ROUTINE;
                                       /* check starting drive letter*/
  if (numargs >= 1 && args[0].strptr) {

    if ((strlen(args[0].strptr) == 2 &&
        args[0].strptr[1] != ':') ||
        strlen(args[0].strptr) > 2 ||
        strlen(args[0].strptr) == 0)
      return INVALID_ROUTINE;
    start = toupper(args[0].strptr[0])-'A'+1;
  }
  if (start < 1 ||                     /* is it in range?            */
      start > 26)
    return INVALID_ROUTINE;
                                       /* check the mode             */
  if (numargs == 2 && args[1].strlength != 0) {

    if (!stricmp(args[1].strptr, "FREE"))
      Mode = FREE;
    else if (!stricmp(args[1].strptr, "USED"))
      Mode = USED;
    else if (!stricmp(args[1].strptr, "RAMDISK"))
      Mode = RAMDISK;
    else if (!stricmp(args[1].strptr, "REMOTE"))
      Mode = REMOTE;
    else if (!stricmp(args[1].strptr, "LOCAL"))
      Mode = LOCAL;
    else if (!stricmp(args[1].strptr, "REMOVABLE"))
      Mode = REMOVABLE;
    else if (!stricmp(args[1].strptr, "CDROM"))
      Mode = CDROM;
    else
      return INVALID_ROUTINE;
  }
                                       /* perform the query          */
  DriveMap = GetLogicalDrives();
  DriveMap>>=start-1;                  /* Shift to the first drive   */
  temp[0] = '\0';                      /* Clear temporary buffer     */

  for (dnum = start; dnum <= 26; dnum++) {

                                       /* Hey, we have a free drive  */
    if (!(DriveMap&(DWORD)1) && Mode == FREE) {
      sprintf(tmpstr, "%c: ", dnum+'A'-1);
      strcat(temp, tmpstr);
    }
                                       /* Hey, we have a used drive  */
    else if ((DriveMap&(DWORD)1) && Mode == USED) {
      sprintf(tmpstr, "%c: ", dnum+'A'-1);
      strcat(temp, tmpstr);
    }

    else if (DriveMap&(DWORD)1) {      /* Check specific drive info  */
      sprintf(DeviceName, "%c:\\", dnum+'A'-1);

      errorMode = SetErrorMode(SEM_FAILCRITICALERRORS);
      rc = (LONG)GetDriveType(DeviceName);
      SetErrorMode(errorMode);
      #ifdef UNDELETE
      DataBufferLen = sizeof DataBuffer;
      DosQueryFSAttach(DeviceName, Ordinal, FSAInfoLevel,
          &DataBuffer, &DataBufferLen);
      rc = DosQueryFSInfo(dnum, 2, buf, sizeof(buf));
      #endif

      if (rc == DRIVE_REMOVABLE && Mode == REMOVABLE) {
                                       /* Hey, we have a removable   */
                                       /* drive                      */
        sprintf(tmpstr, "%c: ", dnum+'A'-1);
        strcat(temp, tmpstr);
      }

      else if (rc == DRIVE_CDROM && Mode == CDROM) {
        sprintf(tmpstr, "%c: ", dnum+'A'-1);
        strcat(temp, tmpstr);
      }

      else if (rc == DRIVE_FIXED && Mode == LOCAL) {
        sprintf(tmpstr, "%c: ", dnum+'A'-1);
        strcat(temp, tmpstr);
      }

      else if (rc == DRIVE_REMOTE && Mode == REMOTE) {
        sprintf(tmpstr, "%c: ", dnum+'A'-1);
        strcat(temp, tmpstr);
      }

      else if (rc == DRIVE_RAMDISK && Mode == RAMDISK) {
        sprintf(tmpstr, "%c: ", dnum+'A'-1);
        strcat(temp, tmpstr);
      }
    }
    DriveMap>>=1;                      /* Shift to the next drive    */
  }

  BUILDRXSTRING(retstr, temp);         /* pass back result           */
  if (retstr->strlength)               /* if not a null string       */
    retstr->strlength--;               /* Get rid of last space      */
  return VALID_ROUTINE;                /* no error on call           */
}


/*************************************************************************
* Function:  SysDropFuncs                                                *
*                                                                        *
* Syntax:    call SysDropFuncs                                           *
*                                                                        *
* Return:    NO_UTIL_ERROR - Successful.                                 *
*************************************************************************/

size_t RexxEntry SysDropFuncs(const char *name, size_t numargs, CONSTRXSTRING args[], const char *queuename, PRXSTRING retstr)
{
    // this is a NOP now
    retstr->strlength = 0;               /* set return value           */
    return VALID_ROUTINE;
}

/*************************************************************************
* Function:  SysFileDelete                                               *
*                                                                        *
* Syntax:    call SysFileDelete file                                     *
*                                                                        *
* Params:    file - file to be deleted.                                  *
*                                                                        *
* Return:    Return code from DeleteFile() function.                     *
*************************************************************************/

size_t RexxEntry SysFileDelete(const char *name, size_t numargs, CONSTRXSTRING args[], const char *queuename, PRXSTRING retstr)
{

  if (numargs != 1)                    /* we need one argument       */
    return INVALID_ROUTINE;            /* raise an error             */

  if (!DeleteFile(args[0].strptr))     /* delete the file            */
     RETVAL(GetLastError())            /* pass back return code      */
  else
     RETVAL(0)
}

/*************************************************************************
* Function:  SysFileSearch                                               *
*                                                                        *
* Syntax:    call SysFileSearch target, file, stem [, options]           *
*                                                                        *
* Params:    target  - String to search for.                             *
*            file    - Filespec to search.                               *
*            stem    - Stem variable name to place results in.           *
*            options - Any combo of the following:                       *
*                       'C' - Case sensitive search (non-default).       *
*                       'N' - Preceed each found string in result stem   *
*                              with it line number in file (non-default).*
*                                                                        *
* Return:    NO_UTIL_ERROR   - Successful.                               *
*            ERROR_NOMEM     - Out of memory.                            *
*************************************************************************/

size_t RexxEntry SysFileSearch(const char *name, size_t numargs, CONSTRXSTRING args[], const char *queuename, PRXSTRING retstr)
{

  CHAR        line[MAX_LINE_LEN];      /* Line read from file        */
  char       *ptr;                     /* Pointer to char str found  */
  ULONG       num = 0;                 /* Line number                */
  size_t      len;                     /* Length of string           */
  size_t      len2;                    /* Length of string           */
  ULONG       rc = 0;                  /* Return code of this func   */
  bool        linenums = false;        /* Set true for linenums in   */
                                       /* output                     */
  bool        sensitive = false;       /* Set true for case-sens     */
                                       /* search                     */
  RXSTEMDATA  ldp;                     /* stem data                  */
  char       *buffer_pointer;          /* current buffer pointer     */
  GetFileData filedata;                /* file read information      */

  BUILDRXSTRING(retstr, NO_UTIL_ERROR);/* pass back result           */
                                       /* validate arguments         */
  if (numargs < 3 || numargs > 4 ||
      !RXVALIDSTRING(args[0]) ||
      !RXVALIDSTRING(args[1]) ||
      !RXVALIDSTRING(args[2]))
    return INVALID_ROUTINE;            /* raise an error             */

  buffer_pointer = NULL;               /* nothing in buffer          */

  const char *target = args[0].strptr;             /* get target pointer         */
  const char *file = args[1].strptr;               /* get file name              */

  if (numargs == 4) {                  /* process options            */
    const char *opts = args[3].strptr;     /* point to the options       */
    if (strstr(opts, "N") || strstr(opts, "n"))
      linenums = true;

    if (strstr(opts, "C") || strstr(opts, "c"))
      sensitive = true;
  }

                                       /* Initialize data area       */
  ldp.count = 0;
  strcpy(ldp.varname, args[2].strptr);
  ldp.stemlen = args[2].strlength;
                                       /* uppercase the name         */
  memupper(ldp.varname, strlen(ldp.varname));

  if (ldp.varname[ldp.stemlen-1] != '.')
    ldp.varname[ldp.stemlen++] = '.';

  if (MyOpenFile(file, &filedata)) {   /* open the file              */
    BUILDRXSTRING(retstr, ERROR_FILEOPEN);
    return VALID_ROUTINE;              /* finished                   */
  }
                                       /* do the search...found lines*/
                                       /* are saved in stem vars     */
  while (!GetLine(line, MAX_LINE_LEN - 1, &filedata)) {
    len = strlen(line);
    num++;
    ptr = mystrstr(line, target, len, args[0].strlength, sensitive);

    if (ptr != NULL) {

      if (linenums) {
        wsprintf(ldp.ibuf, "%d ", num);
        len2 = strlen(ldp.ibuf);
        memcpy(ldp.ibuf+len2, line, min(len, IBUF_LEN-len2));
        ldp.vlen = min(IBUF_LEN, len+len2);
      }
      else {
        memcpy(ldp.ibuf, line, len);
        ldp.vlen = len;
      }
      ldp.count++;
      ltoa((long)ldp.count, ldp.varname+ldp.stemlen, 10);

      if (ldp.ibuf[ldp.vlen-1] == '\n')
        ldp.vlen--;
      ldp.shvb.shvnext = NULL;
      ldp.shvb.shvname.strptr = ldp.varname;
      ldp.shvb.shvname.strlength = strlen(ldp.varname);
      ldp.shvb.shvnamelen = ldp.shvb.shvname.strlength;
      ldp.shvb.shvvalue.strptr = ldp.ibuf;
      ldp.shvb.shvvalue.strlength = ldp.vlen;
      ldp.shvb.shvvaluelen = ldp.vlen;
      ldp.shvb.shvcode = RXSHV_SET;
      ldp.shvb.shvret = 0;
      if (RexxVariablePool(&ldp.shvb) == RXSHV_BADN) {
        CloseFile(&filedata);          /* close the file             */
        return INVALID_ROUTINE;        /* error on non-zero          */
      }
    }
  }

  CloseFile(&filedata);                /* Close that file            */
                                       /* set stem.0 to lines read   */
  ltoa((long)ldp.count, ldp.ibuf, 10);
  ldp.varname[ldp.stemlen] = '0';
  ldp.varname[ldp.stemlen+1] = 0;
  ldp.shvb.shvnext = NULL;
  ldp.shvb.shvname.strptr = ldp.varname;
  ldp.shvb.shvname.strlength = ldp.stemlen+1;
  ldp.shvb.shvnamelen = ldp.stemlen+1;
  ldp.shvb.shvvalue.strptr = ldp.ibuf;
  ldp.shvb.shvvalue.strlength = strlen(ldp.ibuf);
  ldp.shvb.shvvaluelen = ldp.shvb.shvvalue.strlength;
  ldp.shvb.shvcode = RXSHV_SET;
  ldp.shvb.shvret = 0;
  if (RexxVariablePool(&ldp.shvb) == RXSHV_BADN)
    return INVALID_ROUTINE;            /* error on non-zero          */

  return VALID_ROUTINE;                /* no error on call           */
}


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - *\
 *                                                                            *
 *   SysFileTree() implmentation and helper functions.                        *
 *                                                                            *
\* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/**
 * This is a SysFileTree specific function.
 *
 * @param c
 * @param pos
 * @param actual
 */
static void badSFTOptsException(RexxThreadContext *c, size_t pos, CSTRING actual)
{
    char buf[256] = {0};
    _snprintf(buf, sizeof(buf),
             "SysFileTree argument %d must be a combination of F, D, B, S, T, L, I, or O; found \"%s\"",
             pos, actual);

    c->RaiseException1(Rexx_Error_Incorrect_call_user_defined, c->String(buf));
}

/**
 * This is a SysFile specific function.
 *
 * @param c
 * @param pos
 * @param actual
 */
static void badMaskException(RexxThreadContext *c, size_t pos, CSTRING actual)
{
    char buf[256] = {0};
    _snprintf(buf, sizeof(buf),
             "SysFileTree argument %d must be 5 characters or less in length containing only '+', '-', or '*'; found \"%s\"",
             pos, actual);

    c->RaiseException1(Rexx_Error_Incorrect_call_user_defined, c->String(buf));
}

/**
 * Returns a value that is greater than 'need' by doubling 'have' until that
 * value is reached.
 */
inline size_t neededSize(size_t need, size_t have)
{
    while ( have < need )
    {
        have *= 2;
    }
    return have;
}

/**
 * Allocates a buffer that is at least twice as big as the buffer passed in.
 *
 * @param c             Call context we are operating in.
 * @param dBuf          Pointer to the buffer to reallocate
 * @param nBuf          Size of current dBuf buffer. Will be updated on return
 *                      to size of newly allocated buffer.
 * @param needed        Minimum size needed.
 * @param nStaticBuffer Size of original static buffer.
 *
 * @return True on success, false on memory allocation failure.
 *
 * @remarks  NOTE: that the pointer to the buffer to reallocate, may, or may
 *           not, be a pointer to a static buffer.  We must NOT try to free a
 *           static buffer and we MUST free a non-static buffer.
 */
static bool getBiggerBuffer(RexxCallContext *c, char **dBuf, size_t *nBuf, size_t needed, size_t nStaticBuffer)
{
    if ( *nBuf != nStaticBuffer )
    {
        LocalFree(*dBuf);
    }

    *nBuf = neededSize(needed, *nBuf);
    *dBuf = (char *)LocalAlloc(LPTR, *nBuf * sizeof(char));

    if ( *dBuf == NULL )
    {
        outOfMemoryException(c->threadContext);
        return false;
    }

    return true;
}

/**
 * Checks that attr is the same as that specified by the mask.
 *
 * @param mask
 * @param attr
 * @param options
 *
 * @return True for a match, otherwise false.
 */
static bool sameAttr(int32_t *mask, uint32_t attr, uint32_t options)
{
    if ( (options & DO_DIRS) && ! (options & DO_FILES) && ! (attr & FILE_ATTRIBUTE_DIRECTORY) )
    {
        return false;
    }
    if ( ! (options & DO_DIRS) && (options & DO_FILES) && (attr & FILE_ATTRIBUTE_DIRECTORY) )
    {
        return false;
    }
    if ( mask[0] == RXIGNORE )
    {
        return  true;
    }

    if ( mask[0] < 0 && attr & FILE_ATTRIBUTE_ARCHIVE )
    {
        return  false;
    }
    if ( mask[0] > 0 && ! (attr & FILE_ATTRIBUTE_ARCHIVE) )
    {
        return  false;
    }
    if ( mask[1] < 0 && attr & FILE_ATTRIBUTE_DIRECTORY )
    {
        return  false;
    }
    if ( mask[1] > 0 && ! (attr & FILE_ATTRIBUTE_DIRECTORY) )
    {
        return  false;
    }
    if ( mask[2] < 0 && attr & FILE_ATTRIBUTE_HIDDEN )
    {
        return  false;
    }
    if (mask[2] > 0 && ! (attr & FILE_ATTRIBUTE_HIDDEN) )
    {
        return  false;
    }
    if (mask[3] < 0 && attr & FILE_ATTRIBUTE_READONLY )
    {
        return  false;
    }
    if (mask[3] > 0 && ! (attr & FILE_ATTRIBUTE_READONLY) )
    {
        return  false;
    }
    if (mask[4] < 0 && attr & FILE_ATTRIBUTE_SYSTEM )
    {
        return  false;
    }
    if (mask[4] > 0 && ! (attr & FILE_ATTRIBUTE_SYSTEM) )
    {
        return  false;
    }

    return  true;
}

/**
 * Returns a new file attribute value given a mask of attributes to be changed
 * and the current attribute value.
 *
 * @param mask
 * @param attr
 *
 * @return New attribute value.
 */
static uint32_t newAttr(int32_t *mask, uint32_t attr)
{
    if ( mask[0] == RXIGNORE )
    {
      return  attr;
    }

    if ( mask[0] < 0 )
    {
        attr &= ~FILE_ATTRIBUTE_ARCHIVE;   // Clear
    }
    if ( mask[0] > 0 )
    {
        attr |= FILE_ATTRIBUTE_ARCHIVE;    // Set
    }
    if ( mask[1] < 0 )
    {
        attr &= ~FILE_ATTRIBUTE_DIRECTORY; // Clear
    }
    if ( mask[1] > 0 )
    {
        attr |= FILE_ATTRIBUTE_DIRECTORY;  // Set
    }
    if ( mask[2] < 0 )
    {
        attr &= ~FILE_ATTRIBUTE_HIDDEN;    // Clear
    }
    if ( mask[2] > 0 )
    {
        attr |= FILE_ATTRIBUTE_HIDDEN;     // Set
    }
    if ( mask[3] < 0 )
    {
        attr &= ~FILE_ATTRIBUTE_READONLY;  // Clear
    }
    if ( mask[3] > 0 )
    {
        attr |= FILE_ATTRIBUTE_READONLY;   // Set
    }
    if ( mask[4] < 0 )
    {
        attr &= ~FILE_ATTRIBUTE_SYSTEM;    // Clear
    }
    if ( mask[4] > 0 )
    {
        attr |= FILE_ATTRIBUTE_SYSTEM;     // Set
    }

    return  attr;
}

/**
 * Changes the file attributes of the specified file to those specified by attr.
 *
 * @param file  File to change the attributes of.
 *
 * @param attr  New file attributes.
 *
 * @return True on success, false on error.
 *
 * @remarks  Note that this function was named SetFileMode() in the old IBM
 * code.
 */
static bool setAttr(const char *file, uint32_t attr)
{
    if ( SetFileAttributes(file, attr) == 0 )
    {
        return false;
    }
    return true;
}


/**
 * This function is used by SysFileTree only.
 *
 * Formats the line for a found file and adds it to the stem containing all the
 * found files.
 *
 * @param c
 * @parm  path
 * @param treeData
 * @param newMask
 * @param options
 * @param wfd
 *
 * @return True on success, false on error.
 *
 * @remarks  We try to use the static buffers in treeData, but if they are not
 *  big enough, we allocate memory.  If we do allocate memory, we have to free
 *  it of course.  We can determine if the memory needs to be freed by checking
 *  that either nFoundFile, or nFoundFileLine, are the same size as they are
 *  originally set to, or not.
 *
 *  If the file search is a very deep recursion in the host file system, a very
 *  large number of String objects may be created in the single Call context of
 *  SysFileTree.  A reference to each created object is saved in a hash table to
 *  protect it from garbage collection, which can lead to a very large hash
 *  table.  To prevent the creation of a very large hash table, we create a temp
 *  object, pass that object to the interpreter, and then tell the interpreter
 *  the object no longer needs to be protected in this call context.
 */
static bool formatFile(RexxCallContext *c, char *path, RXTREEDATA *treeData, int32_t *newMask,
                       uint32_t options, WIN32_FIND_DATA *wfd)
{
    SYSTEMTIME systime;
    FILETIME   ftLocal;

    char   *dFoundFile = treeData->foundFile;
    size_t  nFoundFile = sizeof(treeData->foundFile);

    int len = _snprintf(dFoundFile, nFoundFile, "%s%s", path, wfd->cFileName);
    if ( len < 0 || len == nFoundFile )
    {
        nFoundFile = strlen(path) + strlen(wfd->cFileName) + 1;
        dFoundFile = (char *)LocalAlloc(LPTR, nFoundFile);
        if ( dFoundFile == NULL )
        {
            outOfMemoryException(c->threadContext);
            return false;
        }

        // Buffer is sure to be big enough now, we we don't check the return.
        _snprintf(dFoundFile, nFoundFile, "%s%s", path, wfd->cFileName);
    }

    if ( options & NAME_ONLY )
    {
        RexxStringObject t = c->String(dFoundFile);

        // Add the file name to the stem and be done with it.
        treeData->count++;
        c->SetStemArrayElement(treeData->files, treeData->count, t);
        c->ReleaseLocalReference(t);

        if ( nFoundFile != sizeof(treeData->foundFile) )
        {
            LocalFree(dFoundFile);
        }
        return true;
    }

    // The file attributes need to be changed before we format the found file
    // line.

    uint32_t changedAttr = newAttr(newMask, wfd->dwFileAttributes);
    if ( changedAttr != wfd->dwFileAttributes )
    {
        // try to set the attributes, but if it fails, just use the exsiting.
        if ( ! setAttr(treeData->foundFile, changedAttr & ~FILE_ATTRIBUTE_DIRECTORY) )
        {
            changedAttr = wfd->dwFileAttributes;
        }
    }

    // Convert UTC to local file time, and then to system format.
    FileTimeToLocalFileTime(&wfd->ftLastWriteTime, &ftLocal);
    FileTimeToSystemTime(&ftLocal, &systime);

    // The fileTime buffer is 64 bytes, and the fileAtt buffer is 16 bytes.
    // Since we can count the characters put into the buffer here, there is
    // no need to check for buffer overflow.

    if ( options & LONG_TIME )
    {
        sprintf(treeData->fileTime, "%4d-%02d-%02d %02d:%02d:%02d  %10lu  ",
                systime.wYear,
                systime.wMonth,
                systime.wDay,
                systime.wHour,
                systime.wMinute,
                systime.wSecond,
                wfd->nFileSizeLow);
    }
    else
    {
        if ( options & EDITABLE_TIME )
        {
            sprintf(treeData->fileTime, "%02d/%02d/%02d/%02d/%02d  %10lu  ",
                    (systime.wYear + 100) % 100,
                    systime.wMonth,
                    systime.wDay,
                    systime.wHour,
                    systime.wMinute,
                    wfd->nFileSizeLow);
        }
        else
        {
            sprintf(treeData->fileTime, "%2d/%02d/%02d  %2d:%02d%c  %10lu  ",
                    systime.wMonth,
                    systime.wDay,
                    (systime.wYear + 100) % 100,
                    (systime.wHour < 13 && systime.wHour != 0 ?
                     systime.wHour : (abs(systime.wHour - (SHORT)12))),
                    systime.wMinute,
                    (systime.wHour < 12 || systime.wHour == 24) ? 'a' : 'p',
                    wfd->nFileSizeLow);
        }
    }

    sprintf(treeData->fileAttr, "%c%c%c%c%c  ",
           (changedAttr & FILE_ATTRIBUTE_ARCHIVE)   ? 'A' : '-',
           (changedAttr & FILE_ATTRIBUTE_DIRECTORY) ? 'D' : '-',
           (changedAttr & FILE_ATTRIBUTE_HIDDEN)    ? 'H' : '-',
           (changedAttr & FILE_ATTRIBUTE_READONLY)  ? 'R' : '-',
           (changedAttr & FILE_ATTRIBUTE_SYSTEM)    ? 'S' : '-');

    // Now format the complete line, allocating memory if we have to.

    char   *dFoundFileLine = treeData->foundFileLine;
    size_t  nFoundFileLine = sizeof(treeData->foundFileLine);

    len = _snprintf(dFoundFileLine, nFoundFileLine, "%s%s%s",
                    treeData->fileTime, treeData->fileAttr, dFoundFile);
    if ( len < 0 || len == nFoundFileLine )
    {
        nFoundFileLine = strlen(treeData->fileTime) + strlen(treeData->fileAttr) + nFoundFile + 1;
        dFoundFileLine = (char *)LocalAlloc(LPTR, nFoundFileLine);

        if ( dFoundFileLine == NULL )
        {
            outOfMemoryException(c->threadContext);
            if ( nFoundFile != sizeof(treeData->foundFile) )
            {
                LocalFree(dFoundFile);
            }
            return false;
        }
        // Buffer is sure to be big enough now so we don't check return.
        _snprintf(dFoundFileLine, nFoundFileLine, "%s%s%s", treeData->fileTime, treeData->fileAttr, dFoundFile);
    }

    // Place found file line in the stem.
    RexxStringObject t = c->String(dFoundFileLine);

    treeData->count++;
    c->SetStemArrayElement(treeData->files, treeData->count, t);
    c->ReleaseLocalReference(t);

    if ( nFoundFile != sizeof(treeData->foundFile) )
    {
        LocalFree(dFoundFile);
    }
    if ( nFoundFileLine != sizeof(treeData->foundFileLine) )
    {
        LocalFree(dFoundFileLine);
    }

    return true;
}

/**
 * Finds all files matching a file specification, formats a file name line and
 * adds the formatted line to a stem.  Much of the data to complete this
 * operation is contained in the treeData struct.
 *
 * This is a recursive function that may search through subdirectories if the
 * recurse option is used.
 *
 * @param c           Call context we are operating in.
 *
 * @param path        Current directory we are searching.
 *
 * @param treeData    Struct containing data pertaining to the search, such as
 *                    the file specification we are searching for, the stem to
 *                    put the results in, etc..
 *
 * @param targetMask  An array of integers which describe the source attribute
 *                    mask.  Only files with attributes matching this mask will
 *                    be found.
 *
 * @param newMask     An array of integers which describe the target attribute
 *                    mask.  Attributes of all found files will be changed / set
 *                    to the values specified by this mask.
 * @param options
 *
 * @return uint32_t
 *
 * @remarks  For both targetMask and newMask, each index of the mask corresponds
 *           to a different file attribute.  Each index and its associated
 *           attribute are as follows:
 *
 *                        mask[0] = FILE_ARCHIVED
 *                        mask[1] = FILE_DIRECTORY
 *                        mask[2] = FILE_HIDDEN
 *                        mask[3] = FILE_READONLY
 *                        mask[4] = FILE_SYSTEM
 *
 *           A negative value at a given index indicates that the attribute bit
 *           of the file is not set.  A positive number indicates that the
 *           attribute should be set. A value of 0 indicates a "Don't Care"
 *           setting.
 *
 *           A close reading of MSDN seems to indicate that as long as we are
 *           compiled for ANSI, which we are, that MAX_PATH is sufficiently
 *           large.  But, we will code for the possibility that it is not large
 *           enough, by mallocing dynamic memory if _snprintf indicates a
 *           failure.
 *
 *           We point dTmpFileName at the static buffer and nTmpFileName is set
 *           to the size of the buffer.  If we have to allocate memory,
 *           nTmpFileName will be set to the size we allocate and if
 *           nTmpFileName does not equal what it is originally set to, we know
 *           we have to free the allocated memory.
 */
static bool recursiveFindFile(RexxCallContext *c, char *path, RXTREEDATA *treeData,
                              int32_t *targetMask, int32_t *newMask, uint32_t options)
{
  WIN32_FIND_DATA  wfd;
  HANDLE           fHandle;
  char             tmpFileName[FNAMESPEC_BUF_LEN];
  char            *dTmpFileName = tmpFileName;       // Dynamic memory for tmpFileName, static memory to begin with.
  size_t           nTmpFileName = FNAMESPEC_BUF_LEN; // CouNt of bytes in dTmpFileName.
  int32_t          len;
  bool             result = true;

  len = _snprintf(dTmpFileName, nTmpFileName, "%s%s", path, treeData->dFNameSpec);
  if ( len < 0 || len == nTmpFileName )
  {
      nTmpFileName = strlen(path) + strlen(treeData->dFNameSpec) + 1;
      dTmpFileName = (char *)LocalAlloc(LPTR, nTmpFileName);
      if ( dTmpFileName == NULL )
      {
          outOfMemoryException(c->threadContext);
          result = false;
          goto done_out;
      }
      // buffer is sure to be big enough now, so we don't check the return.
      _snprintf(dTmpFileName, nTmpFileName, "%s%s", path, treeData->dFNameSpec);
  }

  fHandle = FindFirstFile(dTmpFileName, &wfd);
  if ( fHandle != INVALID_HANDLE_VALUE )
  {
      do
      {
          // Skip dot directories
          if ( strcmp(wfd.cFileName, ".") == 0 || strcmp(wfd.cFileName, "..") == 0 )
          {
              continue;
          }

          if ( sameAttr(targetMask, wfd.dwFileAttributes, options) )
          {
              if ( ! formatFile(c, path, treeData, newMask, options, &wfd) )
              {
                  FindClose(fHandle);
                  result = false;
                  goto done_out;
              }
          }
      } while ( FindNextFile(fHandle, &wfd) );

      FindClose(fHandle);
  }

  if ( options & RECURSE )
  {
      // Build new target spec.  Above, path + fileSpec fit into tmpFileName,
      // fileSpec is always longer than 1 character, so we are okay here.
      sprintf(dTmpFileName, "%s*", path);

      fHandle = FindFirstFile(dTmpFileName, &wfd);
      if ( fHandle != INVALID_HANDLE_VALUE )
      {
          do
          {
              // Skip non-directories and dot directories.
              if ( ! (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) ||
                   strcmp(wfd.cFileName, ".") == 0 || strcmp(wfd.cFileName, "..") == 0 )
              {
                  continue;
              }

              // Build the new directory file name.
              len = _snprintf(dTmpFileName, nTmpFileName, "%s%s\\", path, wfd.cFileName);
              if ( len < 0 || len == nTmpFileName )
              {
                  // We may need to free dTmpFileName if it is now allocated
                  // memory.
                  if ( nTmpFileName != FNAMESPEC_BUF_LEN )
                  {
                      LocalFree(dTmpFileName);
                  }

                  nTmpFileName = strlen(path) + strlen(wfd.cFileName) + 2;
                  dTmpFileName = (char *)LocalAlloc(LPTR, nTmpFileName);
                  if ( dTmpFileName == NULL )
                  {
                      outOfMemoryException(c->threadContext);
                      FindClose(fHandle);
                      result = false;
                      goto done_out;
                  }
                  // buffer is sure to be big enough now, so we don't check the
                  // return.
                  _snprintf(dTmpFileName, nTmpFileName, "%s%s\\", path, wfd.cFileName);
              }

              // Search the next level.
              if ( ! recursiveFindFile(c, tmpFileName, treeData, targetMask, newMask, options) )
              {
                  FindClose(fHandle);
                  result = false;
                  goto done_out;
              }
          }
          while (FindNextFile(fHandle, &wfd));

          FindClose(fHandle);
      }
  }

done_out:

    if ( nTmpFileName != FNAMESPEC_BUF_LEN )
    {
        safeLocalFree(dTmpFileName);
    }
    return result;
}

/**
 * This is a SysFileTree() specific function.  It is only called, indirectly
 * through getPath(), from SysFileTree().
 *
 * This function mimics the old IBM code.
 *
 * Leading spaces are stripped, in some cases. A file specification of "." is
 * changed to "*.*" and a file specification of ".." is changed to "..\*.*"
 *
 * Leading spaces in fSpec are stripped IFF the first character(s) after the
 * leading spaces:
 *
 *       is '\' or '/'
 *     or
 *       is '.\' or './'
 *     or
 *       is '..\' or '../'
 *     or
 *        is z:  (i.e., a drive letter)
 *
 * @param fSpec  The SysFileTree search specification
 *
 * @return A pointer to fSpec, possibly adjust to point to the first non-space
 *         character in the string.
 *
 * @side effects:  fSpec may be changed from "." to "*.*" or may be changed from
 *                 ".." to "..\*.*"
 *
 * @assumes:  The buffer for fSpec is large enough for the possible changes.
 */
static char *adjustFSpec(char *fSpec)
{
    size_t i = 0;

    // Skip leading blanks.
    while ( fSpec[i] == ' ' )
    {
        i++;
    }

    if ( i > 0 )
    {
        size_t len = strlen(fSpec);

        // This series of if statements could be combined in to on huge if, but
        // this is easier to comprehend:
        if ( fSpec[i] == '\\' || fSpec[i] == '/' )                         // The "\" case
        {
            fSpec = &fSpec[i];
        }
        else if ( fSpec[i] == '.' )
        {
            if ( i + 1 < len )
            {
                if ( fSpec[i + 1] == '\\' || fSpec[i + 1] == '/' )         // The ".\" case
                {
                    fSpec = &fSpec[i];
                }
                else if ( i + 2 < len )
                {
                    if ( fSpec[i + 1] == '.' &&
                         (fSpec[i + 2] == '\\' || fSpec[i + 2] == '/') )   // The "..\" case
                    {
                        fSpec = &fSpec[i];
                    }
                }
            }
        }
        else if ( i + 1 < len && fSpec[i + 1] == ':' )                     // The "z:' case
        {
            fSpec = &fSpec[i];
        }
    }

    if ( strcmp(fSpec, ".") == 0 )
    {
        // If fSpec is exactly "." then change it to "*.*"
        strcpy(fSpec, "*.*");
    }
    else if ( strcmp(fSpec, "..") == 0 )
    {
        // Else if fSpec is exactly ".." then change it to "..\*.*"
        strcpy(fSpec, "..\\*.*");
    }

    return fSpec;
}

static bool safeGetCurrentDirectory(RexxCallContext *c, char **pPath, size_t *pPathLen)
{
    size_t  pathLen = *pPathLen;
    char   *path    = *pPath;

    // Get the current directory.  First check that the path buffer is large
    // enough.
    uint32_t ret = GetCurrentDirectory(0, 0);
    if ( ret == 0 )
    {
        systemServiceExceptionCode(c->threadContext, "GetCurrentDirectory", ret);
        return false;
    }

    path = (char *)LocalAlloc(LPTR, ret);
    if ( path == NULL )
    {
        outOfMemoryException(c->threadContext);
        return false;
    }

    // Fix up our input path / pathLen variables now.  The input path buffer
    // is allocated memory, so we need to free it.
    LocalFree(*pPath);
    *pPath    = path;
    *pPathLen = ret;

    ret = GetCurrentDirectory(ret, path);
    if ( ret == 0 )
    {
        systemServiceExceptionCode(c->threadContext, "GetCurrentDirectory", ret);
        return false;
    }

    return true;
}

static bool expandNonPath2fullPath(RexxCallContext *c, char *fSpec, char **pPath, size_t *pPathLen, int *lastSlashPos)
{
    char     *buf    = NULL;  // used to save current dir
    char      drv[3] = {0};   // used to change current drive
    uint32_t  ret    = 0;
    bool      result = false;

    // fSpec could be a drive designator.
    if ( fSpec[1] == ':' )
    {
        // Save the current drive and path, first get needed buffer size.
        ret = GetCurrentDirectory(0, 0);
        if ( ret == 0 )
        {
            systemServiceExceptionCode(c->threadContext, "GetCurrentDirectory", ret);
            goto done_out;
        }

        buf = (char *)LocalAlloc(LPTR, ret);
        if ( buf == NULL )
        {
            outOfMemoryException(c->threadContext);
            goto done_out;
        }

        ret = GetCurrentDirectory(ret, buf);
        if ( ret == 0 )
        {
            systemServiceExceptionCode(c->threadContext, "GetCurrentDirectory", ret);
            goto done_out;
        }

        // Just copy the drive letter and the colon, omit the rest.  This is
        // necessary e.g. for something like "I:*"
        memcpy(drv, fSpec, 2);

        // Change to the specified drive, get the current directory, then go
        // back to where we came from.
        SetCurrentDirectory(drv);
        bool success = safeGetCurrentDirectory(c, pPath, pPathLen);

        SetCurrentDirectory(buf);
        LocalFree(buf);

        if ( ! success )
        {
            systemServiceExceptionCode(c->threadContext, "GetCurrentDirectory", ret);
            goto done_out;
        }

        // make drive the path
        *lastSlashPos = 1;
    }
    else
    {
        // No drive designator, just get the current directory.
        if ( ! safeGetCurrentDirectory(c, pPath, pPathLen) )
        {
            goto done_out;
        }
    }

    // If we need a trailing slash, add one.
    char *path = *pPath;
    if ( path[strlen(path) - 1] != '\\' )
    {
        strcat(path, "\\");
    }

    result = true;

done_out:
    safeLocalFree(buf);
    return result;
}


/**
 * Splits the path portion off from fSpec and returns it in the path buffer.
 *
 * When this function is called, there is always at least one slash in fSpec.
 *
 * @param c
 * @param fSpec
 * @param lastSlashPos
 * @param pPath
 * @param pPathLen
 *
 * @return bool
 *
 * @remarks  The size of the path buffer is guarenteed to be at least the string
 *           length of fSpec + FNAMESPEC_BUF_EXTRA (8) in size.  Or MAX (264)
 *           bytes in size.  Whichever is larger.  So path is big enough to
 *           contain all of fSpec + 7 characters.
 *
 *           We may have to enlarge the passed in path buffer.  If we do, we
 *           need to be sure and update the path buffer pointer and the path
 *           length. As long as we keep pPath and pPathLen correct, the caller
 *           will take care of freeing any memory.
 *
 *           But if we do change pPath, we need to free the buffer it was
 *           pointing to.
 */
static bool expandPath2fullPath(RexxCallContext *c, char *fSpec, size_t lastSlashPos, char **pPath, size_t *pPathLen)
{
    size_t l = 0;    // Used to calculate lengths of strings.

    char   *path    = *pPath;
    size_t  pathLen = *pPathLen;

    // If fSpec starts with a drive designator, then we have a full path. Copy
    // over the path portion, including the last slash, and null terminate it.
    if (fSpec[1] == ':')
    {
        l = lastSlashPos + 1;
        memcpy(path, fSpec, l);
        path[l] = '\0';
    }
    else
    {
        char fpath[MAX_PATH];
        char drive[_MAX_DRIVE];
        char dir[_MAX_DIR];
        char fname[_MAX_FNAME];
        char ext[_MAX_EXT];
        char lastChar;

        // fpath is the only buffer we need to worry about being too small.
        // Although, when compiled for ANSI, which we are, I really think it is
        // impossible for MAX_PATH to be too small.
        char   *dFPath = fpath;
        size_t  nFPath = MAX_PATH;

        if ( lastSlashPos == 0 )
        {
            // Only 1 slash at the beginning, get the full path.
            _fullpath(dFPath, "\\", nFPath);

            l = strlen(dFPath) + strlen(&fSpec[1]) + 1;
            if ( l > nFPath )
            {
                if ( ! getBiggerBuffer(c, &dFPath, &nFPath, l, nFPath) )
                {
                    return false;
                }
            }

            strcat(dFPath, &fSpec[1]);
        }
        else
        {
            // Chop off the path part by putting a null at the last slash, get
            // the full path, and then put the slash back.
            fSpec[lastSlashPos] = '\0';
            if ( _fullpath(dFPath, fSpec, nFPath) == NULL )
            {
                // This will double the buffer until either _fullpath()
                // succeeds, or we run out of memory.  If we go through the loop
                // more than once, and fail, we need to free memory allocated
                // for dFPath.  We fix fSpec on failure, but that is not really
                // needed, the caller(s) will just quit on failure of this
                // function.
                do
                {
                    if ( ! getBiggerBuffer(c, &dFPath, &nFPath, l, MAX_PATH) )
                    {
                        if ( nFPath != MAX_PATH )
                        {
                            LocalFree(dFPath);
                        }

                        fSpec[lastSlashPos] = '\\';
                        return false;
                    }
                } while ( _fullpath(dFPath, fSpec, nFPath) == NULL );
            }

            fSpec[lastSlashPos] = '\\';

            lastChar = dFPath[strlen(dFPath) - 1];
            if (lastChar != '\\' && lastChar != '/')
            {
                l = strlen(dFPath) + strlen(&fSpec[lastSlashPos]) + 1;
                if ( l > nFPath )
                {
                    if ( ! getBiggerBuffer(c, &dFPath, &nFPath, l, MAX_PATH) )
                    {
                        // If dFPath was allocated, free it.
                        if ( nFPath != MAX_PATH )
                        {
                            LocalFree(dFPath);
                        }
                        return false;
                    }
                }

                strcat(dFPath, &fSpec[lastSlashPos]);
            }
        }

        _splitpath(dFPath, drive, dir, fname, ext);

        l = strlen(drive) + strlen(dir) + 1;
        if ( l > pathLen )
        {
            if ( ! getBiggerBuffer(c, &path, &pathLen, l, pathLen) )
            {
                return false;
            }

            LocalFree(*pPath);
            *pPath    = path;
            *pPathLen = pathLen;
        }

        strcpy(path, drive);
        strcat(path, dir);

        // If path is invalid, (the empty string,) for some reason, copy the
        // path from fSpec.  That is from the start of the string up through the
        // last slash.  Then zero terminate it.  The path buffer is guaranteed
        // big enough for this, see the remarks.
        if ( strlen(path) == 0 )
        {
            memcpy(path, fSpec, lastSlashPos + 1);
            path[lastSlashPos + 1] = '\0';
        }

        // If we need a trailing slash, add it.  Again, the path buffer is
        // guaranteed to be big enough.
        if (path[strlen(path) - 1] != '\\')
        {
            strcat(path, "\\");
        }
    }

    return true;
}

/**
 * This is a SysFileTree() specific function..
 *
 * This function expands the file spec passed in to the funcition into its full
 * path name.  The full path name is then split into the path portion and the
 * file name portion.  The path portion is then returned in path and the file
 * name portion is returned in fileName.
 *
 * The path portion will end with the '\' char if fSpec contains a path.
 *
 * @param fSpec
 * @param path       Pointer to path buffer.  Path buffer is allocated memory,
 *                   not a static buffer.
 * @param filename
 * @param pathLen    Pointer to size of the path buffer.
 *
 * @remarks  On entry, the buffer pointed to by fSpec is guaranteed to be at
 *           least strlen(fSpec) + FNAMESPEC_BUF_EXTRA (8).  So, we can strcat
 *           to it at least 7 characters and still have it null terminated.
 *
 *           In addition, the path buffer is guarenteed to be at least that size
 *           also.
 */
static bool getPath(RexxCallContext *c, char *fSpec, char **path, char *filename, size_t *pathLen)
{
    size_t len;                     // length of filespec
    int    lastSlashPos;            // position of last slash

    fSpec = adjustFSpec(fSpec);

    // Find the position of the last slash in fSpec
    len = strlen(fSpec);

    // Get the maximum position of the last '\'
    lastSlashPos = (int)len;

    // Step back through fSpec until at its beginning or at a '\' or '/' character
    while ( fSpec[lastSlashPos] != '\\' && fSpec[lastSlashPos] != '/' && lastSlashPos >= 0 )
    {
        --lastSlashPos;
    }

    // If lastSlashPos is less than 0, then there is no backslash present in
    // fSpec.
    if ( lastSlashPos < 0 )
    {
        if ( ! expandNonPath2fullPath(c, fSpec, path, pathLen, &lastSlashPos) )
        {
            return false;
        }
    }
    else
    {
        if ( ! expandPath2fullPath(c, fSpec, lastSlashPos, path, pathLen) )
        {
            return false;
        }
    }

    // Get the file name from fSpec, the portion just after the last '\'
    if ( fSpec[lastSlashPos + 1] != '\0' )
    {
        // The position after the last slash is not the null terminator so there
        // is something to copy over to the file name segment.
        strcpy(filename, &fSpec[lastSlashPos + 1]);
    }
    else
    {
        // The last slash is the last character in fSpec, just use wildcards for
        // the file name segment.
        strcpy(filename, "*.*");
    }

    return true;
}

/**
 * This is a SysFileTree specific function.
 *
 * Determines the options by converting the character based argument to the
 * correct set of flags.
 *
 * @param c
 * @param opts
 * @param pOpts
 *
 * @return bool
 */
static bool goodOpts(RexxCallContext *c, char *opts, uint32_t *pOpts)
{
    uint32_t options = *pOpts;

    while ( *opts )
    {
        switch( toupper(*opts) )
        {
          case 'S':                      // recurse into subdirectories
              options |= RECURSE;
              break;

          case 'O':                      // only return names
              options |= NAME_ONLY;
              break;

          case 'T':                      // use short time format, ignored if L is used
            options |= EDITABLE_TIME;
            break;

          case 'L':                      // use long time format
              options |= LONG_TIME;
              break;

          case 'F':                      // include only files
              options &= ~DO_DIRS;
              options |= DO_FILES;
              break;

          case 'D':                      // include only directories
              options |= DO_DIRS;
              options &= ~DO_FILES;
              break;

          case 'B':                      // include both files and directories
              options |= DO_DIRS;
              options |= DO_FILES;
              break;

          case 'I':                      // case insensitive? no op on Windows
              break;

          default:                       // error, unknown option
            return false;
        }
        opts++;
    }

    *pOpts = options;
    return true;
}

/**
 * This is a SysFileTree() specific helper function.
 *
 * Set a mask of unsigned ints to what is specified by a mask of chars.
 *
 * @param c
 * @param msk
 * @param mask
 *
 * @return True on success, false on error.
 *
 * @remarks  If a character in position N is a '+' then the unsigned int at
 *           position N is set to 1.  This is turning it on.
 *
 *           If a character in position N is a '-' then the unsigned int at
 *           position N is set to -1.  This is turning it off.
 *
 *           If a character in position N is a '*' then the unsigned int at
 *           position N is set to 0.  This is saying ignore it, it doesn't
 *           matter what the attribute is.
 */
static bool goodMask(RexxCallContext *c, char *msk, int32_t *mask)
{
    uint32_t y = 0;

    while (*msk)
    {
        if ( *msk == '+' )
        {
            mask[y] = 1;
        }
        else if ( *msk == '-' )
        {
            mask[y] = -1;
        }
        else if (*msk == '*')
        {
            mask[y] = 0;
        }
        else
        {
            return false;
        }

        y++;
        msk++;
    }

    return true;
}

/**
 * This is a SysFileTree specific helper function.
 *
 * Checks the validity of an attribute mask argument and converts the character
 * based mask into an integer based mask.
 *
 * @param context
 * @param msk
 * @param mask
 * @param argPos
 *
 * @return bool
 */
static bool getMaskFromArg(RexxCallContext *context, char *msk, int32_t *mask, size_t argPos)
{
    if ( argumentExists(argPos) && strlen(msk) > 0 )
    {
        if ( strlen(msk) > 5 )
        {
            badMaskException(context->threadContext, argPos, msk);
            return false;
        }

        if ( ! goodMask(context, msk, mask) )
        {
            badMaskException(context->threadContext, argPos, msk);
            return false;
        }
    }
    else
    {
        mask[0] = RXIGNORE;
    }

    return true;
}

/**
 * This is a SysFileTree specific helper function.
 *
 * Checks the validity of the options argument to SysFileTree and converts the
 * character based argument to the proper set of flags.
 *
 * @param context
 * @param opts
 * @param options
 * @param argPos
 *
 * @return bool
 */
static bool getOptionsFromArg(RexxCallContext *context, char *opts, uint32_t *options, size_t argPos)
{
    *options = DO_FILES | DO_DIRS;

    if ( argumentExists(argPos) )
    {
        if ( strlen(opts) == 0 )
        {
            nullStringException(context->threadContext, "SysFileTree", argPos);
            return false;
        }

        if ( ! goodOpts(context, opts, options) )
        {
            badSFTOptsException(context->threadContext, argPos, opts);
            return false;
        }
    }

    return true;
}

/**
 * This is a SysFileTree specific helper function.
 *
 * Allocates and returns a buffer containing the file specification to search
 * for.
 *
 * The file specification consists of the search string as sent by the Rexx
 * user, with possibly some glob characters added.  The returned buffer is
 * bigger than the original string to accommodate these, possible, added
 * characters.  The number of bytes added to the buffer is 8, which is what the
 * original IBM code used.  8 is probably 1 byte more than needed, but there is
 * no reason that this needs to be exact, better too long than too short.
 *
 * If the file speicfication ends in a slash ('\') or a period ('.') or two
 * periods ('..'), then a wild card specification ('*.*') is appended.
 *
 * However, note that there is also the case where a single '.' at the end of
 * the file specification is not used as a directory specifier, but rather is
 * tacked on to the end of a file name.
 *
 * Windows has a sometimes used convention that a '.' at the end of a file name
 * can be used to indicate the file has no extension. For example, given a file
 * named: MyFile a command of "dir MyFile." will produce a listing of "MyFile".
 *
 * In this case we want to leave the file specification alone. that is, do not
 * append a "*.*". A command of "dir *." will produce a directory listing of all
 * files that do not have an extension.
 *
 * @param context
 * @param fSpec
 * @param fSpecLen     [returned]  The length of the original fSpec argument,
 *                     not the length of the allocated buffer.
 * @param fSpecBufLen  [returned]  The length of the length of the allocated
 *                     fSpec buffer.
 * @param argPos
 *
 * @return A string specifying the file pattern to search for.  The buffer
 *         holding the string is larger than the original input specify.
 *
 * @remarks  Caller is responsible for freeing memory.  Memory is allocated
 *           using LocalAlloc(), not malloc().
 *
 *           If the returned buffer is null, a condition has been raised.
 *
 *           FNAMESPEC_BUF_EXTRA (8) is sized to contain the terminating NULL.
 *           So the allocated buffer has room to concatenate 7 characters.
 */
static char *getFileSpecFromArg(RexxCallContext *context, CSTRING fSpec, size_t *fSpecLen,
                                size_t *fSpecBufLen, size_t argPos)
{
    size_t len = strlen(fSpec);
    if ( len == 0 )
    {
        nullStringException(context->threadContext, "SysFileTree", argPos);
        return NULL;
    }

    char *fileSpec = (char *)LocalAlloc(LPTR, len + FNAMESPEC_BUF_EXTRA);
    if ( fileSpec == NULL )
    {
        outOfMemoryException(context->threadContext);
        return NULL;
    }

    // Allocated buffer is zero filled (LPTR flag) already, no need to zero
    // terminate.
    memcpy(fileSpec, fSpec, len);

    if ( fileSpec[len - 1] == '\\' )
    {
        strcat(fileSpec, "*.*");
    }
    else if ( fileSpec[len - 1] == '.')
    {
        if ( len == 1 ||
             (len > 1  && (fileSpec[len - 2] == '\\' || fileSpec[len - 2] == '.')) )
        {
            strcat(fileSpec, "\\*.*");
        }
    }

    *fSpecLen    = len;
    *fSpecBufLen = len + FNAMESPEC_BUF_EXTRA;
    return fileSpec;
}

/**
 * This is a SysFileTree specific helper function.
 *
 * Allocates and returns a buffer large enough to contain the path to search
 * along.
 *
 *  We need a minimum size for the path buffer of at least MAX (264).  But the
 *  old code seemed to think fileSpecLen + FNAMESPEC_BUF_EXTRA could be longer
 *  than that.  I guess it could if the user put in a non-existent long file
 *  path.
 *
 *  The old code of checking fSpecLen is still used, but I'm unsure of its exact
 *  purpose.
 *
 * @param context
 * @param fSpecLen
 * @param pathLen
 *
 * @return A buffer the larger of MAX or fSpecLen + FNAMESPEC_BUF_EXTRA bytes in
 *         size.  Returns NULL on failure.
 *
 * @remarks  The caller is resposible for freeing the allocated memory.
 *
 *           LocalAlloc(), not malloc() is used for memory allocation.
 *
 *           Note that the path buffer is guarenteed to be FNAMESPEC_BUF_EXTRA
 *           (8) bytes larger than the fNameSpec buffer in the caller.  This is
 *           important in later checks for buffer overflow.
 */
static char *getPathBuffer(RexxCallContext *context, size_t fSpecLen, size_t *pathLen)
{
    size_t bufLen = MAX;

    if ( fSpecLen + FNAMESPEC_BUF_EXTRA > MAX )
    {
        bufLen = fSpecLen + FNAMESPEC_BUF_EXTRA;
    }

    *pathLen = bufLen;

    char *path = (char *)LocalAlloc(LPTR, bufLen);
    if ( path == NULL )
    {
        outOfMemoryException(context->threadContext);
    }

    return path;
}

/**
 * SysFileTree() implementation.  Searches for files in a directory tree
 * matching the specified search pattern.
 *
 * @param  fSpec  [required] The search pattern, may contain glob characters
 *                 and full or partial path informattion. E.g., *.bat, or
 *                 ..\..\*.txt, or C:\temp
 *
 * @param  files  [required] A stem to contain the returned results.  On return,
 *                files.0 contains the count N of found files and files.1
 *                through files.N will contain the found files.
 *
 * @param  opts   [optional] Any combination of the following letters that
 *                specify how the search takes place, or how the returned found
 *                file line is formatted.  Case is not significant:
 *
 *                  'B' - Search for files and directories.
 *                  'D' - Search for directories only.
 *                  'F' - Search for files only.
 *                  'O' - Only output file names.
 *                  'S' - Recursively scan subdirectories.
 *                  'T' - Combine time & date fields into one.
 *                  'L' - Long time format
 *                  'I' - Case Insensitive search.
 *
 *                The defualt is 'B' using normal time (neither 'T' nor 'L'.)
 *                The 'I'option is meaningless on Windows.
 *
 * @param targetAttr  [optional] Target attribute mask.  Only files with these
 *                    attributes will be searched for.  The default is to ignore
 *                    the attributes of the files found, so all files found are
 *                    returned.
 *
 * @param newAttr     [optional] New attribute mask.  Each found file will have
 *                    its attributes set (changed) to match this mask.  The
 *                    default is to not change any attributes.
 *
 * @return  0 on success, non-zero on error.  For all errors, a condition is
 *          raised.
 *
 * @remarks  The original IBM code passed in fileSpec to recursiveFindFile(),
 *           but then never used it in recursiveFineFile.  So, that has been
 *           eliminated.
 *
 */
RexxRoutine5(uint32_t, SysFileTree, CSTRING, fSpec, RexxStemObject, files, OPTIONAL_CSTRING, opts,
             OPTIONAL_CSTRING, targetAttr, OPTIONAL_CSTRING, newAttr)
{
     uint32_t     result   = 1;        // Return value, 1 is an error.
     char        *fileSpec = NULL;     // File spec to search for.
     size_t       fSpecLen = 0;        // Length of the original fSpec string.
     size_t       fSpecBufLen = 0;     // Length of the allocated fSpec buffer.
     char        *path     = NULL;     // Path to search along.
     size_t       pathLen  = 0;        // Size of buffer holding path.
     RXTREEDATA   treeData = {0};      // Struct for data.

     treeData.files      = files;
     treeData.dFNameSpec = treeData.fNameSpec;
     treeData.nFNameSpec = FNAMESPEC_BUF_LEN;

     fileSpec = getFileSpecFromArg(context, fSpec, &fSpecLen, &fSpecBufLen, 1);
     if ( fileSpec == NULL )
     {
         goto done_out;
     }

     // Some, or all, of fileSpec will eventually be copied into
     // treeData.dFNameSpec. So, if we ensure that treeData.dFNameSpec is big
     // enough to hold fileSpec we do not need to worry about it any more.
     if ( fSpecBufLen >= FNAMESPEC_BUF_LEN )
     {
         if ( ! getBiggerBuffer(context, &treeData.dFNameSpec, &treeData.nFNameSpec, fSpecBufLen + 1, FNAMESPEC_BUF_LEN) )
         {
             goto done_out;
         }
     }

     path = getPathBuffer(context, fSpecLen, &pathLen);
     if ( path == NULL )
     {
         goto done_out;
     }

     uint32_t options = 0;
     if ( ! getOptionsFromArg(context, (char *)opts, &options, 3) )
     {
         goto done_out;
     }

     int32_t targetMask[5] = {0};    // Attribute mask of files to search for.
     int32_t newMask[5]    = {0};    // Attribute mask to set found files to.

     if ( ! getMaskFromArg(context, (char *)targetAttr, targetMask, 4) )
     {
         goto done_out;
     }

     if ( ! getMaskFromArg(context, (char *)newAttr, newMask, 5) )
     {
         goto done_out;
     }

     // Get the full path segment and the file name segment by expanding the
     // file specification string.  It seems highly unlikely, but possible, that
     // this could fail.
     if ( ! getPath(context, fileSpec, &path, treeData.dFNameSpec, &pathLen) )
     {
         goto done_out;
     }

     if ( recursiveFindFile(context, path, &treeData, targetMask, newMask, options) )
     {
         context->SetStemArrayElement(treeData.files, 0, context->WholeNumber(treeData.count));
         result = 0;
     }

done_out:
    safeLocalFree(fileSpec);
    safeLocalFree(path);
    if ( treeData.nFNameSpec != FNAMESPEC_BUF_LEN )
    {
        LocalFree(treeData.dFNameSpec);
    }
    return result;
}


/*************************************************************************
* Function:  SysGetKey                                                   *
*                                                                        *
* Syntax:    call SysGetKey [echo]                                       *
*                                                                        *
* Params:    echo - Either of the following:                             *
*                    'ECHO'   - Echo the inputted key (default).         *
*                    'NOECHO' - Do not echo the inputted key.            *
*                                                                        *
* Return:    The key striked.                                            *
*************************************************************************/

size_t RexxEntry SysGetKey(const char *name, size_t numargs, CONSTRXSTRING args[], const char *queuename, PRXSTRING retstr)
{
  INT       tmp;                       /* Temp var used to hold      */
                                       /* keystroke value            */
  bool      echo = true;               /* Set to false if we         */
                                       /* shouldn't echo             */

  if (numargs > 1)                     /* too many arguments         */
    return INVALID_ROUTINE;            /* raise an error             */

  if (numargs == 1) {                  /* validate arguments         */
    if (!_stricmp(args[0].strptr, "NOECHO"))
      echo = false;
    else if (_stricmp(args[0].strptr, "ECHO"))
      return INVALID_ROUTINE;          /* Invalid option             */
  }
  if (ExtendedFlag) {                  /* if have an extended        */
    tmp = ExtendedChar;                /* get the second char        */
    ExtendedFlag = false;              /* do a real read next time   */
  }
  else {
    tmp = _getch();                    /* read a character           */

                                       /* If a function key or arrow */
    if ((tmp == 0x00) || (tmp == 0xe0)) {
      ExtendedChar = _getch();         /* Read another character     */
      ExtendedFlag = true;
    }
    else
      ExtendedFlag = false;
  }
  if (echo)                            /* echoing?                   */
    _putch(tmp);                       /* write the character back   */

  wsprintf(retstr->strptr, "%c", tmp);
  retstr->strlength = 1;

  return VALID_ROUTINE;                /* no error on call           */
}

/*************************************************************************
* Function:  SysIni                                                      *
*                                                                        *
* Syntax:    call SysIni [inifile], app [,key/stem] [,val/stem]          *
*                                                                        *
* Params:    inifile - INI file from which to query or write info.  The  *
*                       default is the current user INI file.            *
*            app     - Application title to work with.  May be either    *
*                       of the following:                                *
*                        'ALL:' - All app titles will be returned in the *
*                                  stem variable specified by the next   *
*                                  parameter.                            *
*                        other  - Specific app to work with.             *
*            key     - Key to work with.  May be any of the following:   *
*                        'ALL:'    - All key titles will be returned in  *
*                                     the stem variable specified by the *
*                                     next parameter.                    *
*                        'DELETE:' - All keys associated with the app    *
*                                     will be deleted.                   *
*                        other     - Specific key to work with.          *
*            val     - Key to work with. May be either of the following: *
*                        'DELETE:' - Delete app/key pair.                *
*                        other     - Set app/key pair info to data spec- *
*                                     ified.                             *
*            stem    - Name of stem variable in which to store results.  *
*                      Stem.0 = Number found (n).                        *
*                      Stem.1 - Stem.n = Entries found.                  *
*                                                                        *
* Return:    other          - Info queried from specific app/key pair.   *
*            ''             - Info queried and placed in stem or data    *
*                              deleted successfully.                     *
*            ERROR_NOMEM    - Out of memory.                             *
*            ERROR_RETSTR   - Error opening INI or querying/writing info.*
*************************************************************************/

size_t RexxEntry SysIni(const char *name, size_t numargs, CONSTRXSTRING args[], const char *queuename, PRXSTRING retstr)
{
    size_t      x;                       /* Temp counter               */
    size_t      len;                     /* Len var used when creating */
                                         /* stem                       */
    size_t      lSize;                   /* Size of queried info buffer*/
                                         /* area                       */
    LONG        Error = FALSE;           /* Set to true if error       */
                                         /* encountered                */
    bool        WildCard = false;        /* Set to true if a wildcard  */
                                         /* operation                  */
    bool        QueryApps;               /* Set to true if a query     */
                                         /* operation                  */
    bool        terminate = true;        /* perform WinTerminate call  */
    RXSTEMDATA  ldp;                     /* local data                 */
    size_t      buffersize;              /* return buffer size         */


    buffersize = retstr->strlength;      /* save default buffer size   */
    retstr->strlength = 0;               /* set return value           */
    const char *Key = "";
    /* validate arguments         */
    if (numargs < 2 || numargs > 4 || !RXVALIDSTRING(args[1]))
    {
        return INVALID_ROUTINE;
    }
    /* get pointers to args       */
    const char *IniFile = args[0].strptr;
    if (!RXVALIDSTRING(args[0]))         /* not specified?             */
        IniFile = "WIN.INI";               /* default to WIN.INI         */
    const char *App = args[1].strptr;

    if (numargs >= 3 && args[2].strptr)
        Key = args[2].strptr;

    const char *Val = NULL;
    if (numargs == 4)
        Val = args[3].strptr;
    /* Check KEY and APP values   */
    /* for "WildCard"             */
    if (!_stricmp(App, "ALL:"))
    {
        App = "";
        QueryApps = true;
        WildCard = true;

        if (numargs != 3)
            return INVALID_ROUTINE;          /* Error - Not enough args    */
        else
            x = 2;                           /* Arg number of STEM variable*/
    }

    else if (!_stricmp(Key, "ALL:"))
    {
        Key = "";
        Val = "";
        QueryApps = false;
        WildCard = true;

        if (numargs != 4)
            return INVALID_ROUTINE;          /* Error - Not enough args    */

        else
            x = 3;                           /* Arg number of STEM variable*/
    }
    /* If this is a "WildCard     */
    /* search, then allocate mem  */
    /* for stem struct and get the*/
    /* stem name                  */
    if (WildCard == true)
    {

        ldp.count = 0;                     /* get the stem variable name */
        strcpy(ldp.varname, args[x].strptr);
        ldp.stemlen = args[x].strlength;
        /* uppercase the name         */
        memupper(ldp.varname, strlen(ldp.varname));

        if (ldp.varname[ldp.stemlen-1] != '.')
            ldp.varname[ldp.stemlen++] = '.';
    }

    char *returnVal = NULL;
    /* get value if is a query    */
    if ((numargs == 3 && _stricmp(Key, "DELETE:")) ||
        WildCard == true)
    {
        lSize = 0x0000ffffL;
        /* Allocate a large buffer    */
        returnVal = (char *)GlobalAlloc(GPTR, lSize);
        if (returnVal == NULL)
        {
            BUILDRXSTRING(retstr, ERROR_NOMEM);
            return VALID_ROUTINE;
        }

        if (WildCard && QueryApps)
            /* Retrieve the names of all  */
            /* applications.              */
            lSize = GetPrivateProfileString(NULL, NULL, "", returnVal, (DWORD)lSize, IniFile);
        else if (WildCard && !QueryApps)
            /* Retrieve all keys for an   */
            /* application                */
            lSize = GetPrivateProfileString(App, NULL, "", returnVal, (DWORD)lSize, IniFile);
        else
            /* Retrieve a single key value*/
            lSize = GetPrivateProfileString(App, Key, "", returnVal, (DWORD)lSize, IniFile);

        if (lSize <= 0)
        {
            Error = true;
            BUILDRXSTRING(retstr, ERROR_RETSTR);
        }
        else if (WildCard == false)
        {
            if (lSize > buffersize)
                retstr->strptr = (PCH)GlobalAlloc(GMEM_FIXED, lSize);
                if (retstr->strptr == NULL)
                {
                    GlobalFree(returnVal);  /* release buffer */
                    BUILDRXSTRING(retstr, ERROR_NOMEM);
                    return VALID_ROUTINE;
                }
            memcpy(retstr->strptr, returnVal, lSize);
            retstr->strlength = lSize;
        }
    }
    else
    {                               /* Set or delete Key          */

        if (!_stricmp(Key, "DELETE:") || (numargs == 2) || !RXVALIDSTRING(args[2]))
            /* Delete application and all */
            /* associated keys            */
            Error = !WritePrivateProfileString(App, NULL, NULL, IniFile);
        else if (!_stricmp(Val, "DELETE:") ||
                 !RXVALIDSTRING(args[3]))
            /* Delete a single key        */
            Error = !WritePrivateProfileString(App, Key, NULL, IniFile);
        else
        {
            lSize = args[3].strlength;
            /* Set a single key value     */
            Error = !WritePrivateProfileString(App, Key, Val, IniFile);
        }

        if (Error)
        {
            BUILDRXSTRING(retstr, ERROR_RETSTR);
        }
        else
            retstr->strlength = 0;           /* just return a null string  */
    }

    /******************************************
    * If this was a wildcard search, change   *
    * the Val variable from one long string   *
    * of values to a REXX stem variable.      *
    ******************************************/

    if (WildCard == true)
    {              /* fill stem variable         */

        if (Error == false)
        {
            x = 0;
            ldp.count = 0;

            do
            {
                /* Copy string terminated by \0 to Temp.  Last string will end     */
                /* in \0\0 and thus have a length of 0.                            */
                len = 0;

                const char *next = &returnVal[x]; /* point to string            */
                len = strlen(next);            /* get string length          */
                                               /* if non-zero length, then   */
                                               /* set the stem element       */
                if (len != 0)
                {
                    x += (len+1);                /* Increment pointer past the */
                                                 /* new string                 */
                    strcpy(ldp.ibuf, next);
                    ldp.vlen = len;
                    ldp.count++;
                    ltoa((long)ldp.count, ldp.varname+ldp.stemlen, 10);

                    if (ldp.ibuf[ldp.vlen-1] == '\n')
                        ldp.vlen--;
                    ldp.shvb.shvnext = NULL;
                    ldp.shvb.shvname.strptr = ldp.varname;
                    ldp.shvb.shvname.strlength = strlen(ldp.varname);
                    ldp.shvb.shvvalue.strptr = ldp.ibuf;
                    ldp.shvb.shvvalue.strlength = ldp.vlen;
                    ldp.shvb.shvnamelen = ldp.shvb.shvname.strlength;
                    ldp.shvb.shvvaluelen = ldp.vlen;
                    ldp.shvb.shvcode = RXSHV_SET;
                    ldp.shvb.shvret = 0;
                    if (RexxVariablePool(&ldp.shvb) == RXSHV_BADN)
                    {
                        if (returnVal != NULL)
                        {
                            GlobalFree(returnVal);  /* release buffer */
                        }
                        return INVALID_ROUTINE;    /* error on non-zero          */
                    }
                }
            }

            while (returnVal[x] != '\0');
        }

        else
            ldp.count = 0;

        if (returnVal != NULL)
        {
            GlobalFree(returnVal);
            returnVal = NULL;
        }

        /* set number returned        */
        ltoa((long)ldp.count, ldp.ibuf, 10);
        ldp.varname[ldp.stemlen] = '0';
        ldp.varname[ldp.stemlen+1] = 0;
        ldp.shvb.shvnext = NULL;
        ldp.shvb.shvname.strptr = ldp.varname;
        ldp.shvb.shvname.strlength = ldp.stemlen+1;
        ldp.shvb.shvnamelen = ldp.stemlen+1;
        ldp.shvb.shvvalue.strptr = ldp.ibuf;
        ldp.shvb.shvvalue.strlength = strlen(ldp.ibuf);
        ldp.shvb.shvvaluelen = ldp.shvb.shvvalue.strlength;
        ldp.shvb.shvcode = RXSHV_SET;
        ldp.shvb.shvret = 0;
        if (RexxVariablePool(&ldp.shvb) == RXSHV_BADN)
            return INVALID_ROUTINE;          /* error on non-zero          */

    }                                    /* * End - IF (Wildcard ... * */
    if (returnVal != NULL)
    {
        GlobalFree(returnVal);  /* release buffer                              */
    }

    return VALID_ROUTINE;                /* no error on call           */
}


/*************************************************************************
* Function:  SysLoadFuncs                                                *
*                                                                        *
* Syntax:    call SysLoadFuncs [option]                                  *
*                                                                        *
* Params:    none                                                        *
*                                                                        *
* Return:    null string                                                 *
*************************************************************************/

size_t RexxEntry SysLoadFuncs(const char *name, size_t numargs, CONSTRXSTRING args[], const char *queuename, PRXSTRING retstr)
{
    // this is a NOP now
    retstr->strlength = 0;               /* set return value           */
    return VALID_ROUTINE;
}


/*************************************************************************
* Function:  SysMkDir                                                    *
*                                                                        *
* Syntax:    call SysMkDir dir                                           *
*                                                                        *
* Params:    dir - Directory to be created.                              *
*                                                                        *
* Return:    NO_UTIL_ERROR                                               *
*            Return code from CreateDirectory()                          *
*************************************************************************/

size_t RexxEntry SysMkDir(const char *name, size_t numargs, CONSTRXSTRING args[], const char *queuename, PRXSTRING retstr)
{

  if (numargs != 1)                    /* we need one argument       */
    return INVALID_ROUTINE;            /* raise an error             */

  if (!CreateDirectory(args[0].strptr, NULL)) /* make the directory  */
      RETVAL(GetLastError())            /* pass back return code     */
  else
      RETVAL(0)
}


/*************************************************************************
* Function:  SysGetErrortext                                             *
*                                                                        *
* Syntax:    call SysGetErrortext errnumber                              *
*                                                                        *
* Params:    errnumber - error number to be described                    *
*                                                                        *
* Return:    Description or empty string                                 *
*************************************************************************/

size_t RexxEntry SysGetErrortext(const char *name, size_t numargs, CONSTRXSTRING args[], const char *queuename, PRXSTRING retstr)
{
    DWORD  errnum;
    char  *errmsg;

    if (numargs != 1)
    {
        /* If no args, then its an    */
        /* incorrect call             */
        return INVALID_ROUTINE;
    }

    errnum = atoi(args[0].strptr);
    if (FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER|FORMAT_MESSAGE_FROM_SYSTEM,NULL,errnum,0,(LPSTR)&errmsg,64,NULL) == 0)
    {
        retstr->strptr[0] = 0x00;
    }
    else
    {                               /* succeeded                  */
        if (strlen(errmsg)>=retstr->strlength)
        {
            retstr->strptr = (PCH)GlobalAlloc(GMEM_ZEROINIT | GMEM_FIXED, strlen(errmsg+1));
        }
        strcpy(retstr->strptr,errmsg);
        LocalFree(errmsg);
    }
    retstr->strlength = strlen(retstr->strptr);
    return VALID_ROUTINE;
}


/*************************************************************************
* Function:  SysWinEncryptFile (W2K only)                                *
*                                                                        *
* Syntax:    call SysWinEncryptFile filename                             *
*                                                                        *
* Params:    filename - file to be encrypted                             *
*                                                                        *
* Return:    NO_UTIL_ERROR                                               *
*            Return code from EncryptFile()                              *
*************************************************************************/

size_t RexxEntry SysWinEncryptFile(const char *name, size_t numargs, CONSTRXSTRING args[], const char *queuename, PRXSTRING retstr)
{
    ULONG  rc;                           /* Ret code of func           */
    OSVERSIONINFO vi;

    if (numargs != 1)
    {
        /* If no args, then its an    */
        /* incorrect call             */
        return INVALID_ROUTINE;
    }

    vi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);

    if (rc = GetVersionEx(&vi))
    {
        /* allow this only on W2K or newer */
        if (vi.dwPlatformId == VER_PLATFORM_WIN32_NT && vi.dwMajorVersion > 4)
        {
            rc = EncryptFile(args[0].strptr);
        }
        else
        {
            rc = 0;
            SetLastError(ERROR_CANNOT_MAKE);
        }
    }
    if (rc)
    {
        sprintf(retstr->strptr, "%d", 0);
    }
    else
    {
        sprintf(retstr->strptr, "%d", GetLastError());
    }
    retstr->strlength = strlen(retstr->strptr);
    return VALID_ROUTINE;
}

/*************************************************************************
* Function:  SysWinDecryptFile (W2K only)                                *
*                                                                        *
* Syntax:    call SysWinDecryptFile filename                             *
*                                                                        *
* Params:    filename - file to be decrypted                             *
*                                                                        *
* Return:    NO_UTIL_ERROR                                               *
*            Return code from DecryptFile()                              *
*************************************************************************/

size_t RexxEntry SysWinDecryptFile(const char *name, size_t numargs, CONSTRXSTRING args[], const char *queuename, PRXSTRING retstr)
{

  ULONG  rc;                           /* Ret code of func           */
  OSVERSIONINFO vi;

  if (numargs != 1)
                                       /* If no args, then its an    */
                                       /* incorrect call             */
    return INVALID_ROUTINE;

    vi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);

  if (rc = GetVersionEx(&vi)) {
    /* allow this only on W2K or newer */
    if (vi.dwPlatformId == VER_PLATFORM_WIN32_NT && vi.dwMajorVersion > 4)
      rc = DecryptFile(args[0].strptr,0);
    else {
      rc = 0;
      SetLastError(ERROR_CANNOT_MAKE);
    }
  }

  if (rc)
    sprintf(retstr->strptr, "%d", 0);
  else
    sprintf(retstr->strptr, "%d", GetLastError());
  retstr->strlength = strlen(retstr->strptr);
  return VALID_ROUTINE;
}


/*************************************************************************
* Function:  SysWinVer                                                   *
*                                                                        *
* Syntax:    call SysWinVer                                              *
*                                                                        *
* Return:    Windows Version                                             *
*************************************************************************/

size_t RexxEntry SysWinVer(const char *name, size_t numargs, CONSTRXSTRING args[], const char *queuename, PRXSTRING retstr)
{

  OSVERSIONINFO vi;                    /* Return RXSTRING            */
    char chVerBuf[12];

  vi.dwOSVersionInfoSize = sizeof(vi); /* if not set --> violation error */

  if (numargs != 0)                    /* validate arg count         */
    return INVALID_ROUTINE;

  GetVersionEx(&vi);                /* get version with extended api */
  if (vi.dwPlatformId == VER_PLATFORM_WIN32s)
    strcpy(chVerBuf, "Windows");       /* Windows 3.1                */
  else
    if (vi.dwPlatformId == VER_PLATFORM_WIN32_NT)
      strcpy(chVerBuf, "WindowsNT");   /* Windows NT               */
  else strcpy(chVerBuf, "Windows95");  /* Windows 95               */

                                       /* format into the buffer     */
  wsprintf(retstr->strptr,"%s %lu.%02lu",
             chVerBuf,
             vi.dwMajorVersion,
             vi.dwMinorVersion);

  retstr->strlength = strlen(retstr->strptr);
  return VALID_ROUTINE;
}

/*************************************************************************
* Function:  SysVersion                                                  *
*                                                                        *
* Syntax:    Say  SysVersion                                             *
*                                                                        *
* Return:    Operating System and Version                                *
*************************************************************************/

size_t RexxEntry SysVersion(const char *name, size_t numargs, CONSTRXSTRING args[], const char *queuename, PRXSTRING retstr)
{
  /* this is only an alias for SysWinVer */
  return SysWinVer(name, numargs, args, queuename, retstr);
}

/*************************************************************************
* Function:  SysRmDir                                                    *
*                                                                        *
* Syntax:    call SysRmDir dir                                           *
*                                                                        *
* Params:    dir - Directory to be removed.                              *
*                                                                        *
* Return:    NO_UTIL_ERROR                                               *
*            Return code from RemoveDirectory()                          *
*************************************************************************/

size_t RexxEntry SysRmDir(const char *name, size_t numargs, CONSTRXSTRING args[], const char *queuename, PRXSTRING retstr)
{

  if (numargs != 1)                    /* we need one argument       */
    return INVALID_ROUTINE;            /* raise an error             */

  if (!RemoveDirectory(args[0].strptr)) /* remove the directory      */
      RETVAL(GetLastError())           /* pass back return code      */
  else
      RETVAL(0)
}

/*************************************************************************
* Function:  SysSearchPath                                               *
*                                                                        *
* Syntax:    call SysSearchPath path, file [, options]                   *
*                                                                        *
* Params:    path - Environment variable name which specifies a path     *
*                    to be searched (ie 'PATH', 'DPATH', etc).           *
*            file - The file to search for.                              *
*            options -  'C' - Current directory search first (default).  *
*                       'N' - No Current directory search. Only searches *
*                             the path as specified.                     *
*                                                                        *
* Return:    other  - Full path and filespec of found file.              *
*            ''     - Specified file not found along path.               *
*************************************************************************/

size_t RexxEntry SysSearchPath(const char *name, size_t numargs, CONSTRXSTRING args[], const char *queuename, PRXSTRING retstr)
{
    char     szFullPath[_MAX_PATH];      /* returned file name         */
    char     szCurDir[_MAX_PATH];        /* current directory          */
    char     *szEnvStr = NULL;

    LPTSTR pszOnlyFileName;              /* parm for searchpath        */
    LPTSTR lpPath = NULL;                /* ptr to search path+        */
    UINT   errorMode;

    /* validate arguments         */
    if (numargs < 2 || numargs > 3 ||
        !RXVALIDSTRING(args[0]) ||
        !RXVALIDSTRING(args[1]))
    {
        return INVALID_ROUTINE;
    }

    char opt = 'C'; // this is the default
    if (numargs == 3)
    {                  /* process options            */
        opt = toupper(args[2].strptr[0]);
        if (opt != 'C' && opt != 'N')
        {
            return INVALID_ROUTINE;          /* Invalid option             */
        }
    }

    szEnvStr = (LPTSTR) malloc(sizeof(char) * MAX_ENVVAR);
    if (szEnvStr != NULL)
    {
        DWORD charCount = GetEnvironmentVariable(args[0].strptr, szEnvStr, MAX_ENVVAR);
        if (charCount == 0)
        {
            *szEnvStr = '\0';
        }
        else if (charCount > MAX_ENVVAR)
        {
            szEnvStr = (LPTSTR) realloc(szEnvStr, sizeof(char) * charCount);
            if (szEnvStr != NULL)
            {
                DWORD charCount2 = GetEnvironmentVariable(args[0].strptr, szEnvStr, charCount);
                if (charCount2 == 0 || charCount2 > charCount)
                {
                    *szEnvStr = '\0';
                }
            }
        }
    }

    if (opt == 'N')
    {
        lpPath = (szEnvStr == NULL) ? NULL : strdup(szEnvStr);
    }
    else if (opt == 'C')
    {
        /* search current directory   */
        DWORD charCount = GetCurrentDirectory(_MAX_PATH, szCurDir);
        if (charCount == 0 || charCount > _MAX_PATH)
        {
            szCurDir[0] = '\0';
        }

        if (szEnvStr != NULL)
        {
            lpPath = (LPTSTR) malloc(sizeof(char) * (strlen(szCurDir) + 1 + strlen(szEnvStr) + 1));
            if (lpPath != NULL)
            {
                strcpy(lpPath, szCurDir);
                strcat(lpPath, ";");
                strcat(lpPath, szEnvStr);
            }
        }
        else
        {
            lpPath = strdup(szCurDir);
        }
    }

    /* use DosSearchPath          */

    DWORD charCount = 0;
    if (lpPath != NULL)
    {
        errorMode = SetErrorMode(SEM_FAILCRITICALERRORS);
        charCount = SearchPath(
                           (LPCTSTR)lpPath,              /* path srch, NULL will+      */
                           (LPCTSTR)args[1].strptr,      /* address if filename        */
                           NULL,                         /* filename contains .ext     */
                           _MAX_PATH,                    /* size of fullname buffer    */
                           szFullPath,                   /* where to put results       */
                           &pszOnlyFileName);
        SetErrorMode(errorMode);
    }
    if (charCount == 0 || charCount > _MAX_PATH)
    {
        szFullPath[0]='\0';              /* set to NULL if failure     */
    }

    BUILDRXSTRING(retstr, szFullPath);   /* pass back result           */
    free(szEnvStr);
    free(lpPath);
    return VALID_ROUTINE;
}


/*************************************************************************
* Function:  SysSleep                                                    *
*                                                                        *
* Syntax:    call SysSleep secs                                          *
*                                                                        *
* Params:    secs - Number of seconds to sleep.                          *
*                                                                        *
* Return:    NO_UTIL_ERROR                                               *
*************************************************************************/

size_t RexxEntry SysSleep(const char *name, size_t numargs, CONSTRXSTRING args[], const char *queuename, PRXSTRING retstr)
{
  LONG secs;                           /* Time to sleep in secs      */
  MSG msg;

  LONG milliseconds;
  LONG secs_buf;
  size_t length;
  LONG digits;

  if (numargs != 1)                    /* Must have one argument     */
    return INVALID_ROUTINE;

  /* code fragment taken from lrxutil.c: */
  const char *string = args[0].strptr; /* point to the string        */
  length = args[0].strlength;          /* get length of string       */
  if (length == 0 ||                   /* if null string             */
      length > MAX_DIGITS)             /* or too long                */
    return INVALID_ROUTINE;            /* not valid                  */

  secs = 0;                            /* start with zero            */

  while (length) {                     /* while more digits          */
    if (!isdigit(*string))             /* not a digit?               */
      break;                           /* get out of this loop       */
    secs = secs * 10 + (*string - '0');/* add to accumulator         */
    length--;                          /* reduce length              */
    string++;                          /* step pointer               */
  }
  secs_buf = secs;                     /* remember the seconds       */
  secs = secs * 1000;                  /* convert to milliseconds    */
  if (*string == '.') {                /* have a decimal number?     */
    string++;                          /* step over the decimal      */
    length--;                          /* reduce the length          */
    milliseconds = 0;                  /* no milliseconds yet        */
    digits = 0;                        /* and no digits              */
    while (length) {                   /* while more digits          */
      if (!isdigit(*string))           /* not a digit?               */
        return INVALID_ROUTINE;        /* not a valid number         */
      if (++digits <= 3)               /* still within precision?    */
                                       /* add to accumulator         */
        milliseconds = milliseconds * 10 + (*string - '0');
      length--;                        /* reduce length              */
      string++;                        /* step pointer               */
    }
    while (digits < 3) {               /* now adjust up              */
      milliseconds = milliseconds * 10;/* by powers of 10            */
      digits++;                        /* count the digit            */
    }
    secs += milliseconds;              /* now add in the milliseconds*/
  }
  else if (length != 0)                /* invalid character found?   */
    return INVALID_ROUTINE;            /* this is invalid            */


  /** Using Sleep with a long timeout risks sleeping on a thread with a message
   *  queue, which can make the system sluggish, or possibly deadlocked.  If the
   *  sleep is longer than 333 milliseconds use a window timer to avoid this
   *  risk.
   */
  if ( secs > 333 )
  {
      if ( !(SetTimer(NULL, 0, (secs), (TIMERPROC) SleepTimerProc)) )
          return INVALID_ROUTINE;        /* no timer available, need to abort */
      while ( GetMessage (&msg, NULL, 0, 0) )
      {
          if ( msg.message == OM_WAKEUP )  /* If our message, exit loop       */
              break;
          TranslateMessage( &msg );
          DispatchMessage ( &msg );
      }
  }
  else
  {
      Sleep(secs);
  }

  BUILDRXSTRING(retstr, NO_UTIL_ERROR);
  return VALID_ROUTINE;
}

/*********************************************************************
 *                                                                   *
 *  Routine   : SleepTimerProc                                       *
 *                                                                   *
 *  Purpose   : callback routine for SetTimer set in SysSleep        *
 *  Notes     :                                                      *
 *  Arguments : hwnd - window handle                                 *
 *              uMsg - WM_TIMER message                              *
 *              idEvent - timer identifier                           *
 *              dwtime - current system time                         *
 *  Returns   :                                                      *
 *                                                                   *
 *********************************************************************/
 VOID CALLBACK SleepTimerProc(HWND hwnd, UINT uMsg, UINT idEvent, DWORD dwTime) {
   DWORD ThreadId;
   KillTimer(NULL, idEvent);       /* kill the timer that just ended */
   ThreadId = GetCurrentThreadId();
   PostThreadMessage(ThreadId, OM_WAKEUP, 0 , 0L); /* send ourself the wakeup message*/
 }

/*************************************************************************
* Function:  SysTempFileName                                             *
*                                                                        *
* Syntax:    call SysTempFileName template [,filler]                     *
*                                                                        *
* Params:    template - Description of filespec desired.  For example:   *
*                        C:\TEMP\FILE.???                                *
*            filler   - A character which when found in template will be *
*                        replaced with random digits until a unique file *
*                        or directory is found.  The default character   *
*                        is '?'.                                         *
*                                                                        *
* Return:    other - Unique file/directory name.                         *
*            ''    - No more files exist given specified template.       *
*************************************************************************/

size_t RexxEntry SysTempFileName(const char *name, size_t numargs, CONSTRXSTRING args[], const char *queuename, PRXSTRING retstr)
{
    CHAR   filler;                       /* filler character           */

    if (numargs < 1 ||                   /* validate arguments         */
        numargs > 2 ||
        !RXVALIDSTRING(args[0]) ||
        args[0].strlength > 512)
        return INVALID_ROUTINE;

    if (numargs == 2 &&                  /* get filler character       */
        !RXNULLSTRING(args[1]))
    {
        if (args[1].strlength != 1)        /* must be one character      */
            return INVALID_ROUTINE;
        filler = args[1].strptr[0];
    }
    else
    {
        filler = '?';
    }
    /* get the file id            */
    GetUniqueFileName(const_cast<char *>(args[0].strptr), filler, retstr->strptr);
    retstr->strlength = strlen(retstr->strptr);

    return VALID_ROUTINE;
}


/*************************************************************************
* Function:  SysTextScreenRead                                           *
*                                                                        *
* Syntax:    call SysTextScreenRead row, col [,len]                      *
*                                                                        *
* Params:    row - Horizontal row on the screen to start reading from.   *
*                   The row at the top of the screen is 0.               *
*            col - Vertical column on the screen to start reading from.  *
*                   The column at the left of the screen is 0.           *
*            len - The number of characters to read.  The default is the *
*                   rest of the screen.                                  *
*                                                                        *
* Return:    Characters read from text screen.                           *
*************************************************************************/
size_t RexxEntry SysTextScreenRead(const char *name, size_t numargs, CONSTRXSTRING args[], const char *queuename, PRXSTRING retstr)
{

  int   row;                           /* Row from which to start    */
  int   col;                           /* Column from which to start */
  int   len;                           /* nunber of chars to be read */
  int   lPos,lPosOffSet;               /* positioning                */
                                       /* (132x50)                   */
  int  lBufferLen = 16000;             /* default: 200x80 characters */

  COORD coordLine;                     /* coordinates of where to    */
                                       /* read characters from       */
  DWORD dwCharsRead,dwSumCharsRead;    /* Handle to Standard Out     */
  HANDLE hStdout;
  CONSOLE_SCREEN_BUFFER_INFO csbiInfo; /* Console information        */
  PCH temp_strptr;

  hStdout = GetStdHandle(STD_OUTPUT_HANDLE);

  if (GetConsoleScreenBufferInfo(hStdout, &csbiInfo))
    len = csbiInfo.dwSize.Y * csbiInfo.dwSize.X;
  else
    RETVAL(GetLastError())

  if (numargs < 2 ||                   /* validate arguments         */
      numargs > 3 ||
      !RXVALIDSTRING(args[0]) ||
      !RXVALIDSTRING(args[1]) ||
      !string2long(args[0].strptr, &row) || row < 0 ||
      !string2long(args[1].strptr, &col) || col < 0)
    return INVALID_ROUTINE;

  if (numargs == 3) {                  /* check the length           */
    if (!RXVALIDSTRING(args[2]) ||     /* bad string?                */
        !string2long(args[2].strptr, &len) || len < 0)
      return INVALID_ROUTINE;          /* error                      */
  }
  coordLine.X = (SHORT)col;
  coordLine.Y = (SHORT)row;

  if (len > (LONG)retstr->strlength) {
                                       /* allocate a new one         */
    if (!(temp_strptr = (PCH)GlobalAlloc(GMEM_FIXED , len))) { /* use GlobalAlloc */
      BUILDRXSTRING(retstr, ERROR_NOMEM);
      return VALID_ROUTINE        ;
    }
    else
      retstr->strptr = temp_strptr;
  }

  if (len < lBufferLen)
    lBufferLen = len;

  lPos = 0;                            /* current position           */
  lPosOffSet = row * csbiInfo.dwSize.X + col;   /* add offset if not started at beginning */
  dwSumCharsRead = 0;

  while (lPos < len ) {

    if (!ReadConsoleOutputCharacter(hStdout, &retstr->strptr[lPos], lBufferLen, coordLine, &dwCharsRead))
    {
      RETVAL(GetLastError())
    }


    lPos = lPos + lBufferLen;
    coordLine.Y = (SHORT)((lPos + lPosOffSet) / csbiInfo.dwSize.X);
    coordLine.X = (SHORT)((lPos + lPosOffSet) % csbiInfo.dwSize.X);
    dwSumCharsRead = dwSumCharsRead + dwCharsRead;
  }

  retstr->strlength = dwSumCharsRead;
  return VALID_ROUTINE;
}

/*************************************************************************
* Function:  SysTextScreenSize                                           *
*                                                                        *
* Syntax:    call SysTextScreenSize                                      *
*                                                                        *
* Return:    Size of screen in row and columns returned as:  row, col    *
*************************************************************************/

size_t RexxEntry SysTextScreenSize(const char *name, size_t numargs, CONSTRXSTRING args[], const char *queuename, PRXSTRING retstr)
{

  HANDLE    hStdout;                   /* Handle to Standard Out     */

  CONSOLE_SCREEN_BUFFER_INFO csbiInfo; /* Console information        */

  if (numargs != 0)                    /* no arguments on this       */
    return INVALID_ROUTINE;

  hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
                                       /* if in character mode       */
  if (GetConsoleScreenBufferInfo(hStdout, &csbiInfo)) {

    wsprintf(retstr->strptr, "%d %d", csbiInfo.dwSize.Y, csbiInfo.dwSize.X);
    retstr->strlength = strlen(retstr->strptr);
  }
  else
  {
    strcpy(retstr->strptr, "0 0");
    retstr->strlength = strlen(retstr->strptr);
  }
  return VALID_ROUTINE;
}

/*************************************************************************
* Function:  RxWinExec                                                   *
*                                                                        *
* Syntax:    call RxWinExec command,CmdShow                              *
*                                                                        *
*                                                                        *
* Parms:     command    - program to execute                             *
*            CmdShow    - Any of the SW_ type values in winuser.h        *
*                         SW_SHOW                 5                      *
*                         SW_HIDE                 0                      *
*                         SW_MINIMIZE etc...      6                      *
*                         numeric values...                              *
*                                                                        *
* Return:    Process ID or Error code                                    *
*************************************************************************/

size_t RexxEntry RxWinExec(const char *name, size_t numargs, CONSTRXSTRING args[], const char *queuename, PRXSTRING retstr)
{

  int         CmdShow;                 /* show window style flags    */
  int         index;                   /* table index                */
  ULONG       pid;                     /* PID or error return code   */
  size_t      length;                  /* length of option           */
  STARTUPINFO si;
  PROCESS_INFORMATION procInfo;


PSZ    show_styles[] =                 /* show window types          */
    {"SHOWNORMAL",
     "SHOWNOACTIVATE",
     "SHOWMINNOACTIVE",
     "SHOWMINIMIZED",
     "SHOWMAXIMIZED",
     "HIDE",
     "MINIMIZE"
     };

ULONG  show_flags[] =                  /* show window styles        */
    {SW_SHOWNORMAL,
     SW_SHOWNOACTIVATE,
     SW_SHOWMINNOACTIVE,
     SW_SHOWMINIMIZED,
     SW_SHOWMAXIMIZED,
     SW_HIDE,
     SW_MINIMIZE
    };

  BUILDRXSTRING(retstr, NO_UTIL_ERROR);/* pass back result           */

    // Should be 1 or 2 args.
    if ( numargs < 1 || numargs > 2 || !RXVALIDSTRING(args[0]) ||
         (numargs == 2 && !RXVALIDSTRING(args[1])) || args[0].strlength > (32 * 1024) )
    {
        return INVALID_ROUTINE;
    }

  CmdShow=0;                           /* initialize show flags      */
                                       /* validate arguments         */
  if (numargs < 2 ||                   /* no show window style?      */
      args[1].strptr == NULL)
    CmdShow += SW_SHOWNORMAL;          /* set default show style     */
  else {                               /* check various show styles  */
    length = args[1].strlength;        /* get length of option       */
                                       /* search style table         */
    for (index = 0;
         index < sizeof(show_styles)/sizeof(PSZ);
         index++) {
                                       /* find a match?              */
      if (length == strlen(show_styles[index]) &&
          !memicmp(args[1].strptr, show_styles[index], length)) {
        CmdShow += show_flags[index];  /* add to the style           */
        break;
      }
    }/* for */
                                       /* not found?                 */
    if (index == sizeof(show_styles)/sizeof(PSZ))
      return INVALID_ROUTINE;          /* raise an error             */
  }/* else */

  ZeroMemory(&procInfo, sizeof(procInfo));
  ZeroMemory(&si, sizeof(si));
  si.cb = sizeof(si);
  si.dwFlags = STARTF_USESHOWWINDOW;
  si.wShowWindow = (WORD)CmdShow;

  if ( CreateProcess(NULL, (LPSTR)args[0].strptr, NULL, NULL, FALSE, 0, NULL,
                     NULL, &si, &procInfo ) ) {
    pid = procInfo.dwProcessId;
  }
  else {
    pid = GetLastError();
    if ( pid > 31 )                    /* maintain compatibility to  */
      pid = (ULONG)-((int)pid);        /* versions < ooRexx 3.1.2    */
  }
                                       /* return value as string     */
  sprintf(retstr->strptr, "%d", pid);
  retstr->strlength = strlen(retstr->strptr);

  /* Close process / thread handles as they are not used / needed.   */
  CloseHandle(procInfo.hProcess);
  CloseHandle(procInfo.hThread);

  return VALID_ROUTINE;                /* good completion            */
}



/*************************************************************************
* Function:  SysAddRexxMacro                                             *
*                                                                        *
* Syntax:    result = SysAddRexxMacro(name, file, <order>)               *
*                                                                        *
* Params:    name   - loaded name of the macro file                      *
*            file   - file containing the loaded macro                   *
*            order  - Either 'B'efore or 'A'fter.  The default is 'B'    *
*                                                                        *
* Return:    return code from RexxAddMacro                               *
*************************************************************************/

size_t RexxEntry SysAddRexxMacro(const char *name, size_t numargs, CONSTRXSTRING args[], const char *queuename, PRXSTRING retstr)
{
  ULONG       position;                /* added position             */

  if (numargs < 2 || numargs > 3 ||    /* wrong number?              */
      !RXVALIDSTRING(args[0]) ||       /* first is omitted           */
      !RXVALIDSTRING(args[1]))         /* second is omitted          */
    return INVALID_ROUTINE;            /* raise error condition      */

  position = RXMACRO_SEARCH_BEFORE;    /* set default search position*/
  if (numargs == 3) {                  /* have an option?            */
    if (RXZEROLENSTRING(args[2]))      /* null string?               */
      return INVALID_ROUTINE;          /* this is an error           */
                                       /* 'B'efore?                  */
    else if (toupper(args[2].strptr[0]) == 'B')
      position = RXMACRO_SEARCH_BEFORE;/* place before               */
                                       /* 'A'fter?                   */
    else if (toupper(args[2].strptr[0]) == 'A')
      position = RXMACRO_SEARCH_AFTER; /* place after                */
    else                               /* parm given was bad         */
      return INVALID_ROUTINE;          /* raise an error             */
  }
                                       /* try to add the macro       */
  RETVAL(RexxAddMacro(args[0].strptr, args[1].strptr, position))
}

/*************************************************************************
* Function:  SysReorderRexxMacro                                         *
*                                                                        *
* Syntax:    result = SysReorderRexxMacro(name, order)                   *
*                                                                        *
* Params:    name   - loaded name of the macro file                      *
*            order  - Either 'B'efore or 'A'fter.                        *
*                                                                        *
* Return:    return code from RexxReorderMacro                           *
*************************************************************************/

size_t RexxEntry SysReorderRexxMacro(const char *name, size_t numargs, CONSTRXSTRING args[], const char *queuename, PRXSTRING retstr)
{
  ULONG       position;                /* added position             */

  if (numargs != 2 ||                  /* wrong number?              */
      !RXVALIDSTRING(args[0]) ||       /* first is omitted           */
      RXZEROLENSTRING(args[1]))        /* null string?               */
    return INVALID_ROUTINE;            /* raise error condition      */
                                       /* 'B'efore?                  */
  if (toupper(args[1].strptr[0]) == 'B')
    position = RXMACRO_SEARCH_BEFORE;  /* place before               */
                                       /* 'A'fter?                   */
  else if (toupper(args[1].strptr[0]) == 'A')
    position = RXMACRO_SEARCH_AFTER;   /* place after                */
  else                                 /* parm given was bad         */
    return INVALID_ROUTINE;            /* raise an error             */
                                       /* try to add the macro       */
  RETVAL(RexxReorderMacro(args[0].strptr, position));
}

/*************************************************************************
* Function:  SysDropRexxMacro                                            *
*                                                                        *
* Syntax:    result = SysDropRexxMacro(name)                             *
*                                                                        *
* Params:    name   - name of the macro space function                   *
*                                                                        *
* Return:    return code from RexxDropMacro                              *
*************************************************************************/

size_t RexxEntry SysDropRexxMacro(const char *name, size_t numargs, CONSTRXSTRING args[], const char *queuename, PRXSTRING retstr)
{
  if (numargs != 1)                    /* wrong number?              */
    return INVALID_ROUTINE;            /* raise error condition      */

  RETVAL(RexxDropMacro(args[0].strptr));  /* try to drop the macro   */
}

/*************************************************************************
* Function:  SysQueryRexxMacro                                           *
*                                                                        *
* Syntax:    result = SysQueryRexxMacro(name)                            *
*                                                                        *
* Params:    name   - name of the macro space function                   *
*                                                                        *
* Return:    position of the macro ('B' or 'A'), returns null for errors.*
*************************************************************************/

size_t RexxEntry SysQueryRexxMacro(const char *name, size_t numargs, CONSTRXSTRING args[], const char *queuename, PRXSTRING retstr)
{
  USHORT      position;                /* returned position          */

  if (numargs != 1)                    /* wrong number?              */
    return INVALID_ROUTINE;            /* raise error condition      */
                                       /* query the macro position   */
  if (RexxQueryMacro(args[0].strptr, &position))
    retstr->strlength = 0;             /* return a null string       */
  else {
                                       /* before?                    */
    if (position == RXMACRO_SEARCH_BEFORE)
      retstr->strptr[0] = 'B';         /* return a 'B'               */
    else
      retstr->strptr[0] = 'A';         /* must be 'A'fter            */
    retstr->strlength = 1;             /* returning one character    */
  }
  return VALID_ROUTINE;                /* good completion            */
}

/*************************************************************************
* Function:  SysClearRexxMacroSpace                                      *
*                                                                        *
* Syntax:    result = SysClearRexxMacroSpace()                           *
*                                                                        *
* Params:    none                                                        *
*                                                                        *
* Return:    return code from RexxClearMacroSpace()                      *
*************************************************************************/

size_t RexxEntry SysClearRexxMacroSpace(const char *name, size_t numargs, CONSTRXSTRING args[], const char *queuename, PRXSTRING retstr)
{
  if (numargs)                         /* wrong number?              */
    return INVALID_ROUTINE;            /* raise error condition      */
  RETVAL(RexxClearMacroSpace());       /* clear the macro space      */
}

/*************************************************************************
* Function:  SysSaveRexxMacroSpace                                       *
*                                                                        *
* Syntax:    result = SysSaveRexxMacroSpace(file)                        *
*                                                                        *
* Params:    file   - name of the saved macro space file                 *
*                                                                        *
* Return:    return code from RexxSaveMacroSpace()                       *
*************************************************************************/

size_t RexxEntry SysSaveRexxMacroSpace(const char *name, size_t numargs, CONSTRXSTRING args[], const char *queuename, PRXSTRING retstr)
{
  if (numargs != 1)                    /* wrong number?              */
    return INVALID_ROUTINE;            /* raise error condition      */
                                       /* clear the macro space      */
  RETVAL(RexxSaveMacroSpace(0, NULL, args[0].strptr));
}

/*************************************************************************
* Function:  SysLoadRexxMacroSpace                                       *
*                                                                        *
* Syntax:    result = SysLoadRexxMacroSpace(file)                        *
*                                                                        *
* Params:    file   - name of the saved macro space file                 *
*                                                                        *
* Return:    return code from RexxLoadMacroSpace()                       *
*************************************************************************/

size_t RexxEntry SysLoadRexxMacroSpace(const char *name, size_t numargs, CONSTRXSTRING args[], const char *queuename, PRXSTRING retstr)
{
  if (numargs != 1)                    /* wrong number?              */
    return INVALID_ROUTINE;            /* raise error condition      */
                                       /* clear the macro space      */
  RETVAL(RexxLoadMacroSpace(0, NULL, args[0].strptr));
}


/*************************************************************************
* Function:  SysBootDrive                                                *
*                                                                        *
* Syntax:    drive = SysBootDrive()                                      *
*                                                                        *
* Params:    none                                                        *
*                                                                        *
* Return:    'A: B: C: D: ...'                                           *
*************************************************************************/

size_t RexxEntry SysBootDrive(const char *name, size_t numargs, CONSTRXSTRING args[], const char *queuename, PRXSTRING retstr)
{
  if (numargs)                         /* validate arguments         */
    return INVALID_ROUTINE;

  if (GetSystemDirectory(retstr->strptr, 255) > 0)
  {
     retstr->strptr[2] = '\0';
     retstr->strlength = 2;
  }
  else
     retstr->strlength = 0;
  return VALID_ROUTINE;                /* no error on call           */
}


/*************************************************************************
* Function:  SysSystemDirectory                                          *
*                                                                        *
* Syntax:    drive = SysSystemDirectory()                                *
*                                                                        *
* Params:    none                                                        *
*                                                                        *
* Return:    'C:\WINDOWS ...'                                            *
*************************************************************************/

size_t RexxEntry SysSystemDirectory(const char *name, size_t numargs, CONSTRXSTRING args[], const char *queuename, PRXSTRING retstr)
{
  if (numargs)                         /* validate arguments         */
    return INVALID_ROUTINE;

  if (GetSystemDirectory(retstr->strptr, 255) > 0)
  {
     retstr->strlength = strlen(retstr->strptr);
  }
  else
     retstr->strlength = 0;
  return VALID_ROUTINE;                /* no error on call           */
}


/*************************************************************************
* Function:  SysFileSystemType                                           *
*                                                                        *
* Syntax:    result = SysFileSystemType("drive")                         *
*                                                                        *
* Params:    drive - drive letter (in form of 'D:')                      *
*        or  none - current drive                                        *
*                                                                        *
* Return:    result - File System Name attached to the specified drive   *
*                     (FAT, HPFS ....)                                   *
*            '' - Empty string in case of any error                      *
*************************************************************************/

size_t RexxEntry SysFileSystemType(const char *name, size_t numargs, CONSTRXSTRING args[], const char *queuename, PRXSTRING retstr)
{
  char *      drive;
  CHAR chDriveLetter[4];
  UINT errorMode;

                                       /* validate arguments         */
  if (numargs > 1 || ((numargs == 1) &&
      (args[0].strlength > 2 ||
      args[0].strlength == 0)))
    return INVALID_ROUTINE;
                                       /* validate the arg           */
                                       /* drive letter?              */
  if ((numargs == 1) && (strlen(args[0].strptr) == 2 && args[0].strptr[1] != ':'))
    return INVALID_ROUTINE;

  if (numargs == 1)
  {
     if (args[0].strlength == 1)       /* need to add a : if only the*/
     {
        chDriveLetter[0]=args[0].strptr[0]; /* letter was passed in  */
        chDriveLetter[1]=':';
        chDriveLetter[2]='\\';         /* need to add \ because of   */
        chDriveLetter[3]='\0';         /* bug in getvolumeinfo       */
      }
      else                        /* have <letter>: , just copy over */
      {
         strcpy(chDriveLetter, args[0].strptr);
         chDriveLetter[2]='\\';        /* need to add \ because of   */
         chDriveLetter[3]='\0';        /* bug in getvolumeinfo       */
      }
      drive = chDriveLetter;
  } else drive = NULL;

  errorMode = SetErrorMode(SEM_FAILCRITICALERRORS);

  if (GetVolumeInformation(
    drive,    // address of root directory of the file system
    NULL,    // address of name of the volume
    0,    // length of lpVolumeNameBuffer
    NULL,    // address of volume serial number
    NULL,    // address of system's maximum filename length
    NULL,    // address of file system flags
    retstr->strptr,    // address of name of file system
    255     // length of lpFileSystemNameBuffer
  ))
     retstr->strlength = strlen(retstr->strptr);
  else
     retstr->strlength = 0;            /* return a null string       */

  SetErrorMode(errorMode);
  return VALID_ROUTINE;                /* good completion            */
}


/*************************************************************************
* Function:  SysVolumeLabel                                              *
*                                                                        *
* Syntax:    result = SysVolumeLabel("drive")                            *
*                                                                        *
* Params:    drive - drive letter (in form of 'D:')                      *
*        or  none - current drive                                        *
*                                                                        *
* Return     '' - Empty string in case of any error                      *
*************************************************************************/

size_t RexxEntry SysVolumeLabel(const char *name, size_t numargs, CONSTRXSTRING args[], const char *queuename, PRXSTRING retstr)
{
  char  *      drive;
  CHAR chDriveLetter[4];

                                       /* validate arguments         */
  if (numargs > 1 || ((numargs == 1) &&
      (args[0].strlength > 2 ||
      args[0].strlength == 0)))
    return INVALID_ROUTINE;
                                       /* validate the arg           */
                                       /* drive letter?              */
  if ((numargs == 1) && (strlen(args[0].strptr) == 2 && args[0].strptr[1] != ':'))
    return INVALID_ROUTINE;

  if (numargs == 1)
  {
     if (args[0].strlength == 1)       /* need to add a : if only the*/
     {
        chDriveLetter[0]=args[0].strptr[0];  /* letter was passed in */
        chDriveLetter[1]=':';
        chDriveLetter[2]='\\';         /* need to add \ because of   */
        chDriveLetter[3]='\0';         /* bug in getvolumeinfo       */
      }
      else                        /* have <letter>: , just copy over */
      {
         strcpy(chDriveLetter, args[0].strptr);
         chDriveLetter[2]='\\';        /* need to add \ because of   */
         chDriveLetter[3]='\0';        /* bug in getvolumeinfo       */
      }
      drive = chDriveLetter;
  } else drive = NULL;

  if (GetVolumeInformation(
    drive,           /* address of root directory of the file system */
    retstr->strptr,                  /*address of name of the volume */
    255,                             /* length of lpVolumeNameBuffer */
    NULL,                         /* address of volume serial number */
    NULL,             /* address of system's maximum filename length */
    NULL,                            /* address of file system flags */
    NULL,                          /* address of name of file system */
    0                            /* length of lpFileSystemNameBuffer */
  ))
     retstr->strlength = strlen(retstr->strptr);
  else
     retstr->strlength = 0;            /* return a null string       */
  return VALID_ROUTINE;                /* good completion            */
}


/*************************************************************************
* Function:  SysCreateMutexSem                                           *
*                                                                        *
* Syntax:    handle = SysCreateMutexSem(<name>)                          *
*                                                                        *
* Params:    name  - optional name for a mutex semaphore                 *
*                                                                        *
* Return:    handle - token used as a mutex handle for                   *
*                     SysRequestMutexSem, SysReleaseMutexSem,            *
*                     SysCloseMutexSem, and SysOpenEventSem              *
*            '' - Empty string in case of any error                      *
*************************************************************************/

size_t RexxEntry SysCreateMutexSem(const char *name, size_t numargs, CONSTRXSTRING args[], const char *queuename, PRXSTRING retstr)
{
    HANDLE    handle = 0;
    SECURITY_ATTRIBUTES sa={sizeof(SECURITY_ATTRIBUTES), NULL, true};

    if ( numargs == 1 )
    {
        // Request for a named semaphore
        handle = CreateMutex(&sa, FALSE, args[0].strptr);
        if ( handle == NULL )
        {
            // It may have been already created, so open it by name.
            handle = OpenMutex(MUTEX_ALL_ACCESS, TRUE, args[0].strptr);
        }
    }
    else
    {
        // Request for an unamed semaphore.
        handle = CreateMutex(&sa, FALSE, NULL);
    }

    if ( handle == NULL )
    {
        // Failed, return the empty string.
        retstr->strlength = 0;
    }
    else
    {
        // Good handle, format it as a pointer string.
        retstr->strlength = sprintf(retstr->strptr, "%p", handle);
    }
    return VALID_ROUTINE;                /* good completion            */
}

/*************************************************************************
* Function:  SysOpenMutexSem                                             *
*                                                                        *
* Syntax:    result = SysOpenMutexSem(name)                              *
*                                                                        *
* Params:    name - name of the mutex semaphore                          *
*                                                                        *
* Return:    result - handle to the mutex                                *
*************************************************************************/

size_t RexxEntry SysOpenMutexSem(const char *name, size_t numargs, CONSTRXSTRING args[], const char *queuename, PRXSTRING retstr)
{
  HANDLE handle;

  // One and only one argument accepted.
  if (numargs != 1)
  {
    return INVALID_ROUTINE;
  }

  handle = OpenMutex(MUTEX_ALL_ACCESS, TRUE, args[0].strptr);
  if ( handle == NULL )
  {
      RETVAL(0)
  }

  // Good handle, format it as a pointer string.
  retstr->strlength = wsprintf(retstr->strptr, "%p", handle);
  return VALID_ROUTINE;                /* good completion            */
}

/*************************************************************************
* Function:  SysReleaseMutexSem                                          *
*                                                                        *
* Syntax:    result = SysReleaseMutexSem(handle)                         *
*                                                                        *
* Params:    handle - token returned from SysCreateMutexSem              *
*                                                                        *
* Return:    result - return code from ReleaseMutex                      *
*************************************************************************/

size_t RexxEntry SysReleaseMutexSem(const char *name, size_t numargs, CONSTRXSTRING args[], const char *queuename, PRXSTRING retstr)
{
  void     *handle;                    /* mutex handle               */

  if (numargs != 1)                    /* Only one argument accepted */
    return INVALID_ROUTINE;            /* raise error condition      */
                                       /* get a binary handle        */
  if (!string2pointer(args[0].strptr, &handle))
    return INVALID_ROUTINE;            /* raise error if bad         */
  if (!ReleaseMutex((HANDLE)handle))
     RETVAL(GetLastError())
  else
     RETVAL(0)
}

/*************************************************************************
* Function:  SysCloseMutexSem                                            *
*                                                                        *
* Syntax:    result = SysCloseMutexSem(handle)                           *
*                                                                        *
* Params:    handle - token returned from SysCreateMutexSem              *
*                                                                        *
* Return:    result - return code from CloseHandle                       *
*************************************************************************/

size_t RexxEntry SysCloseMutexSem(const char *name, size_t numargs, CONSTRXSTRING args[], const char *queuename, PRXSTRING retstr)
{
  void     *handle;                    /* mutex handle               */

  if (numargs != 1)                    /* Only one argument accepted */
    return INVALID_ROUTINE;            /* raise error condition      */
                                       /* get a binary handle        */
  if (!string2pointer(args[0].strptr, &handle))
    return INVALID_ROUTINE;            /* raise error if bad         */
  if (!CloseHandle((HANDLE)handle))
     RETVAL(GetLastError())
  else
     RETVAL(0)
}

/*************************************************************************
* Function:  SysRequestMutexSem                                          *
*                                                                        *
* Syntax:    result = SysRequestMutexSem(handle, <timeout>)              *
*                                                                        *
* Params:    handle - token returned from SysCreateMutexSem              *
*                                                                        *
* Return:    result - return code from WaitForSingleObject               *
*************************************************************************/

size_t RexxEntry SysRequestMutexSem(const char *name, size_t numargs, CONSTRXSTRING args[], const char *queuename, PRXSTRING retstr)
{
  void     *handle;                    /* mutex handle               */
  RexxReturnCode    rc;                        /* creation return code       */
  int       timeout;                   /* timeout value              */

  if (numargs < 1 ||                   /* too few, or                */
      numargs > 2 ||                   /* too many, or               */
      !RXVALIDSTRING(args[0]))         /* first is omitted           */
    return INVALID_ROUTINE;            /* raise error condition      */
  timeout = INFINITE;                  /* default is no timeout      */
  if (numargs == 2) {                  /* have a timeout value?      */
                                       /* get number of seconds      */
    if (!string2long(args[1].strptr, &timeout))
      return INVALID_ROUTINE;          /* raise error if bad         */
  }
                                       /* get a binary handle        */
  if (!string2pointer(args[0].strptr, &handle))
    return INVALID_ROUTINE;            /* raise error if bad         */
                                       /* request the semaphore      */
  rc = WaitForSingleObject((HANDLE)handle, timeout);
  if (rc == WAIT_FAILED)
    wsprintf(retstr->strptr, "%d", GetLastError());
  else
    wsprintf(retstr->strptr, "%d", rc);/* format the return code     */
  retstr->strlength = strlen(retstr->strptr);
  return VALID_ROUTINE;                /* good completion            */
}

/*************************************************************************
* Function:  SysCreateEventSem                                           *
*                                                                        *
* Syntax:    handle = SysCreateEventSem(<name>,<manual>)                 *
*                                                                        *
* Params:    name  - optional name for a event semaphore                 *
*            any second argument means manual reset event                *
* Return:    handle - token used as a event sem handle for               *
*                     SysPostEventSem, SysClearEventSem,                 *
*                     SysCloseEventSem, and SysOpenEventSem              *
*            '' - Empty string in case of any error                      *
*************************************************************************/

size_t RexxEntry SysCreateEventSem(const char *name, size_t numargs, CONSTRXSTRING args[], const char *queuename, PRXSTRING retstr)
{
    HANDLE    handle = 0;
    SECURITY_ATTRIBUTES sa={sizeof(SECURITY_ATTRIBUTES), NULL, TRUE};
    bool      manual;

    if ( numargs > 2 )
    {
        return INVALID_ROUTINE;
    }
    else if ( numargs == 2 )
    {
        manual = true;
    }
    else
    {
        manual = false;
    }

    if ( (numargs >= 1) && args[0].strptr != 0 && (strlen(args[0].strptr) > 0) )
    {
        // Request for a named semaphore.
        handle = CreateEvent(&sa, manual, FALSE, args[0].strptr);
        if ( handle == NULL )
        {
            // It may already exist, so try to open it.
            handle = OpenEvent(EVENT_ALL_ACCESS, TRUE, args[0].strptr);
        }
    }
    else
    {
        // Request for unamed semaphore.
        handle = CreateEvent(&sa, manual, FALSE, NULL);
    }

    if ( handle == NULL )
    {
        // Failed, return null string.
        retstr->strlength = 0;
    }
    else
    {
        // Format the result as a pointer string.
        retstr->strlength = sprintf(retstr->strptr, "%p", handle);
    }
    return VALID_ROUTINE;                /* good completion            */
}

/*************************************************************************
* Function:  SysOpenEventSem                                             *
*                                                                        *
* Syntax:    result = SysOpenEventSem(name)                              *
*                                                                        *
* Params:    name - name of the event semaphore                          *
*                                                                        *
* Return:    result - return code from OpenEvent                         *
*************************************************************************/

size_t RexxEntry SysOpenEventSem(const char *name, size_t numargs, CONSTRXSTRING args[], const char *queuename, PRXSTRING retstr)
{
      HANDLE handle;

      /* Only one argument accepted */
      if (numargs != 1)
      {
          return INVALID_ROUTINE;
      }

      handle = OpenEvent(EVENT_ALL_ACCESS, TRUE, args[0].strptr);
      if ( handle == NULL )
      {
          RETVAL(0)
      }

      // Good handle, format it as pointer string.
      retstr->strlength = wsprintf(retstr->strptr, "%p", handle);
      return VALID_ROUTINE;
}

/*************************************************************************
* Function:  SysPostEventSem                                             *
*                                                                        *
* Syntax:    result = SysPostEventSem(handle)                            *
*                                                                        *
* Params:    handle - token returned from SysCreateEventSem              *
*                                                                        *
* Return:    result - return code from SetEvent                          *
*************************************************************************/

size_t RexxEntry SysPostEventSem(const char *name, size_t numargs, CONSTRXSTRING args[], const char *queuename, PRXSTRING retstr)
{
  void     *handle;                    /* mutex handle               */

  if (numargs != 1)                    /* Only one argument accepted */
    return INVALID_ROUTINE;            /* raise error condition      */
                                       /* get a binary handle        */
  if (!string2pointer(args[0].strptr, &handle))
    return INVALID_ROUTINE;            /* raise error if bad         */
  if (!SetEvent((HANDLE)handle))
     RETVAL(GetLastError())
  else
     RETVAL(0)
}

/*************************************************************************
* Function:  SysResetEventSem                                            *
*                                                                        *
* Syntax:    result = SysResetEventSem(handle)                           *
*                                                                        *
* Params:    handle - token returned from SysCreateEventSem              *
*                                                                        *
* Return:    result - return code from ResetEvent                        *
*************************************************************************/

size_t RexxEntry SysResetEventSem(const char *name, size_t numargs, CONSTRXSTRING args[], const char *queuename, PRXSTRING retstr)
{
  void     *handle;                    /* mutex handle               */

  if (numargs != 1)                    /* Only one argument accepted */
    return INVALID_ROUTINE;            /* raise error condition      */
                                       /* get a binary handle        */
  if (!string2pointer(args[0].strptr, &handle))
    return INVALID_ROUTINE;            /* raise error if bad         */
  if (!ResetEvent((HANDLE)handle))
     RETVAL(GetLastError())
  else
     RETVAL(0)
}


/*************************************************************************
* Function:  SysPulseEventSem                                            *
*                                                                        *
* Syntax:    result = SysPulseEventSem(handle)                           *
*                                                                        *
* Params:    handle - token returned from SysCreateEventSem              *
*                                                                        *
* Return:    result - return code from PulseEvent                        *
*************************************************************************/

size_t RexxEntry SysPulseEventSem(const char *name, size_t numargs, CONSTRXSTRING args[], const char *queuename, PRXSTRING retstr)
{
  void     *handle;                    /* mutex handle               */

  if (numargs != 1)                    /* Only one argument accepted */
    return INVALID_ROUTINE;            /* raise error condition      */
                                       /* get a binary handle        */
  if (!string2pointer(args[0].strptr, &handle))
    return INVALID_ROUTINE;            /* raise error if bad         */
  if (!PulseEvent((HANDLE)handle))
     RETVAL(GetLastError())
  else
     RETVAL(0)
}


/*************************************************************************
* Function:  SysCloseEventSem                                            *
*                                                                        *
* Syntax:    result = SysCloseEventSem(handle)                           *
*                                                                        *
* Params:    handle - token returned from SysCreateEventSem              *
*                                                                        *
* Return:    result - return code from CloseHandle                       *
*************************************************************************/

size_t RexxEntry SysCloseEventSem(const char *name, size_t numargs, CONSTRXSTRING args[], const char *queuename, PRXSTRING retstr)
{
  void     *handle;                    /* mutex handle               */

  if (numargs != 1)                    /* Only one argument accepted */
    return INVALID_ROUTINE;            /* raise error condition      */
                                       /* get a binary handle        */
  if (!string2pointer(args[0].strptr, &handle))
    return INVALID_ROUTINE;            /* raise error if bad         */
  if (!CloseHandle((HANDLE)handle))
     RETVAL(GetLastError())
  else
     RETVAL(0)
}

/*************************************************************************
* Function:  SysWaitEventSem                                             *
*                                                                        *
* Syntax:    result = SysWaitEventSem(handle, <timeout>)                 *
*                                                                        *
* Params:    handle - token returned from SysWaitEventSem                *
*                                                                        *
* Return:    result - return code from WaitForSingleObject               *
*************************************************************************/

size_t RexxEntry SysWaitEventSem(const char *name, size_t numargs, CONSTRXSTRING args[], const char *queuename, PRXSTRING retstr)
{
  void     *handle;                    /* mutex handle               */
  RexxReturnCode    rc;                        /* creation return code       */
  int       timeout;                   /* timeout value              */

  if (numargs < 1 ||                   /* too few, or                */
      numargs > 2 ||                   /* too many, or               */
      !RXVALIDSTRING(args[0]))         /* first is omitted           */
    return INVALID_ROUTINE;            /* raise error condition      */
  timeout = INFINITE;       /* default is no timeout      */
  if (numargs == 2) {                  /* have a timeout value?      */
                                       /* get number of seconds      */
    if (!string2long(args[1].strptr, &timeout))
      return INVALID_ROUTINE;          /* raise error if bad         */
  }
                                       /* get a binary handle        */
  if (!string2pointer(args[0].strptr, &handle))
    return INVALID_ROUTINE;            /* raise error if bad         */
                                       /* request the semaphore      */
  rc = WaitForSingleObject((HANDLE)handle, timeout);
  if (rc == WAIT_FAILED)
    wsprintf(retstr->strptr, "%d", GetLastError());
  else
    wsprintf(retstr->strptr, "%d", rc);/* format the return code     */
  retstr->strlength = strlen(retstr->strptr);
  return VALID_ROUTINE;                /* good completion            */
}



/*************************************************************************
* Function:  SysSetPriority                                              *
*                                                                        *
* Syntax:    result = SysSetPriority(Class, Level)                       *
*                                                                        *
* Params: Class - The priority class (0-3 or HIGH,REALTIME,NORMAL,IDLE)  *
*         Level  - Amount to change (-15 to +15 or IDLE, LOWEST,...)     *
*                                                                        *
*************************************************************************/

size_t RexxEntry SysSetPriority(const char *name, size_t numargs, CONSTRXSTRING args[], const char *queuename, PRXSTRING retstr)
{
  int       pclass;                    /* priority class             */
  int       level;                     /* priority level             */
  RexxReturnCode    rc;                        /* creation return code       */
  HANDLE    process;
  HANDLE    thread;
  DWORD     iclass=-1;

  process = GetCurrentProcess();
  thread = GetCurrentThread();

  if (numargs != 2 ||                  /* must have two              */
      !RXVALIDSTRING(args[0]))         /* first is omitted           */
    return INVALID_ROUTINE;            /* raise error condition      */

  if (string2long(args[0].strptr, &pclass))
  {
    if (pclass < 0 || pclass > 3)
        return INVALID_ROUTINE;        /* raise error condition      */
    switch (pclass) {
       case 0: iclass = IDLE_PRIORITY_CLASS;
               break;
       case 1: iclass = NORMAL_PRIORITY_CLASS;
               break;
       case 2: iclass = HIGH_PRIORITY_CLASS;
               break;
       case 3: iclass = REALTIME_PRIORITY_CLASS;
    };
  }
  else
  if (!stricmp(args[0].strptr, "REALTIME")) iclass = REALTIME_PRIORITY_CLASS; else
  if (!stricmp(args[0].strptr, "HIGH")) iclass = HIGH_PRIORITY_CLASS; else
  if (!stricmp(args[0].strptr, "NORMAL")) iclass = NORMAL_PRIORITY_CLASS; else
  if (!stricmp(args[0].strptr, "IDLE")) iclass = IDLE_PRIORITY_CLASS;
  if (iclass == -1)  return INVALID_ROUTINE; /* raise error condition*/


  if (string2long(args[1].strptr, &level))
  {
      if (level < -15 || level > 15)
          return INVALID_ROUTINE;      /* raise error condition      */
  }
  else
  {
      if (!stricmp(args[1].strptr, "ABOVE_NORMAL")) level = THREAD_PRIORITY_ABOVE_NORMAL; else
      if (!stricmp(args[1].strptr, "BELOW_NORMAL")) level = THREAD_PRIORITY_BELOW_NORMAL; else
      if (!stricmp(args[1].strptr, "HIGHEST")) level = THREAD_PRIORITY_HIGHEST; else
      if (!stricmp(args[1].strptr, "LOWEST")) level = THREAD_PRIORITY_LOWEST; else
      if (!stricmp(args[1].strptr, "NORMAL")) level = THREAD_PRIORITY_NORMAL; else
      if (!stricmp(args[1].strptr, "IDLE")) level = THREAD_PRIORITY_IDLE; else
      if (!stricmp(args[1].strptr, "TIME_CRITICAL")) level = THREAD_PRIORITY_TIME_CRITICAL;
      else return INVALID_ROUTINE;     /* raise error condition      */
  }

  rc = SetPriorityClass(process, iclass);
  if (rc)
     rc = SetThreadPriority(thread, level);

  if (!rc)
     RETVAL(GetLastError())
  else
     RETVAL(0)
}

/*************************************************************************
* Function:  SysQueryProcess                                             *
*                                                                        *
* Params:    "PID" - (default) returns current process ID                *
*            "TID" - (default) returns current thread ID                 *
*            "PPRIO" - (default) returns current process priority        *
*            "TPRIO" - (default) returns current thread priority         *
*            "PTIME" - (default) returns current process times           *
*            "TTIME" - (default) returns current thread times            *
*************************************************************************/

size_t RexxEntry SysQueryProcess(const char *name, size_t numargs, CONSTRXSTRING args[], const char *queuename, PRXSTRING retstr)
{
  if (numargs > 1)                  /* none or one argument accepted */
    return INVALID_ROUTINE;            /* raise error condition      */

  if ((numargs == 0) || (!stricmp(args[0].strptr, "PID")))
  {
      ltoa(GetCurrentProcessId(), retstr->strptr, 10);
      retstr->strlength = strlen(retstr->strptr);
      return VALID_ROUTINE;            /* good completion            */
  } else
  if (!stricmp(args[0].strptr, "TID"))
  {
      ltoa(GetCurrentThreadId(), retstr->strptr, 10);
      retstr->strlength = strlen(retstr->strptr);
      return VALID_ROUTINE;            /* good completion            */
  } else
  if (!stricmp(args[0].strptr, "PPRIO"))
  {
      LONG p;
      p = GetPriorityClass(GetCurrentProcess());

      switch(p) {
        case HIGH_PRIORITY_CLASS: strcpy(retstr->strptr, "HIGH");
            break;
        case IDLE_PRIORITY_CLASS: strcpy(retstr->strptr, "IDLE");
            break;
        case NORMAL_PRIORITY_CLASS: strcpy(retstr->strptr, "NORMAL");
            break;
        case REALTIME_PRIORITY_CLASS: strcpy(retstr->strptr, "REALTIME");
            break;
        default: strcpy(retstr->strptr, "UNKNOWN");
      }
      retstr->strlength = strlen(retstr->strptr);
      return VALID_ROUTINE;            /* good completion            */
  } else
  if (!stricmp(args[0].strptr, "TPRIO"))
  {
      LONG p;
      p = GetThreadPriority(GetCurrentThread());

      switch(p) {
        case THREAD_PRIORITY_ABOVE_NORMAL: strcpy(retstr->strptr, "ABOVE_NORMAL");
            break;
        case THREAD_PRIORITY_BELOW_NORMAL: strcpy(retstr->strptr, "BELOW_NORMAL");
            break;
        case THREAD_PRIORITY_HIGHEST: strcpy(retstr->strptr, "HIGHEST");
            break;
        case THREAD_PRIORITY_IDLE: strcpy(retstr->strptr, "IDLE");
            break;
        case THREAD_PRIORITY_LOWEST: strcpy(retstr->strptr, "LOWEST");
            break;
        case THREAD_PRIORITY_NORMAL: strcpy(retstr->strptr, "NORMAL");
            break;
        case THREAD_PRIORITY_TIME_CRITICAL: strcpy(retstr->strptr, "TIME_CRITICAL");
            break;
        default: strcpy(retstr->strptr, "UNKNOWN");
      }
      retstr->strlength = strlen(retstr->strptr);
      return VALID_ROUTINE;            /* good completion            */
  } else
  if ((!stricmp(args[0].strptr, "PTIME")) || (!stricmp(args[0].strptr, "TTIME")))
  {
      FILETIME createT, kernelT, userT, dummy;
      SYSTEMTIME createST, kernelST, userST;
      BOOL ok;

      if ((args[0].strptr[0] == 'T') || (args[0].strptr[0] == 't'))
          ok = GetThreadTimes(GetCurrentThread(), &createT,&dummy,&kernelT, &userT);
      else
          ok = GetProcessTimes(GetCurrentProcess(), &createT,&dummy,&kernelT, &userT);

      if (ok)
      {
          FileTimeToLocalFileTime(&createT, &createT);
          FileTimeToSystemTime(&createT, &createST);
          FileTimeToSystemTime(&kernelT, &kernelST);
          FileTimeToSystemTime(&userT, &userST);

          wsprintf(retstr->strptr,"Create: %4.4d/%2.2d/%2.2d %d:%2.2d:%2.2d:%3.3d  "\
              "Kernel: %d:%2.2d:%2.2d:%3.3d  User: %d:%2.2d:%2.2d:%3.3d",
              createST.wYear,createST.wMonth,createST.wDay,createST.wHour,createST.wMinute,
              createST.wSecond,createST.wMilliseconds,
              kernelST.wHour,kernelST.wMinute,kernelST.wSecond,kernelST.wMilliseconds,
              userST.wHour,userST.wMinute,userST.wSecond,userST.wMilliseconds);

          retstr->strlength = strlen(retstr->strptr);
          return VALID_ROUTINE;        /* good completion            */
      }
  }
  return INVALID_ROUTINE;              /* good completion            */
}

/**********************************************************************
* Function:  SysShutDownSystem                                        *
*                                                                     *
* Syntax:    call SysShutDownSystem(<computer>,<message>,<timeout>,<appclose>,<reboot> *
*                                                                     *
* Params:    <computer> - name of the remote machine ('' = local)     *
*            <message>  - message for dialog                          *
*            <timeout>  - time to display message                     *
*            <appclose> - no dialog "save unsaved data"               *
*            <reboot>   - reboot the system                           *
*                                                                     *
* Return:    success (1) or failure (0) string                        *
**********************************************************************/

size_t RexxEntry SysShutDownSystem(const char *name, size_t numargs, CONSTRXSTRING args[], const char *queuename, PRXSTRING retstr)
{
    const char  * machine = NULL;
    const char  * message = NULL;
    size_t timeout= 0;
    LONG  rc = 0;
    size_t forceClose = false;
    size_t reboot = false;

    // More than 5 args is invalid.
    if ( numargs>5 )
    {
        return INVALID_ROUTINE;
    }

    if ( numargs >= 1 && RXVALIDSTRING(args[0]) )
    {
        machine = args[0].strptr;
    }
    if ( numargs >= 2 && RXVALIDSTRING(args[1]) )
    {
        message = args[1].strptr;
    }
    if ( numargs >= 3 && RXVALIDSTRING(args[2]) )
    {
         if ( !string2ulong(args[2].strptr, &timeout) )
         {
            return INVALID_ROUTINE;
         }
    }
    if ( numargs >= 4 && RXVALIDSTRING(args[3]) )
    {
        if ( !string2ulong(args[3].strptr, &forceClose) )
        {
            return INVALID_ROUTINE;
        }
    }
    if ( numargs >= 5 && RXVALIDSTRING(args[4]) )
    {
        if ( !string2ulong(args[4].strptr, &reboot) )
        {
            return INVALID_ROUTINE;
        }
    }

    /* Display the shutdown dialog box and start the time-out countdown. */
    if ( !InitiateSystemShutdown(const_cast<LPSTR>(machine), const_cast<LPSTR>(message), (DWORD)timeout, (BOOL)forceClose, (BOOL)reboot) )
    {
        RETVAL(GetLastError())
    }
    else
    {
        RETVAL(0)
    }
}

/*************************************************************************
* Function:  SysSwitchSession                                            *
*                                                                        *
* Syntax:    result = SysSwitchSession(name)                             *
*                                                                        *
* Params:    name   - name of target session                             *
*                                                                        *
* Return:    OS/2 error return code                                      *
*************************************************************************/

size_t RexxEntry SysSwitchSession(const char *name, size_t numargs, CONSTRXSTRING args[], const char *queuename, PRXSTRING retstr)
{
    HWND hwnd;

    if ( numargs != 1 )
    {
        /* raise error condition  */
        return INVALID_ROUTINE;
    }

    hwnd = FindWindow(NULL, args[0].strptr);

    if ( hwnd == NULL )
    {
        RETVAL(GetLastError())
    }
    else
    {
      if ( !SetForegroundWindow(hwnd) )
      {
          RETVAL(GetLastError())
      }
      else
      {
          RETVAL(0)
      }
    }
    return VALID_ROUTINE;                /* good completion            */
}

/*************************************************************************
* Function:  SysWaitNamedPipe                                            *
*                                                                        *
* Syntax:    result = SysWaitNamedPipe(name, timeout)                    *
*                                                                        *
* Params:    name - name of the pipe                                     *
*            timeout - amount of time to wait.                           *
*                                                                        *
* Return:    Return code from WaitNamedPipe                              *
*************************************************************************/

size_t RexxEntry SysWaitNamedPipe(const char *name, size_t numargs, CONSTRXSTRING args[], const char *queuename, PRXSTRING retstr)
{
  int         timeout;                 /* timeout value              */

  if (numargs < 1 ||                   /* wrong number of arguments? */
      numargs > 2 ||
      !RXVALIDSTRING(args[0]))
    return INVALID_ROUTINE;            /* raise error condition      */
  timeout = NMPWAIT_USE_DEFAULT_WAIT;

  if (numargs == 2) {                  /* have a timeout value?      */
    if (!string2long(args[1].strptr, &timeout) ||
        (timeout < 0 && timeout != -1))
      return INVALID_ROUTINE;          /* raise error condition      */
  }
  if (timeout == 0) timeout = NMPWAIT_USE_DEFAULT_WAIT;
  else if (timeout == -1) timeout = NMPWAIT_WAIT_FOREVER;

  if (WaitNamedPipe(args[0].strptr, timeout))
      RETVAL(0)
  else
      RETVAL(GetLastError())
  return VALID_ROUTINE;
}

/*********************************************************************/
/* Function FormatFloat:  Common routine to format a floating point  */
/* result for the math functions                                     */
/*********************************************************************/
void FormatResult(
  double    result,                    /* formatted result           */
  size_t    precision,                 /* required precision         */
  PRXSTRING retstr )                   /* return string              */
{
  if (result == 0)                     /* zero result?               */
    strcpy(retstr->strptr, "0");       /* make exactly 0             */
  else
                                       /* format the result          */
    _gcvt(result, (int)precision, retstr->strptr);
                                       /* set the length             */
  retstr->strlength = strlen(retstr->strptr);
                                       /* end in a period?           */
  if (retstr->strptr[retstr->strlength - 1] == '.')
    retstr->strlength--;               /* remove the period          */
}

/*********************************************************************/
/* Function ValidateMath: Common validation routine for math         */
/* that are of the form fn(number, <precision>)                      */
/*********************************************************************/
LONG  ValidateMath(
  size_t    numargs,                   /* Number of arguments.       */
  CONSTRXSTRING  args[],               /* Function arguments.        */
  double   *x,                         /* input number               */
  size_t   *precision )                /* returned precision         */
{
  LONG      rc;                        /* validation code            */

  rc = VALID_ROUTINE;                  /* set default completion     */
  *precision = DEFAULT_PRECISION;      /* set max digits count       */

  if (numargs < 1 ||                   /* no arguments               */
      numargs > 2 ||
      !RXVALIDSTRING(args[0]))         /* first is omitted           */
    rc = INVALID_ROUTINE;              /* this is invalid            */
  else if (numargs == 2 &&             /* have a precision           */
      !string2ulong(args[1].strptr, precision))
    rc = INVALID_ROUTINE;              /* this is invalid            */
                                       /* convert input number       */
  else if (sscanf(args[0].strptr, " %lf", x) != 1)
    rc = INVALID_ROUTINE;              /* this is invalid            */
                                       /* truncate to maximum        */
  *precision = min(*precision, MAX_PRECISION);
  return rc;                           /* return success code        */
}

/*********************************************************************/
/* Function ValidateTrig: Common validation routine for math         */
/* that are of the form fn(number, <precision>, <unit>)              */
/*********************************************************************/
LONG  ValidateTrig(
  size_t    numargs,                   /* Number of arguments.       */
  CONSTRXSTRING  args[],               /* Function arguments.        */
  PRXSTRING retstr,                    /* return string              */
  INT       function )                 /* function to perform        */
{
  LONG      rc;                        /* validation code            */
  INT       units;                     /* angle type                 */
  double    angle;                     /* working angle              */
  double    nsi;                       /* convertion factor          */
  double    nco;                       /* convertion factor          */
  size_t    precision;                 /* returned precision         */
  double    result;                    /* result                     */

  rc = VALID_ROUTINE;                  /* set default completion     */
  precision = DEFAULT_PRECISION;       /* set max digits count       */
  units = DEGREES;                     /* default angle is degrees   */
  nsi = 1.;                            /* set default conversion     */
  nco = 1.;                            /* set default conversion     */

  if (numargs < 1 ||                   /* no arguments               */
      numargs > 3 ||
      !RXVALIDSTRING(args[0]))         /* first is omitted           */
    rc = INVALID_ROUTINE;              /* this is invalid            */
  else if (numargs >= 2 &&             /* have a precision           */
      RXVALIDSTRING(args[1]) &&        /* and it is real string      */
      !string2ulong(args[1].strptr, &precision))
    rc = INVALID_ROUTINE;              /* this is invalid            */
                                       /* convert input number       */
  else if (sscanf(args[0].strptr, " %lf", &angle) != 1)
    rc = INVALID_ROUTINE;              /* this is invalid            */
  else if (numargs == 3) {             /* have an option             */
    if (RXZEROLENSTRING(args[2]))      /* null string?               */
      rc = INVALID_ROUTINE;            /* this is invalid            */
    else {                             /* process the options        */
                                       /* get the option character   */
      units = toupper(args[2].strptr[0]);
                                       /* was it a good option?      */
      if (units != DEGREES && units != RADIANS && units != GRADES)
        rc = INVALID_ROUTINE;          /* bad option is error        */
    }
  }
  if (!rc) {                           /* everything went well?      */
                                       /* truncate to maximum        */
    precision = min(precision, MAX_PRECISION);
    if (units == DEGREES) {            /* need to convert degrees    */
      nsi = (angle < 0.) ? -1. : 1.;   /* get the direction          */
      angle = fmod(fabs(angle), 360.); /* make modulo 360            */
      if (angle <= 45.)                /* less than 45?              */
        angle = angle * pi / 180.;
      else if (angle < 135.) {         /* over on the other side?    */
        angle = (90. - angle) * pi / 180.;
        function = MAXTRIG - function; /* change the function        */
        nco = nsi;                     /* swap around the conversions*/
        nsi = 1.;
      }
      else if (angle <= 225.) {        /* around the other way?      */
        angle = (180. - angle) * pi / 180.;
        nco = -1.;
      }
      else if (angle < 315.) {         /* close to the origin?       */
        angle = (angle - 270.) * pi / 180.;
        function = MAXTRIG - function; /* change the function        */
        nco = -nsi;
        nsi = 1.;
      }
      else
        angle = (angle - 360.) * pi / 180.;
    }
    else if (units == GRADES) {        /* need to convert degrees    */
      nsi = (angle < 0.) ? -1. : 1.;   /* get the direction          */
      angle = fmod(fabs(angle), 400.); /* make modulo 400            */
      if (angle <= 50.)
        angle = angle * pi / 200.;
      else if (angle < 150.) {
        angle = (100. - angle) * pi / 200.;
        function = MAXTRIG - function; /* change the function        */
        nco = nsi;                     /* swap the conversions       */
        nsi = 1.;
      }
      else if (angle <= 250.) {
        angle = (200. - angle) * pi / 200.;
        nco = -1.;
      }
      else if (angle < 350.) {
        angle = (angle - 300.) * pi / 200.;
        function = MAXTRIG - function; /* change the function        */
        nco = -nsi;
        nsi = 1.;
      }
      else
        angle = (angle - 400.) * pi / 200.;
    }
    switch (function) {                /* process the function       */
      case SINE:                       /* Sine function              */
        result = nsi * sin(angle);
        break;
      case COSINE:                     /* Cosine function            */
        result = nco * cos(angle);
        break;
      case TANGENT:                    /* Tangent function           */
        result = nsi * nco * tan(angle);
        break;
      case COTANGENT:                  /* cotangent function         */
                                       /* overflow?                  */
        if ((result = tan(angle)) == 0.0)
          rc = 40;                     /* this is an error           */
        else
          result = nsi * nco / result; /* real result                */
        break;
    }
    if (!rc)                           /* good result?               */
                                       /* format the result          */
      FormatResult(result, precision, retstr);
  }
  return rc;                           /* return success code        */
}

/*********************************************************************/
/* Function ValidateATrig: Common validation routine for math        */
/* that are of the form fn(number, <precision>, <units>)             */
/*********************************************************************/
LONG  ValidateArcTrig(
  size_t     numargs,                  /* Number of arguments.       */
  CONSTRXSTRING   args[],              /* Function arguments.        */
  PRXSTRING  retstr,                   /* return string              */
  INT        function )                /* function to perform        */
{
  LONG      rc;                        /* validation code            */
  INT       units;                     /* angle type                 */
  double    angle;                     /* working angle              */
  double    nsi;                       /* convertion factor          */
  double    nco;                       /* convertion factor          */
  size_t    precision;                 /* returned precision         */
  double    x;                         /* input number               */

  rc = VALID_ROUTINE;                  /* set default completion     */
  precision = DEFAULT_PRECISION;       /* set max digits count       */
  units = DEGREES;                     /* default angle is degrees   */
  nsi = 1.;                            /* set default conversion     */
  nco = 1.;                            /* set default conversion     */

  if (numargs < 1 ||                   /* no arguments               */
      numargs > 3 ||
      !RXVALIDSTRING(args[0]))         /* first is omitted           */
    rc = INVALID_ROUTINE;              /* this is invalid            */
  else if (numargs >= 2 &&             /* have a precision           */
      RXVALIDSTRING(args[1]) &&        /* and it is real string      */
      !string2ulong(args[1].strptr, &precision))
    rc = INVALID_ROUTINE;              /* this is invalid            */
                                       /* convert input number       */
  else if (sscanf(args[0].strptr, " %lf", &x) != 1)
    rc = INVALID_ROUTINE;              /* this is invalid            */
  else if (numargs == 3) {             /* have an option             */
    if (RXZEROLENSTRING(args[2]))      /* null string?               */
      rc = INVALID_ROUTINE;            /* this is invalid            */
    else {                             /* process the options        */
                                       /* get the option character   */
      units = toupper(args[2].strptr[0]);
                                       /* was it a good option?      */
      if (units != DEGREES && units != RADIANS && units != GRADES)
        rc = INVALID_ROUTINE;          /* bad option is error        */
    }
  }
  if (!rc) {                           /* everything went well?      */
                                       /* truncate to maximum        */
    precision = min(precision, MAX_PRECISION);
    switch (function) {                /* process the function       */
      case ARCSINE:                    /* ArcSine function           */
        angle = asin(x);
        break;
      case ARCCOSINE:                  /* ArcCosine function         */
        angle = acos(x);
        break;
      case ARCTANGENT:                 /* ArcTangent function        */
        angle = atan(x);
        break;
    }
    if (units == DEGREES)              /* have to convert the result?*/
      angle = angle * 180. / pi;       /* make into degrees          */
    else if (units == GRADES)          /* need it in grades?         */
      angle = angle * 200. / pi;       /* convert to base 400        */
                                       /* format the result          */
    FormatResult(angle, precision, retstr);
  }
  return rc;                           /* return success code        */
}

/********************************************************************/
/* Functions:           SysSqrt(), SysExp(), SysLog(), SysLog10,    */
/* Functions:           SysSinH(), SysCosH(), SysTanH()             */
/* Description:         Returns function value of argument.         */
/* Input:               One number.                                 */
/* Output:              Value of the function requested for arg.    */
/*                      Returns 0 if the function executed OK,      */
/*                      40 otherwise.  The interpreter will fail    */
/*                      if the function returns a negative result.  */
/* Notes:                                                           */
/*   These routines take one to two parameters.                     */
/*   The form of the call is:                                       */
/*   result = func_name(x <, prec> <,angle>)                        */
/*                                                                  */
/********************************************************************/
size_t RexxEntry SysSqrt(const char *name, size_t numargs, CONSTRXSTRING args[], const char *queuename, PRXSTRING retstr)
{
  double    x;                         /* input number               */
  size_t    precision;                 /* precision used             */
  LONG      rc;                        /* function return code       */

                                       /* validate the inputs        */
  rc = ValidateMath(numargs, args, &x, &precision);
  if (!rc)                             /* good function call         */
                                       /* format the result          */
    FormatResult(sqrt(x), precision, retstr);
  return rc;                           /* return error code          */
}

/*==================================================================*/
size_t RexxEntry SysExp(const char *name, size_t numargs, CONSTRXSTRING args[], const char *queuename, PRXSTRING retstr)
{
  double    x;                         /* input number               */
  size_t    precision;                 /* precision used             */
  LONG      rc;                        /* validation return code     */

                                       /* validate the inputs        */
  rc = ValidateMath(numargs, args, &x, &precision);
  if (!rc)                             /* good function call         */
                                       /* format the result          */
    FormatResult(exp(x), precision, retstr);
  return rc;                           /* return error code          */
}

/*==================================================================*/
size_t RexxEntry SysLog(const char *name, size_t numargs, CONSTRXSTRING args[], const char *queuename, PRXSTRING retstr)
{
  double    x;                         /* input number               */
  size_t    precision;                 /* precision used             */
  LONG      rc;                        /* validation return code     */

                                       /* validate the inputs        */
  rc = ValidateMath(numargs, args, &x, &precision);
  if (!rc)                             /* good function call         */
                                       /* format the result          */
    FormatResult(log(x), precision, retstr);
  return rc;                           /* return error code          */
}

/*==================================================================*/
size_t RexxEntry SysLog10(const char *name, size_t numargs, CONSTRXSTRING args[], const char *queuename, PRXSTRING retstr)
{
  double    x;                         /* input number               */
  size_t    precision;                 /* precision used             */
  LONG      rc;                        /* validation return code     */

                                       /* validate the inputs        */
  rc = ValidateMath(numargs, args, &x, &precision);
  if (!rc)                             /* good function call         */
                                       /* format the result          */
    FormatResult(log10(x), precision, retstr);
  return rc;                           /* return error code          */
}

/*==================================================================*/
size_t RexxEntry SysSinH(const char *name, size_t numargs, CONSTRXSTRING args[], const char *queuename, PRXSTRING retstr)                 /* Hyperbolic sine function.  */
{
  double    x;                         /* input number               */
  size_t    precision;                 /* precision used             */
  LONG      rc;                        /* validation return code     */

                                       /* validate the inputs        */
  rc = ValidateMath(numargs, args, &x, &precision);
  if (!rc)                             /* good function call         */
                                       /* format the result          */
    FormatResult(sinh(x), precision, retstr);
  return rc;                           /* return error code          */
}

/*==================================================================*/
size_t RexxEntry SysCosH(const char *name, size_t numargs, CONSTRXSTRING args[], const char *queuename, PRXSTRING retstr)
{
  double    x;                         /* input number               */
  size_t    precision;                 /* precision used             */
  LONG      rc;                        /* validation return code     */

                                       /* validate the inputs        */
  rc = ValidateMath(numargs, args, &x, &precision);
  if (!rc)                             /* good function call         */
                                       /* format the result          */
    FormatResult(cosh(x), precision, retstr);
  return rc;                           /* return error code          */
}

/*==================================================================*/
size_t RexxEntry SysTanH(const char *name, size_t numargs, CONSTRXSTRING args[], const char *queuename, PRXSTRING retstr)
{
  double    x;                         /* input number               */
  size_t    precision;                 /* precision used             */
  LONG      rc;                        /* validation return code     */

                                       /* validate the inputs        */
  rc = ValidateMath(numargs, args, &x, &precision);
  if (!rc)                             /* good function call         */
                                       /* format the result          */
    FormatResult(tanh(x), precision, retstr);
  return rc;                           /* return error code          */
}

/********************************************************************/
/* Functions:           SysPower()                                  */
/* Description:         Returns function value of arguments.        */
/* Input:               Two numbers.                                */
/* Output:              Value of the x to the power y.              */
/*                      Returns 0 if the function executed OK,      */
/*                      -1 otherwise.  The interpreter will fail    */
/*                      if the function returns a negative result.  */
/* Notes:                                                           */
/*   This routine takes two to three parameters.                    */
/*   The form of the call is:                                       */
/*   result = func_name(x, y <, prec>)                              */
/*                                                                  */
/********************************************************************/
size_t RexxEntry SysPower(const char *name, size_t numargs, CONSTRXSTRING args[], const char *queuename, PRXSTRING retstr)
{
  double    x;                         /* input number               */
  double    y;                         /* second input number        */
  size_t    precision;                 /* precision used             */
  LONG      rc;                        /* validation code            */

  rc = VALID_ROUTINE;                  /* set default completion     */
  precision = DEFAULT_PRECISION;       /* set max digits count       */

  if (numargs < 2 ||                   /* no arguments               */
      numargs > 3 ||
      !RXVALIDSTRING(args[0]) ||       /* first is omitted           */
      !RXVALIDSTRING(args[1]))         /* second is omitted          */
    rc = INVALID_ROUTINE;              /* this is invalid            */
  else if (numargs == 3 &&             /* have a precision           */
      !string2ulong(args[2].strptr, &precision))
    rc = INVALID_ROUTINE;              /* this is invalid            */
                                       /* convert input number       */
  else if (sscanf(args[0].strptr, " %lf", &x) != 1)
    rc = INVALID_ROUTINE;              /* this is invalid            */
                                       /* convert second input       */
  else if (sscanf(args[1].strptr, " %lf", &y) != 1)
    rc = INVALID_ROUTINE;              /* this is invalid            */
  if (!rc) {                           /* good function call         */
                                       /* keep to maximum            */
    precision = min(precision, MAX_PRECISION);
                                       /* format the result          */
    FormatResult(pow(x,y), precision, retstr);
  }
  return rc;                           /* return error code          */
}

/********************************************************************/
/* Functions:           RxSin(), RxCos(), RxTan(), RxCotan()        */
/* Description:         Returns trigonometric angle value.          */
/* Input:               Angle in radian or degree or grade          */
/* Output:              Trigonometric function value for Angle.     */
/*                      Returns 0 if the function executed OK,      */
/*                      -1 otherwise.  The interpreter will fail    */
/*                      if the function returns a negative result.  */
/* Notes:                                                           */
/*   These routines take one to three parameters.                   */
/*   The form of the call is:                                       */
/*   x = func_name(angle <, prec> <, [R | D | G]>)                  */
/*                                                                  */
/********************************************************************/
size_t RexxEntry SysSin(const char *name, size_t numargs, CONSTRXSTRING args[], const char *queuename, PRXSTRING retstr)
{
                                       /* call common routine        */
  return ValidateTrig(numargs, args, retstr, SINE);
}

/*==================================================================*/
size_t RexxEntry SysCos(const char *name, size_t numargs, CONSTRXSTRING args[], const char *queuename, PRXSTRING retstr)
{
                                       /* call common routine        */
  return ValidateTrig(numargs, args, retstr, COSINE);
}

/*==================================================================*/
size_t RexxEntry SysTan(const char *name, size_t numargs, CONSTRXSTRING args[], const char *queuename, PRXSTRING retstr)
{
                                       /* call common routine        */
  return ValidateTrig(numargs, args, retstr, TANGENT);
}

/*==================================================================*/
size_t RexxEntry SysCotan(const char *name, size_t numargs, CONSTRXSTRING args[], const char *queuename, PRXSTRING retstr)
{
                                       /* call common routine        */
  return ValidateTrig(numargs, args, retstr, COTANGENT);
}

/********************************************************************/
/* Functions:           SysPi()                                     */
/* Description:         Returns value of pi for given precision     */
/* Input:               Precision.   Default is 9                   */
/* Output:              Value of the pi to given precision          */
/* Notes:                                                           */
/*   This routine takes one parameters.                             */
/*   The form of the call is:                                       */
/*   result = syspi(<precision>)                                    */
/*                                                                  */
/********************************************************************/
size_t RexxEntry SysPi(const char *name, size_t numargs, CONSTRXSTRING args[], const char *queuename, PRXSTRING retstr)
{
  size_t    precision;                 /* required precision         */

  precision = DEFAULT_PRECISION;       /* set default precision      */
  if (numargs > 1 ||                   /* too many arguments?        */
      (numargs == 1 &&                 /* bad precision?             */
      !string2ulong(args[0].strptr, &precision)))
    return INVALID_ROUTINE;            /* bad routine                */
                                       /* keep to maximum            */
  precision = min(precision, MAX_PRECISION);
                                       /* format the result          */
  FormatResult(pi, precision, retstr); /* format the result          */
  return VALID_ROUTINE;                /* good result                */
}

/********************************************************************/
/* Functions:           SysArcSin(), SysArcCos(), SysArcTan()       */
/* Description:         Returns angle from trigonometric value.     */
/* Input:               Angle in radian or degree or grade          */
/* Output:              Angle for matching trigonometric value.     */
/*                      Returns 0 if the function executed OK,      */
/*                      -1 otherwise.  The interpreter will fail    */
/*                      if the function returns a negative result.  */
/* Notes:                                                           */
/*   These routines take one to three parameters.                   */
/*   The form of the call is:                                       */
/*   a = func_name(arg <, prec> <, [R | D | G]>)                    */
/*                                                                  */
/********************************************************************/
size_t RexxEntry SysArcSin(const char *name, size_t numargs, CONSTRXSTRING args[], const char *queuename, PRXSTRING retstr)
{
                                       /* call common routine        */
  return ValidateArcTrig(numargs, args, retstr, ARCSINE);
}

/*==================================================================*/
size_t RexxEntry SysArcCos(const char *name, size_t numargs, CONSTRXSTRING args[], const char *queuename, PRXSTRING retstr)
{
                                       /* call common routine        */
  return ValidateArcTrig(numargs, args, retstr, ARCCOSINE);
}

/*==================================================================*/
size_t RexxEntry SysArcTan(const char *name, size_t numargs, CONSTRXSTRING args[], const char *queuename, PRXSTRING retstr)
{
                                       /* call common routine        */
  return ValidateArcTrig(numargs, args, retstr, ARCTANGENT);
}


/*************************************************************************
* Function:  SysDumpVariables                                            *
*                                                                        *
* Syntax:    result = SysDumpVariables([filename])                       *
*                                                                        *
* Params:    filename - name of the file where variables are appended to *
*                       (dump is written to stdout if omitted)           *
*                                                                        *
* Return:    0 - dump completed OK                                       *
*            -1 - failure during dump                                     *
*************************************************************************/

size_t RexxEntry SysDumpVariables(const char *name, size_t numargs, CONSTRXSTRING args[], const char *queuename, PRXSTRING retstr)
{
  LONG      rc;                        /* Ret code                   */
  SHVBLOCK  shvb;
  HANDLE    outFile = NULL;
  bool      fCloseFile = false;
  DWORD     dwBytesWritten = 0;
  char     *buffer = NULL;             /* ENG: write result file to  */
  char     *current, *end;             /* memory first, much faster! */
  size_t    buffer_size = 10240;       /* buffer, realloc'd if needed*/
  size_t    new_size;                  /* real new size              */

  if ( (numargs > 1) ||                /* wrong number of arguments? */
       ((numargs > 0) && !RXVALIDSTRING(args[0])) )
    return INVALID_ROUTINE;            /* raise error condition      */

  if (numargs > 0)
  {
    /* open output file for append */
    outFile = CreateFile(args[0].strptr, GENERIC_WRITE | GENERIC_READ,
                         0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL |
                         FILE_FLAG_WRITE_THROUGH, NULL);
    if (outFile)
    {
      fCloseFile = true;

      /* seek to end of file */
      SetFilePointer(outFile, 0, 0, FILE_END);
    }
  }
  else
    outFile = GetStdHandle(STD_OUTPUT_HANDLE);

                                       /* write results to memory    */
                                       /* first and then in one step */
                                       /* to disk                    */
  buffer = (char*) calloc(buffer_size,1);
  if (buffer == NULL)
    return INVALID_ROUTINE;            /* raise error condition      */
  current = buffer;
  end = current + buffer_size;

  do
  {
    /* prepare request block */
    shvb.shvnext = NULL;
    shvb.shvname.strptr = NULL;      /* let REXX allocate the memory */
    shvb.shvname.strlength = 0;
    shvb.shvnamelen = 0;
    shvb.shvvalue.strptr = NULL;     /* let REXX allocate the memory */
    shvb.shvvalue.strlength = 0;
    shvb.shvvaluelen = 0;
    shvb.shvcode = RXSHV_NEXTV;
    shvb.shvret = 0;

    rc = RexxVariablePool(&shvb);

    if (rc == RXSHV_OK)
    {
      new_size = 5 + 9 + 3 + shvb.shvname.strlength + shvb.shvvalue.strlength;
                                     /* if buffer is not big enough, */
                                     /* reallocate                   */
      if (current + new_size >= end) {
        size_t offset = current - buffer;
        buffer_size *= 2;
        /* if new buffer too small, use the minimal fitting size */
        if (buffer_size - offset < new_size) {
          buffer_size = new_size + offset;
        }
        buffer = (char *)realloc(buffer,buffer_size);
        current = buffer + offset;
        end = buffer + buffer_size;
      }
      sprintf(current, "Name=");
      current += 5;
      memcpy(current, shvb.shvname.strptr, shvb.shvname.strlength);
      current += shvb.shvname.strlength;
      sprintf(current, ", Value='");
      current += 9;
      memcpy(current, shvb.shvvalue.strptr, shvb.shvvalue.strlength);
      current += shvb.shvvalue.strlength;
      sprintf(current, "'\r\n");
      current += 3;

      /* free memory allocated by REXX */
      RexxFreeMemory((void *)shvb.shvname.strptr);
      RexxFreeMemory((void *)shvb.shvvalue.strptr);

      /* leave loop if this was the last var */
      if (shvb.shvret & RXSHV_LVAR)
        break;
    }
  } while (rc == RXSHV_OK);

  WriteFile(outFile, buffer, (DWORD)(current - buffer), &dwBytesWritten, NULL);
  free(buffer);

  if (fCloseFile)
    CloseHandle(outFile);

/*  if (rc != RXSHV_OK)  */
  if (rc != RXSHV_LVAR)
    RETVAL(-1)
  else
    RETVAL(0)
}



/*************************************************************************
* Function:  SysSetFileDateTime                                          *
*                                                                        *
* Syntax:    result = SysSetFileDateTime(filename [,newdate] [,newtime]) *
*                                                                        *
* Params:    filename - name of the file to update                       *
*            newdate  - new date to set in format YYYY-MM-DD (YYYY>1800) *
*            newtime  - new time to set in format HH:MM:SS               *
*                                                                        *
* Return:    0 - file date/time was updated correctly                    *
*            -1 - failure attribute update                               *
*************************************************************************/

size_t RexxEntry SysSetFileDateTime(const char *name, size_t numargs, CONSTRXSTRING args[], const char *queuename, PRXSTRING retstr)
{
  BOOL      fOk = TRUE;
  HANDLE    setFile = NULL;
  FILETIME  sFileTime;
  FILETIME  sLocalFileTime;
  SYSTEMTIME sLocalSysTime;

  /* we expect one to three parameters, if three parameters are      */
  /* specified then the second may be omitted to set only a new time,*/
  /* if only one is specified then the file is set to current time   */
  if ( (numargs < 1) || (numargs > 3) ||
       ((numargs == 2) && !RXVALIDSTRING(args[1])) ||
       ((numargs == 3) && !RXVALIDSTRING(args[2])) )
    return INVALID_ROUTINE;            /* raise error condition      */

  /* open output file for read/write for update */
  setFile = CreateFile(args[0].strptr, GENERIC_WRITE | GENERIC_READ,
                       FILE_SHARE_READ | FILE_SHARE_WRITE,
                       NULL, OPEN_EXISTING, FILE_FLAG_WRITE_THROUGH |
                       FILE_FLAG_BACKUP_SEMANTICS, NULL);
  if (setFile && (setFile != INVALID_HANDLE_VALUE))
  {
    fOk = GetFileTime(setFile, NULL, NULL, &sFileTime);
    fOk &= FileTimeToLocalFileTime(&sFileTime, &sLocalFileTime);
    fOk &= FileTimeToSystemTime(&sLocalFileTime, &sLocalSysTime);
    if (fOk)
    {
      /* file date/time could be read, now parse the new date/time */
      if ((numargs >= 2) && RXVALIDSTRING(args[1]))
      {
        /* parse new date */
        if (sscanf(args[1].strptr, "%4hu-%2hu-%2hu", &sLocalSysTime.wYear,
                   &sLocalSysTime.wMonth, &sLocalSysTime.wDay) != 3)
          fOk = false;

        if (sLocalSysTime.wYear < 1800)
          fOk = false;
      }

      if ((numargs == 3) && RXVALIDSTRING(args[2]))
      {
        /* parse new time */
        if (sscanf(args[2].strptr, "%2hu:%2hu:%2hu", &sLocalSysTime.wHour,
                   &sLocalSysTime.wMinute, &sLocalSysTime.wSecond) != 3)
          fOk = false;
      }

      if (numargs == 1)
      {
        /* we set the timestamp to the current time and date */
        GetLocalTime(&sLocalSysTime);
      }

      if (fOk)
      {
        fOk &= SystemTimeToFileTime(&sLocalSysTime, &sLocalFileTime);
        fOk &= LocalFileTimeToFileTime(&sLocalFileTime, &sFileTime);
        fOk &= SetFileTime(setFile, NULL, NULL, &sFileTime);
      }
    }

    CloseHandle(setFile);
  }
  else
    fOk = false;

  if (fOk)
    RETVAL(0)
  else
    RETVAL(-1)
}

/*************************************************************************
* Function:  SysGetFileDateTime                                          *
*                                                                        *
* Syntax:    result = SysGetFileDateTime(filename [,timesel])            *
* Params:    filename - name of the file to query                        *
*            timesel  - What filetime to query: Created/Access/Write     *
*                                                                        *
* Return:    -1 - file date/time query failed                            *
*            other - date and time as YYYY-MM-DD HH:MM:SS                *
*************************************************************************/

size_t RexxEntry SysGetFileDateTime(const char *name, size_t numargs, CONSTRXSTRING args[], const char *queuename, PRXSTRING retstr)
{
  BOOL      fOk = TRUE;
  HANDLE    setFile = NULL;
  FILETIME  sFileTime;
  FILETIME  sLocalFileTime;
  FILETIME  *psFileCreated = NULL;
  FILETIME  *psFileAccessed = NULL;
  FILETIME  *psFileWritten = NULL;
  SYSTEMTIME sLocalSysTime;

  /* we expect one to three parameters, if three parameters are      */
  /* specified then the second may be omitted to set only a new time,*/
  /* if only one is specified then the file is set to current time   */
  if ( (numargs < 1) || (numargs > 2) ||
       ((numargs == 2) && !RXVALIDSTRING(args[1])) )
    return INVALID_ROUTINE;            /* raise error condition      */

  if (numargs > 1)
  {
    switch (args[1].strptr[0])
    {
      case 'c':
      case 'C':
        psFileCreated = &sFileTime;
        break;
      case 'a':
      case 'A':
        psFileAccessed = &sFileTime;
        break;
      case 'w':
      case 'W':
        psFileWritten = &sFileTime;
        break;
      default:
        return INVALID_ROUTINE;
    }
  }
  else
    psFileWritten = &sFileTime;

  /* open file for read to query time */
  setFile = CreateFile(args[0].strptr, GENERIC_READ,
                       FILE_SHARE_READ | FILE_SHARE_WRITE,
                       NULL, OPEN_EXISTING, FILE_FLAG_WRITE_THROUGH |
                       FILE_FLAG_BACKUP_SEMANTICS, NULL);
  if (setFile && (setFile != INVALID_HANDLE_VALUE))
  {
    fOk = GetFileTime(setFile, psFileCreated, psFileAccessed, psFileWritten);
    fOk &= FileTimeToLocalFileTime(&sFileTime, &sLocalFileTime);
    fOk &= FileTimeToSystemTime(&sLocalFileTime, &sLocalSysTime);
    if (fOk)
    {
      sprintf(retstr->strptr, "%4d-%02d-%02d %02d:%02d:%02d",
              sLocalSysTime.wYear,
              sLocalSysTime.wMonth,
              sLocalSysTime.wDay,
              sLocalSysTime.wHour,
              sLocalSysTime.wMinute,
              sLocalSysTime.wSecond);
      retstr->strlength = strlen(retstr->strptr);
    }

    CloseHandle(setFile);
  }
  else
    fOk = false;

  if (!fOk)
    RETVAL(-1)
  else
    return VALID_ROUTINE;
}


RexxReturnCode REXXENTRY RexxStemSort(const char *stemname, int order, int type,
    size_t start, size_t end, size_t firstcol, size_t lastcol);

/*************************************************************************
* Function:  SysStemSort                                                 *
*                                                                        *
* Syntax:    result = SysStemSort(stem, order, type, start, end,         *
*                                 firstcol, lastcol)                     *
*                                                                        *
* Params:    stem - name of stem to sort                                 *
*            order - 'A' or 'D' for sort order                           *
*            type - 'C', 'I', 'N' for comparision type                   *
*            start - first index to sort                                 *
*            end - last index to sort                                    *
*            firstcol - first column to use as sort key                  *
*            lastcol - last column to use as sort key                    *
*                                                                        *
* Return:    0 - sort was successful                                     *
*            -1 - sort failed                                            *
*************************************************************************/

size_t RexxEntry SysStemSort(const char *name, size_t numargs, CONSTRXSTRING args[], const char *queuename, PRXSTRING retstr)
{
    CHAR          stemName[255];
    size_t        first = 1;
    size_t        last = SIZE_MAX;
    size_t        firstCol = 0;
    size_t        lastCol = SIZE_MAX;
    INT           sortType = SORT_CASESENSITIVE;
    INT           sortOrder = SORT_ASCENDING;

    // validate arguments
    if ( (numargs < 1) || (numargs > 7) || !RXVALIDSTRING(args[0]) )
    {
        return INVALID_ROUTINE;
    }

    // remember stem name
    memset(stemName, 0, sizeof(stemName));
    strcpy(stemName, args[0].strptr);
    if ( stemName[args[0].strlength-1] != '.' )
    {
        stemName[args[0].strlength] = '.';
    }

    // check other parameters.  sort order
    if ( (numargs >= 2) && RXVALIDSTRING(args[1]) )
    {
        switch ( args[1].strptr[0] )
        {
            case 'A':
            case 'a':
                sortOrder = SORT_ASCENDING;
                break;
            case 'D':
            case 'd':
                sortOrder = SORT_DECENDING;
                break;
            default:
                return INVALID_ROUTINE;
        }
    }
    // sort type
    if ( (numargs >= 3) && RXVALIDSTRING(args[2]) )
    {
        switch ( args[2].strptr[0] )
        {
            case 'C':
            case 'c':
                sortType = SORT_CASESENSITIVE;
                break;
            case 'I':
            case 'i':
                sortType = SORT_CASEIGNORE;
                break;
            default:
                return INVALID_ROUTINE;
        }
    }
    // first element to sort
    if ( (numargs >= 4) && RXVALIDSTRING(args[3]) )
    {
        if (!string2size_t(args[3].strptr, &first))
        {
            return INVALID_ROUTINE;
        }
        if ( first == 0 )
        {
            return INVALID_ROUTINE;
        }
    }
    // last element to sort
    if ( (numargs >= 5) && RXVALIDSTRING(args[4]) )
    {
        if (!string2size_t(args[4].strptr, &last))
            return INVALID_ROUTINE;
        if ( last < first )
            return INVALID_ROUTINE;
    }
    // first column to sort
    if ( (numargs >= 6) && RXVALIDSTRING(args[5]) )
    {
        if (!string2size_t(args[5].strptr, &firstCol))
        {
            return INVALID_ROUTINE;
        }
        firstCol--;
    }
    // last column to sort
    if ( (numargs == 7) && RXVALIDSTRING(args[6]) )
    {
        if (!string2size_t(args[6].strptr, &lastCol))
        {
            return INVALID_ROUTINE;
        }
        lastCol--;
        if ( lastCol < firstCol )
        {
            return INVALID_ROUTINE;
        }

    }

    // the sorting is done in the interpreter
    if ( !RexxStemSort(stemName, sortOrder, sortType, first, last, firstCol, lastCol) )
    {
        sprintf(retstr->strptr, "-1");
        retstr->strlength = 2;
        return INVALID_ROUTINE;
    }

    sprintf(retstr->strptr, "0");
    retstr->strlength = 1;
    return VALID_ROUTINE;
}


/*************************************************************************
* Function:  SysStemDelete                                               *
*                                                                        *
* Syntax:    result = SysStemDelete(stem, startitem [,itemcount])        *
*                                                                        *
* Params:    stem - name of stem where item will be deleted              *
*            startitem - index of item to delete                         *
*            itemcount - number of items to delete if more than 1        *
*                                                                        *
* Return:    0 - delete was successful                                   *
*            -1 - delete failed                                          *
*************************************************************************/

RexxRoutine3(int, SysStemDelete, RexxStemObject, toStem, stringsize_t, start, OPTIONAL_stringsize_t, count)

{
    if (argumentOmitted(3))
    {
        count = 1;
    }

    stringsize_t items;

    RexxObjectPtr temp = context->GetStemArrayElement(toStem, 0);
    if (temp == NULLOBJECT || !context->StringSize(temp, &items))
    {
        context->InvalidRoutine();
        return 0;
    }

    // make sure the deletion site is within the bounds
    if (start + count - 1 > items)
    {
        context->InvalidRoutine();
        return 0;
    }

    stringsize_t index;
    /* now copy the remaining indices up front */
    for ( index = start;  index + count <= items; index++)
    {
        // copy from the old index to the new index
        RexxObjectPtr value = context->GetStemArrayElement(toStem, index + count);
        // is this a sparse array?
        if (value == NULLOBJECT)
        {
            // return this as a failure
            return -1;
        }
        context->SetStemArrayElement(toStem, index, value);
    }

    /* now delete the items at the end */
    for (index = items - count + 1; index <= items; index++)
    {
        context->DropStemArrayElement(toStem, index);
    }

    context->SetStemArrayElement(toStem, 0, context->StringSize(items - count));
    return 0;
}


/*************************************************************************
* Function:  SysStemInsert                                               *
*                                                                        *
* Syntax:    result = SysStemInsert(stem, position, value)               *
*                                                                        *
* Params:    stem - name of stem where item will be inserted             *
*            position - index where new item will be inserted            *
*            value - new item value                                      *
*                                                                        *
* Return:    0 - insert was successful                                   *
*            -1 - insert failed                                          *
*************************************************************************/

RexxRoutine3(int, SysStemInsert, RexxStemObject, toStem, stringsize_t, position, RexxObjectPtr, newValue)
{
    stringsize_t count;

    RexxObjectPtr temp = context->GetStemArrayElement(toStem, 0);
    if (temp == NULLOBJECT || !context->StringSize(temp, &count))
    {
        context->InvalidRoutine();
        return 0;
    }

    /* check wether new position is within limits */
    if (position == 0 || (position > count + 1))
    {
        context->InvalidRoutine();
        return 0;
    }

    for (size_t index = count; index >= position; index--)
    {
        // copy from the old index to the new index
        RexxObjectPtr value = context->GetStemArrayElement(toStem, index);
        // is this a sparse array?
        if (value == NULLOBJECT)
        {
            // return this as a failure
            return -1;
        }
        context->SetStemArrayElement(toStem, index + 1, value);
    }

    // now set the new value and increase the count at stem.0
    context->SetStemArrayElement(toStem, position, newValue);
    context->SetStemArrayElement(toStem, 0, context->WholeNumber(count + 1));
    return 0;
}


/*************************************************************************
* Function:  SysStemCopy                                                 *
*                                                                        *
* Syntax:    result = SysStemCopy(fromstem, tostem, from, to, count      *
*                                 [,insert])                             *
*                                                                        *
* Params:    fromstem - name of source stem                              *
*            tostem - - name of target stem                              *
*            from  - first index in source stem to copy                  *
*            to - position where items are copied/inserted in target stem*
*            count - number of items to copy/insert                      *
*            insert - 'I' to indicate insert instead of 'O' overwrite    *
*                                                                        *
* Return:    0 - stem copy was successful                                *
*            -1 - stem copy failed                                       *
*************************************************************************/

size_t RexxEntry SysStemCopy(const char *name, size_t numargs, CONSTRXSTRING args[], const char *queuename, PRXSTRING retstr)
{
  RexxReturnCode        rc;
  CHAR          szFromStemName[255];
  CHAR          szToStemName[255];
  PSZ           pszFromStemIdx;
  PSZ           pszToStemIdx;
  CHAR          szValue[255];
  SHVBLOCK      shvb;
  ULONG         ulIdx;
  ULONG         ulToCount;
  ULONG         ulFromCount;
  ULONG         ulFrom = 1;
  ULONG         ulTo = 1;
  ULONG         ulCopyCount = 0;
  bool          fInsert = false;
  bool          fOk = true;

  if ( (numargs < 2) || (numargs > 6) ||  /* validate arguments      */
      !RXVALIDSTRING(args[0]) || !RXVALIDSTRING(args[1]) ||
      ((numargs == 6) && !RXVALIDSTRING(args[5])) )
    return INVALID_ROUTINE;

  /* remember stem names */
  memset(szFromStemName, 0, sizeof(szFromStemName));
  strcpy(szFromStemName, args[0].strptr);
  if (szFromStemName[args[0].strlength-1] != '.')
    szFromStemName[args[0].strlength] = '.';
  pszFromStemIdx = &(szFromStemName[strlen(szFromStemName)]);

  memset(szToStemName, 0, sizeof(szToStemName));
  strcpy(szToStemName, args[1].strptr);
  if (szToStemName[args[1].strlength-1] != '.')
    szToStemName[args[1].strlength] = '.';
  pszToStemIdx = &(szToStemName[strlen(szToStemName)]);

  /* get from item index */
  if ((numargs >= 3) && RXVALIDSTRING(args[2]))
    if (sscanf(args[2].strptr, "%ld", &ulFrom) != 1)
      return INVALID_ROUTINE;

  /* get to item index */
  if ((numargs >= 4) && RXVALIDSTRING(args[3]))
    if (sscanf(args[3].strptr, "%ld", &ulTo) != 1)
      return INVALID_ROUTINE;

  /* get copy count */
  if ((numargs >= 5) && RXVALIDSTRING(args[4]))
    if (sscanf(args[4].strptr, "%ld", &ulCopyCount) != 1)
      return INVALID_ROUTINE;

  /* get copy type */
  if (numargs >= 6)
  {
    switch (args[5].strptr[0])
    {
      case 'I':
      case 'i':
        fInsert = true;
        break;
      case 'O':
      case 'o':
        fInsert = false;
        break;
      default:
        return INVALID_ROUTINE;
    } /* endswitch */
  } /* endif */

  /* retrieve the number of elements in stems */
  strcpy(pszFromStemIdx, "0");
  shvb.shvnext = NULL;
  shvb.shvname.strptr = szFromStemName;
  shvb.shvname.strlength = strlen((const char *)szFromStemName);
  shvb.shvvalue.strptr = szValue;
  shvb.shvvalue.strlength = sizeof(szValue);
  shvb.shvnamelen = shvb.shvname.strlength;
  shvb.shvvaluelen = shvb.shvvalue.strlength;
  shvb.shvcode = RXSHV_SYFET;
  shvb.shvret = 0;
  if (RexxVariablePool(&shvb) == RXSHV_OK)
  {
    /* index retrieved fine */
    if (sscanf(shvb.shvvalue.strptr, "%ld", &ulFromCount) != 1)
      return INVALID_ROUTINE;

    if ((ulCopyCount > (ulFromCount - ulFrom + 1)) || (ulFromCount == 0))
      return INVALID_ROUTINE;
  }
  else
    fOk = false;

  if (fOk)
  {
    strcpy(pszToStemIdx, "0");
    shvb.shvnext = NULL;
    shvb.shvname.strptr = szToStemName;
    shvb.shvname.strlength = strlen((const char *)szToStemName);
    shvb.shvvalue.strptr = szValue;
    shvb.shvvalue.strlength = sizeof(szValue);
    shvb.shvnamelen = shvb.shvname.strlength;
    shvb.shvvaluelen = shvb.shvvalue.strlength;
    shvb.shvcode = RXSHV_SYFET;
    shvb.shvret = 0;
    rc = RexxVariablePool(&shvb);
    if (rc == RXSHV_OK)
    {
      /* index retrieved fine */
      if (sscanf(shvb.shvvalue.strptr, "%ld", &ulToCount) != 1)
        return INVALID_ROUTINE;
    }
    else
    {
      if (rc == RXSHV_NEWV)
      {
        /* tostem.0 is not set, we assume empty target stem */
        ulToCount = 0;
      }
      else
        fOk = false;
    } /* endif */

    if (fOk)
    {
      if (ulTo > ulToCount + 1)
        return INVALID_ROUTINE;
    } /* endif */
  } /* endif */

  /* set copy count to number of items in source stem if not already set */
  if (ulCopyCount == 0)
    ulCopyCount = ulFromCount - ulFrom + 1;

  if (fOk && fInsert)
  {
    /* if we are about to insert the items we have to make room */
    for (ulIdx = ulToCount; ulIdx >= ulTo; ulIdx--)
    {
      /* get element to relocate */
      sprintf(pszToStemIdx, "%ld", ulIdx);
      shvb.shvnext = NULL;
      shvb.shvname.strptr = szToStemName;
      shvb.shvname.strlength = strlen((const char *)szToStemName);
      shvb.shvvalue.strptr = NULL;
      shvb.shvvalue.strlength = 0;
      shvb.shvnamelen = shvb.shvname.strlength;
      shvb.shvvaluelen = shvb.shvvalue.strlength;
      shvb.shvcode = RXSHV_SYFET;
      shvb.shvret = 0;

      if (RexxVariablePool(&shvb) == RXSHV_OK)
      {
        sprintf(pszToStemIdx, "%ld", ulIdx + ulCopyCount);
        shvb.shvnext = NULL;
        shvb.shvname.strptr = szToStemName;
        shvb.shvname.strlength = strlen((const char *)szToStemName);
        shvb.shvnamelen = shvb.shvname.strlength;
        shvb.shvvaluelen = shvb.shvvalue.strlength;
        shvb.shvcode = RXSHV_SYSET;
        shvb.shvret = 0;
        rc = RexxVariablePool(&shvb);
        if ((rc != RXSHV_OK) && (rc != RXSHV_NEWV))
          fOk = false;

        /* free memory allocated by REXX */
        RexxFreeMemory(shvb.shvvalue.strptr);
      }
      else
        fOk = false;

      if (!fOk)
        break;
    } /* endfor */

    if (fOk)
    {
      /* set the new count for the target stem */
      strcpy(pszToStemIdx, "0");
      ulToCount += ulCopyCount;
      sprintf(szValue, "%ld", ulToCount);
      shvb.shvnext = NULL;
      shvb.shvname.strptr = szToStemName;
      shvb.shvname.strlength = strlen((const char *)szToStemName);
      shvb.shvvalue.strptr = szValue;
      shvb.shvvalue.strlength = strlen(szValue);
      shvb.shvnamelen = shvb.shvname.strlength;
      shvb.shvvaluelen = shvb.shvvalue.strlength;
      shvb.shvcode = RXSHV_SYSET;
      shvb.shvret = 0;
      rc = RexxVariablePool(&shvb);
      if ((rc != RXSHV_OK) && (rc != RXSHV_NEWV))
        fOk = false;
    } /* endif */
  } /* endif */

  if (fOk)
  {
    /* now do the actual copying from the source to target */
    for (ulIdx = 0; ulIdx < ulCopyCount; ulIdx++)
    {
      /* get element to copy */
      sprintf(pszFromStemIdx, "%ld", ulFrom + ulIdx);
      shvb.shvnext = NULL;
      shvb.shvname.strptr = szFromStemName;
      shvb.shvname.strlength = strlen((const char *)szFromStemName);
      shvb.shvvalue.strptr = NULL;
      shvb.shvvalue.strlength = 0;
      shvb.shvnamelen = shvb.shvname.strlength;
      shvb.shvvaluelen = shvb.shvvalue.strlength;
      shvb.shvcode = RXSHV_SYFET;
      shvb.shvret = 0;

      if (RexxVariablePool(&shvb) == RXSHV_OK)
      {
        sprintf(pszToStemIdx, "%ld", ulTo + ulIdx);
        shvb.shvnext = NULL;
        shvb.shvname.strptr = szToStemName;
        shvb.shvname.strlength = strlen((const char *)szToStemName);
        shvb.shvnamelen = shvb.shvname.strlength;
        shvb.shvvaluelen = shvb.shvvalue.strlength;
        shvb.shvcode = RXSHV_SYSET;
        shvb.shvret = 0;
        rc = RexxVariablePool(&shvb);
        if ((rc != RXSHV_OK) && (rc != RXSHV_NEWV))
          fOk = false;

        /* free memory allocated by REXX */
        RexxFreeMemory(shvb.shvvalue.strptr);
      }
      else
        fOk = false;

      if (!fOk)
        break;
    } /* endfor */
  } /* endif */

  if (fOk && (ulTo + ulCopyCount - 1 > ulToCount))
  {
    /* set the new count for the target stem */
    strcpy(pszToStemIdx, "0");
    ulToCount = ulTo + ulCopyCount - 1;
    sprintf(szValue, "%ld", ulToCount);
    shvb.shvnext = NULL;
    shvb.shvname.strptr = szToStemName;
    shvb.shvname.strlength = strlen((const char *)szToStemName);
    shvb.shvvalue.strptr = szValue;
    shvb.shvvalue.strlength = strlen(szValue);
    shvb.shvnamelen = shvb.shvname.strlength;
    shvb.shvvaluelen = shvb.shvvalue.strlength;
    shvb.shvcode = RXSHV_SYSET;
    shvb.shvret = 0;
    rc = RexxVariablePool(&shvb);
    if ((rc != RXSHV_OK) && (rc != RXSHV_NEWV))
      fOk = false;
  } /* endif */

  if (fOk)
    RETVAL(0)
  else
    RETVAL(-1)
}


/*************************************************************************
* Function:  SysUtilVersion                                              *
*                                                                        *
* Syntax:    Say SysUtilVersion                                          *
*                                                                        *
* Return:    REXXUTIL.DLL Version                                        *
*************************************************************************/

size_t RexxEntry SysUtilVersion(const char *name, size_t numargs, CONSTRXSTRING args[], const char *queuename, PRXSTRING retstr)
{
  if (numargs != 0)                    /* validate arg count         */
    return INVALID_ROUTINE;
                                       /* format into the buffer     */
  sprintf(retstr->strptr, "%d.%d.%d", ORX_VER, ORX_REL, ORX_MOD);
  retstr->strlength = strlen(retstr->strptr);

  return VALID_ROUTINE;
}

/*************************************************************************
* Function:  SetRexxStem                                                 *
*                                                                        *
*            internal function to set a stem value in the varpool        *
*            for not UNICODE character strings                           *
*                                                                        *
* Return:    false - failed                                              *
*            true  - no error                                            *
*************************************************************************/
bool SetRexxStem(const char* name, const char *tailname, const char *data)
{
   SHVBLOCK shvb;
   CHAR buffer[MAX];

   sprintf(buffer,"%s%s",name,tailname);

   shvb.shvnext = NULL;
   shvb.shvname.strptr = buffer;
   shvb.shvname.strlength = strlen(buffer);
   shvb.shvnamelen = shvb.shvname.strlength;
   shvb.shvvalue.strptr = const_cast<char *>(data);
   shvb.shvvalue.strlength = strlen(data);
   shvb.shvvaluelen = strlen(data);
   shvb.shvcode = RXSHV_SYSET;
   shvb.shvret = 0;
   if (RexxVariablePool(&shvb) == RXSHV_BADN)
     return false;
   return true;
}

/*************************************************************************
* Function:  SetRexxStemLength                                           *
*                                                                        *
*            internal function to set a stem value in the varpool        *
*            for not UNICODE character strings                           *
*                                                                        *
* Return:    false - failed                                              *
*            true  - no error                                            *
*************************************************************************/
bool SetRexxStemLength(const char *name, const char *tailname, const char * data, size_t datalen)
{
   SHVBLOCK shvb;
   CHAR buffer[MAX];

   sprintf(buffer,"%s%s",name,tailname);

   shvb.shvnext = NULL;
   shvb.shvname.strptr = buffer;
   shvb.shvname.strlength = strlen(buffer);
   shvb.shvnamelen = shvb.shvname.strlength;
   shvb.shvvalue.strptr = const_cast<char *>(data);
   shvb.shvvalue.strlength = datalen;
   shvb.shvvaluelen = datalen;
   shvb.shvcode = RXSHV_SYSET;
   shvb.shvret = 0;
   if (RexxVariablePool(&shvb) == RXSHV_BADN)
     return false;
   return true;
}


/**
 * Check if the dwFlags arguement to WideCharToMultiByte() can be used by the
 * specified code page.  See MSDN documentation for WideCharToMultiByte() for
 * clarification.  This is used by SysFromUnicode()
 *
 * @param cp  Code page to check.
 *
 * @return Return true if dwFlags can be non-zero, return false if dwFlags must
 *         be zero.
 */
static bool canUseWideCharFlags(UINT cp)
{
    if ( cp == CP_SYMBOL || cp == CP_UTF7 || cp == CP_UTF8 )
    {
        return false;
    }
    if ( 50220 <= cp && cp <= 50222  )
    {
        return false;
    }
    if ( cp == 50225 || cp == 50227 || cp == 50229 || cp == 52936 || cp == 54936 )
    {
        return false;
    }
    if ( 57002 <= cp && cp <= 57011  )
    {
        return false;
    }
    return true;
}

/*************************************************************************
* Function:  SysFromUnicode                                              *
*            Converts a UNICODE string to an ASCII string                *
*                                                                        *
* Syntax:    result = SysFromUniCode(string,CodePage,MappingFlags,       *
*                                    DefaultChar, outstem.)              *
*                                                                        *
* Params:    string       - unicode string to be converted               *
*            Codepage     - target codepage                              *
*            MappingFlags - Mapping flags                                *
*            DefaultChar  - default for unmappable chars                 *
*             outstem.    - stem containg the result                     *
*              .!USEDDEFAULTCHAR - 1: character used as default          *
*              .!TEXT     - converted text                               *
*                                                                        *
*                                                                        *
* Return:    0 - successfull completetion                                *
*            error code from WideCharToMultiByte                         *

  The following are the OEM code-page identifiers.

    437  MS-DOS United States
    708  Arabic (ASMO 708)
    709  Arabic (ASMO 449+, BCON V4)
    710  Arabic (Transparent Arabic)
    720  Arabic (Transparent ASMO)
    737  Greek (formerly 437G)
    775  Baltic
    850  MS-DOS Multilingual (Latin I)
    852  MS-DOS Slavic (Latin II)
    855  IBM Cyrillic (primarily Russian)
    857  IBM Turkish
    860  MS-DOS Portuguese
    861  MS-DOS Icelandic
    862  Hebrew
    863  MS-DOS Canadian-French
    864  Arabic
    865  MS-DOS Nordic
    866  MS-DOS Russian (former USSR)
    869  IBM Modern Greek
    874  Thai
    932  Japan
    936  Chinese (PRC, Singapore)
    949  Korean
    950  Chinese (Taiwan; Hong Kong SAR, PRC)
    1361 Korean (Johab)

  The following are the ANSI code-page identifiers.

    874  Thai
    932  Japan
    936  Chinese (PRC, Singapore)
    949  Korean
    950  Chinese (Taiwan; Hong Kong SAR, PRC)
    1200 Unicode (BMP of ISO 10646)
    1250 Windows 3.1 Eastern European
    1251 Windows 3.1 Cyrillic
    1252 Windows 3.1 Latin 1 (US, Western Europe)
    1253 Windows 3.1 Greek
    1254 Windows 3.1 Turkish
    1255 Hebrew
    1256 Arabic
    1257 Baltic

  COMPOSITECHECK :
    Convert composite characters to precomposed characters.

  DISCARDNS :
    Discard nonspacing characters during conversion.

  SEPCHARS :
    Generate separate characters during conversion. This is the default conversion behavior.

  DEFAULTCHAR :
    Replace exceptions with the default character during conversion.

*************************************************************************/

RexxRoutine5(int, SysFromUniCode, RexxStringObject, sourceString, OPTIONAL_CSTRING, codePageOpt,
    OPTIONAL_CSTRING, mappingFlags, OPTIONAL_CSTRING, defaultChar, RexxStemObject, stem)
{
    const char *source = context->StringData(sourceString);
    size_t sourceLength = context->StringLength(sourceString);

    UINT  codePage;
    /* evaluate codepage          */
    if (codePageOpt == NULL)
    {
        codePage = GetOEMCP();
    }
    else
    {
        if (_stricmp(codePageOpt, "THREAD_ACP") == 0)
        {
            codePage = CP_THREAD_ACP;
        }
        else if (_stricmp(codePageOpt,"ACP") == 0)
        {
            codePage = CP_ACP;
        }
        else if (_stricmp(codePageOpt,"MACCP") == 0)
        {
            codePage = CP_MACCP;
        }
        else if (_stricmp(codePageOpt,"OEMCP") == 0)
        {
            codePage = CP_OEMCP;
        }
        else if (_stricmp(codePageOpt,"SYMBOL") == 0)
        {
            codePage = CP_SYMBOL;
        }
        else if (_stricmp(codePageOpt,"UTF7") == 0)
        {
            codePage = CP_UTF7;
        }
        else if (_stricmp(codePageOpt,"UTF8") == 0)
        {
            codePage = CP_UTF8;
        }
        else
        {
            codePage = atoi(codePageOpt);
        }
    }

    DWORD dwFlags = 0;
    /* evaluate the mapping flags */
    if (mappingFlags != NULL && *mappingFlags != '\0' )
    {
        /* The WC_SEPCHARS, WC_DISCARDNS, and WC_DEFAULTCHAR flags must also
         * specify the WC_COMPOSITECHECK flag.  So, we add that for the user if
         * they skipped it. Those 4 flags are only available for code pages <
         * 50000, excluding 42 (CP_SYMBOL).  See the remarks section in the MSDN
         * docs for clarification.
         */
        if ( codePage < 50000 && codePage != CP_SYMBOL )
        {
            if ( StrStrI(mappingFlags, "COMPOSITECHECK") != NULL )
            {
                dwFlags |= WC_COMPOSITECHECK;
            }
            if ( StrStrI(mappingFlags, "SEPCHARS") != NULL )
            {
                dwFlags |= WC_SEPCHARS | WC_COMPOSITECHECK;
            }
            if ( StrStrI(mappingFlags, "DISCARDNS") != NULL )
            {
                dwFlags |= WC_DISCARDNS| WC_COMPOSITECHECK;
            }
            if ( StrStrI(mappingFlags, "DEFAULTCHAR") != NULL )
            {
                dwFlags |= WC_DEFAULTCHAR | WC_COMPOSITECHECK;
            }
        }

        if ( StrStrI(mappingFlags, "NO_BEST_FIT") != NULL )
        {
            dwFlags |= WC_NO_BEST_FIT_CHARS;
        }

        if ( StrStrI(mappingFlags, "ERR_INVALID") != NULL )
        {
            if ( codePage == CP_UTF8 && isAtLeastVista() )
            {
                dwFlags |= WC_ERR_INVALID_CHARS;
            }
        }
        else if ( dwFlags == 0 && ! (codePage < 50000 && codePage != CP_SYMBOL) )
        {
            context->InvalidRoutine();
            return 0;
        }
    }

    /* evaluate default charcter  */
    const char  *strDefaultChar = NULL;
    BOOL  bUsedDefaultChar = FALSE;
    BOOL  *pUsedDefaultChar = &bUsedDefaultChar;

    if (defaultChar != NULL && (dwFlags & WC_DEFAULTCHAR) == WC_DEFAULTCHAR)
    {
        strDefaultChar = defaultChar;
    }
    else
    {
        /* use our own default character rather than relying on the windows default */
        strDefaultChar = "?";
    }

    /* There are a number of invalid combinations of arguments to
     *  WideCharToMultiByte(), see the MSDN docs. Eliminate them here.
     */
    if ( codePage == CP_UTF8 && dwFlags == WC_ERR_INVALID_CHARS)
    {
        strDefaultChar = NULL;
        pUsedDefaultChar = NULL;
    }
    else if ( ! canUseWideCharFlags(codePage) )
    {
        dwFlags = 0;
        strDefaultChar = NULL;
        pUsedDefaultChar = NULL;
    }

    /* Allocate space for the string, to allow double zero byte termination */
    char *strptr = (char *)GlobalAlloc(GMEM_FIXED|GMEM_ZEROINIT, sourceLength + 4);
    if (strptr == NULL)
    {
        context->InvalidRoutine();
        return 0;
    }
    memcpy(strptr, source, sourceLength);

    /* Query the number of bytes required to store the Dest string */
    int iBytesNeeded = WideCharToMultiByte( codePage,
                                        dwFlags,
                                        (LPWSTR) strptr,
                                        (int)(sourceLength/2),
                                        NULL,
                                        0,
                                        NULL,
                                        NULL);

    if (iBytesNeeded == 0)
    {
        GlobalFree(strptr);
        return GetLastError();
    }

        // hard error, stop
    char *str = (char *)GlobalAlloc(GMEM_FIXED|GMEM_ZEROINIT, iBytesNeeded + 4);
    if (str == NULL)
    {
        context->InvalidRoutine();
        return 0;
    }

        /* Do the conversion */
    int iBytesDestination = WideCharToMultiByte(codePage,           // codepage
                                            dwFlags,                // conversion flags
                                            (LPWSTR) strptr,        // source string
                                            (int)(sourceLength/2),  // source string length
                                            str,                    // target string
                                            (int)iBytesNeeded,      // size of target buffer
                                            strDefaultChar,
                                            pUsedDefaultChar);

    if (iBytesDestination == 0) // call to function fails
    {
        GlobalFree(str);          //  free allocated string
        GlobalFree(strptr);       // free allocated string
        return GetLastError();    // return error from function call
    }

    // set whether the default character was used in the output stem
    if (bUsedDefaultChar)
    {
        context->SetStemElement(stem, "!USEDDEFAULTCHAR", context->True());
    }
    else
    {
        context->SetStemElement(stem, "!USEDDEFAULTCHAR", context->False());
    }

    context->SetStemElement(stem, "!TEXT", context->String(str, iBytesNeeded));
    GlobalFree(strptr);          // free allocated string
    GlobalFree(str);             // free allocated string
    return 0;
}

/*************************************************************************
* Function:  SetRexxUIStem                                               *
*                                                                        *
*            internal function to set a stem value in the varpool        *
*            for UNICODE character strings                               *
*                                                                        *
* Return:    false - failed                                              *
*            true  - no error                                            *
*************************************************************************/

LONG SetRexxUIStem(CHAR * name, char * tailname, LPWSTR data, int datalen)
{
   SHVBLOCK shvb;
   CHAR buffer[MAX];

   sprintf(buffer,"%s%s",name,tailname);

   shvb.shvnext = NULL;
   shvb.shvname.strptr = buffer;
   shvb.shvname.strlength = strlen(buffer);
   shvb.shvnamelen = shvb.shvname.strlength;
   shvb.shvvalue.strptr = (char*)data;
   shvb.shvvalue.strlength = datalen;
   shvb.shvvaluelen = datalen;
   shvb.shvcode = RXSHV_SYSET;
   shvb.shvret = 0;
   if (RexxVariablePool(&shvb) == RXSHV_BADN)
     return false;
   return true;
}

/*
* Syntax:    result = SysToUniCode(string,CodePage,MappingFlags,outstem.)
*/
/*************************************************************************
* Function:  SysToUnicode                                                *
*            Converts an ASCII to UNICODE                                *
*                                                                        *
* Syntax:    result = SysToUniCode(string,CodePage,MappingFlags,outstem.)*
*                                                                        *
* Params:    string       - ascii string to be converted                 *
*            Codepage     - target codepage                              *
*            MappingFlags - Mapping flags                                *
*             outstem.    - stem containg the result                     *
*              .!TEXT     - converted text                               *
*                                                                        *
* Return:    0 - successfull completetion                                *
*            error code from WideCharToMultiByteToWideChars              *

  For available codepages see function SysFromUniCode.

  Additional parameters for codepages:

    ACP        ANSI code page
    MACCP      Macintosh code page
    OEMCP      OEM code page
    SYMBOL     Windows 2000: Symbol code page (42)
    THREAD_ACP Windows 2000: The current thread's ANSI code page
    UTF7       Windows NT 4.0 and Windows 2000: Translate using UTF-7
    UTF8       Windows NT 4.0 and Windows 2000: Translate using UTF-8.
               When this is set, dwFlags must be zero.

    PRECOMPOSED       Always use precomposed characters—that is, characters
                      in which a base character and a nonspacing character
                      have a single character value.
                      This is the default translation option.
                      Cannot be used with MB_COMPOSITE.
    COMPOSITE         Always use composite characters that is,
                      characters in which a base character and a nonspacing
                      character have different character values.
                      Cannot be used with MB_PRECOMPOSED.
    ERR_INVALID_CHARS If the function encounters an invalid input character,
                      it fails and GetLastError returns
                      ERROR_NO_UNICODE_TRANSLATION.
    USEGLYPHCHARS     Use glyph characters instead of control characters.



*************************************************************************/
size_t RexxEntry SysToUniCode(const char *name, size_t numargs, CONSTRXSTRING args[], const char *queuename, PRXSTRING retstr)
{
  ULONG  ulDataLen, ulWCharsNeeded;
  DWORD  dwFlags = 0;
  UINT   codePage;
  LPWSTR lpwstr = NULL;
  CHAR   stemName[MAX];

  // check number of arguments
  // arguments must always be 4. Args 1 and 4 must be valid strings
  if ( numargs != 4 || !RXVALIDSTRING(args[0]) || !RXVALIDSTRING(args[3]) )
//  if ( numargs < 2 || numargs > 4 ||
//       !RXVALIDSTRING(args[3]) )
    return INVALID_ROUTINE;            /* Invalid call to routine    */

  // evaluate codepage
  if (args[1].strlength == 0)
    codePage = GetOEMCP();
  else
  {
    if (_stricmp(args[1].strptr,"THREAD_ACP") == 0)
      codePage = CP_THREAD_ACP;
    else if (_stricmp(args[1].strptr,"ACP") == 0)
      codePage = CP_ACP;
    else if (_stricmp(args[1].strptr,"MACCP") == 0)
      codePage = CP_MACCP;
    else if (_stricmp(args[1].strptr,"OEMCP") == 0)
      codePage = CP_OEMCP;
    else if (_stricmp(args[1].strptr,"SYMBOL") == 0)
      codePage = CP_SYMBOL;
    else if (_stricmp(args[1].strptr,"UTF7") == 0)
      codePage = CP_UTF7;
    else if (_stricmp(args[1].strptr,"UTF8") == 0)
      codePage = CP_UTF8;
    else
      codePage = atoi(args[1].strptr);
  }

  // evaluate the mapping flags
  if (args[2].strlength != 0)
  {
    if (mystrstr(args[2].strptr,"PRECOMPOSED",args[2].strlength,11,false))   dwFlags |= MB_PRECOMPOSED;
    if (mystrstr(args[2].strptr,"COMPOSITE",args[2].strlength,9,false))     dwFlags  |= MB_COMPOSITE;
    if (mystrstr(args[2].strptr,"ERR_INVALID",args[2].strlength,11,false))   dwFlags |= MB_ERR_INVALID_CHARS;
    if (mystrstr(args[2].strptr,"USEGLYPHCHARS",args[2].strlength,13,false)) dwFlags |= MB_USEGLYPHCHARS;
    if (dwFlags == 0)
      return INVALID_ROUTINE;
  }

  // evaluate output stem
  strcpy(stemName, args[3].strptr);

  /* uppercase the name         */
  memupper(stemName, args[3].strlength);

  if (stemName[args[3].strlength-1] != '.')
  {
    stemName[args[3].strlength] = '.';
    stemName[args[3].strlength+1] = '\0';
  }

  /* Query the number of bytes required to store the Dest string */
  ulWCharsNeeded = MultiByteToWideChar( codePage,
                                        dwFlags,
                                        args[0].strptr,
                                        (int)args[0].strlength,
                                        NULL,
                                        NULL);

  if (ulWCharsNeeded == 0) RETVAL(GetLastError())  // call to function fails

  ulDataLen = (ulWCharsNeeded)*2;

  // hard error, stop
  if (!(lpwstr = (LPWSTR)GlobalAlloc(GMEM_FIXED|GMEM_ZEROINIT, ulDataLen+4)))
    return INVALID_ROUTINE;


  /* Do the conversion */
  ulWCharsNeeded = MultiByteToWideChar( codePage,
                                        dwFlags,
                                        args[0].strptr,
                                        (int)args[0].strlength,
                                        lpwstr,
                                        ulWCharsNeeded);

  if (ulWCharsNeeded == 0) // call to function fails
  {
     GlobalFree(lpwstr);       // free allocated string
     RETVAL(GetLastError())    // return error from function call
  }

  if (!SetRexxUIStem(stemName, "!TEXT", lpwstr, ulDataLen)) {
    GlobalFree(lpwstr);        // free allocated string
    return INVALID_ROUTINE;
  }

  GlobalFree(lpwstr);        // free allocated string
  BUILDRXSTRING(retstr, NO_UTIL_ERROR);/* set default result         */
  return VALID_ROUTINE;                /* no error on call           */

}

/*************************************************************************
* Function:  SysWinGetPrinters                                           *
*                                                                        *
* Syntax:    call SysWinGetPrinters stem.                                *
*                                                                        *
* Params:    stem. - stem to store infos in                              *
*                                                                        *
* Return:    error number                                                *
*************************************************************************/
size_t RexxEntry SysWinGetPrinters(const char *name, size_t numargs, CONSTRXSTRING args[],
                                   const char *queuename, PRXSTRING retstr)
{
    DWORD realSize = 0;
    DWORD entries = 0;
    BOOL  fSuccess = FALSE;
    char  szBuffer[256];
    PRINTER_INFO_2 *pResult;
    DWORD currentSize = 10*sizeof(PRINTER_INFO_2)*sizeof(char);
    char *pArray = (char*) malloc(sizeof(char)*currentSize);

    // If no args, then its an incorrect call.
    if ( numargs != 1 )
    {
        return INVALID_ROUTINE;
    }

    // must be a stem!
    if ( args[0].strptr[args[0].strlength-1] != '.' )
    {
        return INVALID_ROUTINE;
    }

    while ( fSuccess == false )
    {
        fSuccess = EnumPrinters(PRINTER_ENUM_LOCAL|PRINTER_ENUM_CONNECTIONS, NULL, 2, (LPBYTE)pArray,
                                currentSize, &realSize, &entries);
        if ( currentSize < realSize )
        {
            currentSize = realSize;
            realSize = 0;
            pArray = (char*) realloc(pArray, sizeof(char)*currentSize);
            fSuccess = false;
        }
        else
        {
            fSuccess = true;
        }
    }

    pResult = (PRINTER_INFO_2*) pArray;
    fSuccess = false;

    // set stem.0 to the number of entries then add all the found printers
    sprintf(szBuffer,"%d",entries);
    if ( SetRexxStem(args[0].strptr, "0", szBuffer) )
    {
        fSuccess = true;
        while ( entries-- )
        {
            sprintf(szBuffer,"%s,%s,%s", pResult[entries].pPrinterName, pResult[entries].pDriverName,
                    pResult[entries].pPortName);
            char tailBuffer[20];
            sprintf(tailBuffer, "%d", entries + 1);

            if ( ! SetRexxStem(args[0].strptr, tailBuffer, szBuffer) )
            {
                fSuccess = false;
                break;
            }
        }
    }
    free(pArray);

    sprintf(retstr->strptr, "%s", fSuccess==TRUE ? "0" : "1");
    retstr->strlength = strlen(retstr->strptr);

    return VALID_ROUTINE;
}

/*************************************************************************
* Function:  SysWinGetDefaultPrinter                                     *
*                                                                        *
* Syntax:    call SysWinGetDefaultPrinter                                *
*                                                                        *
* Params:    none                                                        *
*                                                                        *
* Return:    string describing default printer                           *
*************************************************************************/

size_t RexxEntry SysWinGetDefaultPrinter(const char *name, size_t numargs, CONSTRXSTRING args[], const char *queuename, PRXSTRING retstr)
{

  DWORD  errnum = 0;
  OSVERSIONINFO osv;

  if (numargs != 0)                    /* If args, then its an       */
                                       /* incorrect call             */
    return INVALID_ROUTINE;

  // What version of Windows are you running?
  osv.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
  GetVersionEx(&osv);

  // If Windows 95 or 98, use EnumPrinters...
  if (osv.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS) {
    DWORD dwNeeded = 0;
    DWORD dwReturned;
    PRINTER_INFO_2 *printerInfo = NULL;

    // find out how much memory is needed
    EnumPrinters(PRINTER_ENUM_DEFAULT, NULL, 2, NULL, 0, &dwNeeded, &dwReturned);
    if (dwNeeded == 0)
      return INVALID_ROUTINE;

    printerInfo = (PRINTER_INFO_2*) malloc(sizeof(char)*dwNeeded);
    if (!printerInfo)
      return INVALID_ROUTINE;

    // fill in info
    if (! EnumPrinters(PRINTER_ENUM_DEFAULT, NULL, 2, (LPBYTE) printerInfo, dwNeeded, &dwNeeded, &dwReturned) ) {
      free(printerInfo);
      return INVALID_ROUTINE;
    }

    lstrcpy(retstr->strptr,printerInfo->pPrinterName);

    free(printerInfo);

  } else
    // NT / W2K:
    GetProfileString("Windows", "DEVICE", ",,,", retstr->strptr, (DWORD)retstr->strlength);

  retstr->strlength = strlen(retstr->strptr);

  return VALID_ROUTINE;
}

/* TODO FIXME we should have a general purpose function for determing the
 *  Window version.  There is one in the incubator in WinShell.
 */
bool getOSVersionStruct(OSVERSIONINFOEX *pVersionInfo)
{
   // First try calling GetVersionEx with the extended struct and if it fails,
   // try with the original struct.

   ZeroMemory(pVersionInfo, sizeof(OSVERSIONINFOEX));
   pVersionInfo->dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);

   if( ! GetVersionEx((OSVERSIONINFO *)pVersionInfo) )
   {
      pVersionInfo->dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
      if ( ! GetVersionEx((OSVERSIONINFO *) pVersionInfo) )
      {
          return FALSE;
      }
   }
   return true;
}

/*************************************************************************
* Function:  SysWinSetDefaultPrinter                                     *
*                                                                        *
* Syntax:    call SysWinSetDefaultPrinter printer                        *
*                                                                        *
* Params:    string describing default printer                           *
*                                                                        *
* Return:    0 on success, otherwise the OS system error number.         *
*************************************************************************/
size_t RexxEntry SysWinSetDefaultPrinter(const char *name, size_t numargs, CONSTRXSTRING args[],
                                         const char *queuename, PRXSTRING retstr)
{
    DWORD  errnum = 0;
    bool   success = true;
    UINT   count = 0;
    OSVERSIONINFOEX osv;

    if (numargs != 1)
    {
        return INVALID_ROUTINE;
    }

    // Two forms of input are allowed.  The old form of
    // "Printername,Drivername,Portname" and for W2K or later just the printer
    // name.  Count the commas to determine which form this might be.
    for ( size_t i = 0; i < args[0].strlength; i++ )
    {
        if ( args[0].strptr[i] == ',' )
        {
            count++;
        }
    }

    if ( ! getOSVersionStruct(&osv) )
    {
        return INVALID_ROUTINE;
    }

    SetLastError(0);

    if ( osv.dwMajorVersion >= 5 && count == 0 )
    {
        // This is W2K or later and the user specified just the printer name.
        // This code will work on W2K through Vista.
        if ( SetDefaultPrinter(args[0].strptr) == 0 )
        {
            success = false;
        }
    }
    else if ( count == 2 )
    {
        // NT or earlier, or the user still specified the old format. Microssoft
        // only provides WriteProfileString() for backward compatibility to
        // 16-bit Windows, and at some point this may no longer be supported.
        // But it still appears to work on XP.
        if ( WriteProfileString("Windows", "DEVICE", args[0].strptr) == 0 )
        {
            success = false;
        }
        else
        {
            if ( SendMessageTimeout(HWND_BROADCAST, WM_SETTINGCHANGE, 0L, 0L, SMTO_NORMAL, 1000, NULL) == 0 )
            {
                // If a window just timed out, then GetLastError() will return 0
                // and the user will get the succes code.  If GetLastError()
                // does not return 0, then chances are something really is
                // wrong.
                success = false;
            }
        }
    }
    else
    {
        return INVALID_ROUTINE;
    }

    if ( ! success )
    {
        errnum = GetLastError();
    }

    sprintf(retstr->strptr, "%d", errnum);
    retstr->strlength = strlen(retstr->strptr);

    return VALID_ROUTINE;
}

/*************************************************************************
* Function:  SysFileCopy                                                 *
*                                                                        *
* Syntax:    call SysFileCopy FROMfile TOfile                            *
*                                                                        *
* Params:    FROMfile - file to be copied.                               *
*            TOfile - target file of copy operation.                     *
*                                                                        *
* Return:    Return code from CopyFile() function.                       *
*************************************************************************/

RexxRoutine2(int, SysFileCopy, CSTRING, fromFile, CSTRING, toFile)
{
    return CopyFile(fromFile, toFile, 0) ? 0 : GetLastError();
}

/*************************************************************************
* Function:  SysFileMove                                                 *
*                                                                        *
* Syntax:    call SysFileMove FROMfile TOfile                            *
*                                                                        *
* Params:    FROMfile - file to be moved.                                *
*            TOfile - target file of move operation.                     *
*                                                                        *
* Return:    Return code from MoveFile() function.                       *
*************************************************************************/

RexxRoutine2(int, SysFileMove, CSTRING, fromFile, CSTRING, toFile)
{
    return MoveFile(fromFile, toFile) ? 0 : GetLastError();
}

/*************************************************************************
* Function:  SysFileExist                                                *
*                                                                        *
* Syntax:    call SysFileExist file                                      *
*                                                                        *
* Params:    file - file to check existance of.                          *
*                                                                        *
* Return:    Logical.                                                    *
*************************************************************************/

RexxRoutine1(logical_t, SysIsFile, CSTRING, file)
{
    DWORD dwAttrs = GetFileAttributes(file);
    // not a file if either of these is one
    return (dwAttrs != 0xffffffff) && ((dwAttrs & (FILE_ATTRIBUTE_DIRECTORY | FILE_ATTRIBUTE_REPARSE_POINT)) == 0);
}

/*************************************************************************
* Function:  SysDirExist                                                 *
*                                                                        *
* Syntax:    call SysDirExist dir                                        *
*                                                                        *
* Params:    dir - dir to check existance of.                            *
*                                                                        *
* Return:    Logical.                                                    *
*************************************************************************/

RexxRoutine1(logical_t, SysIsFileDirectory, CSTRING, file)
{
    DWORD dwAttrs = GetFileAttributes(file);
    return (dwAttrs != 0xffffffff) && (dwAttrs & FILE_ATTRIBUTE_DIRECTORY);
}

/*************************************************************************
* Function:  SysIsFileLink                                               *
*                                                                        *
* Syntax:    call SysIsFileLink file                                     *
*                                                                        *
* Params:    file - file to check if it is a Link (Alias).               *
*                                                                        *
* Return:    Logical.                                                    *
*************************************************************************/

RexxRoutine1(logical_t, SysIsFileLink, CSTRING, file)
{
    DWORD dwAttrs = GetFileAttributes(file);
    return (dwAttrs != 0xffffffff) && (dwAttrs & FILE_ATTRIBUTE_REPARSE_POINT);
}

/*************************************************************************
* Function:  SysIsFileCompressed                                         *
*                                                                        *
* Syntax:    call SysIsFileCompressed file                               *
*                                                                        *
* Params:    file - file to check if it is compressed.                   *
*                                                                        *
* Return:    Logical.                                                    *
*************************************************************************/

RexxRoutine1(logical_t, SysIsFileCompressed, CSTRING, file)
{
    DWORD dwAttrs = GetFileAttributes(file);
    return (dwAttrs != 0xffffffff) && (dwAttrs & FILE_ATTRIBUTE_COMPRESSED);
}

/*************************************************************************
* Function:  SysIsFileEncrypted                                          *
*                                                                        *
* Syntax:    call SysIsFileEncrypted file                                *
*                                                                        *
* Params:    file - file to check if it is Encrypted.                    *
*                                                                        *
* Return:    Logical.                                                    *
*************************************************************************/

RexxRoutine1(logical_t, SysIsFileEncrypted, CSTRING, file)
{
    DWORD dwAttrs = GetFileAttributes(file);
    return (dwAttrs != 0xffffffff) && (dwAttrs & FILE_ATTRIBUTE_ENCRYPTED);
}

/*************************************************************************
* Function:  SysIsFileNotContentIndexed                                  *
*                                                                        *
* Syntax:    call SysIsFileNotContentIndexed file                        *
*                                                                        *
* Params:    file - file to check if it is to be indexed by the indexing *
*             service.                                                   *
*                                                                        *
* Return:    Logical.                                                    *
*************************************************************************/

RexxRoutine1(logical_t, SysIsFileNotContentIndexed, CSTRING, file)
{
    DWORD dwAttrs = GetFileAttributes(file);
    return (dwAttrs != 0xffffffff) && (dwAttrs & FILE_ATTRIBUTE_NOT_CONTENT_INDEXED);
}

/*************************************************************************
* Function:  SysIsFileOffline                                            *
*                                                                        *
* Syntax:    call SysIsFileOffline file                                  *
*                                                                        *
* Params:    file - file to check if it is offline                       *
*                                                                        *
* Return:    Logical.                                                    *
*************************************************************************/

RexxRoutine1(logical_t, SysIsFileOffline, CSTRING, file)
{
    DWORD dwAttrs = GetFileAttributes(file);
    return (dwAttrs != 0xffffffff) && (dwAttrs & FILE_ATTRIBUTE_OFFLINE);
}

/*************************************************************************
* Function:  SysIsFileSparse                                             *
*                                                                        *
* Syntax:    call SysIsFileSparse file                                   *
*                                                                        *
* Params:    file - file to check if it is sparse                        *
*                                                                        *
* Return:    Logical.                                                    *
*************************************************************************/

RexxRoutine1(logical_t, SysIsFileSparse, CSTRING, file)
{
    DWORD dwAttrs = GetFileAttributes(file);
    return (dwAttrs != 0xffffffff) && (dwAttrs & FILE_ATTRIBUTE_SPARSE_FILE);
}


/*************************************************************************
* Function:  SysIsFileTemporary                                          *
*                                                                        *
* Syntax:    call SysIsFileTemporary file                                *
*                                                                        *
* Params:    file - file to check if it is temporary                     *
*                                                                        *
* Return:    Logical.                                                    *
*************************************************************************/

RexxRoutine1(logical_t, SysIsFileTemporary, CSTRING, file)
{
    DWORD dwAttrs = GetFileAttributes(file);
    return (dwAttrs != 0xffffffff) && (dwAttrs & FILE_ATTRIBUTE_TEMPORARY);
}


/*************************************************************************
* Function:  SysFileExists                                               *
*                                                                        *
* Syntax:    call SysFileExists  file                                    *
*                                                                        *
* Params:    file - file to check existence                              *
*                                                                        *
* Return:    Logical.                                                    *
*************************************************************************/

RexxRoutine1(logical_t, SysFileExists, CSTRING, file)
{
    DWORD dwAttrs = GetFileAttributes(file);
    return (dwAttrs != 0xffffffff);
}


// now build the actual entry list
RexxRoutineEntry rexxutil_routines[] =
{
    REXX_CLASSIC_ROUTINE(SysCls,                      SysCls),
    REXX_CLASSIC_ROUTINE(SysCurPos,                   SysCurPos),
    REXX_CLASSIC_ROUTINE(SysCurState,                 SysCurState),
    REXX_CLASSIC_ROUTINE(SysDriveInfo,                SysDriveInfo),
    REXX_CLASSIC_ROUTINE(SysDriveMap,                 SysDriveMap),
    REXX_CLASSIC_ROUTINE(SysDropFuncs,                SysDropFuncs),
    REXX_CLASSIC_ROUTINE(SysFileDelete,               SysFileDelete),
    REXX_CLASSIC_ROUTINE(SysFileSearch,               SysFileSearch),
    REXX_TYPED_ROUTINE(SysFileTree,                   SysFileTree),
    REXX_CLASSIC_ROUTINE(SysGetKey,                   SysGetKey),
    REXX_CLASSIC_ROUTINE(SysIni,                      SysIni),
    REXX_CLASSIC_ROUTINE(SysLoadFuncs,                SysLoadFuncs),
    REXX_CLASSIC_ROUTINE(SysMkDir,                    SysMkDir),
    REXX_CLASSIC_ROUTINE(SysWinVer,                   SysWinVer),
    REXX_CLASSIC_ROUTINE(SysVersion,                  SysVersion),
    REXX_CLASSIC_ROUTINE(SysRmDir,                    SysRmDir),
    REXX_CLASSIC_ROUTINE(SysSearchPath,               SysSearchPath),
    REXX_CLASSIC_ROUTINE(SysSleep,                    SysSleep),
    REXX_CLASSIC_ROUTINE(SysTempFileName,             SysTempFileName),
    REXX_CLASSIC_ROUTINE(SysTextScreenRead,           SysTextScreenRead),
    REXX_CLASSIC_ROUTINE(SysTextScreenSize,           SysTextScreenSize),
    REXX_CLASSIC_ROUTINE(SysPi,                       SysPi),
    REXX_CLASSIC_ROUTINE(SysSqrt,                     SysSqrt),
    REXX_CLASSIC_ROUTINE(SysExp,                      SysExp),
    REXX_CLASSIC_ROUTINE(SysLog,                      SysLog),
    REXX_CLASSIC_ROUTINE(SysLog10,                    SysLog10),
    REXX_CLASSIC_ROUTINE(SysSinH,                     SysSinH),
    REXX_CLASSIC_ROUTINE(SysCosH,                     SysCosH),
    REXX_CLASSIC_ROUTINE(SysTanH,                     SysTanH),
    REXX_CLASSIC_ROUTINE(SysPower,                    SysPower),
    REXX_CLASSIC_ROUTINE(SysSin,                      SysSin),
    REXX_CLASSIC_ROUTINE(SysCos,                      SysCos),
    REXX_CLASSIC_ROUTINE(SysTan,                      SysTan),
    REXX_CLASSIC_ROUTINE(SysCotan,                    SysCotan),
    REXX_CLASSIC_ROUTINE(SysArcSin,                   SysArcSin),
    REXX_CLASSIC_ROUTINE(SysArcCos,                   SysArcCos),
    REXX_CLASSIC_ROUTINE(SysArcTan,                   SysArcTan),
    REXX_CLASSIC_ROUTINE(SysAddRexxMacro,             SysAddRexxMacro),
    REXX_CLASSIC_ROUTINE(SysDropRexxMacro,            SysDropRexxMacro),
    REXX_CLASSIC_ROUTINE(SysReorderRexxMacro,         SysReorderRexxMacro),
    REXX_CLASSIC_ROUTINE(SysQueryRexxMacro,           SysQueryRexxMacro),
    REXX_CLASSIC_ROUTINE(SysClearRexxMacroSpace,      SysClearRexxMacroSpace),
    REXX_CLASSIC_ROUTINE(SysLoadRexxMacroSpace,       SysLoadRexxMacroSpace),
    REXX_CLASSIC_ROUTINE(SysSaveRexxMacroSpace,       SysSaveRexxMacroSpace),
    REXX_CLASSIC_ROUTINE(SysBootDrive,                SysBootDrive),
    REXX_CLASSIC_ROUTINE(SysSystemDirectory,          SysSystemDirectory),
    REXX_CLASSIC_ROUTINE(SysFileSystemType,           SysFileSystemType),
    REXX_CLASSIC_ROUTINE(SysVolumeLabel,              SysVolumeLabel),
    REXX_CLASSIC_ROUTINE(SysCreateMutexSem,           SysCreateMutexSem),
    REXX_CLASSIC_ROUTINE(SysOpenMutexSem,             SysOpenMutexSem),
    REXX_CLASSIC_ROUTINE(SysCloseMutexSem,            SysCloseMutexSem),
    REXX_CLASSIC_ROUTINE(SysRequestMutexSem,          SysRequestMutexSem),
    REXX_CLASSIC_ROUTINE(SysReleaseMutexSem,          SysReleaseMutexSem),
    REXX_CLASSIC_ROUTINE(SysCreateEventSem,           SysCreateEventSem),
    REXX_CLASSIC_ROUTINE(SysOpenEventSem,             SysOpenEventSem),
    REXX_CLASSIC_ROUTINE(SysCloseEventSem,            SysCloseEventSem),
    REXX_CLASSIC_ROUTINE(SysResetEventSem,            SysResetEventSem),
    REXX_CLASSIC_ROUTINE(SysPostEventSem,             SysPostEventSem),
    REXX_CLASSIC_ROUTINE(SysPulseEventSem,            SysPulseEventSem),
    REXX_CLASSIC_ROUTINE(SysWaitEventSem,             SysWaitEventSem),
    REXX_CLASSIC_ROUTINE(SysSetPriority,              SysSetPriority),
    REXX_CLASSIC_ROUTINE(SysShutDownSystem,           SysShutDownSystem),
    REXX_CLASSIC_ROUTINE(SysSwitchSession,            SysSwitchSession),
    REXX_CLASSIC_ROUTINE(SysWaitNamedPipe,            SysWaitNamedPipe),
    REXX_CLASSIC_ROUTINE(SysQueryProcess,             SysQueryProcess),
    REXX_CLASSIC_ROUTINE(SysDumpVariables,            SysDumpVariables),
    REXX_CLASSIC_ROUTINE(SysSetFileDateTime,          SysSetFileDateTime),
    REXX_CLASSIC_ROUTINE(SysGetFileDateTime,          SysGetFileDateTime),
    REXX_CLASSIC_ROUTINE(SysStemSort,                 SysStemSort),
    REXX_TYPED_ROUTINE(SysStemDelete,                 SysStemDelete),
    REXX_CLASSIC_ROUTINE(SysStemCopy,                 SysStemCopy),
    REXX_CLASSIC_ROUTINE(SysUtilVersion,              SysUtilVersion),
    REXX_CLASSIC_ROUTINE(RxWinExec,                   RxWinExec),
    REXX_CLASSIC_ROUTINE(SysWinEncryptFile,           SysWinEncryptFile),
    REXX_CLASSIC_ROUTINE(SysWinDecryptFile,           SysWinDecryptFile),
    REXX_CLASSIC_ROUTINE(SysGetErrortext,             SysGetErrortext),
    REXX_CLASSIC_ROUTINE(SysToUniCode,                SysToUniCode),
    REXX_CLASSIC_ROUTINE(SysWinGetPrinters,           SysWinGetPrinters),
    REXX_CLASSIC_ROUTINE(SysWinGetDefaultPrinter,     SysWinGetDefaultPrinter),
    REXX_CLASSIC_ROUTINE(SysWinSetDefaultPrinter,     SysWinSetDefaultPrinter),
    REXX_TYPED_ROUTINE(SysFileCopy,                   SysFileCopy),
    REXX_TYPED_ROUTINE(SysFileMove,                   SysFileMove),
    REXX_TYPED_ROUTINE(SysIsFile,                     SysIsFile),
    REXX_TYPED_ROUTINE(SysIsFileDirectory,            SysIsFileDirectory),
    REXX_TYPED_ROUTINE(SysIsFileLink,                 SysIsFileLink),
    REXX_TYPED_ROUTINE(SysIsFileCompressed,           SysIsFileCompressed),
    REXX_TYPED_ROUTINE(SysIsFileEncrypted,            SysIsFileEncrypted),
    REXX_TYPED_ROUTINE(SysIsFileNotContentIndexed,    SysIsFileNotContentIndexed),
    REXX_TYPED_ROUTINE(SysIsFileOffline,              SysIsFileOffline),
    REXX_TYPED_ROUTINE(SysIsFileSparse,               SysIsFileSparse),
    REXX_TYPED_ROUTINE(SysIsFileTemporary,            SysIsFileTemporary),
    REXX_TYPED_ROUTINE(SysFileExists,                 SysFileExists),
    REXX_TYPED_ROUTINE(SysFromUniCode,                SysFromUniCode),
    REXX_TYPED_ROUTINE(SysStemInsert,                 SysStemInsert),
    REXX_LAST_ROUTINE()
};

RexxPackageEntry rexxutil_package_entry =
{
    STANDARD_PACKAGE_HEADER
    REXX_INTERPRETER_4_0_0,              // anything after 4.0.0 will work
    "REXXUTIL",                          // name of the package
    "4.0",                               // package information
    NULL,                                // no load/unload functions
    NULL,
    rexxutil_routines,                   // the exported functions
    NULL                                 // no methods in this package
};

// package loading stub.
OOREXX_GET_PACKAGE(rexxutil);
