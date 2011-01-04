/*----------------------------------------------------------------------------*/
/*                                                                            */
/* Copyright (c) 1995, 2004 IBM Corporation. All rights reserved.             */
/* Copyright (c) 2005-2010 Rexx Language Association. All rights reserved.    */
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

/**
 * oodUtilities.cpp
 *
 * Contains utility classes, including the .DlgUtil class.
 */
#include "ooDialog.hpp"     // Must be first, includes windows.h, commctrl.h, and oorexxapi.h

#include <shlwapi.h>
#include <stdio.h>
#include "APICommon.hpp"
#include "oodCommon.hpp"
#include "oodDeviceGraphics.hpp"


/**
 * Defines, structs, and methods for the DlgUtil class.
 */
#define DLGUTIL_CLASS      "DlgUtil"

/**
 * Do not include ICC_STANDARD_CLASSES.  Some versions of Windows XP have a bug
 * that causes InitCommonControlsEx() to fail when that flag is used.  The flag
 * itself is not needed under any version of Windows.
 *
 * Note: These flags are valid under any supported CommCtrl32 version.  But, as
 * ooDialog adds support for more dialog controls, it may become necessary to
 * check the CommCtrl32 version and have a different set of flags for certain
 * versions.
 */
#define INITCOMMONCONTROLS_CLASS_FLAGS    ICC_WIN95_CLASSES | ICC_DATE_CLASSES

const char *comctl32VersionPart(DWORD id, DWORD type)
{
    const char *part;
    switch ( id )
    {
        case COMCTL32_4_0 :
            if ( type == COMCTL32_NUMBER_PART )
            {
                part = "4.0";
            }
            else if ( type == COMCTL32_OS_PART )
            {
                part = "W95 / NT4";
            }
            else
            {
                part = "comctl32.dll version 4.0 (W95 / NT4)";
            }
            break;

        case COMCTL32_4_7 :
            if ( type == COMCTL32_NUMBER_PART )
            {
                part = "4.7";
            }
            else if ( type == COMCTL32_OS_PART )
            {
                part = "IE 3.x";
            }
            else
            {
                part = "comctl32.dll version 4.7 (IE 3.x)";
            }
            break;

        case COMCTL32_4_71 :
            if ( type == COMCTL32_NUMBER_PART )
            {
                part = "4.71";
            }
            else if ( type == COMCTL32_OS_PART )
            {
                part = "IE 4.0";
            }
            else
            {
                part = "comctl32.dll version 4.71 (IE 4.0)";
            }
            break;

        case COMCTL32_4_72 :
            if ( type == COMCTL32_NUMBER_PART )
            {
                part = "4.72";
            }
            else if ( type == COMCTL32_OS_PART )
            {
                part = "W98 / IE 4.01";
            }
            else
            {
                part = "comctl32.dll version 4.72 (W98 / IE 4.01)";
            }
            break;

        case COMCTL32_5_8 :
            if ( type == COMCTL32_NUMBER_PART )
            {
                part = "5.8";
            }
            else if ( type == COMCTL32_OS_PART )
            {
                part = "IE 5";
            }
            else
            {
                part = "comctl32.dll version 5.8 (IE 5)";
            }
            break;

        case COMCTL32_5_81 :
            if ( type == COMCTL32_NUMBER_PART )
            {
                part = "5.81";
            }
            else if ( type == COMCTL32_OS_PART )
            {
                part = "W2K / ME";
            }
            else
            {
                part = "comctl32.dll version 5.81 (W2K / ME)";
            }
            break;

        case COMCTL32_6_0 :
            if ( type == COMCTL32_NUMBER_PART )
            {
                part = "6.0";
            }
            else if ( type == COMCTL32_OS_PART )
            {
                part = "XP";
            }
            else
            {
                part = "comctl32.dll version 6.0 (XP)";
            }
            break;

        case COMCTL32_6_10 :
            if ( type == COMCTL32_NUMBER_PART )
            {
                part = "6.10";
            }
            else if ( type == COMCTL32_OS_PART )
            {
                part = "Vista SP2 / Windows 7";
            }
            else
            {
                part = "comctl32.dll version 6.10 (Vista SP2 / Windows 7)";
            }
            break;

        default :
            part = "Unknown";
            break;
    }
    return part;
}

/**
 * Determines the version of comctl32.dll and compares it against a minimum
 * required version.
 *
 * @param  context      The ooRexx method context.
 * @param  pDllVersion  The loaded version of comctl32.dll is returned here as a
 *                      packed unsigned long. This number is created using
 *                      Microsoft's suggested process and can be used for
 *                      numeric comparisons.
 * @param  minVersion   The minimum acceptable version.
 * @param  packageName  The name of the package initiating this check.
 * @param  errTitle     The title for the error dialog if it is displayed.
 *
 * @note  If this function fails, an exception is raised.
 */
bool getComCtl32Version(RexxMethodContext *context, DWORD *pDllVersion, DWORD minVersion,
                         CSTRING packageName, CSTRING errTitle)
{
    bool success = false;
    *pDllVersion = 0;

    HINSTANCE hinst = LoadLibrary(TEXT(COMMON_CONTROL_DLL));
    if ( hinst )
    {
        DLLGETVERSIONPROC pDllGetVersion;

        pDllGetVersion = (DLLGETVERSIONPROC)GetProcAddress(hinst, DLLGETVERSION_FUNCTION);
        if ( pDllGetVersion )
        {
            HRESULT hr;
            DLLVERSIONINFO info;

            ZeroMemory(&info, sizeof(info));
            info.cbSize = sizeof(info);

            hr = (*pDllGetVersion)(&info);
            if ( SUCCEEDED(hr) )
            {
                *pDllVersion = MAKEVERSION(info.dwMajorVersion, info.dwMinorVersion);
                _snprintf(ComCtl32VersionStr, COMCTL32_VERSION_STRING_LEN, "ComCtl32 v%d.%d",
                          info.dwMajorVersion, info.dwMinorVersion);
                success = true;
            }
            else
            {
                systemServiceExceptionComCode(context->threadContext, COM_API_FAILED_MSG, DLLGETVERSION_FUNCTION, hr);
            }
        }
        else
        {
            systemServiceExceptionCode(context->threadContext, NO_PROC_MSG, DLLGETVERSION_FUNCTION);
        }
        FreeLibrary(hinst);
    }
    else
    {
        systemServiceExceptionCode(context->threadContext, NO_HMODULE_MSG, COMMON_CONTROL_DLL);
    }

    if ( *pDllVersion == 0 )
    {
        CHAR msg[256];
        _snprintf(msg, sizeof(msg),
                  "The version of the Windows Common Controls library (%s)\n"
                  "could not be determined.  %s can not continue",
                  COMMON_CONTROL_DLL, packageName);

        internalErrorMsgBox(msg, errTitle);
        success = false;
    }
    else if ( *pDllVersion < minVersion )
    {
        CHAR msg[256];
        _snprintf(msg, sizeof(msg),
                  "%s can not continue with this version of the Windows\n"
                  "Common Controls library(%s.)  The minimum\n"
                  "version required is: %s.\n\n"
                  "This system has: %s\n",
                  packageName, COMMON_CONTROL_DLL, comctl32VersionName(minVersion),
                  comctl32VersionName(*pDllVersion));

        internalErrorMsgBox(msg, errTitle);
        *pDllVersion = 0;
        success = false;
    }
    return success;
}

/**
 * Initializes the common control library for the specified classes.
 *
 * @param classes       Flag specifing the classes to be initialized.
 * @param  packageName  The name of the package initializing the classes.
 * @param  errTitle     The title for the error dialog if it is displayed.
 *
 * @return True on success, otherwise false.
 *
 * @note   An exception has been raised when false is returned.
 */
bool initCommonControls(RexxMethodContext *context, DWORD classes, CSTRING packageName, CSTRING errTitle)
{
    INITCOMMONCONTROLSEX ctrlex;

    ctrlex.dwSize = sizeof(ctrlex);
    ctrlex.dwICC = classes;

    if ( ! InitCommonControlsEx(&ctrlex) )
    {
        systemServiceExceptionCode(context->threadContext, NO_COMMCTRL_MSG, "Common Control Library");

        CHAR msg[128];
        _snprintf(msg, sizeof(msg),
                  "Initializing the Windows Common Controls\n"
                  "library failed.  %s can not continue.\n\n"
                  "Windows System Error Code: %d\n", packageName, GetLastError());

        internalErrorMsgBox(msg, errTitle);
        return false;
    }
    return true;
}

/** DlgUtil::init() [class method]
 *
 * The .DlgUtil class init() method.  It executes when the .DlgUtil class is
 * constructed, which is done during the processing of the ::requires directive
 * for oodPlain.cls.  This makes it the ideal place for any initialization that
 * must be done prior to ooDialog starting.
 *
 * Note that an exception raised here effectively terminates ooDialog before any
 * user code is executed.
 *
 * The method:
 *
 * 1.) Determines the version of comctl32.dll and initializes the common
 * controls.  The minimum acceptable version of 4.71 is supported on Windows 95
 * with Internet Explorer 4.0, Windows NT 4.0 with Internet Explorer 4.0,
 * Windows 98, and Windows 2000.
 *
 * 2.) Initializes a null pointer Pointer object and places it in the .local
 * directory. (.NullHandle)  This allows ooRexx code to use a null handle for an
 * argument where appropriate.
 *
 * 3.) Places the SystemErrorCode (.SystemErrorCode) variable in the .local
 * directory.
 *
 * @return .true if comctl32.dll is at least version 4.71, otherwise .false.
 */
RexxMethod0(logical_t, dlgutil_init_cls)
{
    RexxMethodContext *c = context;
    TheTrueObj = context->True();
    TheFalseObj = context->False();
    TheNilObj = context->Nil();
    TheNullPtrObj = context->NewPointer(NULL);
    TheZeroObj = TheFalseObj;
    TheOneObj = TheTrueObj;

    if ( ! getComCtl32Version(context, &ComCtl32Version, COMCTL32_4_71, "ooDialog", COMCTL_ERR_TITLE) )
    {
        return false;
    }

    if ( ! initCommonControls(context, INITCOMMONCONTROLS_CLASS_FLAGS, "ooDialog", COMCTL_ERR_TITLE) )
    {
        ComCtl32Version = 0;
        return false;
    }

    RexxDirectoryObject local = context->GetLocalEnvironment();
    if ( local != NULLOBJECT )
    {
        TheDotLocalObj = local;
        TheNegativeOneObj = context->WholeNumber(-1);
        TheTwoObj = context->WholeNumber(2);
        context->SetObjectVariable("THENEGATIVEONEOBJ", TheNegativeOneObj);
        context->SetObjectVariable("THENTWOOBJ", TheTwoObj);
        context->DirectoryPut(local, TheNullPtrObj, "NULLHANDLE");
        context->DirectoryPut(local, context->WholeNumberToObject(0), "SYSTEMERRORCODE");
    }
    else
    {
        severeErrorException(context->threadContext, NO_LOCAL_ENVIRONMENT_MSG);
        return false;
    }

    return true;
}

/** DlgUtil::comCtl32Version()  [class method]
 *
 * Returns the comctl32.dll version that ooDialog is currently using in one of
 * the 4 formats listed below.
 *
 * @param format  [optional] Keyword indicating the format, only the first
 *                letter is needed, case not significant.  The default is short.
 *                Incorrect strings are ignored and the default is used.
 *
 * The formats are:
 *   Full:  A complete string including the DLL name, version number and the
 *          minimum Windows OS name that the DLL is found on.
 *
 *   Number:  The version number part of the full string.
 *   Short:   Same as number.
 *   OS:      The OS name part of the full string.
 */
RexxMethod1(RexxStringObject, dlgutil_comctl32Version_cls, OPTIONAL_CSTRING, format)
{
    const char *ver;
    char f = argumentOmitted(1) ? 'S' : *format;

    switch ( f )
    {
        case 'f' :
        case 'F' :
            ver = comctl32VersionName(ComCtl32Version);
            break;

        case 'o' :
        case 'O' :
            ver = comctl32VersionPart(ComCtl32Version, COMCTL32_OS_PART);
            break;

        case 's' :
        case 'S' :
        case 'n' :
        case 'N' :
        default :
            ver = comctl32VersionPart(ComCtl32Version, COMCTL32_NUMBER_PART);
            break;
    }
    return context->String(ver);
}

/** DlgUtil::version()  [class method]
 *
 *  Returns the ooDialog version string, either the full string, or just the
 *  number part of the string.
 *
 * @param  format  [optional]  Keyword indicating which format the returned
 *                 string should be in.  Keywords are:
 *
 *         Short   4.1.0.5814
 *
 *         Full    ooDialog Version 4.1.0.5814 (an ooRexx Windows Extension)
 *
 *         Level   4.2.0
 *
 *                 Only the first letter is required and case is not
 *                 significant.  If the argument is omitted the Full format is
 *                 the default.
 */
RexxMethod1(RexxStringObject, dlgutil_version_cls, OPTIONAL_CSTRING, format)
{
    char buf[64];

    switch ( toupper(*format) )
    {
        case 'L' :
            _snprintf(buf, sizeof(buf), "%u.%u.%u", OOD_LVL_MAJOR, OOD_LVL_MINOR, OOD_LVL_BIT);
            break;

        case 'S' :
            _snprintf(buf, sizeof(buf), "%u.%u.%u.%u", ORX_VER, ORX_REL, ORX_MOD, OOREXX_BLD);
            break;

        case '\0' :
        default :
            _snprintf(buf, sizeof(buf), "ooDialog Version %u.%u.%u.%u (an ooRexx Windows Extension)",
                      ORX_VER, ORX_REL, ORX_MOD, OOREXX_BLD);
            break;

    }
    return context->String(buf);
}

RexxMethod1(uint16_t, dlgutil_hiWord_cls, uint32_t, dw) { return HIWORD(dw); }
RexxMethod1(uint16_t, dlgutil_loWord_cls, uint32_t, dw) { return LOWORD(dw); }
RexxMethod2(intptr_t, dlgutil_makeLPARAM_cls, int16_t, loWord, int16_t, hiWord) { return MAKELPARAM(loWord, hiWord); }
RexxMethod2(intptr_t, dlgutil_makeWPARAM_cls, int16_t, loWord, int16_t, hiWord) { return MAKEWPARAM(loWord, hiWord); }

RexxMethod2(uint64_t, dlgutil_and_cls, CSTRING, s1, CSTRING, s2)
{
    uint64_t n1, n2;
    if ( ! rxStr2Number(context, s1, &n1, 1) || ! rxStr2Number(context, s2, &n2, 2) )
    {
        return 0;
    }
    return (n1 & n2);
}

RexxMethod1(uint64_t, dlgutil_or_cls, ARGLIST, args)
{
    RexxMethodContext *c = context;
    uint64_t result, n1;

    size_t count = c->ArraySize(args);
    if ( count == 0 )
    {
        return 0;
    }

    CSTRING s1 = c->ObjectToStringValue(c->ArrayAt(args, 1));
    if ( ! rxStr2Number(c, s1, &result, 1) )
    {
        return 0;
    }

    for ( size_t i = 2; i <= count; i++)
    {
        s1 = c->ObjectToStringValue(c->ArrayAt(args, i));
        if ( ! rxStr2Number(c, s1, &n1, (int)i) )
        {
            return 0;
        }
        result |= n1;
    }

    return result;
}

/** DlgUtil::getSystemMetrics()  [class method]
 *
 *  Returns the system metric for the give index.
 *
 *  @param index  The index of the system metric to look up.
 *
 *  @note There was a classic Rexx external function documented prior to 4.0.0
 *        with the function name of GetSysMetrics.  That function is now marked
 *        deprecated in the docs and the places where it was used are mapped to
 *        this .DlgUtil method.
 *
 *        The intent was to extend this function in the future to get multiple,
 *        and perhaps all, values at once.  This method could be enhanced to do
 *        that.
 *
 *        MSDN documents that GetLastError does not provide extended error
 *        information.
 */
RexxMethod1(uint32_t, dlgutil_getSystemMetrics_cls, int32_t, index)
{
    return GetSystemMetrics(index);
}

/** DlgUtil::screenSize()  [class method]
 *
 *  Retrieves the screen size in either pixels, dialog units, or both.
 *
 *  @param  _flag  [optional]  Keyword signaling whether to return the size in
 *                 pixels, dialog units, or both.  The default is both.  The
 *                 keywords are Pixels, DialogUnits, and Both.  Only the first
 *                 letter is checked and case is insignificant.
 *
 *  @param  dlgObj  [optional]  A Rexx dialog object.  When this argument is
 *                  used, dialog units are calculated correctly for the dialog.
 *                  If it is omitted, the dialog units are calculated using 8 pt
 *                  System font, which will be incorrect for any dialog that
 *                  uses a different fault.  However, this will give the same
 *                  result as was returned prior to ooRexx 4.0.0.
 *
 *  @return  The return is dependent on the flag in use:
 *           Pixels:       A .Size object with the screen size in pixels.
 *           DialogUnits:  A .Size object with the screen size in dialog units.
 *           Both:         An array:
 *                         a[1] = duX, a[2] = duY, a[3] = pixelX, a[4] = pixelY
 *
 */
RexxMethod2(RexxObjectPtr, dlgutil_screenSize_cls, OPTIONAL_CSTRING, _flag, OPTIONAL_RexxObjectPtr, dlgObj)
{
    RexxObjectPtr result = NULLOBJECT;

    uint32_t pixelX = GetSystemMetrics(SM_CXSCREEN);
    uint32_t pixelY = GetSystemMetrics(SM_CYSCREEN);

    char flag = 'B';
    if ( argumentExists(1) )
    {
        flag = toupper(*_flag);
        if ( ! (flag == 'B' || flag == 'D' || flag == 'P') )
        {
            wrongArgValueException(context->threadContext, 1, "DialogUnit, Pixel, Both", _flag);
            goto done_out;
        }
    }

    uint32_t duX, duY;
    if ( flag == 'B' || flag == 'D' )
    {
        // We need to calculate the dialog units.  Iff we have a dlgObj, we'll
        // calculate them correctly, otherwise we use the broken method.
        if ( argumentExists(2) )
        {
            if ( ! requiredClass(context->threadContext, dlgObj, "PlainBaseDialog", 2) )
            {
                goto done_out;
            }

            POINT point = {pixelX, pixelY};
            mapPixelToDu(context, dlgObj, &point, 1);
            duX = point.x;
            duY = point.y;
        }
        else
        {
            long bu = GetDialogBaseUnits();
            duX = (pixelX * 4) / LOWORD(bu);
            duY = (pixelY * 8) / HIWORD(bu);
        }
    }

    if ( flag == 'B')
    {
        result = context->ArrayOfFour(context->UnsignedInt32(duX),
                                      context->UnsignedInt32(duY),
                                      context->UnsignedInt32(pixelX),
                                      context->UnsignedInt32(pixelY));
    }
    else if ( flag == 'D' )
    {
        result = rxNewSize(context, duX, duY);
    }
    else
    {
        result = rxNewSize(context, pixelX, pixelY);
    }

done_out:
    return result;
}


/** DlgUtil::screenArea()  [class method]
 *
 *  Gets the usable screen area (work area.) on the primary display monitor. The
 *  work area is the portion of the screen not obscured by the system taskbar or
 *  by application desktop toolbars.
 *
 *  @return  The work area as a .Rect object.
 *
 *  @note Sets the .SystemErrorCode.
 */
RexxMethod0(RexxObjectPtr, dlgutil_screenArea_cls)
{
    oodResetSysErrCode(context->threadContext);
    RexxMethodContext *c = context;
    RECT r = {0};
    if ( ! SystemParametersInfo(SPI_GETWORKAREA, 0, &r, 0) )
    {
        oodSetSysErrCode(context->threadContext);
    }

    return rxNewRect(context, &r);
}

/**
 * A temporary utility to convert from a handle that is still being stored in
 * ooDialog in string form ("0xFFFFAAAA") to its actual pointer value.  The
 * interface is needed to facilitate testing Windows extensions that have been
 * converted to only use pointer valules.
 */
RexxMethod1(POINTER, dlgutil_handleToPointer_cls, POINTERSTRING, handle)
{
    return handle;
}

/** DlgUtil::threadID()  [class method]
 *
 *  Simple method to use for testing.
 */
RexxMethod0(uint32_t, dlgutil_threadID_cls)
{
    return GetCurrentThreadId();
}

/** DlgUtil::test()  [class method]
 *
 *  Simple method to use for testing.
 */
RexxMethod3(uint32_t, dlgutil_test_cls, POINTERSTRING, hwnd, POINTERSTRING, hwndBehind, RexxObjectPtr, pos)
{
    POINT *pt = rxGetPoint(context, pos, 3);
    if ( ! SetWindowPos((HWND)hwnd, (HWND)hwndBehind, pt->x, pt->y, 0, 0, SWP_NOSIZE) )
    {
        return GetLastError();
    }
    return 0;
}

/** ListBox::setTabulators()
 *  PlainBaseDialog::setListTabulators()
 *  CategoryDialog::setCategoryListTabulators()
 *
 *  Sets the tab stop positions in a list-box.
 *
 *  This is generic implementation used by several different classes.  The
 *  resourceID and categoryId arguments are not always present.
 *
 *  @param resourceID  The resource ID (may be symbolic) of the list-box.
 *
 *  @param tabstop     The tab stop position.  This argument may repeat any
 *                     number of times.  Each argument is the next succesive tab
 *                     stop.  See the notes below for a fuller explanation.
 *
 *  @param categoryID  For a CategoryDialog, the catalog page that contains the
 *                     ListBox.
 *
 *  @return 0 on success, -1 for an invalid resource ID, and 1 for an API
 *          failure.
 *
 *  @note  The tab stop units are dialog template units. The tab stops must be
 *         listed in ascending order. You can't place a tab stop behind a
 *         previous tab stop.
 *
 *         If no tab stop is specified, than that signals the list-box to place
 *         tab stops equidistant at the default of 2 dialog units.  If 1 tab
 *         stop is specified, then equidistant tab stops are placed at the
 *         distance specified.  Othewise, a tab stop is placed at each position
 *         specified.
 */
RexxMethod2(int32_t, generic_setListTabulators, ARGLIST, args, OSELF, self)
{
    HWND hControl = NULL;
    int  rc = -1;
    uint32_t id;
    uint32_t *tabs = NULL;
    oodClass_t objects[] = {oodCategoryDialog, oodPlainBaseDialog, oodListBox};

    size_t count = context->ArrayItems((RexxArrayObject) args);
    size_t tabStart = 1;

    // Determine which object has invoked this method and parse the argument
    // list.  The object class determines how to get the handle to the listbox,
    // the count of tab stops, and at which arg position the tab stops start.
    switch ( oodClass(context, self, objects, sizeof(objects) / sizeof(oodClass_t))  )
    {
        case oodListBox :
        {
            hControl = controlToHCtrl(context, self);
        } break;

        case oodPlainBaseDialog :
        {
            if ( count < 1 )
            {
                missingArgException(context->threadContext, 1);
                goto done_out;
            }

            RexxObjectPtr resourceID = context->ArrayAt(args, 1);
            if ( ! oodSafeResolveID(&id, context, self, resourceID, -1, 1) )
            {
                goto done_out;
            }

            pCPlainBaseDialog pcpbd = dlgToCSelf(context, self);
            hControl = GetDlgItem(pcpbd->hDlg, (int)id);
            tabStart = 2;
            count--;

        } break;

        case oodCategoryDialog :
        {
            if ( count < 2 )
            {
                missingArgException(context->threadContext, (count == 1 ? 2 : 1));
                goto done_out;
            }

            RexxObjectPtr resourceID = context->ArrayAt(args, 1);
            if ( ! oodSafeResolveID(&id, context, self, resourceID, -1, 1) )
            {
                goto done_out;
            }

            // CatagoryDialogs have this basic construct to hold the dialog
            // handles for each page:
            //  catalogDialog~catalog['handles'][categoryID] == hwndDialog

            RexxDirectoryObject catalog = (RexxDirectoryObject)context->SendMessage0(self, "CATALOG");
            if ( catalog == NULLOBJECT )
            {
                ooDialogInternalException(context, __FUNCTION__, __LINE__, __DATE__, __FILE__);
                goto done_out;
            }

            RexxArrayObject handles = (RexxArrayObject)context->DirectoryAt(catalog, "handles");
            if ( handles == NULLOBJECT || ! context->IsArray(handles) )
            {
                ooDialogInternalException(context, __FUNCTION__, __LINE__, __DATE__, __FILE__);
                goto done_out;
            }
            RexxObjectPtr categoryID = context->ArrayAt(args, count);
            RexxObjectPtr rxHwnd = context->SendMessage1(handles, "AT", categoryID);
            if ( context->CheckCondition() )
            {
                goto done_out;
            }

            // From here on out, we might get NULL for window handles.  We just
            // ignore that and let LB_SETTABSTOPS fail;
            HWND hwnd = (HWND)string2pointer(context->ObjectToStringValue(rxHwnd));

            hControl = GetDlgItem(hwnd, (int)id);
            tabStart = 2;
            count -= 2;

        } break;

        default :
            ooDialogInternalException(context, __FUNCTION__, __LINE__, __DATE__, __FILE__);
            goto done_out;
            break;
    }

    if ( count > 0 )
    {
        tabs = (uint32_t *)malloc(sizeof(uint32_t *) * count);
        if ( tabs == NULL )
        {
            outOfMemoryException(context->threadContext);
            goto done_out;
        }

        uint32_t *p = tabs;
        for ( size_t i = 0; i < count; i++, p++, tabStart++ )
        {
            RexxObjectPtr tab = context->ArrayAt(args, tabStart);
            if ( tab == NULLOBJECT )
            {
                missingArgException(context->threadContext, tabStart);
                goto done_out;
            }
            if ( ! context->ObjectToUnsignedInt32(tab, p) )
            {
                notPositiveArgException(context->threadContext, tabStart, tab);
                goto done_out;
            }
        }
    }

    // LB_SETTABSTOPS returns true on success, otherwise false.  Reverse the
    // return so that 0 is returned for success and 1 for failure.
    rc = (SendMessage(hControl, LB_SETTABSTOPS, (WPARAM)count, (LPARAM)tabs) == 0);

done_out:
    safeFree(tabs);
    return rc;
}


/**
 *  Methods for the .OS class.
 */
#define OS_CLASS        "OS"

bool _is32on64Bit(void)
{
    if ( _isAtLeastXP() )
    {
        BOOL isWow64 = FALSE;
        typedef BOOL (WINAPI *PFNISWOW)(HANDLE, PBOOL);

        PFNISWOW fnIsWow64Process = (PFNISWOW)GetProcAddress(GetModuleHandle("kernel32"), "IsWow64Process");

        if ( fnIsWow64Process != NULL)
        {
            if ( fnIsWow64Process(GetCurrentProcess(), &isWow64) && isWow64 )
            {
                return true;
            }
        }
    }
    return false;
}

bool _isVersion(DWORD major, DWORD minor, unsigned int sp, unsigned int type, unsigned int condition)
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
        return true;
    else
        return false;
}


RexxMethod0(RexxObjectPtr, os_is64bit)
{
    return (_is64Bit() ? TheTrueObj : TheFalseObj);
}

RexxMethod0(RexxObjectPtr, os_is32on64bit)
{
    return (_is32on64Bit() ? TheTrueObj : TheFalseObj);
}

RexxMethod1(RexxObjectPtr, os_isVersion, NAME, method)
{
    bool isVersion = false;
    CSTRING p;

    if ( method[2] == 'A' )
    {
        p = method + 9;
        if (      strcmp(p, "W2K") == 0      ) isVersion = _isAtLeastW2K();
        else if ( strcmp(p, "XP") == 0       ) isVersion = _isAtLeastXP();
        else if ( strcmp(p, "W2K3") == 0     ) isVersion = _isAtLeastW2K3();
        else if ( strcmp(p, "VISTA") == 0    ) isVersion = _isAtLeastVista();
        else if ( strcmp(p, "WINDOWS7") == 0 ) isVersion = _isAtLeastWindows7();
    }
    else
    {
        p = method + 2;
        if (      strcmp(p, "W2K") == 0          ) isVersion = _isW2K();
        else if ( strcmp(p, "XP") == 0           ) isVersion = _isXP();
        else if ( strcmp(p, "XP32") == 0         ) isVersion = _isXP32();
        else if ( strcmp(p, "XP64") == 0         ) isVersion = _isXP64();
        else if ( strcmp(p, "W2K3") == 0         ) isVersion = _isW2K3();
        else if ( strcmp(p, "VISTA") == 0        ) isVersion = _isVista();
        else if ( strcmp(p, "SERVER2008") == 0   ) isVersion = _isServer2008();
        else if ( strcmp(p, "WINDOWS7") == 0     ) isVersion = _isWindows7();
        else if ( strcmp(p, "SERVER2008R2") == 0 ) isVersion = _isServer2008R2();
    }

    return (isVersion ? TheTrueObj : TheFalseObj);
}


/**
 *  Methods for the .ResourceUtils mixin class.
 */
#define RESOURCEUTILS_CLASS        "ResourceUtils"

RexxMethod2(int32_t, rsrcUtils_checkID, RexxObjectPtr, rxID, OSELF, self)
{
    return checkID(context, rxID, self);
}

RexxMethod1(int32_t, rsrcUtils_idError, RexxObjectPtr, rxID)
{
    return idError(context, rxID);
}

RexxMethod2(int32_t, rsrcUtils_resolveResourceID, RexxObjectPtr, rxID, OSELF, self)
{
    return resolveResourceID(context, rxID, self);
}

RexxMethod2(int32_t, rsrcUtils_resolveIconID_pvt, RexxObjectPtr, rxID, OSELF, self)
{
    return resolveIconID(context, rxID, self);
}

/**
 *  Methods for the .Window class.
 */
#define WINDOW_CLASS  "Window"

RexxMethod2(RexxObjectPtr, window_init, POINTERSTRING, hwnd, OSELF, self)
{
    if ( !IsWindow((HWND)hwnd) )
    {
        invalidTypeException(context->threadContext, 1, " window handle");
    }
    else
    {
        initWindowBase(context, (HWND)hwnd, self, NULL);
    }
    return NULLOBJECT;
}

/** Window::unInit()
 *
 *  Release the global reference for CWindowBase::rexxHwnd.
 *
 */
RexxMethod1(RexxObjectPtr, window_unInit, CSELF, pCSelf)
{
    if ( pCSelf != NULLOBJECT )
    {
        pCWindowBase pcwb = (pCWindowBase)pCSelf;
        if ( pcwb->rexxHwnd != TheZeroObj )
        {
            context->ReleaseGlobalReference(pcwb->rexxHwnd);
            pcwb->rexxHwnd = TheZeroObj;
        }
    }
    return NULLOBJECT;
}


/**
 * Methods for the ooDialog .Point class.
 */
#define POINT_CLASS  "Point"


RexxMethod2(RexxObjectPtr, point_init, OPTIONAL_int32_t,  x, OPTIONAL_int32_t, y)
{
    RexxBufferObject obj = context->NewBuffer(sizeof(POINT));
    context->SetObjectVariable("CSELF", obj);

    POINT *p = (POINT *)context->BufferData(obj);

    p->x = argumentExists(1) ? x : 0;
    p->y = argumentExists(2) ? y : p->x;

    return NULLOBJECT;
}

RexxMethod1(int32_t, point_x, CSELF, p) { return ((POINT *)p)->x; }
RexxMethod1(int32_t, point_y, CSELF, p) { return ((POINT *)p)->y; }
RexxMethod2(RexxObjectPtr, point_setX, CSELF, p, int32_t, x) { ((POINT *)p)->x = x; return NULLOBJECT; }
RexxMethod2(RexxObjectPtr, point_setY, CSELF, p, int32_t, y) { ((POINT *)p)->y = y; return NULLOBJECT; }

/**
 * Methods for the ooDialog .Size class.
 */
#define SIZE_CLASS  "Size"

RexxMethod1(RexxObjectPtr, size_init_cls, OSELF, self)
{
    if ( isOfClassType(context, self, SIZE_CLASS) )
    {
        TheSizeClass = (RexxClassObject)self;
        context->RequestGlobalReference(TheSizeClass);
    }
    return NULLOBJECT;
}

RexxMethod2(RexxObjectPtr, size_init, OPTIONAL_int32_t, cx, OPTIONAL_int32_t, cy)
{
    RexxBufferObject obj = context->NewBuffer(sizeof(SIZE));
    context->SetObjectVariable("CSELF", obj);

    SIZE *s = (SIZE *)context->BufferData(obj);

    s->cx = argumentExists(1) ? cx : 0;
    s->cy = argumentExists(2) ? cy : s->cx;

    return NULLOBJECT;
}

RexxMethod1(int32_t, size_cx, CSELF, s) { return ((SIZE *)s)->cx; }
RexxMethod1(int32_t, size_cy, CSELF, s) { return ((SIZE *)s)->cy; }
RexxMethod2(RexxObjectPtr, size_setCX, CSELF, s, int32_t, cx) { ((SIZE *)s)->cx = cx; return NULLOBJECT; }
RexxMethod2(RexxObjectPtr, size_setCY, CSELF, s, int32_t, cy) { ((SIZE *)s)->cy = cy; return NULLOBJECT; }

/**
 * Methods for the ooDialog .Rect class.
 */
#define RECT_CLASS  "Rect"

RexxMethod4(RexxObjectPtr, rect_init, OPTIONAL_int32_t, left, OPTIONAL_int32_t, top,
            OPTIONAL_int32_t, right, OPTIONAL_int32_t, bottom)
{
    RexxBufferObject obj = context->NewBuffer(sizeof(RECT));
    context->SetObjectVariable("CSELF", obj);

    RECT *r = (RECT *)context->BufferData(obj);

    r->left = argumentExists(1) ? left : 0;
    r->top = argumentExists(2) ? top : r->left;
    r->right = argumentExists(3) ? right : r->left;
    r->bottom = argumentExists(4) ? bottom : r->left;

    return NULLOBJECT;
}

RexxMethod1(int32_t, rect_left, CSELF, pRect) { return ((RECT *)pRect)->left; }
RexxMethod1(int32_t, rect_top, CSELF, pRect) { return ((RECT *)pRect)->top; }
RexxMethod1(int32_t, rect_right, CSELF, pRect) { return ((RECT *)pRect)->right; }
RexxMethod1(int32_t, rect_bottom, CSELF, pRect) { return ((RECT *)pRect)->bottom; }
RexxMethod2(RexxObjectPtr, rect_setLeft, CSELF, pRect, int32_t, left) { ((RECT *)pRect)->left = left; return NULLOBJECT; }
RexxMethod2(RexxObjectPtr, rect_setTop, CSELF, pRect, int32_t, top) { ((RECT *)pRect)->top = top; return NULLOBJECT; }
RexxMethod2(RexxObjectPtr, rect_setRight, CSELF, pRect, int32_t, right) { ((RECT *)pRect)->right = right; return NULLOBJECT; }
RexxMethod2(RexxObjectPtr, rect_setBottom, CSELF, pRect, int32_t, bottom) { ((RECT *)pRect)->bottom = bottom; return NULLOBJECT; }



typedef struct _dayState
{
    uint32_t  val;
} DAYSTATE, *PDAYSTATE;


/**
 * Methods for the ooDialog .DayState class.
 */
#define DAYSTATE_CLASE  "DayState"

RexxMethod1(RexxObjectPtr, ds_init, ARGLIST, args)
{
    RexxBufferObject obj = context->NewBuffer(sizeof(DAYSTATE));
    context->SetObjectVariable("CSELF", obj);

    PDAYSTATE dayState = (PDAYSTATE)context->BufferData(obj);
    uint32_t val = 0;
    uint32_t day;

    size_t count = context->ArraySize(args);
    for ( size_t i = 1; i <= count; i++ )
    {
        RexxObjectPtr _day = context->ArrayAt(args, i);
        if ( ! context->UnsignedInt32(_day, &day) )
        {
            notNonNegativeException(context->threadContext, i, _day);
        }
        if ( day < 1 || day > 31 )
        {
            wrongRangeException(context->threadContext, i, 1, 31, _day);
        }
        val |= (0x00000001 << (day - 1));
    }
    dayState->val = val;

    return NULLOBJECT;
}

RexxMethod1(uint32_t, ds_dayStateValue, CSELF, ds) { return ((PDAYSTATE)ds)->val; }


/**
 * Methods for the ooDialog .DayStates class.
 */
#define DAYSTATES_CLASE  "DayStates"


RexxObjectPtr makeDayStateBuffer(RexxMethodContext *c, RexxArrayObject list, size_t count, LPMONTHDAYSTATE *ppmds)
{
    RexxBufferObject _mds = c->NewBuffer(count * sizeof(MONTHDAYSTATE));
    if ( _mds == NULLOBJECT )
    {
        return TheFalseObj;
    }

    MONTHDAYSTATE *pmds = (MONTHDAYSTATE *)c->BufferData(_mds);
    if ( ppmds != NULL )
    {
        *ppmds = pmds;
    }

    RexxObjectPtr  rxMDSVal;
    PDAYSTATE      pDayState;

    for ( size_t i = 1; i <= count; i++, pmds++ )
    {
        rxMDSVal = c->ArrayAt(list, i);
        if ( rxMDSVal == NULLOBJECT || ! c->IsOfType(rxMDSVal, "DAYSTATE") )
        {
            wrongObjInArrayException(c->threadContext, 1, i, "a DayState object");
            return TheFalseObj;
        }

        pDayState = (PDAYSTATE)c->ObjectToCSelf(rxMDSVal);
        *pmds = pDayState->val;
        printf("makeDayStateBuffer() address pmds=%p pmds val=0x%08x val=0x%08x\n", pmds, *pmds, pDayState->val);
    }

    return _mds;
}

RexxObjectPtr quickDayStateBuffer(RexxMethodContext *c, uint32_t ds1, uint32_t ds2, uint32_t ds3, LPMONTHDAYSTATE *ppmds)
{
    RexxBufferObject _mds = c->NewBuffer(3 * sizeof(MONTHDAYSTATE));
    if ( _mds == NULLOBJECT )
    {
        return TheFalseObj;  // Need to think about this.
    }

    MONTHDAYSTATE *pmds = (MONTHDAYSTATE *)c->BufferData(_mds);
    *pmds = ds1;
    *(pmds + 1) = ds2;
    *(pmds + 2) = ds3;

    if ( ppmds != NULL )
    {
        *ppmds = pmds;
    }
    return _mds;
}

RexxMethod3(RexxObjectPtr, dss_makeDayStateBuffer, RexxArrayObject, list, size_t, count, OSELF, self)
{
    return makeDayStateBuffer(context, list, count, NULL);
}

RexxMethod3(RexxObjectPtr, dss_quickDayStateBuffer, uint32_t, ds1, uint32_t, ds2, uint32_t, ds3)
{
    return quickDayStateBuffer(context, ds1, ds2, ds3, NULL);
}


