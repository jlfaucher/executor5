/*----------------------------------------------------------------------------*/
/*                                                                            */
/* Copyright (c) 1995, 2004 IBM Corporation. All rights reserved.             */
/* Copyright (c) 2005-2011 Rexx Language Association. All rights reserved.    */
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
 * oodPropertySheetDialog.cpp
 *
 * Contains the classes used to implement a Property Sheet dialog, (as oppossed
 * to the PropertySheet.cls which is not a Windows property sheet at all.)
 */
#include "ooDialog.hpp"     // Must be first, includes windows.h, commctrl.h, and oorexxapi.h

#include <stdio.h>
#include <dlgs.h>
#include <shlwapi.h>
#include "APICommon.hpp"
#include "oodCommon.hpp"
#include "oodMessaging.hpp"
#include "oodUser.hpp"
#include "oodDeviceGraphics.hpp"
#include "oodData.hpp"
#include "oodResources.hpp"
#include "oodResourceIDs.hpp"
#include "oodControl.hpp"
#include "oodPropertySheetDialog.hpp"


PROPSHEETHOOKDATA *PropSheetHookData[MAX_PROPSHEET_DIALOGS];
size_t CountPropSheetHooks = 0;


class PropSheetThreadArgs
{
public:
    pCPropertySheetDialog  pcpsd;
    PROPSHEETHEADER       *psh;
    bool                  *release; // Used to signal thread initialization complete
};

#define VALID_PSNRET_LIST       "PSNRET_NOERROR, PSNRET_INVALID, or PSNRET_INVALID_NOCHANGEPAGE"
#define VALID_PSNRET_MSG_LIST   "PSNRET_NOERROR or PSNRET_MESSAGEHANDLED"
#define VALID_PROPSHEET_BUTTONS  "APPLYNOW, BACK, CANCEL, FINISH, HELP, NEXT, or OK"

bool psnCheckForCondition(RexxThreadContext *c, pCPropertySheetDialog pcpsd)
{
    if ( checkForCondition(c, true) )
    {
        endDialogPremature(pcpsd->pcpbd, pcpsd->hDlg, RexxConditionRaised);
        return true;
    }
    return false;
}

bool goodReply(RexxThreadContext *c, pCPropertySheetDialog pcpsd, RexxObjectPtr r, CSTRING method)
{
    if ( ! psnCheckForCondition(c, pcpsd) )
    {
        if ( r == NULLOBJECT )
        {
            c->RaiseException1(Rexx_Error_No_result_object_message, c->String(method));
            psnCheckForCondition(c, pcpsd);
            return false;
        }
        return true;
    }
    return false;
}

inline void psnMemoryErr(RexxThreadContext *c, pCPropertySheetDialog pcpsd)
{
    outOfMemoryException(c);
    psnCheckForCondition(c, pcpsd);
}


/**
 *  98.900
 *  Error 98 - Execution error
 *        The language processor detected a specific error during execution.
 *
 *  The return from method "name"() must a whole number from "min" to "max";
 *  found "value"
 *
 *  The return from method setActive() must be a whole number from -1 to
 *  2,147,483,647; found an Array
 *
 *  The exception is raised, printed, and the dialog is ended.
 */
void wrongPSNRangeException(RexxThreadContext *c, CSTRING method, int32_t min, int32_t max, RexxObjectPtr actual,
                            pCPropertySheetDialog pcpsd)
{
    TCHAR buf[256];
    _snprintf(buf, sizeof(buf), "The return from method %s() must be a whole number from %d to %d; found %s",
              method, min, max, c->ObjectToStringValue(actual));

    c->RaiseException1(Rexx_Error_Execution_user_defined, c->String(buf));
    psnCheckForCondition(c, pcpsd);
}


/**
 *  98.900
 *  Error 98 - Execution error
 *        The language processor detected a specific error during execution.
 *
 *  The return from method "name"() must one of "a list"; found "value"
 *
 *  The return from method killActive() must be one of PSNRET_NOERROR,
 *  PSNRET_INVALID, or PSNRET_INVALID_NOCHANGEPAGE; found 15
 *
 *  The exception is raised, printed, and the dialog is ended.
 */
void invalidPSNReturnListException(RexxThreadContext *c, CSTRING method, CSTRING list, RexxObjectPtr actual,
                                   pCPropertySheetDialog pcpsd)
{
    TCHAR buf[256];
    _snprintf(buf, sizeof(buf), "The return from method %s() must be one of %s; found %s",
              method, list, c->ObjectToStringValue(actual));

    c->RaiseException1(Rexx_Error_Execution_user_defined, c->String(buf));
    psnCheckForCondition(c, pcpsd);
}


RexxStringObject wm2string(RexxThreadContext *c, uint32_t wmMsg)
{
    char *str = "";
    switch ( wmMsg )
    {
        case WM_KEYDOWN :
            str = "KEYDOWN";
            break;
        case WM_KEYUP       :
            str = "KEYUP";
            break;
        case WM_CHAR        :
            str = "CHAR";
            break;
        case WM_DEADCHAR    :
            str = "DEADCHAR";
            break;
        case WM_SYSKEYDOWN  :
            str = "SYSKEYDOWN";
            break;
        case WM_SYSKEYUP    :
            str = "SYSKEYUP";
            break;
        case WM_SYSCHAR     :
            str = "SYSCHAR";
            break;
        case WM_SYSDEADCHAR :
            str = "SYSDEADCHAR";
            break;
    }
    return c->String(str);
}

/**
 * Creates a Rexx argument array for the property sheet PSN_TRANSLATEACCELERATOR
 * notification.
 *
 * @param c       Thread context we are operating in.
 * @param wmMsg   The windows message (WM_xxx).
 * @param wParam  The WPARAM for the message, the charater code for the key.
 * @param lParam  The LPARAM for the message, contains state info.
 *
 * @return An array of args.
 *
 * @remarks
 */
RexxArrayObject getTranslateAccelatorArgs(RexxThreadContext *c, uint32_t _wmMsg, WPARAM wParam, LPARAM lParam,
                                          RexxObjectPtr propSheet)
{
    RexxStringObject wmMsg = wm2string(c, _wmMsg);
    RexxObjectPtr    keyCode = c->WholeNumber(wParam);

    BOOL released   = (lParam & KEY_RELEASED)   ? 1 : 0;
    BOOL wasDown    = (lParam & KEY_WASDOWN)    ? 1 : 0;
    BOOL isExtended = (lParam & KEY_ISEXTENDED) ? 1 : 0;
    BOOL altHeld    = (lParam & KEY_ALTHELD)    ? 1 : 0;

    BOOL shiftHeld   = (GetAsyncKeyState(VK_SHIFT) & ISDOWN)   ? 1 : 0;
    BOOL controlHeld = (GetAsyncKeyState(VK_CONTROL) & ISDOWN) ? 1 : 0;

    RexxDirectoryObject d = (RexxDirectoryObject)rxNewBuiltinObject(c, "DIRECTORY");

    c->DirectoryPut(d, c->Logical(released),    "RELEASED");
    c->DirectoryPut(d, c->Logical(wasDown),     "WASDOWN");
    c->DirectoryPut(d, c->Logical(isExtended),  "ISEXTENDED");
    c->DirectoryPut(d, c->Logical(altHeld),     "ALTHELD");
    c->DirectoryPut(d, c->Logical(shiftHeld),   "SHIFTHELD");
    c->DirectoryPut(d, c->Logical(controlHeld), "CONTROLHELD");

    RexxArrayObject args = c->ArrayOfFour(wmMsg, keyCode, d, propSheet);

    return args;
}


/**
 * Translates the Rexx reply for a PSN_SETACTIVE notification to the Windows
 * equivalent.
 *
 * Note that the replies to PSN_SETACTIVE, PSN_WIZBACK, and PSN_WIZNEXT are all
 * handled by this function.
 *
 * The Rexx programmer returns 0 to allow the page to become active (or to go to
 * the previous page for WIZBACK, or to go to the next page for WIZNEXT,) -1 to
 * prevent a page change, and the page index, or page ID, to go to a specific
 * page.  The Windows property sheet actually uses the page ID.
 *
 * The Rexx programmer can get the proper page ID by using the indexToID()
 * method.  That method returns a .Pointer object.  Otherwise, it seems simplier
 * for the Rexx programmet to simply use the page index and we'll look it up
 * here.  The short of it is, the Rexx progammer can either reply using the page
 * ID, in which case result is a Pointer object.  Or they can simple use the
 * page index.
 *
 * @param c
 * @param pcpsd
 * @param result
 * @param name
 *
 * @return INT_PRT
 */
INT_PTR getSetActiveValue(RexxThreadContext *c, pCPropertySheetDialog pcpsd, RexxObjectPtr result, CSTRING name)
{
    INT_PTR ret = -2;

    if ( c->IsPointer(result) )
    {
        return (INT_PTR)c->PointerValue((RexxPointerObject)result);
    }

    int max = (int)pcpsd->pageCount;

    int32_t index;
    if ( ! c->Int32(result, &index) || (index < -1 || index > max) )
    {
        TCHAR buf[256];
        _snprintf(buf, sizeof(buf), "The return from method %s() must be a whole number from %d to %d; found %s",
                  name, -1, max, c->ObjectToStringValue(result));

        c->RaiseException1(Rexx_Error_Execution_user_defined, c->String(buf));
        return ret;
    }

    if ( index < 1 )
    {
        ret = index;
    }
    else
    {
        pCPropertySheetPage pcpsp = pcpsd->cppPages[index - 1];
        ret = pcpsp->pageID;
    }
    return ret;
}


bool setPropSheetHook(pCPropertySheetDialog pcpsd)
{
    PROPSHEETHOOKDATA *pshd = NULL;
    bool               result = false;
    uint32_t           threadID = 0;

    pshd = (PROPSHEETHOOKDATA *)LocalAlloc(LPTR, sizeof(PROPSHEETHOOKDATA));
    if ( pshd != NULL )
    {
        pshd->pcpsd = pcpsd;
        SetLastError(0);

        EnterCriticalSection(&ps_crit_sec);

        if ( CountPropSheetHooks < MAX_PROPSHEET_DIALOGS )
        {
            threadID = GetCurrentThreadId();
            pshd->hHook = SetWindowsHookEx(WH_CBT, (HOOKPROC)PropSheetCBTProc, (HINSTANCE)NULL, GetCurrentThreadId());

            if ( pshd->hHook != NULL )
            {
                pshd->threadID = threadID;
                PropSheetHookData[CountPropSheetHooks] = pshd;
                CountPropSheetHooks++;
                result = true;
            }
        }

        LeaveCriticalSection(&ps_crit_sec);

        if ( result == true )
        {
            goto done_out;
        }
    }

    // Only here if we had an error, figure out which one.
    if ( pshd == NULL )
    {
        outOfMemoryException(pcpsd->dlgProcContext);
    }
    else if ( threadID == 0 )
    {
        userDefinedMsgException(pcpsd->dlgProcContext, TOO_MANY_PROPSHEET_DIALOGS, MAX_PROPSHEET_DIALOGS);
    }
    else
    {
        systemServiceExceptionCode(pcpsd->dlgProcContext, API_FAILED_MSG, "SetWindowsHookEx", GetLastError());
    }

    safeLocalFree(pshd);

done_out:
   return result;
}

/**
 * Searches for a match of the property sheet hook data struct.  The match is
 * keyed on the current thread ID and hwnd.
 *
 * This function is called twice.  The first time from PropCBTProc() where the
 * hwnd has not yet been placed into the struct; the match is by thread ID only.
 * If found, the struct is returned and PropSheetCBTProc() adds the hwnd to the
 * struct and unhooks the hook.
 *
 * The second time this function is called, it is from the PropSheetCallback()
 * function and this invocation passes in the hwnd.  If found, when hwnd is not
 * null, the struct is removed from the table and the caller frees its memory.
 *
 * The use of the hwnd and second invocation is because of the AeroWizard style
 * property sheet.  When the CSelf struct is placed in the GWLP_USEDATA index fo
 * the property sheet, it ends up getting corrupted.  Probably because the Aero
 * Wizard uses that index for its own purpose.  But, maybe because the Aero
 * Wizard dialog classs does not seem to be a WC_DIALOG but a NativeHWNDHost.
 *
 * @param hwnd  If hwnd is not null, and a match is found, we are done with the
 *              struct, remove it from the table.
 *
 * @return PROPSHEETHOOKDATA*
 *
 * @remarks.  The caller is responsible for freeing the stuct when the caller
 *            passes in a non-null hwnd.
 */
PROPSHEETHOOKDATA *getPropSheetHookData(HWND hwnd)
{
    register size_t i;
    DWORD id = GetCurrentThreadId();
    PROPSHEETHOOKDATA *pshd = NULL;

    EnterCriticalSection(&ps_crit_sec);

    for ( i = 0; i < CountPropSheetHooks; i++ )
    {
        if ( PropSheetHookData[i]->threadID == id && PropSheetHookData[i]->hPropSheet == hwnd )
        {
            pshd = PropSheetHookData[i];
            break;
        }
    }

    // It seems almost impossible that pshd could be null, but there is nothing
    // to do about it if it is.
    if ( pshd != NULL && hwnd != NULL )
    {
        // If the slot found is the last, just set it to null.
        if ( i == CountPropSheetHooks - 1 )
        {
            PropSheetHookData[i] = NULL;
        }
        else
        {
            // Since it is not the last, i + 1 must be valid memory.
            memcpy(&PropSheetHookData[i], &PropSheetHookData[i + 1], (CountPropSheetHooks - i - 1) * sizeof(PROPSHEETHOOKDATA *));
            PropSheetHookData[CountPropSheetHooks - 1] = NULL;
        }
        CountPropSheetHooks--;
    }

    LeaveCriticalSection(&ps_crit_sec);

    return pshd;
}


/**
 * Common code for getting the value of a Rexx argument where the argument can
 * be an Image object or a resource ID.
 *
 * @param c
 * @param image
 * @param argPos
 * @param isImage
 * @param type
 *
 * @return The handle to the image or the resource ID.
 */
INT_PTR getImageOrID(RexxMethodContext *c, RexxObjectPtr self, RexxObjectPtr image, size_t argPos,
                     bool *isImage, uint8_t *type)
{
    INT_PTR result = 0;

    if ( c->IsOfType(image, "IMAGE") )
    {
        POODIMAGE oodImage = rxGetOodImage(c, image, argPos);
        if ( oodImage != NULL )
        {
            *isImage = true;
            *type = (uint8_t)oodImage->type;
            result = (INT_PTR)oodImage->hImage;
        }
    }
    else
    {
        int32_t id = oodResolveSymbolicID(c->threadContext, self, image, -1, argPos, true);

        if ( id == OOD_ID_EXCEPTION )
        {
            if ( ! isOutOfMemoryException(c->threadContext) )
            {
                // We want our own wording for the exception.
                c->ClearCondition();
                wrongArgValueException(c->threadContext, 1, "a postive numeric ID, valid symbolic ID, or .Image object", image);
            }
        }
        else
        {
            *isImage = false;
            result = id;
        }
    }
    return result;
}


/**
 * A generic function used to set the title, or header titel, or header subtitle
 * text for a property sheet page.
 *
 * These text strings can be set before the property sheet is created.  But they
 * can also be changed after the property sheet begins execution.  Because of
 * this, the function is called from both a property sheet method context and a
 * property sheet page context.
 *
 * When replacing already existing text, the old text needs to be freed.  During
 * delDialog for a property sheet page, the existing text is freed.
 *
 * @param c
 * @param pcpsp
 * @param text
 * @param part
 *
 * @return bool
 */
bool setPageText(RexxMethodContext *c, pCPropertySheetPage pcpsp, CSTRING text, pagePart_t part)
{
    char *t = (char *)LocalAlloc(LPTR, strlen(text) + 1);
    if ( t == NULL )
    {
        outOfMemoryException(c->threadContext);
        return false;
    }

    strcpy(t, text);

    switch ( part )
    {
        case headerSubtext :
            safeLocalFree(pcpsp->headerSubTitle);
            pcpsp->headerSubTitle = t;
            break;
        case headerText :
            safeLocalFree(pcpsp->headerTitle);
            pcpsp->headerTitle = t;
            break;
        case pageText :
            safeLocalFree(pcpsp->pageTitle);
            pcpsp->pageTitle = t;
            break;
    }

    return true;
}


/**
 * Called from the property sheet callback function when signaled that the
 * property sheet dialog is about to be created.  At this point, the in-memory
 * dialog template can be accessed.
 *
 * This allows us to alter the template.  The one practical use of this, would
 * be to change the font of the property sheet.
 *
 * Note this is just temp code that shows it can actually be done.  Because the
 * dialog template items are variable in size, we need to calculate the
 * beginning and end of memory used by the dialog items, change the font type
 * name and then move the dialog items to the correct place.
 *
 * @param tmplate
 */
void adjustPropSheetTemplate(LPARAM tmplate, bool quickReturn)
{
    if ( quickReturn )
    {
        return;
    }

    DLGTEMPLATE *pDlgTemplate;
    DLGTEMPLATEEX *pDlgTemplateEx;
    bool hasFontInfo;

    pDlgTemplateEx = (DLGTEMPLATEEX *)tmplate;
    if (pDlgTemplateEx->signature == 0xFFFF)
    {
        hasFontInfo = (pDlgTemplateEx->style & DS_SHELLFONT) || (pDlgTemplateEx->style & DS_SETFONT);

        printf("PropertySheet, extended template. Has font info=%d\n", hasFontInfo);
    }
    else
    {
           pDlgTemplate = (DLGTEMPLATE *)tmplate;
           hasFontInfo = (pDlgTemplate->style & DS_SHELLFONT) || (pDlgTemplate->style & DS_SETFONT);

           printf("PropertySheet, regular template. Has font info=%d\n", hasFontInfo);

           WORD *pStruct = (WORD *)tmplate;
           printf(" count items=%d x=%d y=%d cx=%d cy=%d menu=%d class=%d titleWord=%d\n",
                  pStruct[4], pStruct[5], pStruct[6], pStruct[7], pStruct[8],
                  pStruct[9], pStruct[10], pStruct[11]);


           WCHAR *wstr = (WCHAR *)(pStruct + 13);
           printf("  pointSize=%d ", pStruct[12]);
           wprintf(L"Font=%s\n", wstr);

           // Temp, change the font size and font name.  Really temp.  Font name
           // has to be exactly same number of characters as MS Shell Dlg.
           //
           // Arial Italic
           // Book Antiqua
           // Cooper Black
           // Poor Richard
           *(pStruct + 12) = 12;
           putUnicodeText((LPWORD)wstr, "Arial Italic");
    }

}


/**
 * Called from the property sheet callback function when signaled that the
 * property sheet dialog is being initialized.  Performs the initialization
 * normally done in the window loop thread function and the execute() method for
 * normal ooDialog dialogs.
 *
 * @param hPropSheet  Window handle of the property sheet.
 */
static void initializePropSheet(HWND hPropSheet)
{
    PROPSHEETHOOKDATA *pshd = getPropSheetHookData(hPropSheet);

    if ( pshd != NULL )
    {
        pCPropertySheetDialog pcpsd = pshd->pcpsd;
        RexxThreadContext *c = pcpsd->dlgProcContext;
        pCPlainBaseDialog pcpbd = pcpsd->pcpbd;

        LocalFree(pshd);

        pcpbd->hDlg = hPropSheet;

        // Not sure about using the whole top dialog thing for property sheets.
        installNecessaryStuff(pcpbd, NULL);

        pcpbd->hDlg = hPropSheet;
        pcpbd->isActive = true;
        pcpbd->childDlg[0] = hPropSheet;

        setDlgHandle(c, pcpbd);
        setFontAttrib(c, pcpbd);

        pcpbd->onTheTop = true;
        pcpbd->threadID = GetCurrentThreadId();

        // Do we have a modal dialog?  TODO need to check this for modeless property sheet.
        checkModal((pCPlainBaseDialog)pcpbd->previous, pcpsd->modeless);

        c->SendMessage0(pcpsd->rexxSelf, "INITDIALOG");
    }
}


/**
 * Called from WM_INITDIALOG for a property sheet page.  Performs the
 * initialization for the dialog that is usually done in the window loop thread
 * furnction and the execute() method for regular ooDialog dialogs.
 *
 * @param hPage
 * @param pcpsp
 *
 * @remarks  Note that we pass NULL into doDataAutoDetection() here because it
 *           requires a method context rather than a thread context, which we
 *           don't have.  However, the context is only used to raise an out of
 *           memory exception, so we simply check for that condition and raise
 *           the excepion here.
 */
static void initializePropSheetPage(HWND hPage, pCPropertySheetPage pcpsp)
{
    pCPlainBaseDialog pcpbd = pcpsp->pcpbd;
    RexxThreadContext *c = pcpsp->dlgProcContext;

    setWindowPtr(hPage, GWLP_USERDATA, (LONG_PTR)pcpsp);

    pcpsp->hPage = hPage;
    pcpsp->activated = true;

    pcpbd->hDlg = hPage;
    pcpbd->isActive = true;
    pcpbd->childDlg[0] = hPage;

    setDlgHandle(c, pcpbd);
    if ( pcpsp->pageType == oodResPSPDialog )
    {
        setFontAttrib(c, pcpbd);

        if ( pcpbd->autoDetect )
        {
            if ( doDataAutoDetection(NULL, pcpbd) == OOD_MEMORY_ERR )
            {
                outOfMemoryException(c);
                return;
            }
        }
    }

    c->SendMessage0(pcpsp->rexxSelf, "Execute");
}

/**
 * Attempts to match the window being created in a CBTProc hook function as a
 * property sheet window
 *
 * @param cs  Pointer to the window create struct.
 *
 * @return True for a match, otherwise false.
 *
 * @remarks  Tough to be sure we get the right window in the CBTProc function.
 *           But, for our use, I think it is impossible to create two windows in
 *           the same thread without the property sheet window being first.
 *
 *           Trying to match the Windows styles for all different types of
 *           property sheets, including AeroWizards, is not working well.  For
 *           now, just assume if it is a dialog or a NativeHWNDHost on Vista or
 *           later, that we are okay.
 */
inline bool isPropSheetMatch(LPCREATESTRUCT cs)
{
#if 0
    uint32_t styleMask = WS_POPUP | WS_CAPTION | WS_SYSMENU | 0x000000c4;
    uint32_t exStyleMask = WS_EX_CONTROLPARENT | WS_EX_DLGMODALFRAME;

    printf("Matching styleMask=0x%08x exStyleMask=0x%08x\n", styleMask, exStyleMask);
    printf("Actual       style=0x%08x     exStyle=0x%08x\n", cs->style, cs->dwExStyle);
#endif

    if ( cs->lpszClass == WC_DIALOG || (_isAtLeastVista() && strcmp(cs->lpszClass, "NativeHWNDHost") == 0) )
    {
        return true;
    }
    return false;
}

/**
 * Forces a property sheet dialog to end.  Setting each page's abort flag
 * prevents the page from canceling the close.  We then programmatically press
 * the Cancel key.
 *
 * This was originally intended only for modal propert sheetes.  Usually it is
 * not needed with modeless property sheets as DestroyWindow() works fine,
 * however there is no reason it can not be used with modeless property sheet
 * dialogs.
 *
 * @param pcpsd
 * @param hDlg
 * @param t
 */
void abortPropertySheet(pCPropertySheetDialog pcpsd, HWND hDlg, DlgProcErrType t)
{
    uint32_t count = pcpsd->pageCount;
    for ( uint32_t i = 0; i < count; i++ )
    {
        pcpsd->cppPages[i]->abort = true;
    }

    PropSheet_PressButton(hDlg, PSBTN_CANCEL);
}


/**
 * Filter for property sheet notification messages.
 *
 * @param uMsg    The windowm message id.
 * @param lParam  The LPARAM argument for the message.
 *
 * @return Return true if the message is a property sheet notification message,
 *         otherwise false.
 */
inline bool isPSMsg(uint32_t uMsg, LPARAM lParam)
{
    uint32_t code;
    return (uMsg == PSM_QUERYSIBLINGS) ||
           (uMsg == WM_NOTIFY && ((code = ((NMHDR *)lParam)->code) >= PSN_LAST  && code <= PSN_FIRST));
}


/**
 * The PSN_SETACTIVE, PSN_WIZBACK, and PSN_WIZNEXT notifications all use
 * identical code.
 *
 * The Rexx programmer returns 0 to allow the page to become active (or to go to
 * the previous page for WIZBACK, or to go to the next page for WIZNEXT,) -1 to
 * prevent a page change, and the page index, or page ID, to go to a specific
 * page.  The Windows property sheet actually uses the page ID.
 *
 * The Rexx programmer can get the proper page ID by using the indexToID()
 * method.  That method returns a .Pointer object.  Otherwise, it seems simplier
 * for the Rexx programmet to simply use the page index and we'll look it up
 * here.  The short of it is, the Rexx progammer can either reply using the page
 * ID, in which case result is a Pointer object.  Or they can simple use the
 * page index.
 *
 * @param c
 * @param pcpsd
 * @param hPage
 * @param name
 */
void doSetActiveCommon(RexxThreadContext *c, pCPropertySheetPage pcpsp, HWND hPage, CSTRING name)
{
    pCPropertySheetDialog pcpsd = (pCPropertySheetDialog)pcpsp->cppPropSheet;
    INT_PTR reply = 0;

    RexxObjectPtr result = c->SendMessage1(pcpsp->rexxSelf, name, pcpsd->rexxSelf);

    if ( goodReply(c, pcpsd, result, name) )
    {
        reply = getSetActiveValue(c, pcpsd, result, name);
        psnCheckForCondition(c, pcpsd);
    }
    setWindowPtr(hPage, DWLP_MSGRESULT, (LPARAM)reply);
}


/**
 * Handles the PSN_QUERYINITIALFOCUS notification.
 *
 * The Rexx programmer returns 0 to set the focus to the default control and
 * returns the dialog control resource ID to set that focus to that control.
 * The ID can be numeric or symbolic.
 *
 * The Windows return is 0 to set the focus to the default control and the
 * handle of the dialog control to set the focuse to some control other than the
 * default
 *
 * @param c
 * @param pcpsp
 * @param lParam
 *
 * @remarks  lParam of the PSHNOTIFY struct contains the HWND of the dialog
 *           control that will receive the focus by default.  We use that to get
 *           the resource ID of that control and use that ID as the first
 *           argument sent to Rexx.
 *
 */
void doQueryInitialFocus(RexxThreadContext *c, pCPropertySheetPage pcpsp, LPARAM lParam)
{
    pCPropertySheetDialog pcpsd = (pCPropertySheetDialog)pcpsp->cppPropSheet;

    int32_t id = GetDlgCtrlID((HWND)((LPPSHNOTIFY)lParam)->lParam);

    RexxObjectPtr result = c->SendMessage2(pcpsp->rexxSelf, QUERYINITIALFOCUS_MSG, c->Int32(id), pcpsd->rexxSelf);

    HWND   hPage = pcpsp->hPage;
    LPARAM reply = 0;

    if ( goodReply(c, pcpsd, result, QUERYINITIALFOCUS_MSG) )
    {
        // We need 0 to be okay, but not -1.
        id = oodResolveSymbolicID(c, pcpsp->rexxSelf, result, -1, 1, false);
        if ( id == OOD_ID_EXCEPTION || id == -1 )
        {
            if ( isOutOfMemoryException(c) )
            {
                psnMemoryErr(c, pcpsd);
            }
            else
            {
                invalidPSNReturnListException(c, QUERYINITIALFOCUS_MSG, "0, or a valid resource ID", result, pcpsd);
            }
        }
        else if ( id != 0 )
        {
            reply = (LPARAM)GetDlgItem(hPage, id);
        }
    }
    setWindowPtr(hPage, DWLP_MSGRESULT, reply);
}

/**
 * Handles the PSN_WIZFINISH notification.
 *
 * The Rexx programmer returns 0 to allow the wizard to finish, -1 to prevent
 * the wizard from finishing, or the resource ID of a control on the wizard
 * page.  Sending the resource ID prevents the wizard from finishing and sets
 * the focus on the page to that dialog control.
 *
 * The Windows return is 0 to allow the wizard to finish, true to prevent the
 * wizard from finishing, or the window handle of a dialog control on the page
 * to prevent the finish and set that page as the current page.
 *
 * @param c
 * @param pcpsd
 * @param result
 * @param hPage
 * @param name
 *
 * @remarks
 */
void doWizFinish(RexxThreadContext *c, pCPropertySheetPage pcpsp)
{
    pCPropertySheetDialog pcpsd = (pCPropertySheetDialog)pcpsp->cppPropSheet;

    RexxObjectPtr result = c->SendMessage1(pcpsp->rexxSelf, WIZFINISH_MSG, pcpsd->rexxSelf);

    HWND   hPage = pcpsp->hPage;
    LPARAM reply = 0;

    if ( goodReply(c, pcpsd, result, WIZFINISH_MSG) )
    {
        int32_t id = oodResolveSymbolicID(c, pcpsp->rexxSelf, result, -1, 1, false);
        if ( id == OOD_ID_EXCEPTION )
        {
            if ( isOutOfMemoryException(c) )
            {
                psnMemoryErr(c, pcpsd);
            }
            else
            {
                invalidPSNReturnListException(c, WIZFINISH_MSG, "-1, 0, or a valid resource ID", result, pcpsd);
            }
        }
        else if ( id != 0 )
        {
            reply = (LPARAM)GetDlgItem(hPage, id);
        }
    }
    setWindowPtr(hPage, DWLP_MSGRESULT, reply);
}


int queryFromSibling(RexxThreadContext *c, pCPropertySheetPage pcpsp, WPARAM wParam, LPARAM lParam)
{
    pCPropertySheetDialog pcpsd = (pCPropertySheetDialog)pcpsp->cppPropSheet;
    int                   reply = 0;

    RexxArrayObject args   = c->ArrayOfThree((RexxObjectPtr)wParam, (RexxObjectPtr)lParam, pcpsd->rexxSelf);
    RexxObjectPtr   result = c->SendMessage(pcpsp->rexxSelf, QUERYFROMSIBLING_MSG, args);

    if ( goodReply(c, pcpsd, result, QUERYFROMSIBLING_MSG) )
    {
        if ( ! c->Int32(result, &reply) )
        {
            wrongPSNRangeException(c, QUERYFROMSIBLING_MSG, MININT, MAXINT, result, pcpsd);
        }
    }
    return reply;
}


/**
 * Handler for all the property sheet notification messages and the special case
 * PSM_QUERYSIBLINGS message.
 *
 * Rather than use a connectPropertySheetEvent() method, the property sheet
 * dialog handles every notification.  The dialog class supplies the correct
 * default implementation for each notification.  The Rexx programmer over-rides
 * the default implementation for any notification he wants to handle.
 *
 * The special PSM_QUERYSIBLINGS message is sent to a property sheet and the
 * property sheet then forwards it to each page.  So, the page recieving the
 * message is very similar to receiving a notification.  The sequence is
 * initiated through the PropertySheet::querySiblings() method and replied to
 * through the PropertySheetPage::queryFromSibling() method.
 *
 * @param pcpsp
 * @param pcpbd
 * @param wParam
 * @param lParam
 */
static int doPSMessage(pCPropertySheetPage pcpsp, pCPlainBaseDialog pcpbd, uint32_t msg, WPARAM wParam, LPARAM lParam)
{
    pCPropertySheetDialog pcpsd = (pCPropertySheetDialog)pcpsp->cppPropSheet;
    HWND hPropSheet = pcpsd->hDlg;
    HWND hPage = pcpsp->hPage;
    RexxThreadContext *c = pcpbd->dlgProcContext;

    if ( msg == PSM_QUERYSIBLINGS )
    {
        return queryFromSibling(c, pcpsp, wParam, lParam);
    }

    switch ( ((NMHDR *)lParam)->code )
    {
        case PSN_APPLY :
        {
            LPPSHNOTIFY lppsn        = (LPPSHNOTIFY)lParam;
            uint32_t reply           = PSNRET_NOERROR;
            RexxObjectPtr isOkButton = lppsn->lParam ? TheTrueObj : TheFalseObj;

            RexxObjectPtr result = c->SendMessage2(pcpsp->rexxSelf, APPLY_MSG, isOkButton, pcpsd->rexxSelf);

            if ( goodReply(c, pcpsd, result, APPLY_MSG) )
            {
                if ( ! c->UnsignedInt32(result, &reply) || ! (reply >= PSNRET_NOERROR && reply <= PSNRET_INVALID_NOCHANGEPAGE) )
                {
                    invalidPSNReturnListException(c, APPLY_MSG, VALID_PSNRET_LIST, result, pcpsd);
                }
            }
            setWindowPtr(hPage, DWLP_MSGRESULT, (LPARAM)reply);
            break;
        }

        case PSN_GETOBJECT :
        {
            // I have not been able to produce this notification, so there is no implementation for it.
            if ( pcpsp->wantGetObject )
            {
                printf("No implementation for PSN_GETOBJECT\n");
            }

            break;
        }

        case PSN_HELP :
        {
            RexxObjectPtr result = c->SendMessage1(pcpsp->rexxSelf, HELP_MSG, pcpsd->rexxSelf);
            goodReply(c, pcpsd, result, HELP_MSG);
            break;
        }

        case PSN_KILLACTIVE :
        {
            // Send TRUE to *cancel* the page change.  The Rexx programmer
            // should send .false to cancel.
            long reply = FALSE;

            RexxObjectPtr result = c->SendMessage1(pcpsp->rexxSelf, KILLACTIVE_MSG, pcpsd->rexxSelf);

            if ( goodReply(c, pcpsd, result, KILLACTIVE_MSG) )
            {
                if ( result != TheTrueObj && result != TheFalseObj )
                {
                    invalidPSNReturnListException(c, KILLACTIVE_MSG, "true or false", result, pcpsd);
                }
                else if ( result == TheFalseObj )
                {
                    reply = TRUE;
                }
            }
            setWindowPtr(hPage, DWLP_MSGRESULT, (LPARAM)reply);
            break;
        }

        case PSN_QUERYCANCEL :
        {
            // Send TRUE to *prevent* the dialog from closing.  The Rexx
            // programmer should send .false to disallow the dialog from
            // closing.
            long reply = FALSE;

            if ( ! pcpsp->abort )
            {
                RexxObjectPtr result = c->SendMessage1(pcpsp->rexxSelf, QUERYCANCEL_MSG, pcpsd->rexxSelf);

                if ( goodReply(c, pcpsd, result, QUERYCANCEL_MSG) )
                {
                    if ( result != TheTrueObj && result != TheFalseObj )
                    {
                        invalidPSNReturnListException(c, QUERYCANCEL_MSG, "true or false", result, pcpsd);
                    }
                    else if ( result == TheFalseObj )
                    {
                        reply = TRUE;
                    }
                }
            }

            setWindowPtr(hPage, DWLP_MSGRESULT, (LPARAM)reply);
            break;
        }

        case PSN_RESET :
        {
            LPPSHNOTIFY lppsn            = (LPPSHNOTIFY)lParam;
            RexxObjectPtr isCancelButton = lppsn->lParam ? TheTrueObj : TheFalseObj;

            RexxObjectPtr result = c->SendMessage2(pcpsp->rexxSelf, RESET_MSG, isCancelButton, pcpsd->rexxSelf);
            goodReply(c, pcpsd, result, RESET_MSG);
            break;
        }

        case PSN_TRANSLATEACCELERATOR :
        {
            if ( pcpsp->wantAccelerators )
            {
            LPMSG    pMsg  = (LPMSG)((LPPSHNOTIFY)lParam)->lParam;
            uint32_t reply = PSNRET_NOERROR;

            RexxArrayObject args = getTranslateAccelatorArgs(c, pMsg->message, pMsg->wParam, pMsg->lParam, pcpsd->rexxSelf);
            RexxObjectPtr result = c->SendMessage(pcpsp->rexxSelf, TRANSLATEACCELERATOR_MSG, args);

            if ( goodReply(c, pcpsd, result, TRANSLATEACCELERATOR_MSG) )
            {
                if ( ! c->UnsignedInt32(result, &reply) || (reply != PSNRET_NOERROR && reply != PSNRET_MESSAGEHANDLED) )
                {
                    invalidPSNReturnListException(c, TRANSLATEACCELERATOR_MSG, VALID_PSNRET_MSG_LIST, result, pcpsd);
                }
            }
            setWindowPtr(hPage, DWLP_MSGRESULT, reply);
            }
            break;
        }

        case PSN_SETACTIVE :
            doSetActiveCommon(c, pcpsp, hPage, SETACTIVE_MSG);
            break;

        case PSN_WIZBACK :
            doSetActiveCommon(c, pcpsp, hPage, WIZBACK_MSG);
            break;

        case PSN_WIZNEXT :
            doSetActiveCommon(c, pcpsp, hPage, WIZNEXT_MSG);
            break;

        case PSN_WIZFINISH :
            doWizFinish(c, pcpsp);
            break;

        case PSN_QUERYINITIALFOCUS :
            doQueryInitialFocus(c, pcpsp, lParam);
            break;

        default :
            break;
    }
    return TRUE;
}

/**
 * The thread and message pump function for a modeless property sheet.
 *
 *
 * @param arg
 *
 * @return DWORD WINAPI
 */
DWORD WINAPI PropSheetLoopThread(void *arg)
{
    ULONG ret;
    PropSheetThreadArgs *args = (PropSheetThreadArgs *)arg;

    pCPropertySheetDialog pcpsd = args->pcpsd;
    pCPlainBaseDialog pcpbd     = pcpsd->pcpbd;
    PROPSHEETHEADER *psh        = args->psh;
    bool *release               = args->release;

    // We need a thread context, so if we can not attach, we bail.
    RexxThreadContext *c = NULL;
    if ( ! pcpbd->interpreter->AttachThread(&c) )
    {
        *release = true;
        goto done_out;
    }

    RexxSetProcessMessages(FALSE);
    pcpbd->dlgProcContext = c;
    assignPSDThreadContext(pcpsd, c);

    HWND hPropSheet = NULL;

    if ( setPropSheetHook(pcpsd) )
    {
        hPropSheet = (HWND)PropertySheet(psh);
    }
    *release = true;

    if ( hPropSheet == NULL )
    {
        goto done_out;
    }

    c->SendMessage1(pcpbd->rexxSelf, "START", c->String("WAITFORDIALOG"));

    MSG msg;
    BOOL result;

    while ( (result = GetMessage(&msg, NULL, 0, 0)) != 0 && pcpbd->dlgAllocated )
    {
        if ( result == -1 )
        {
            break;
        }
        if ( ! PropSheet_IsDialogMessage(hPropSheet, &msg) )
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        if ( PropSheet_GetCurrentPageHwnd(hPropSheet) == NULL )
        {
            pcpsd->getResultValue = (int)PropSheet_GetResult(hPropSheet);
            break;
        }
    }

done_out:

    // Need to synchronize here, otherwise dlgAllocated may still be true, but
    // delDialog() is already running.
    EnterCriticalSection(&crit_sec);
    if ( pcpbd->dlgAllocated )
    {
        ret = delDialog(pcpbd, pcpbd->dlgProcContext);
        pcpbd->hDlgProcThread = NULL;
    }
    LeaveCriticalSection(&crit_sec);

    if ( hPropSheet != NULL )
    {
        ensureFinished(pcpbd, c, TheFalseObj);
    }

    if ( c != NULL )
    {
        c->DetachThread();
        pcpbd->dlgProcContext = NULL;
        pcpsd->dlgProcContext = NULL;
    }

    safeLocalFree((void *)psh->ppsp);
    safeLocalFree(psh);
    return ret;
}


/**
 * The dialog procedure for control dialogs used by the PropertySheetDialog.
 *
 * These are 'nested' dialogs, or dialogs within a top-level dialog.  For the
 * most part, the procedure is exactly the same as for top-level dialogs.  See
 * the remarks for the differences
 *
 * @param hDlg
 * @param uMsg
 * @param wParam
 * @param lParam
 *
 * @return LRESULT CALLBACK
 *
 * @remarks  PropertySheetPage dialogs are not created by the ooDialog
 *           framework, but rather by the OS.  So, in WM_INITDIALOG we do the
 *           Rexx dialog set up normally done in the window loop thread and the
 *           execute() method.
 *
 *           In WM_DESTROY we do the final steps / clean up normally done in the
 *           execute() method.
 */
LRESULT CALLBACK RexxPropertySheetDlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    if ( uMsg == WM_INITDIALOG )
    {
        PROPSHEETPAGE *p = (PROPSHEETPAGE *)lParam;
        pCPropertySheetPage pcpsp = (pCPropertySheetPage)p->lParam;

        if ( pcpsp == NULL )
        {
            // Theoretically impossible.  But ... if it should happen, abort.
            return endDialogPremature(NULL, hDlg, NoPCPSPpased);
        }

        initializePropSheetPage(hDlg, pcpsp);

        return TRUE;
    }

    pCPropertySheetPage pcpsp = (pCPropertySheetPage)getWindowPtr(hDlg, GWLP_USERDATA);
    if ( pcpsp == NULL )
    {
        // A number of messages arrive before WM_INITDIALOG, we just ignore them.
        return FALSE;
    }

    pCPlainBaseDialog pcpbd = pcpsp->pcpbd;
    if ( pcpbd->dlgProcContext == NULL )
    {
        if ( ! pcpbd->isActive )
        {
            return FALSE;
        }

        // Once again, theoretically impossible ...
        return endDialogPremature(pcpbd, hDlg, NoThreadContext);
    }

    if ( uMsg == WM_DESTROY )
    {
        pcpsp->dlgProcContext->SendMessage0(pcpsp->rexxSelf, "ENDEXECUTE");
        return TRUE;
    }

    bool msgEnabled = IsWindowEnabled(hDlg) ? true : false;

    // Do not search message table for WM_PAINT to improve redraw.
    if ( msgEnabled && uMsg != WM_PAINT && uMsg != WM_NCPAINT )
    {
        if ( isPSMsg(uMsg, lParam) )
        {
            return doPSMessage(pcpsp, pcpbd, uMsg, wParam, lParam);
        }

        MsgReplyType searchReply = searchMessageTables(uMsg, wParam, lParam, pcpbd);
        if ( searchReply != ContinueProcessing )
        {
            // Note pre 4.0.1, we always returned FALSE, (pass on to the system
            // to process.) But, post 4.0.1 we sometimes reply TRUE, the message
            // has been handled.
            return (searchReply == ReplyTrue ? TRUE : FALSE);
        }
    }

    switch ( uMsg )
    {
        case WM_PAINT:
            if ( pcpbd->bkgBitmap != NULL )
            {
                drawBackgroundBmp(pcpbd, hDlg);
            }
            break;

        case WM_DRAWITEM:
            if ( lParam != 0 )
            {
                return drawBitmapButton(pcpbd, lParam, msgEnabled);
            }
            break;

        case WM_CTLCOLORDLG:
            if ( pcpbd->bkgBrush )
            {
                return(LRESULT)pcpbd->bkgBrush;
            }
            break;

        case WM_CTLCOLORSTATIC:
        case WM_CTLCOLORBTN:
        case WM_CTLCOLOREDIT:
        case WM_CTLCOLORLISTBOX:
        case WM_CTLCOLORMSGBOX:
        case WM_CTLCOLORSCROLLBAR:
        {
            HBRUSH hbrush = NULL;

            if ( pcpbd->CT_nextIndex > 0 )
            {
                // See of the user has set the dialog item with a different
                // color.
                long id = GetWindowLong((HWND)lParam, GWL_ID);
                if ( id > 0 )
                {
                    register size_t i = 0;
                    while ( i < pcpbd->CT_nextIndex && pcpbd->ColorTab[i].itemID != id )
                    {
                        i++;
                    }
                    if ( i < pcpbd->CT_nextIndex )
                    {
                        hbrush = pcpbd->ColorTab[i].ColorBrush;
                    }

                    if ( hbrush )
                    {
                        if ( pcpbd->ColorTab[i].isSysBrush )
                        {
                            SetBkColor((HDC)wParam, GetSysColor(pcpbd->ColorTab[i].ColorBk));
                            if ( pcpbd->ColorTab[i].ColorFG != -1 )
                            {
                                SetTextColor((HDC)wParam, GetSysColor(pcpbd->ColorTab[i].ColorFG));
                            }
                        }
                        else
                        {
                            SetBkColor((HDC)wParam, PALETTEINDEX(pcpbd->ColorTab[i].ColorBk));
                            if ( pcpbd->ColorTab[i].ColorFG != -1 )
                            {
                                SetTextColor((HDC)wParam, PALETTEINDEX(pcpbd->ColorTab[i].ColorFG));
                            }
                        }
                    }
                }
            }
            if ( hbrush )
                return(LRESULT)hbrush;
            else
                return DefWindowProc(hDlg, uMsg, wParam, lParam);
        }

        case WM_QUERYNEWPALETTE:
        case WM_PALETTECHANGED:
            return paletteMessage(pcpbd, hDlg, uMsg, wParam, lParam);

        // For now, don't let the user created nested, nested dialogs.  In
        // addition, keyboard hooks should only be created in a top-level
        // dialog.
        case WM_USER_CREATECHILD:
        case WM_USER_CREATECONTROL_DLG:
        case WM_USER_CREATECONTROL_RESDLG:
        case WM_USER_HOOK:
            ReplyMessage((LRESULT)NULL);
            return TRUE;

        case WM_USER_INTERRUPTSCROLL:
            pcpbd->stopScroll = wParam;
            return TRUE;

        case WM_USER_GETFOCUS:
            ReplyMessage((LRESULT)GetFocus());
            return TRUE;

        case WM_USER_GETSETCAPTURE:
            if ( wParam == 0 )
            {
                ReplyMessage((LRESULT)GetCapture());
            }
            else if ( wParam == 2 )
            {
                uint32_t rc = 0;
                if ( ReleaseCapture() == 0 )
                {
                    rc = GetLastError();
                }
                ReplyMessage((LRESULT)rc);
            }
            else
            {
                ReplyMessage((LRESULT)SetCapture((HWND)lParam));
            }
            return TRUE;

        case WM_USER_GETKEYSTATE:
            ReplyMessage((LRESULT)GetAsyncKeyState((int)wParam));
            return TRUE;

        case WM_USER_SUBCLASS:
        {
            SUBCLASSDATA *pData = (SUBCLASSDATA *)lParam;

            pData->dlgProcContext = pcpbd->dlgProcContext;
            pData->rexxDialog = pcpbd->rexxSelf;

            BOOL success = SetWindowSubclass(pData->hCtrl, (SUBCLASSPROC)wParam, pData->uID, (DWORD_PTR)pData);

            ReplyMessage((LRESULT)success);
            return TRUE;
        }

        case WM_USER_SUBCLASS_REMOVE:
            ReplyMessage((LRESULT)RemoveWindowSubclass(GetDlgItem(hDlg, (int)lParam), (SUBCLASSPROC)wParam, (int)lParam));
            return TRUE;

        case WM_USER_CONTEXT_MENU:
        {
            PTRACKPOP ptp = (PTRACKPOP)wParam;
            uint32_t cmd;

            SetLastError(0);
            cmd = (uint32_t)TrackPopupMenuEx(ptp->hMenu, ptp->flags, ptp->point.x, ptp->point.y,
                                             ptp->hWnd, ptp->lptpm);

            // If TPM_RETURNCMD is specified, the return is the menu item
            // selected.  Otherwise, the return is 0 for failure and
            // non-zero for success.
            if ( ! (ptp->flags & TPM_RETURNCMD) )
            {
                cmd = (cmd == 0 ? FALSE : TRUE);
                if ( cmd == FALSE )
                {
                    ptp->dwErr = GetLastError();
                }
            }
            ReplyMessage((LRESULT)cmd);
            return TRUE;
        }
    }

    return FALSE;
}


LRESULT CALLBACK PropSheetCBTProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    PROPSHEETHOOKDATA *pshd = getPropSheetHookData(NULL);
    if ( pshd == NULL )
    {
        // Without the hook handle, there is nothing that can be done.
        return 0;
    }

    HHOOK hHook = pshd->hHook;

    if ( nCode == HCBT_CREATEWND )
    {
        LPCREATESTRUCT cs = (LPCREATESTRUCT)((CBT_CREATEWND *)lParam)->lpcs;


        if ( isPropSheetMatch(cs) )
        {
            pshd->pcpsd->hDlg = (HWND)wParam;
            pshd->hPropSheet = (HWND)wParam;
            UnhookWindowsHookEx(hHook);
        }
        else
        {
            printf("PropSheetCBTProc() DID NOT MATCH !! hwnd=%p className=%s name=%s style=0x%08x exStyle=0x%08x\n",
                    wParam, cs->lpszClass, cs->lpszName, cs->style, cs->dwExStyle);
        }
    }

    return CallNextHookEx(hHook, nCode, wParam, lParam);
}


/**
 * An implementation of the PropSheetProc callback.  The system calls this
 * function when the property sheet is being created and initialized.
 *
 * There are only 3 uMsg messages sent.
 *
 * PSCB_PRECREATE Sent before the PropertySheet is created. lParam is a pointer
 * to the dialog template.  hwndPropSheet is null.  This callback could be used
 * to change something in the dialog template.  Don't see any good use for it at
 * this point.
 *
 * PSCB_INITIALIZED  Sent when the PropertySheet is initializing.  We use this
 * callback to do the Rexx dialog initialization that is usually done in the
 * thread creation function and the execute() method.
 *
 * PSCB_BUTTONPRESSED  Sent when a button in the property sheet is pressed.
 * Seems as though it would be useful.  But, it actually does not provide any
 * thing not already provided by the regular notification messages.  Not used at
 * this point.
 *
 * @param hwndPropSheet
 * @param uMsg
 * @param lParam
 *
 * @return void
 *
 * @remarks  Typical implementation would use a switch statement.  Since only 1
 *           uMsg is of interest, I've removed the switch.
 */
void CALLBACK PropSheetCallback(HWND hwndPropSheet, UINT uMsg, LPARAM lParam)
{
    if ( uMsg == PSCB_INITIALIZED )
    {
        initializePropSheet(hwndPropSheet);
    }
    else if ( uMsg == PSCB_PRECREATE )
    {
        adjustPropSheetTemplate(lParam, true);
    }
}


/**
 * The property sheet page callback function.  It is called with 3 different
 * messages:
 *
 * PSPCB_ADDREF   Called when a page is being created.
 *
 * PSPCB_CREATE   Called when the dialog box for the page is being created.
 *
 * PSPCP_RELEASE  Called when the page is being destroyed
 *
 * We use PSPCB_CREATE to call back in to Rexx by invoking the pageCreate()
 * method of the PropertySheetPage.  The default implementation of pageCreate()
 * simply returns true.
 *
 * The programmer can over-ride pageCreate(), for whatever reason.  Returning 0
 * prevents the page dialog from being created.  Although, I'm not sure what use
 * the Rexx programmer would make of that.
 *
 * The other messages are ignored at this time.  (And I'm not sure if the one
 * callback is that useful for the Rexx programmer.)
 *
 *
 * @param hwnd  Reserved by MS, always NULL.
 * @param uMsg
 * @param ppsp
 *
 * @return UINT CALLBACK
 *
 * @remarks  The property sheet page thread context is assigned during the
 *           creation of the property sheet, on the thread where the property
 *           sheet is created.  Since the property sheet is created before any
 *           of the pages are created, the dlgProcContext for the page can not
 *           be null.
 */
uint32_t CALLBACK PropSheetPageCallBack(HWND hwnd, uint32_t msg, LPPROPSHEETPAGE ppsp)
{
    pCPropertySheetPage pcpsp = (pCPropertySheetPage)ppsp->lParam;

    if ( msg == PSPCB_CREATE )
    {
        RexxThreadContext *c = pcpsp->dlgProcContext;
        uint32_t reply = TRUE;

        if ( c != NULL )
        {
            pCPropertySheetDialog pcpsd = (pCPropertySheetDialog)pcpsp->cppPropSheet;

            RexxObjectPtr result = c->SendMessage0(pcpsp->rexxSelf, PAGECREATE_MSG);

            if ( goodReply(c, pcpsd, result, PAGECREATE_MSG) )
            {
                if ( result != TheTrueObj && result != TheFalseObj )
                {
                   invalidPSNReturnListException(c, PAGECREATE_MSG, "true or false", result, pcpsd);
                }
                else
                {
                    reply = (result == TheTrueObj ? 1 : 0);
                }
            }
        }
        return reply;
    }
    return 0;
}


/**
 *  Methods for the .PropertySheetDialog class.
 */
#define PROPERTYSHEETDIALOG_CLASS  "PropertySheetDialog"


bool setCaption(RexxMethodContext *c, pCPropertySheetDialog pcpsd, CSTRING text)
{
    pcpsd->caption = (char *)LocalAlloc(LPTR, strlen(text) + 1);
    if ( pcpsd->caption == NULL )
    {
        outOfMemoryException(c->threadContext);
        return false;
    }

    strcpy(pcpsd->caption, text);
    c->SetObjectVariable("CAPTION", c->String(text));
    return true;
}


static bool parsePropSheetOpts(RexxMethodContext *c, pCPropertySheetDialog pcpsd, CSTRING options)
{
    uint32_t opts = PSH_PROPSHEETPAGE | PSH_USECALLBACK;

    if ( options == NULL )
    {
        pcpsd->isNotWizard = true;
        goto done_out;
    }

    if ( StrStrI(options, "USEPAGELANG") != NULL ) opts |= PSH_USEPAGELANG;
    if ( StrStrI(options, "RTLREADING ") != NULL ) opts |= PSH_RTLREADING;

    if ( StrStrI(options, "AEROWIZARD") != NULL )
    {
        if ( ! requiredComCtl32Version(c, COMCTL32_6_0, "the AEROWIZARD style") ||
             ! requiredOS(c, Vista_OS, "the AEROWIZARD style", "Vista") )
        {
            goto err_out;
        }

        pcpsd->isAeroWiz = true;
        opts |= PSH_AEROWIZARD | PSH_WIZARD;

        if ( StrStrI(options, "RESIZABLE") != NULL ) opts |= PSH_RESIZABLE;
        if ( StrStrI(options, "NOMARGIN")  != NULL ) opts |= PSH_NOMARGIN;
    }
    else if ( StrStrI(options, "WIZARD97") != NULL )
    {
        pcpsd->isWiz97 = true;
        opts |= PSH_WIZARD97;
    }
    else if ( StrStrI(options, "WIZARDLITE") != NULL )
    {
        pcpsd->isWizLite = true;
        opts |= PSH_WIZARD_LITE;
    }
    else
    {
        pcpsd->isNotWizard = true;
        if ( StrStrI(options, "NOCONTEXTHELP") != NULL )
        {
            opts |= PSH_NOCONTEXTHELP;
        }
    }

    if ( ! pcpsd->isAeroWiz )
    {
        if ( StrStrI(options, "HASHELP")    != NULL ) opts |= PSH_HASHELP;
        if ( StrStrI(options, "NOAPPLYNOW") != NULL ) opts |= PSH_NOAPPLYNOW;
        if ( StrStrI(options, "PROPTITLE")  != NULL ) opts |= PSH_PROPTITLE;
    }

    if ( (pcpsd->isAeroWiz || pcpsd->isWiz97) && StrStrI(options, "WIZARDHASFINISH") != NULL )
    {
        opts |= PSH_WIZARDHASFINISH;
    }

    if ( (pcpsd->isWiz97 || pcpsd->isWizLite) && StrStrI(options, "WIZARDCONTEXTHELP") != NULL )
    {
        opts |= PSH_WIZARDCONTEXTHELP;
    }

done_out:
    pcpsd->propSheetFlags = opts;

    return true;

err_out:
    return false;
}

/**
 * Checks that the Rexx owner dialog passed in to execute() is valid.
 *
 * @param c       Method context we are operating in.
 * @param owner   Rexx object to check.
 * @param argPos  Argument position of the Rexx object.
 *
 * @return Windown handle of the dialog, if okay, otherise NULL.  If NULL is
 *         returned, an exceptions has been raised.
 */
static HWND checkPropSheetOwner(RexxMethodContext *c, RexxObjectPtr owner, size_t argPos)
{
    pCPlainBaseDialog pcpbdOwner = requiredDlgCSelf(c, owner, oodPlainBaseDialog, argPos);
    if ( pcpbdOwner == NULL )
    {
        return NULL;
    }

    if ( pcpbdOwner->isControlDlg || pcpbdOwner->isPageDlg )
    {
        userDefinedMsgException(c, argPos, "The owner dialog of a PropertySheetDialog can not be a control dialog.");
        return NULL;
    }

    if ( pcpbdOwner->hDlg == NULL )
    {
        methodCanNotBeInvokedException(c, "execute()", pcpbdOwner->rexxSelf,
                                       "using an owner dialog whose Windows dialog does not exist");
        return NULL;
    }
    return pcpbdOwner->hDlg;
}


void assignPSDThreadContext(pCPropertySheetDialog pcpsd, RexxThreadContext *c)
{
    pcpsd->dlgProcContext = c;

    uint32_t count = pcpsd->pageCount;
    for ( uint32_t i = 0; i < count; i++ )
    {
        pcpsd->cppPages[i]->dlgProcContext = c;
        pcpsd->cppPages[i]->pcpbd->dlgProcContext = c;
    }
}


bool psdInitSuper(RexxMethodContext *context, RexxClassObject super, RexxStringObject hFile)
{
    RexxArrayObject newArgs = context->NewArray(4);

    context->ArrayPut(newArgs, context->NullString(), 1);
    context->ArrayPut(newArgs, TheZeroObj, 2);
    if ( hFile != NULLOBJECT )
    {
        context->ArrayPut(newArgs, hFile, 4);
    }
    RexxObjectPtr result = context->ForwardMessage(NULL, NULL, super, newArgs);

    return isInt(0, result, context->threadContext);
}

/**
 * Determines if an icon for the tab is going to be used and sets the property
 * sheet page struct accordingly.
 *
 * The user can specify to use an icon for the tab by setting the imageList
 * attribute of the property sheet dialog or by setting the icon attribute of
 * the propert sheet page dialog.  In addition, the icon attribute itself can
 * either be an icon handle or an icon resource ID.
 *
 * The precedence for a tab icon is: image list in property sheet, loaded icon,
 * icon ID.
 *
 * @param c
 * @param pcpsd
 * @param psp
 *
 * @return uint32_t
 */
uint32_t maybeSetTabIcon(RexxMethodContext *c, pCPropertySheetDialog pcpsd, PROPSHEETPAGE *psp, uint32_t index)
{
    uint32_t flag = 0;

    if ( pcpsd->imageList != NULL )
    {
        HICON hIcon = ImageList_GetIcon(pcpsd->imageList, index, INDEXTOOVERLAYMASK(index) | ILD_NORMAL);

        psp->hIcon = hIcon;
        flag = PSP_USEHICON;
    }
    else
    {
        pCPropertySheetPage pcpsp = pcpsd->cppPages[index];

        if ( pcpsp->hIcon != NULL )
        {
            psp->hIcon = pcpsp->hIcon;
            flag = PSP_USEHICON;
        }
        else if ( pcpsp->iconID != 0 && (pcpsp->hInstance != NULL || pcpsp->pageType == oodResPSPDialog) )
        {
            psp->pszIcon = MAKEINTRESOURCE(pcpsp->iconID);
            flag = PSP_USEICONID;
        }
    }
    return flag;
}


/**
 * Initializes a single PROPSHEETPAGE struct.
 *
 * @param c
 * @param pcpsd
 * @param psp
 * @param i
 *
 * @return bool
 *
 * @remarks  On error, it is the caller's repsonsibility to clean up psp memory.
 *
 * @remarks  For ResDialogs, the user has to include all other resources, header
 *           bitmap, etc., in the resource dll for the dialog.  But, for other
 *           types of dialog pages the user can use a resource image for the
 *           additional resources by using the resourceImage attribute.  When we
 *           fill in the struct for a page we ignore the resourceImage attribute
 *           if it is a ResDialog page.
 */
bool initPSP(RexxMethodContext *c, pCPropertySheetDialog pcpsd, PROPSHEETPAGE *psp, uint32_t i, bool isExteriorPage)
{
    RexxObjectPtr       dlg   = pcpsd->rexxPages[i];
    pCPropertySheetPage pcpsp = pcpsd->cppPages[i];
    pCDynamicDialog     pcdd  = pcpsp->pcdd;
    uint32_t            flags = pcpsp->pageFlags;
    bool                success = false;

    RexxObjectPtr result = c->SendMessage0(dlg, "INITTEMPLATE");
    if ( result == TheFalseObj )
    {
        noWindowsPageDlgException(c, i + 1);
        goto done_out;
    }

    psp->dwSize      = sizeof(PROPSHEETPAGE);
    psp->pfnDlgProc  = (DLGPROC)RexxPropertySheetDlgProc;
    psp->lParam      = (LPARAM)pcpsp;
    psp->pfnCallback = PropSheetPageCallBack;

    if ( pcpsp->pageTitle != NULL )
    {
        flags |= PSP_USETITLE;
        psp->pszTitle = pcpsp->pageTitle;
    }

    if ( pcpsp->pageType == oodResPSPDialog )
    {
        psp->hInstance   = pcpsp->pcpbd->hInstance;
        psp->pszTemplate = MAKEINTRESOURCE(pcpsp->resID);
    }
    else
    {
        flags |= PSP_DLGINDIRECT;
        psp->pResource = (PROPSHEETPAGE_RESOURCE)pcdd->base;

        if ( pcpsp->hInstance != NULL )
        {
            psp->hInstance = pcpsp->hInstance;
        }
    }

    flags |= maybeSetTabIcon(c, pcpsd, psp, i);

    if ( (pcpsd->isWiz97 || pcpsd->isWizLite)  )
    {
        if ( pcpsp->headerTitle != NULL )
        {
            psp->pszHeaderTitle = pcpsp->headerTitle;
            flags |= PSP_USEHEADERTITLE;
        }

        if ( pcpsp->headerSubTitle != NULL )
        {
            psp->pszHeaderSubTitle = pcpsp->headerSubTitle;
            flags |= PSP_USEHEADERSUBTITLE;
        }

        if ( pcpsp->headerTitle == NULL && pcpsp->headerSubTitle == NULL && isExteriorPage )
        {
            flags |= PSP_HIDEHEADER;
        }
    }

    psp->dwFlags = flags;
    success = true;

done_out:
    return success;
}


/**
 * Allocates the memory for the property sheet page structs and fills in the
 * struct for all the pages of the property sheet.
 *
 * @param c        Method context we are operating in.
 * @param pcpsd    Pointer to the CSelf struct for a PropertySheetDialog.
 * @param hParent  Possible handle of parent dialog, can be null.
 *
 * @return The pointer to the property sheet page struct(s) on success, null on
 *         failure.
 *
 * @remarks  When we call initPSP() we pass true for the isExteriorPage argument
 *           on the first and last page without checking if the property sheet
 *           is a wizard or not. initPSP() does that check.
 */
PROPSHEETPAGE *initPropSheetPages(RexxMethodContext *c, pCPropertySheetDialog pcpsd)
{
    PROPSHEETPAGE *psp = NULL;
    uint32_t count = pcpsd->pageCount;

    psp = (PROPSHEETPAGE *)LocalAlloc(LPTR, MAXPROPPAGES * sizeof(PROPSHEETPAGE));
    if ( psp == NULL )
    {
        outOfMemoryException(c->threadContext);
        goto done_out;
    }

    // Initialize each individual page.
    for ( uint32_t i = 0; i < count; i++ )
    {
        if ( ! initPSP(c, pcpsd, psp + i, i, (i == 0 || i == count - 1)) )
        {
            LocalFree(psp);
            psp = NULL;
        }
    }

done_out:
    return psp;
}

/**
 * Allocates the memory for the property sheet header struct and fills in the
 * struct.
 *
 * @param c        Method context we are operating in.
 * @param pcpsd    Pointer to the CSelf struct for a PropertySheetDialog.
 * @param psp      Pointer to the property sheet page struct(s) for the property
 *                 sheet.
 * @param hParent  Possible handle of parent dialog, can be null.
 *
 * @return The pointer to the property sheet header on success, null on failure.
 */
PROPSHEETHEADER *initPropSheetHeader(RexxMethodContext *c, pCPropertySheetDialog pcpsd, LPCPROPSHEETPAGE psp, HWND hParent)
{
    PROPSHEETHEADER *psh = (PROPSHEETHEADER *)LocalAlloc(LPTR, sizeof(PROPSHEETHEADER));
    if ( psh == NULL )
    {
        outOfMemoryException(c->threadContext);
        return NULL;
    }

    uint32_t flags = pcpsd->propSheetFlags;

    psh->dwSize           = sizeof(PROPSHEETHEADER);
    psh->hwndParent       = hParent;
    psh->nPages           = pcpsd->pageCount;
    psh->ppsp             = (LPCPROPSHEETPAGE)psp;
    psh->pfnCallback      = (PFNPROPSHEETCALLBACK)PropSheetCallback;

    if ( pcpsd->hInstance != NULL )
    {
        psh->hInstance = pcpsd->hInstance;
    }

    if ( pcpsd->caption != NULL )
    {
        if ( pcpsd->isAeroWiz )
        {
            LPWSTR newCaption = ansi2unicode(pcpsd->caption);
            if ( newCaption != NULL )
            {
                LocalFree(pcpsd->caption);
                pcpsd->caption = (char *)newCaption;
            }
        }

        psh->pszCaption = pcpsd->caption;
    }

    if ( pcpsd->startPage != 0 )
    {
        psh->nStartPage = pcpsd->startPage - 1;
    }

    if ( pcpsd->hIcon != NULL )
    {
        psh->hIcon = pcpsd->hIcon;
        flags |= PSH_USEHICON;
    }
    else if ( pcpsd->iconID != 0 && pcpsd->hInstance != NULL )
    {
        psh->pszIcon = MAKEINTRESOURCE(pcpsd->iconID);
        flags |= PSH_USEICONID;
    }

    if ( pcpsd->isWiz97 || pcpsd->isAeroWiz )
    {
        if ( pcpsd->hHeaderBitmap != NULL )
        {
            psh->hbmHeader = pcpsd->hHeaderBitmap;

            flags |= PSH_USEHBMHEADER;
            if ( pcpsd->isWiz97 )
            {
                flags |= PSH_HEADER;
            }
        }
        else if ( pcpsd->headerBitmapID != 0 && pcpsd->hInstance != NULL )
        {
            psh->pszbmHeader = MAKEINTRESOURCE(pcpsd->headerBitmapID);
            flags |= pcpsd->isWiz97 ? PSH_HEADER : PSH_HEADERBITMAP;
        }

        if ( ! pcpsd->isAeroWiz )
        {
            if ( pcpsd->hWatermark != NULL )
            {
                psh->hbmWatermark = pcpsd->hWatermark;
                flags |= PSH_USEHBMWATERMARK | PSH_WATERMARK;
            }
            else if ( pcpsd->watermarkID != 0 && pcpsd->hInstance != NULL )
            {
                psh->pszbmWatermark = MAKEINTRESOURCE(pcpsd->watermarkID);
                flags |= PSH_WATERMARK;
            }
        }
    }

    if ( pcpsd->modeless )
    {
        flags |= PSH_MODELESS;
    }

    psh->dwFlags = flags;

    return psh;
}


HWND getValidPageHwnd(RexxMethodContext *c, pCPropertySheetDialog pcpsd, RexxObjectPtr page, size_t pos)
{
    RexxObjectPtr match = NULL;

    uint32_t count = pcpsd->pageCount;
    for ( uint32_t i = 0; i < count; i++ )
    {
        if ( page == pcpsd->rexxPages[i] )
        {
            match = pcpsd->rexxPages[i];
            break;
        }
    }

    if ( match == NULL )
    {
        noSuchPageException(c, page, pos);
        return NULL;
    }

    HWND hPage = dlgToPSPHDlg(c, match);
    if ( hPage == NULL )
    {
        noWindowsPageException(c, dlgToPSPCSelf(c, page)->pageNumber + 1, pos);
    }

    return hPage;
}


/** PropertySheetDialog::pages          [Attrbiute Get]
 *
 *  Gets the array of page dialogs for this property sheet.
 *
 *  @return  An array of Rexx dialogs.  Each index in the array contains the
 *           Rexx dialog for the page matching the index.  Page indexes are
 *           one-based.
 *
 *  @remarks.  There is no set method for this attribute, it is set in the
 *             native code when the user instantiates the property sheet.
 *
 *             We return a copy of the actual array so that the user can not
 *             alter the actual array.
 */
RexxMethod1(RexxObjectPtr, psdlg_getPages_atr, CSELF, pCSelf)
{
    pCPropertySheetDialog pcpsd = (pCPropertySheetDialog)pCSelf;

    uint32_t count = pcpsd->pageCount;
    RexxArrayObject pages = context->NewArray(count);

    for ( uint32_t i = 0; i < count; i++ )
    {
        context->ArrayPut(pages, pcpsd->rexxPages[i], i + 1);
    }
    return pages;
}


/** PropertySheetDialog::caption()      [Attribute set]
 *
 */
RexxMethod2(RexxObjectPtr, psdlg_setCaption_atr, CSTRING, text, CSELF, pCSelf)
{
    pCPropertySheetDialog pcpsd = (pCPropertySheetDialog)pCSelf;
    setCaption(context, pcpsd, text);
    return NULLOBJECT;
}


/** PropertySheetDialog::resources()      [Attribute set]
 *
 */
RexxMethod2(RexxObjectPtr, psdlg_setResources_atr, RexxObjectPtr, resourceImage, CSELF, pCSelf)
{
    pCPropertySheetDialog pcpsd = (pCPropertySheetDialog)pCSelf;

    PRESOURCEIMAGE ri = rxGetResourceImage(context, resourceImage, 1);
    if ( ri != NULL )
    {
        pcpsd->hInstance = ri->hMod;
        context->SetObjectVariable("RESOURCES", resourceImage);
    }
    return NULLOBJECT;
}

/** PropertySheetDialog::appIcon()      [Attribute set]
 *
 *  Sets the icon for the appIcon attribute.  The user can specify the icon as
 *  either a resource ID (numeric or symbolic) or as an .Image object.
 *
 *  @remarks  If the user specifies the icon as an .Image object, then it has to
 *            be an icon image, not some other type of image, like a bitmap,
 *            etc.  The rxGetImageIcon() call will raise an exception if the
 *            image is not an icon.
 */
RexxMethod2(RexxObjectPtr, psdlg_setAppIcon_atr, RexxObjectPtr, icon, CSELF, pCSelf)
{
    pCPropertySheetDialog pcpsd = (pCPropertySheetDialog)pCSelf;

    bool    isImage;
    uint8_t type;

    INT_PTR result = getImageOrID(context, pcpsd->rexxSelf, icon, 1, &isImage, &type);
    if ( result != 0 )
    {
        if ( isImage )
        {
            if ( type != IMAGE_ICON )
            {
                wrongArgValueException(context->threadContext, 1, "Icon, Cursor", getImageTypeName(type));
                return NULLOBJECT;
            }
            pcpsd->hIcon = (HICON)result;
        }
        else
        {
            pcpsd->iconID = (uint32_t)result;
        }

        context->SetObjectVariable("ICON", icon);
    }

    return NULLOBJECT;
}

/** PropertySheetDialog::header()        [Attribute set]
 *
 *  Sets the header bitmap used for a Wizard (Wizard97 or AeroWizard.)
 *
 *  For a Wizard97, the user can specify the bitmap as either a resource ID
 *  (numeric or symbolic) or as an .Image object.  However for an AeroWizard,
 *  the bitmap must be specified as an .Image object.
 *
 *  @remarks  If the user specifies the header as an .Image object, then it has
 *            to be a bitmap image, not some other type of image, like an icon,
 *            etc.
 *
 *            If this property sheet is an aero wizard we don't check that this
 *            is Vista or later because it is checked when the AeroWizard
 *            keyword is first used.
 */
RexxMethod2(RexxObjectPtr, psdlg_setHeader_atr, RexxObjectPtr, header, CSELF, pCSelf)
{
    pCPropertySheetDialog pcpsd = (pCPropertySheetDialog)pCSelf;

    if ( ! (pcpsd->isAeroWiz || pcpsd->isWiz97) )
    {
        invalidAttributeException(context, pcpsd->rexxSelf);
        goto done_out;
    }

    bool    isImage;
    uint8_t type;

    INT_PTR result = getImageOrID(context, pcpsd->rexxSelf, header, 1, &isImage, &type);
    if ( result != 0 )
    {
        if ( isImage )
        {
            if ( type != IMAGE_BITMAP )
            {
                wrongArgValueException(context->threadContext, 1, "Bitmap", getImageTypeName(type));
                goto done_out;
            }

            pcpsd->hHeaderBitmap = (HBITMAP)result;
        }
        else
        {
            if ( pcpsd->isAeroWiz )
            {
                wrongClassException(context->threadContext, 1, "Image");
                goto done_out;
            }

            pcpsd->headerBitmapID = (uint32_t)result;
        }

        context->SetObjectVariable("HEADER", header);
    }

done_out:
    return NULLOBJECT;
}

/** PropertySheetDialog::watermark()        [Attribute set]
 *
 *  Sets the watermark bitmap used for a Wizard97 wizard.
 *
 *  The user can specify the bitmap as either a resource ID (numeric or
 *  symbolic) or as an .Image object.
 *
 *  @remarks  If the user specifies the header as an .Image object, then it has
 *            to be a bitmap image, not some other type of image, like an icon,
 *            etc.
 */
RexxMethod2(RexxObjectPtr, psdlg_setWatermark_atr, RexxObjectPtr, watermark, CSELF, pCSelf)
{
    pCPropertySheetDialog pcpsd = (pCPropertySheetDialog)pCSelf;

    if ( ! pcpsd->isWiz97 )
    {
        invalidAttributeException(context, pcpsd->rexxSelf);
        goto done_out;
    }

    bool    isImage;
    uint8_t type;

    INT_PTR result = getImageOrID(context, pcpsd->rexxSelf, watermark, 1, &isImage, &type);
    if ( result != 0 )
    {
        if ( isImage )
        {
            if ( type != IMAGE_BITMAP )
            {
                wrongArgValueException(context->threadContext, 1, "Bitmap", getImageTypeName(type));
                goto done_out;
            }
            pcpsd->hWatermark = (HBITMAP)result;
        }
        else
        {
            pcpsd->watermarkID = (uint32_t)result;
        }

        context->SetObjectVariable("WATERMARK", watermark);
    }

done_out:
    return NULLOBJECT;
}

/** PropertySheetDialog::startPage()      [Attribute set]
 *
 */
RexxMethod2(RexxObjectPtr, psdlg_setStartPage_atr, uint32_t, startPage, CSELF, pCSelf)
{
    pCPropertySheetDialog pcpsd = (pCPropertySheetDialog)pCSelf;

    if ( startPage < 1 || startPage > MAXPROPPAGES )
    {
        wrongRangeException(context->threadContext, 1, 1, MAXPROPPAGES, startPage);
    }
    else
    {
        pcpsd->startPage = startPage;
        context->SetObjectVariable("STARTPAGE", context->UnsignedInt32(startPage));
    }

    return NULLOBJECT;
}

/** PropertySheetDialog::imageList()      [Attribute set]
 *
 */
RexxMethod2(RexxObjectPtr, psdlg_setImageList_atr, RexxObjectPtr, imageList, CSELF, pCSelf)
{
    pCPropertySheetDialog pcpsd = (pCPropertySheetDialog)pCSelf;

    pcpsd->imageList = rxGetImageList(context, imageList, 1);
    if ( pcpsd->imageList != NULL )
    {
        context->SetObjectVariable("IMAGELIST", imageList);
    }
    return NULLOBJECT;
}


/** PropertySheetDialog::init()
 *
 *  The initialization of the property sheet dialog.
 *
 *
 */
RexxMethod6(wholenumber_t, psdlg_init, RexxArrayObject, pages, OPTIONAL_CSTRING, opts, OPTIONAL_CSTRING, caption,
            OPTIONAL_RexxStringObject, hFile, SUPER, super, OSELF, self)
{
    // This is an error return.
    wholenumber_t result = 1;

    if ( ! psdInitSuper(context, super, hFile) )
    {
        goto done_out;
    }

    pCPlainBaseDialog pcpbd = (pCPlainBaseDialog)context->GetCSelf();

    // Get a buffer for the PropertySheetDialog CSelf.
    RexxBufferObject cselfBuffer = context->NewBuffer(sizeof(CPropertySheetDialog));
    if ( cselfBuffer == NULLOBJECT )
    {
        goto done_out;
    }

    pCPropertySheetDialog pcpsd = (pCPropertySheetDialog)context->BufferData(cselfBuffer);
    memset(pcpsd, 0, sizeof(CPropertySheetDialog));

    pcpbd->dlgPrivate = pcpsd;
    pcpsd->pcpbd = pcpbd;
    pcpsd->rexxSelf = self;
    context->SetObjectVariable("CSELF", cselfBuffer);

    // Now process the arguments and do the rest of the initialization.
    uint32_t count = (uint32_t)context->ArrayItems(pages);
    if ( count == 0 )
    {
        emptyArrayException(context->threadContext, 1);
        goto done_out;
    }
    pcpsd->pageCount = count;

    pCPropertySheetPage *cppPages = (pCPropertySheetPage *)LocalAlloc(LPTR, MAXPROPPAGES * sizeof(pCPropertySheetPage *));
    RexxObjectPtr *rexxPages = (RexxObjectPtr *)LocalAlloc(LPTR, MAXPROPPAGES * sizeof(RexxObjectPtr *));

    if ( cppPages == NULL || rexxPages == NULL )
    {
        outOfMemoryException(context->threadContext);
        goto done_out;
    }

    pCPropertySheetPage *pPage = cppPages;
    RexxObjectPtr *pRexxPage = rexxPages;
    for ( uint32_t i = 1; i <= count; i++, pPage++, pRexxPage++ )
    {
        RexxObjectPtr dlg = context->ArrayAt(pages, i);
        if ( dlg == NULLOBJECT )
        {
            sparseArrayException(context->threadContext, 1, i);
            goto done_out;
        }
        if ( ! context->IsOfType(dlg, "PROPERTYSHEETPAGE") )
        {
            wrongObjInArrayException(context->threadContext, 1, i, "a PropertySheetPage", dlg);
            goto done_out;
        }

        pCPropertySheetPage pcpsp = dlgToPSPCSelf(context, dlg);
        pcpsp->pageNumber = i;
        pcpsp->rexxPropSheet = pcpsd->rexxSelf;
        pcpsp->cppPropSheet = pcpsd;
        pcpsp->isWizardPage = ! pcpsd->isNotWizard;

        *pPage = pcpsp;
        *pRexxPage = dlg;
    }

    pcpsd->cppPages = cppPages;
    pcpsd->rexxPages = rexxPages;
    pcpsd->getResultValue = OOD_NO_VALUE;

    // Set the pages attribute object variable so that the Rexx pages object is
    // not garbage collected.
    context->SetObjectVariable("PAGES", pages);

    if ( argumentExists(3) )
    {
        if ( ! setCaption(context, pcpsd, caption) )
        {
            goto done_out;
        }
    }
    else
    {
        context->SetObjectVariable("CAPTION", TheNilObj);
    }

    context->SetObjectVariable("ICON", TheNilObj);
    context->SetObjectVariable("RESOURCES", TheNilObj);

    if ( parsePropSheetOpts(context, pcpsd, opts) )
    {
        result = 0;
    }

done_out:
    pcpbd->wndBase->initCode = result;
    return result;
}


/** PropertySheetDialog::getPage()
 *
 *  Gets the page dialog specified by index.
 *
 *  @index  The one-based index of the page whose dialog is desired.
 *
 *  @return  The Rexx dialog object for the page specified.
 *
 *  @notes  Raises an exception if index is not correct.
 *
 *  @remarks  This method is not an interface to a PSM_x message, it is a helper
 *            function for ooDialog programmers.
 */
RexxMethod2(RexxObjectPtr, psdlg_getPage, uint32_t, index, CSELF, pCSelf)
{
    pCPropertySheetDialog pcpsd = (pCPropertySheetDialog)pCSelf;

    uint32_t count = pcpsd->pageCount;
    if ( index < 1 || index > count )
    {
        wrongRangeException(context->threadContext, 1, 1, (int)count, index);
        return NULLOBJECT;
    }

    return pcpsd->rexxPages[index - 1];
}


/** PropertySheetDialog::execute()
 *
 *  Creates a modal property sheet dialog.
 *
 */
RexxMethod2(RexxObjectPtr, psdlg_execute, OPTIONAL_RexxObjectPtr, owner, CSELF, pCSelf)
{
    RexxObjectPtr   result = TheNegativeOneObj;
    HWND            hParent = NULL;

    PROPSHEETPAGE *psp = NULL;
    PROPSHEETHEADER *psh = NULL;

    if ( argumentExists(1) )
    {
        hParent = checkPropSheetOwner(context, owner, 1);
        if ( hParent == NULL )
        {
            goto done_out;
        }
    }

    pCPropertySheetDialog pcpsd = (pCPropertySheetDialog)pCSelf;

    psp = initPropSheetPages(context, pcpsd);
    if ( psp == NULL )
    {
        goto done_out;
    }

    // It is not necessary to set pcpsd->modeless to false, it is false by default.

    psh = initPropSheetHeader(context, pcpsd, psp, hParent);
    if ( psh == NULL )
    {
        goto done_out;
    }

    INT_PTR ret;
    if ( hParent == NULL )
    {
        assignPSDThreadContext(pcpsd, context->threadContext);

        if ( setPropSheetHook(pcpsd) )
        {
            ret = PropertySheet(psh);
        }
        else
        {
            ret = -1;
        }
    }
    else
    {
        ret = (INT_PTR)SendMessage(hParent, WM_USER_CREATEPROPSHEET_DLG, (WPARAM)psh, (LPARAM)pcpsd);
    }

    result = context->WholeNumber(ret);

done_out:
    safeLocalFree(psp);
    safeLocalFree(psh);
    stopDialog(pcpsd->pcpbd, context->threadContext);
    return result;
}


/** PropertySheetDialog::popup()
 *
 *
 *  @notes  AeroWizard dialogs do not support modeless
 */
RexxMethod2(RexxObjectPtr, psdlg_popup, NAME, methodName, CSELF, pCSelf)
{
    pCPropertySheetDialog pcpsd = (pCPropertySheetDialog)pCSelf;
    pCPlainBaseDialog pcpbd = pcpsd->pcpbd;

    PROPSHEETPAGE *psp = NULL;
    PROPSHEETHEADER *psh = NULL;

    if ( pcpsd->isAeroWiz )
    {
        methodCanNotBeInvokedException(context, "popup", pcpsd->rexxSelf, "with the AeroWizard style");
        goto err_out;
    }

    psp = initPropSheetPages(context, pcpsd);
    if ( psp == NULL )
    {
        goto err_out;
    }

    pcpsd->modeless = true;

    psh = initPropSheetHeader(context, pcpsd, psp, NULL);
    if ( psh == NULL )
    {
        goto err_out;
    }

    DWORD threadID;
    bool Release = false;

    EnterCriticalSection(&crit_sec);

    PropSheetThreadArgs threadArgs;
    threadArgs.pcpsd = pcpsd;
    threadArgs.psh = psh;
    threadArgs.release = &Release;

    pcpbd->hDlgProcThread = CreateThread(NULL, 2000, PropSheetLoopThread, &threadArgs, 0, &threadID);

    // Wait for thread to signal us to continue, don't wait if the thread was not created.
    while ( ! Release && pcpbd->hDlgProcThread != NULL )
    {
        Sleep(1);
    }
    LeaveCriticalSection(&crit_sec);

    if ( pcpbd->hDlgProcThread != NULL )
    {
        return TheTrueObj;
    }
    else
    {
        // Something failed in the the thread function.  In that case, things
        // are cleaned up in the thread function.
        return TheFalseObj;
    }

err_out:
    safeLocalFree(psp);
    safeLocalFree(psh);
    stopDialog(pcpsd->pcpbd, context->threadContext);
    return TheFalseObj;
}


/**
 * Checks the requirements, and allocates the PROPSHEETPAGE struct, for adding a
 * single page to an existing property sheet page
 *
 * @param c
 * @param pcpsd
 * @param _page
 * @param index
 * @param abortDialog
 *
 * @return PROPSHEETPAGE*
 */
PROPSHEETPAGE *getPSPMemory(RexxMethodContext *c, pCPropertySheetDialog pcpsd, pCPropertySheetPage *ppcpsp,
                            RexxObjectPtr page, uint32_t index, bool *abortDialog)
{
    PROPSHEETPAGE         *psp   = NULL;

    *abortDialog = false;

    if ( pcpsd->pageCount > MAXPROPPAGES )
    {
        userDefinedMsgException(c->threadContext, TOO_MANY_PROPSHEET_PAGES, MAXPROPPAGES);
        goto done_out;
    }

    if ( index < 1 || index > pcpsd->pageCount )
    {
        wrongRangeException(c->threadContext, 1, 1, pcpsd->pageCount, index);
        goto done_out;
    }

    if ( ! requiredClass(c->threadContext, page, "PROPERTYSHEETPAGE", 1) )
    {
        goto done_out;
    }

    pCPropertySheetPage pcpsp = dlgToPSPCSelf(c, page);
    *ppcpsp = pcpsp;

    if ( pcpsp->activated )
    {
        userDefinedMsgException(c->threadContext, 1, PROPSHEET_PAGE_ALREADY_ACTIVATED);
        goto done_out;
    }

    psp = (PROPSHEETPAGE *)LocalAlloc(LPTR, sizeof(PROPSHEETPAGE));
    if ( psp == NULL )
    {
        *abortDialog = true;
        outOfMemoryException(c->threadContext);
    }

done_out:
    return psp;
    }


/**
 * Updates some of the CSelf fields for a property sheet page that is to be
 * added to a property sheet.
 *
 *
 * @param pcpsd
 * @param ppcpsp
 * @param index   Zero-based index of the page.
 *
 * @remarks  The original property sheet pages have their dialog procedure
 *           thread context set when the property sheet is created.  When pages
 *           are added or inserted we need to remember to set the thread
 *           context.  This thread context is the same as the property sheet's
 *           context.  (The thread context could also be copied from one of the
 *           other pages.)
 */
void updatePageCSelf(pCPropertySheetDialog pcpsd, pCPropertySheetPage pcpsp, uint32_t index)
{
    pcpsp->pageNumber = index;
    pcpsp->rexxPropSheet = pcpsd->rexxSelf;
    pcpsp->cppPropSheet = pcpsd;
    pcpsp->isWizardPage = ! pcpsd->isNotWizard;

    pcpsp->dlgProcContext = pcpsd->dlgProcContext;
    pcpsp->pcpbd->dlgProcContext = pcpsd->dlgProcContext;
}


/** PropertySheetDialog::addPage()
 *
 *  Adds a page to the property sheet at the end of the pages.
 *
 *  @param _page          The Rexx property sheet page dialog.
 *
 *  @param isExteriorPage [OPTIONAL]  If the added page is an exterior page in a
 *                        wizard.  The default is false.  If the property sheet
 *                        is not a wizard, this argument is ignored.  The
 *                        default is false, the page is NOT an exterior page.
 *
 *  @return  True on success, otherwise false.
 *
 *  @notes  Syntax conditions are raised if the _page argument is not correct.
 *          _page must be a PropertySheetPage dialog object.
 *
 *          The dialog object must not have already been "used" as a property
 *          sheet page.  In addition to adding or inserting pages in a prooerty
 *          sheet, pages can be removed from a property sheet.  When the page is
 *          removed, the operating system destroys the underlying Windows
 *          dialog.  Although the Rexx dialog object is still active, that
 *          object can not be reinserted into a property sheet as a page.
 *
 *          It is possible that the user never 'visited' an inserted page before
 *          it was removed. In this case the Rexx dialog object could be used to
 *          add or insert a new page at some later point, because the underlying
 *          Windows dialog would never have been created. The wasActivated
 *          attribute can be used to test for this condition.  However, it is
 *          probably simplier to always instantiate a new Rexx dialog object for
 *          each page added to a property sheet.
 *
 *          The Windows operating system restricts the maximum number of pages
 *          that a property sheet can hold.  The MAXPROPPAGES constant of the
 *          PropertySheetDialog reflects this number.  If the programer tries to
 *          insert or add a page past this number, a condition is raised.
 *
 */
RexxMethod3(RexxObjectPtr, psdlg_addPage, RexxObjectPtr, _page, OPTIONAL_logical_t, isExteriorPage, CSELF, pCSelf)
{
    pCPropertySheetDialog  pcpsd = (pCPropertySheetDialog)pCSelf;

    bool abortDialog;
    uint32_t count = pcpsd->pageCount;

    pCPropertySheetPage pcpsp = NULL;

    PROPSHEETPAGE *psp = getPSPMemory(context, pcpsd, &pcpsp, _page, count, &abortDialog);
    if ( psp == NULL )
    {
        goto err_out;
    }

    pcpsp->psp = psp;
    abortDialog = false;

    updatePageCSelf(pcpsd, pcpsp, count);

    pcpsd->cppPages[count] = pcpsp;
    pcpsd->rexxPages[count] = _page;
    pcpsd->pageCount++;

    if ( ! initPSP(context, pcpsd, psp, count, isExteriorPage ? true : false) )
    {
        goto err_out;
    }

    abortDialog = true;

    pcpsp->hPropSheetPage = CreatePropertySheetPage(psp);
    if ( pcpsp->hPropSheetPage == NULL )
    {
        systemServiceExceptionCode(context->threadContext, API_FAILED_MSG, "CreatePropertySheetPage");
        goto err_out;
    }

    if ( ! PropSheet_AddPage(pcpsd->hDlg, pcpsp->hPropSheetPage) )
    {
        systemServiceExceptionCode(context->threadContext, API_FAILED_MSG, "PropSheet_AddPage");
        goto err_out;
    }

    PropSheet_RecalcPageSizes(pcpsd->hDlg);

    return TheTrueObj;

err_out:
    safeLocalFree(psp);

    if ( pcpsp != NULL )
    {
        pcpsp->psp = NULL;

    if ( pcpsp->hPropSheetPage != NULL )
        {
            DestroyPropertySheetPage(pcpsp->hPropSheetPage);
            pcpsp->hPropSheetPage = NULL;
        }
    }

    if ( abortDialog )
    {
        endDialogPremature(pcpsd->pcpbd, pcpsd->hDlg, RexxConditionRaised);
    }
    return TheFalseObj;
}


/** PropertySheetDialog::insertPage()
 *
 *
 */
RexxMethod4(RexxObjectPtr, psdlg_insertPage, RexxObjectPtr, _page, uint32_t, index, OPTIONAL_logical_t, isExteriorPage, CSELF, pCSelf)
{
    pCPropertySheetDialog  pcpsd = (pCPropertySheetDialog)pCSelf;

    bool     abortDialog;
    uint32_t max = pcpsd->pageCount;

    pCPropertySheetPage pcpsp = NULL;

    PROPSHEETPAGE *psp = getPSPMemory(context, pcpsd, &pcpsp, _page, max, &abortDialog);
    if ( psp == NULL )
    {
        goto err_out;
    }

    pcpsp->psp = psp;
    abortDialog = false;

    index--;
    updatePageCSelf(pcpsd, pcpsp, index);

    if ( index + 1 < max )
    {
        for ( uint32_t i = max - 1, j = max; i >= index; i--, j-- )
        {
            pcpsd->rexxPages[j] = pcpsd->rexxPages[i];
            pcpsd->cppPages[j] = pcpsd->cppPages[i];
            pcpsd->cppPages[j]->pageNumber = j;
        }
    }

    pcpsd->cppPages[index] = pcpsp;
    pcpsd->rexxPages[index] = _page;
    pcpsd->pageCount++;

    if ( ! initPSP(context, pcpsd, psp, index, isExteriorPage ? true : false) )
    {
        goto err_out;
    }

    abortDialog = true;

    pcpsp->hPropSheetPage = CreatePropertySheetPage(psp);
    if ( pcpsp->hPropSheetPage == NULL )
    {
        systemServiceExceptionCode(context->threadContext, API_FAILED_MSG, "CreatePropertySheetPage");
        goto err_out;
    }

    if ( PropSheet_InsertPage(pcpsd->hDlg, index, pcpsp->hPropSheetPage) == 0 )
    {
        systemServiceExceptionCode(context->threadContext, API_FAILED_MSG, "PropSheet_RemovePage");
        goto err_out;
    }

    PropSheet_RecalcPageSizes(pcpsd->hDlg);

    return TheTrueObj;

err_out:
    safeLocalFree(psp);

    if ( pcpsp != NULL )
    {
        pcpsp->psp = NULL;

        if ( pcpsp->hPropSheetPage != NULL )
        {
            DestroyPropertySheetPage(pcpsp->hPropSheetPage);
            pcpsp->hPropSheetPage = NULL;
        }
    }

    if ( abortDialog )
    {
        endDialogPremature(pcpsd->pcpbd, pcpsd->hDlg, RexxConditionRaised);
    }
    return TheFalseObj;
}


/** PropertySheetDialog::removePage()
 *
 *  Removes a page from the property sheet.
 *
 *  @param  index  The one-based index of the page to be removed.
 *
 *  @return  Zero always.
 *
 *  @remarks  If the page being removed is not the last page we need to move the
 *            pointers in the arrays down 1.  However, we also need to adjust
 *            the page numbers in the Rexx page object.  So, rather than do a
 *            memmove for the pointers, we may as well just walk the arrays.
 */
RexxMethod2(RexxObjectPtr, psdlg_removePage, OPTIONAL_uint32_t, index, CSELF, pCSelf)
{
    pCPropertySheetDialog pcpsd = (pCPropertySheetDialog)pCSelf;

    uint32_t max = pcpsd->pageCount;

    if ( argumentOmitted(1) )
    {
        index = max;
    }

    if ( index < 1 || index > max )
    {
        wrongRangeException(context->threadContext, 1, 1, max, index);
        return TheZeroObj;
    }

    index--;
    pCPropertySheetPage pcpspRemove = pcpsd->cppPages[index];

    if ( index + 1 < max )
    {
        for ( uint32_t i = index, j = index + 1; i > max - 1; i++, j++ )
        {
            pcpsd->rexxPages[i] = pcpsd->rexxPages[j];
            pcpsd->cppPages[i] = pcpsd->cppPages[j];
            pcpsd->cppPages[i]->pageNumber = i;
        }
    }

    pcpsd->rexxPages[max] = NULL;
    pcpsd->cppPages[max] = NULL;
    pcpsd->pageCount--;

    pcpspRemove->inRemovePage = true;

    PropSheet_RemovePage(pcpsd->hDlg, index, NULL);
    PropSheet_RecalcPageSizes(pcpsd->hDlg);

    if ( pcpspRemove->hPropSheetPage != NULL )
    {
        DestroyPropertySheetPage(pcpspRemove->hPropSheetPage);
        pcpspRemove->hPropSheetPage = NULL;

        safeLocalFree(pcpspRemove->psp);
        pcpspRemove->psp = NULL;
    }

    pcpspRemove->inRemovePage = false;

    return TheZeroObj;
}


/** PropertySheetDialog::changed()
 *
 */
RexxMethod2(RexxObjectPtr, psdlg_changed, RexxObjectPtr, _page, CSELF, pCSelf)
{
    pCPropertySheetDialog pcpsd = (pCPropertySheetDialog)pCSelf;

    HWND page = getValidPageHwnd(context, pcpsd, _page, 1);
    if ( page != NULL )
    {
        PropSheet_Changed(pcpsd->hDlg, page);
    }
    return TheZeroObj;
}


/** PropertySheetDialog::unchanged()
 *
 */
RexxMethod2(RexxObjectPtr, psdlg_unchanged, RexxObjectPtr, _page, CSELF, pCSelf)
{
    pCPropertySheetDialog pcpsd = (pCPropertySheetDialog)pCSelf;

    HWND page = getValidPageHwnd(context, pcpsd, _page, 1);
    if ( page != NULL )
    {
        PropSheet_UnChanged(pcpsd->hDlg, page);
    }
    return TheZeroObj;
}


/** PropertySheetDialog::setWizButtons()
 *
 *  Enables or disables the Back, Next, and Finish buttons in a wizard.
 *
 *  @param  opts  Keyword(s) that control which buttons are enabled or disabled.
 *
 *  @return  True this property sheet is a wizard, otherwise false.
 *
 *  @notes  Wizards display either three or four buttons below each page. This
 *          method is used to specify which buttons are enabled. Wizards
 *          normally display Back, Cancel, and either a Next or Finish button.
 *          Typically, enable only the Next button for the welcome page, Next
 *          and Back for interior pages, and Back and Finish for the completion
 *          page.  The Cancel button is always enabled.  Normally, setting
 *          FINISH or DISABLEDFINISH replaces the Next button with a Finish
 *          button.  To display Next and Finish buttons simultaneously, set the
 *          WIZARDHASFINISH keyword in the options when the PropertySheetDialog
 *          is instantiated.  Every page will then display all four buttons.
 *
 *          If this property sheet is not a Wizard, this method has no effect.
 *
 *  @remarks  We do not enforce that this is only called for a Wizard, although
 *            maybe we should.
 *
 *            The prop sheet marco does not return a value.
 */
RexxMethod2(RexxObjectPtr, psdlg_setWizButtons, CSTRING, opts, CSELF, pCSelf)
{
    pCPropertySheetDialog pcpsd = (pCPropertySheetDialog)pCSelf;
    if ( pcpsd->isNotWizard )
    {
        return TheFalseObj;
    }

    uint32_t flags = 0;

    if ( StrStrI(opts, "BACK")           != NULL ) flags |= PSWIZB_BACK;
    if ( StrStrI(opts, "NEXT")           != NULL ) flags |= PSWIZB_NEXT;
    if ( StrStrI(opts, "FINISH")         != NULL ) flags |= PSWIZB_FINISH;
    if ( StrStrI(opts, "DISABLEDFINISH") != NULL ) flags |= PSWIZB_DISABLEDFINISH;

    PropSheet_SetWizButtons(pcpsd->hDlg, flags);
    return TheTrueObj;
}


/** PropertySheetDialog::showWizButtons()
 *
 *  Show or hide buttons in a wizard.
 *
 *  @param  opts       One or more of the keyword values that specify which
 *                     property sheet buttons are to be shown. If a button value
 *                     is included in both this argument and the optsButtons
 *                     argument, then the button is shown.
 *
 *  @param optsButtons One or more of the same keywords used in opts. Here,
 *                     they specify which property sheet buttons are to be shown
 *                     or hidden. If a keyword appears in this argument but not
 *                     in opts, it indicates that the button should be hidden.
 *
 *  @param  Returns true if this is a Wizard property sheet on Vista on later,
 *          otherwise false.
 *
 *  @notes  This method requires Vista or later, a condition is raised if the OS
 *          is not Vista or later.
 *
 *          This method has no effect if the property sheet is not a Wizard.
 */
RexxMethod3(RexxObjectPtr, psdlg_showWizButtons, CSTRING, opts, CSTRING, optsButtons, CSELF, pCSelf)
{
    pCPropertySheetDialog pcpsd = (pCPropertySheetDialog)pCSelf;

    if ( ! requiredOS(context, "showWizButtons", "Vista", Vista_OS) || pcpsd->isNotWizard )
    {
        return TheFalseObj;
    }

    uint32_t flags = 0;
    uint32_t buttons = 0;

    if ( StrStrI(opts, "BACK")   != NULL ) flags |= PSWIZB_BACK;
    if ( StrStrI(opts, "NEXT")   != NULL ) flags |= PSWIZB_NEXT;
    if ( StrStrI(opts, "FINISH") != NULL ) flags |= PSWIZB_FINISH;
    if ( StrStrI(opts, "CANCEL") != NULL ) flags |= PSWIZB_CANCEL;

    if ( StrStrI(optsButtons, "BACK")   != NULL ) buttons |= PSWIZB_BACK;
    if ( StrStrI(optsButtons, "NEXT")   != NULL ) buttons |= PSWIZB_NEXT;
    if ( StrStrI(optsButtons, "FINISH") != NULL ) buttons |= PSWIZB_FINISH;
    if ( StrStrI(optsButtons, "CANCEL") != NULL ) buttons |= PSWIZB_CANCEL;

    PropSheet_ShowWizButtons(pcpsd->hDlg, flags, buttons);
    return TheTrueObj;
}


/** PropertySheetDialog::apply()
 *
 *  Simulates the selection of the Apply button, indicating that one or more
 *  pages have changed and the changes need to be validated and recorded.
 *
 */
RexxMethod1(RexxObjectPtr, psdlg_apply, CSELF, pCSelf)
{
    pCPropertySheetDialog pcpsd = (pCPropertySheetDialog)pCSelf;

    return PropSheet_Apply(pcpsd->hDlg) ? TheTrueObj : TheFalseObj;
}


/** PropertySheetDialog::cancelToClose()
 *
 *  Used when changes made since the most recent PSN_APPLY notification cannot
 *  be canceled.
 *
 */
RexxMethod1(RexxObjectPtr, psdlg_cancelToClose, CSELF, pCSelf)
{
    pCPropertySheetDialog pcpsd = (pCPropertySheetDialog)pCSelf;

    PropSheet_CancelToClose(pcpsd->hDlg);
    return TheZeroObj;
}


/** PropertySheetDialog::getCurrentPageHwnd()
 *
 *  Retrieves a handle to the window of the current page of a property sheet.
 *
 */
RexxMethod1(RexxObjectPtr, psdlg_getCurrentPageHwnd, CSELF, pCSelf)
{
    pCPropertySheetDialog pcpsd = (pCPropertySheetDialog)pCSelf;
    return pointer2string(context, PropSheet_GetCurrentPageHwnd(pcpsd->hDlg));
}


/** PropertySheetDialog::getResult()
 *
 *  Used by modeless property sheets to retrieve the same information returned
 *  to modal property sheets.
 *
 */
RexxMethod1(RexxObjectPtr, psdlg_getResult, CSELF, pCSelf)
{
    pCPropertySheetDialog pcpsd = (pCPropertySheetDialog)pCSelf;

    RexxObjectPtr result;
    switch ( pcpsd->getResultValue )
    {
        case -1 :
            result = TheNegativeOneObj;
            break;
        case ID_PSRESTARTWINDOWS :
            result = context->String("RESTARTWINDOWS");
            break;
        case ID_PSREBOOTSYSTEM :
            result = context->String("REBOOTSYSTEM");
            break;
        case 0 :
            result = TheZeroObj;
            break;
        default :
            result = TheNilObj;
            break;
    }
    return result;
}


/** PropertySheetDialog::getTabControl()
 *
 *  Retrieves the Rexx tab control object for the tab control of the property
 *  sheet.
 *
 *  Note that in this case, once we instantiate the Rexx tab object, we do not
 *  want it garbage collected so we use specify true in the
 *  createControlFromHwnd() call to have it put in the dialog's control bag.
 */
RexxMethod1(RexxObjectPtr, psdlg_getTabControl, CSELF, pCSelf)
{
    pCPropertySheetDialog pcpsd = (pCPropertySheetDialog)pCSelf;
    return createControlFromHwnd(context, pcpsd->pcpbd, PropSheet_GetTabControl(pcpsd->hDlg), winTab, true);
}


/** PropertySheetDialog::hwndToIndex()
 *
 *  Takes a window handle of the property sheet page and returns its one-based
 *  index.
 *
 *  @param  hwnd  The window handle of a property sheet page.
 *
 *  @return  The one-based index of the page on success, or 0 on error.
 */
RexxMethod2(uint32_t, psdlg_hwndToIndex, POINTERSTRING, hwnd, CSELF, pCSelf)
{
    pCPropertySheetDialog pcpsd = (pCPropertySheetDialog)pCSelf;

    int index = PropSheet_HwndToIndex(pcpsd->hDlg, hwnd);
    return ++index;
}


/** PropertySheetDialog::idToIndex()
 *
 *  Takes a property sheet page ID and returns its one-based index.
 *
 *  @param  id  The property sheet page ID.
 *
 *  @return  The one-based index of the page on success, or 0 on error.
 */
RexxMethod2(uint32_t, psdlg_idToIndex, POINTER, id, CSELF, pCSelf)
{
    pCPropertySheetDialog pcpsd = (pCPropertySheetDialog)pCSelf;

    int index = PropSheet_IdToIndex(pcpsd->hDlg, id);
    return ++index;
}


/** PropertySheetDialog::indexToID()
 *
 *  Given the index of a page, returns its ID.
 *
 *  Page IDs are different depending on the type of the dialog page,
 *  UserPSPDialog, ResPSPDialog, etc..  In order to get the correct ID, the Rexx
 *  programmer must use this method.
 *
 *  There are also two special case values.  The page ID would most commonly be
 *  used in the setActive, wizBack, or wizNext event notification methods that
 *  signal a page is being changed.  To accept the page change, 0 is returned,
 *  and to cancel the page change -1 is returned.  So, 0 and -1 are acceptable
 *  here and return the proper value for those methods.
 *
 *  @param  index  The one-based index of the page.  The special values 0 and -1
 *                 are also acceptable.
 *
 *  @return  The proper page ID for the index.
 *
 *  @notes  A syntax condition is raised if the index is not -1, 0, or within
 *          the range of existing pages.
 *
 *  @remarks  In Windows the page id is either the resource ID of the dialog for
 *            a dialog template bound to an executable, or the pointer to the
 *            in-memory template for a dynamically constructed template.  We
 *            keep track of the proper id when the template is used and return
 *            it here to the programmer when requested.
 */
RexxMethod2(RexxObjectPtr, psdlg_indexToID, int32_t, index, CSELF, pCSelf)
{
    pCPropertySheetDialog pcpsd = (pCPropertySheetDialog)pCSelf;

    INT_PTR result = -2;

    int max = (int)pcpsd->pageCount;

    if ( index < -1 || index > max )
    {
        wrongRangeException(context->threadContext, 1, -1, max, index);
    }
    else if ( index < 1 )
    {
        result = index;
    }
    else
    {
        pCPropertySheetPage pcpsp = pcpsd->cppPages[index - 1];
        result = pcpsp->pageID;
    }

    return context->NewPointer((void *)result);
}


/** PropertySheetDialog::indexToHwnd()
 *
 *  Given the index to a page, returns the page dialog's window handle.
 *
 *  @param  index  The one-based page index.
 *
 *  @return  The window handle for the page.
 *
 ** PropertySheetDialog::indexToPage()
 *
 *  Given the index to a page, returns its HPROPSHEETPAGE handle
 *
 *  @param  index  The one-based page index.
 *
 *  @return  The HPROPSHEETPAGE handle for the page.
 *
 *  @notes  A syntax condition is raised if index is not the index of an
 *          existing page in the propertye sheet.
 *
 */
RexxMethod3(RexxObjectPtr, psdlg_indexToHandle, int, index, NAME, method, CSELF, pCSelf)
{
    pCPropertySheetDialog pcpsd = (pCPropertySheetDialog)pCSelf;

    int max = (int)pcpsd->pageCount;

    if ( index < 1 || index > max )
    {
        wrongRangeException(context->threadContext, 1, 1, max, index);
        return NULL;
    }

    index--;
    if ( *(method + 7) == 'H' )
    {
        return pointer2string(context, PropSheet_IndexToHwnd(pcpsd->hDlg, index));
    }
    else
    {
        RexxMethodContext *c = context;
        return c->NewPointer(PropSheet_IndexToPage(pcpsd->hDlg, index));
    }
}


/** PropertySheetDialog::pageToIndex()
 *
 *  Takes the handle of a property sheet page and returns its one-based index.
 *
 *  @param  hPage  The handle of a property sheet page.
 *
 *  @return  The one-based index of the page on success, or 0 on error.
 */
RexxMethod2(uint32_t, psdlg_pageToIndex, POINTER, hPage, CSELF, pCSelf)
{
    pCPropertySheetDialog pcpsd = (pCPropertySheetDialog)pCSelf;

    int index = PropSheet_HwndToIndex(pcpsd->hDlg, hPage);
    return ++index;
}


/** PropertySheetDialog::pressButton()
 *
 *
 */
RexxMethod2(RexxObjectPtr, psdlg_pressButton, CSTRING, button, CSELF, pCSelf)
{
    pCPropertySheetDialog pcpsd = (pCPropertySheetDialog)pCSelf;

    int flag = 0;

    if ( StrStrI(button, "APPLYNOW")    != NULL ) flag = PSBTN_APPLYNOW;
    else if ( StrStrI(button, "BACK")   != NULL ) flag = PSBTN_BACK;
    else if ( StrStrI(button, "CANCEL") != NULL ) flag = PSBTN_CANCEL;
    else if ( StrStrI(button, "FINISH") != NULL ) flag = PSBTN_FINISH;
    else if ( StrStrI(button, "HELP")   != NULL ) flag = PSBTN_HELP;
    else if ( StrStrI(button, "NEXT")   != NULL ) flag = PSBTN_NEXT;
    else if ( StrStrI(button, "OK")     != NULL ) flag = PSBTN_OK;
    else
    {
        wrongArgValueException(context->threadContext, 1, VALID_PROPSHEET_BUTTONS, button);
        return TheZeroObj;
    }

    PropSheet_PressButton(pcpsd->hDlg, flag);
    return TheZeroObj;
}


/** PropertySheetDialog::querySiblings()
 *
 *
 */
RexxMethod3(int32_t, psdlg_querySiblings, RexxObjectPtr, wParam, RexxObjectPtr, lParam, CSELF, pCSelf)
{
    pCPropertySheetDialog pcpsd = (pCPropertySheetDialog)pCSelf;
    return (int32_t)PropSheet_QuerySiblings(pcpsd->hDlg, (WPARAM)wParam, (LPARAM)lParam);
}


/** PropertySheetDialog::setCurSel()
 *
 *  Activates the specified page in a property sheet.
 *
 *  @param  hPage  [OPTIONAL] The property sheet page handle.
 *  @param  index  [OPTIONAL] The one-based page index.
 *
 *  @return  True on success, otherwise false.
 *
 *  @notes The property sheet page to active can be specified by either the
 *         property sheet page handle, or the page index, or both.  If both are
 *         specified, the property sheet page handle takes precedence.
 *
 *         Although both arguments are optional, they are optional individually.
 *         At least one of the arguments must be specified or a condition is
 *         raised.  In addition, if the index argument is used to specify the
 *         page is not a valid index, a condition is raised.
 */
RexxMethod3(RexxObjectPtr, psdlg_setCurSel, OPTIONAL_POINTER, hPage, OPTIONAL_int32_t, index, CSELF, pCSelf)
{
    pCPropertySheetDialog pcpsd = (pCPropertySheetDialog)pCSelf;

    RexxObjectPtr result = TheFalseObj;

    if ( argumentExists(1) )
    {
        result = PropSheet_SetCurSel(pcpsd->hDlg, (HPROPSHEETPAGE)hPage, NULL) ? TheTrueObj : TheFalseObj;
    }
    else
    {
        if ( argumentOmitted(2) )
        {
            missingArgException(context->threadContext, 2);
            goto done_out;
        }

        int max = (int)pcpsd->pageCount;

        if ( index < 1 || index > max )
        {
            wrongRangeException(context->threadContext, 2, 1, max, index);
            goto done_out;
        }

        index--;
        result = PropSheet_SetCurSel(pcpsd->hDlg, NULL, index) ? TheTrueObj : TheFalseObj;
    }

done_out:
    return result;
}


/** PropertySheetDialog::setCurSelByID()
 *
 *  Activates the specified page in a property sheet.
 *
 *  @param  id  The property sheet page ID.
 *
 *  @return  True on success, otherwise false.
 *
 *  @notes The property sheet page ID can be obtained using the indexToID()
 *         method.  Do not confuse a page ID with a page handle, they are 2
 *         separate things.  The only way for the Rexx programmer to obtain the
 *         page ID is through indexToID().
 */
RexxMethod2(RexxObjectPtr, psdlg_setCurSelByID, POINTER, id, CSELF, pCSelf)
{
    pCPropertySheetDialog pcpsd = (pCPropertySheetDialog)pCSelf;

    RexxObjectPtr result = PropSheet_SetCurSelByID(pcpsd->hDlg, id) ? TheTrueObj : TheFalseObj;
    return result;
}


/** PropertySheetDialog::setFinishText()
 *
 *  Sets the text of the Finish button in a wizard, shows and enables the
 *  button, and hides the Next and Back buttons.
 *
 *  @param  text  The text for the Finish button.
 *
 *  @return  0, always.
 *
/** PropertySheetDialog::setNextText()
 *
 *  Sets the text of the Next button in a wizard.
 *
 *  @param  text  The text for the Next button.
 *
 *  @return  0, always.
 *
 */
RexxMethod3(RexxObjectPtr, psdlg_setButtonText, CSTRING, text, NAME, method, CSELF, pCSelf)
{
    pCPropertySheetDialog pcpsd = (pCPropertySheetDialog)pCSelf;

    if ( *(method + 3) == 'F' )
    {
        PropSheet_SetFinishText(pcpsd->hDlg, text);
    }
    else
    {
        PropSheet_SetNextText(pcpsd->hDlg, text);
    }
    return TheZeroObj;
}


#if 0
/** PropertySheetDialog::setHeaderBitmap()
 *
 *  The property sheet PropSheet_SetHeaderBitmap and
 *  PropSheet_SetHeaderBitmapResource macros are apparently not implemented.  I
 *  wrote this code before discovering that.  Going to save the code for future
 *  use.
 */
RexxMethod3(RexxObjectPtr, psdlg_setHeaderBitmap, uint32_t, index, RexxObjectPtr, bitmap, CSELF, pCSelf)
{
    pCPropertySheetDialog pcpsd = (pCPropertySheetDialog)pCSelf;

    if ( ! requiredOS(context, "setHeaderBitmap", "Vista", Vista_OS) || pcpsd->isNotWizard )
    {
        goto done_out;
    }

    uint32_t max = pcpsd->pageCount;

    if ( index < 1 || index > max )
    {
        wrongRangeException(context->threadContext, 1, 1, max, index);
        goto done_out;
    }

    index--;

    POODIMAGE oodImage = rxGetImageBitmap(context, bitmap, 2);
    if ( oodImage == NULL )
    {
        goto done_out;
    }

    PropSheet_SetHeaderBitmap(pcpsd->hDlg, index, oodImage->hImage);

done_out:
    return TheZeroObj;
}
#endif


/** PropertySheetDialog::setHeaderSubtitle()
 *  PropertySheetDialog::setHeaderTitle()
 *
 *  Resets the text for a page in a property sheet wizard dialog.  These methods
 *  are not available for Aero wizards
 *
 *  The same native function handles the setHeaderTitle() and
 *  setHeaderSubtitle() methods.
 *
 *  @param  index    The one-based index of the page whose text is being
 *                   changed.
 *  @param  newText  New text for the page.
 *
 *  @return  Zero, always.
 */
RexxMethod4(RexxObjectPtr, psdlg_resetPageText, uint32_t, index, CSTRING, newText, NAME, method, CSELF, pCSelf)
{
    pCPropertySheetDialog pcpsd = (pCPropertySheetDialog)pCSelf;

    if ( pcpsd->isAeroWiz || pcpsd->isNotWizard )
    {
        goto done_out;
    }

    uint32_t max = pcpsd->pageCount;

    if ( index < 1 || index > max )
    {
        wrongRangeException(context->threadContext, 1, 1, max, index);
        goto done_out;
    }

    pCPropertySheetPage pcpsp = pcpsd->cppPages[--index];

    if ( *(method + 9) == 'S' )
    {
        if ( setPageText(context, pcpsp, newText, headerSubtext) )
        {
            PropSheet_SetHeaderSubTitle(pcpsd->hDlg, index, pcpsp->headerSubTitle);

        }
    }
    else
    {
        if ( setPageText(context, pcpsp, newText, headerText) )
        {
            PropSheet_SetHeaderTitle(pcpsd->hDlg, index, pcpsp->headerTitle);

        }
    }

done_out:
    return TheZeroObj;
}


/** PropertySheetDialog::setTitle()
 *
 *  Sets the title for a property sheet dialog.  This method is not available
 *  for Aero wizards, and does nothing in other wizards
 *
 *  @param  title             The title for the property sheet.
 *  @param  addPropertiesFor  [OPTIONAL] If true the text "Properties for" is
 *                            prefixed to the title.  If omitted or false, there
 *                            is no text added.
 *
 *  @return  Zero, always.
 *
 *  @remarks  The MSDN documentation seems to indicate that this is valid for
 *            wizards, but experimentation shows it does not work for any
 *            wizard.
 */
RexxMethod3(RexxObjectPtr, psdlg_setTitle, CSTRING, title, OPTIONAL_logical_t, addPropertiesFor, CSELF, pCSelf)
{
    pCPropertySheetDialog pcpsd = (pCPropertySheetDialog)pCSelf;

    if ( pcpsd->isAeroWiz )
    {
        goto done_out;
    }

    PropSheet_SetTitle(pcpsd->hDlg, addPropertiesFor, title);

done_out:
    return TheZeroObj;
}


/** PropertySheetDialog::test()
 *
 *  Testing method
 */
RexxMethod1(RexxObjectPtr, psdlg_test, CSELF, pCSelf)
{
    printf("No test set up at this time\n");
    printf("Make version for 6.1=%d\n", MAKEVERSION(6, 1));
    printf("Make version for 6.01=%d\n", MAKEVERSION(6, 01));
    return TheZeroObj;
}


/**
 *  Methods for the .PropertySheetPage mixin class.
 */
#define PROPERTYSHEETPAGE_CLASS  "PropertySheetPage"


/**
 * Performs the initialization of the PropertySheetPage mixin class.
 *
 * We create the CSelf struct for the class and then send te struct to the
 * init_propertySheetPage() method.  That method will raise an exception if its
 * arg is not a RexxBufferObject.  This implies that the PropertySheetPage mixin
 * class can only be initialized through the native API.
 *
 * The plain base dialog, dialog control, and window classes in ooDialog inherit
 * WindowBase.
 *
 * @param c        Method context we are operating in.
 *
 * @param
 *
 * @param self     The Rexx object that inherited WindowBase.
 *
 * @return True on success, otherwise false.  If false an exception has been
 *         raised.
 *
 * @remarks
 */
bool initPropertySheetPage(RexxMethodContext *c, pCPlainBaseDialog pcpbd, pCDynamicDialog pcdd,
                           RexxObjectPtr self)
{
    RexxBufferObject obj = c->NewBuffer(sizeof(CPropertySheetPage));
    if ( obj == NULLOBJECT )
    {
        return false;
    }

    pCPropertySheetPage pcpsp = (pCPropertySheetPage)c->BufferData(obj);
    memset(pcpsp, 0, sizeof(CPropertySheetPage));

    pcpsp->pcpbd         = pcpbd;
    pcpsp->pcdd          = pcdd;
    pcpsp->rexxPropSheet = TheNilObj;
    pcpsp->rexxSelf      = self;
    pcpsp->interpreter   = pcpbd->interpreter;

    pcpbd->dlgPrivate = pcpsp;

    c->SendMessage1(self, "INIT_PROPERTYSHEETPAGE", obj);

    return true;
}


static void parsePageOpts(RexxMethodContext *c, pCPropertySheetPage pcpsp, CSTRING options)
{
    uint32_t opts = PSP_DEFAULT | PSP_USECALLBACK;

    if ( pcpsp->pageType != oodResPSPDialog )
    {
        pcpsp->cx = PROP_MED_CXDLG;
        pcpsp->cy = PROP_MED_CYDLG;
        if ( options != NULL )
        {
            if ( StrStrI(options, "LARGE") )
            {
                pcpsp->cx = PROP_LG_CXDLG;
                pcpsp->cy = PROP_LG_CYDLG;
            }
            else if ( StrStrI(options, "SMALL") )
            {
                pcpsp->cx = PROP_SM_CXDLG;
                pcpsp->cy = PROP_SM_CYDLG;
            }
        }
    }

    if ( options != NULL )
    {
        if ( StrStrI(options, "USETITLE")          != NULL ) opts |= PSP_USETITLE;
        if ( StrStrI(options, "RTLREADING")        != NULL ) opts |= PSP_RTLREADING;
        if ( StrStrI(options, "HASHELP")           != NULL ) opts |= PSP_HASHELP;
        if ( StrStrI(options, "USEREFPARENT")      != NULL ) opts |= PSP_USEREFPARENT;
        if ( StrStrI(options, "PREMATURE")         != NULL ) opts |= PSP_PREMATURE;
        if ( StrStrI(options, "HIDEHEADER")        != NULL ) opts |= PSP_HIDEHEADER;
        if ( StrStrI(options, "USEHEADERTITLE")    != NULL ) opts |= PSP_USEHEADERTITLE;
        if ( StrStrI(options, "USEHEADERSUBTITLE") != NULL ) opts |= PSP_USEHEADERSUBTITLE;
        if ( StrStrI(options, "USEFUSIONCONTEXT")  != NULL ) opts |= PSP_USEFUSIONCONTEXT;
    }
    pcpsp->pageFlags = opts;
}

bool initPageDlgFrame(RexxMethodContext *c, pCPropertySheetPage pcpsp)
{
    pCPlainBaseDialog pcpbd = pcpsp->pcpbd;
    pCDynamicDialog pcdd = pcpsp->pcdd;

    if ( pcpsp->pageTitle == NULL )
    {
        char buf[32];
        _snprintf(buf, sizeof(buf), "Page %d", pcpsp->pageNumber);

        if ( ! setPageText(c, pcpsp, buf, pageText) )
        {
            return false;
        }
    }

    pcpbd->wndBase->sizeX = pcpsp->cx;
    pcpbd->wndBase->sizeY = pcpsp->cy;

    uint32_t style = DS_SHELLFONT | DS_3DLOOK | DS_CONTROL | WS_CHILD | WS_TABSTOP;

    DLGTEMPLATEEX *pBase;

    return startDialogTemplate(c, &pBase, pcdd, 0, 0, pcpsp->cx, pcpsp->cy, NULL, "", pcpbd->fontName, pcpbd->fontSize, style);
}

/**
 * Creates the in-memory dialog template for a user property sheet page.
 *
 * @param c      Method context we are operating in.
 * @param pcpsp  Pointer to the property sheet page CSelf
 *
 * @return True on success false on error.
 *
 * @remarks  For a user defined dialog template we always use "" for the dialog
 *           title in the template.  If the user has not assigned a page title,
 *           we create one for him.  When the
 *           see that the pageTitle field is not null and add the PSP
 */
RexxObjectPtr initUserTemplate(RexxMethodContext *c, pCPropertySheetPage pcpsp)
{
    if ( ! initPageDlgFrame(c, pcpsp) )
    {
        goto err_out;
    }

    pCPlainBaseDialog pcpbd = pcpsp->pcpbd;
    pCDynamicDialog pcdd = pcpsp->pcdd;

    RexxObjectPtr result = c->SendMessage0(pcpbd->rexxSelf, "DEFINEDIALOG");

    if ( ! c->CheckCondition() && pcdd->active != NULL )
    {
        pcpsp->pageID = (INT_PTR)pcdd->base;

        // Set the number of dialog items field in the dialog template.
        ((DLGTEMPLATEEX *)pcdd->base)->cDlgItems = (WORD)pcdd->count;

        return TheTrueObj;
    }

err_out:
    // No underlying windows dialog can be created.  We still need to clean up
    // the CSelf struct, which was allocated when the Rexx dialog object was
    // instantiated.  But I'm not sure it needs to be done here.
    stopDialog(pcpbd, c->threadContext);
    return TheFalseObj;
}

RexxObjectPtr initRcTemplate(RexxMethodContext *c, pCPropertySheetPage pcpsp)
{
    pCPlainBaseDialog pcpbd = pcpsp->pcpbd;
    pCDynamicDialog pcdd = pcpsp->pcdd;

    RexxObjectPtr result = c->SendMessage1(pcpsp->rexxSelf, "LOADFRAME", c->UnsignedInt32(pcdd->expected));

    // Checking that result is 1 is probably sufficient.
    if ( ! isInt(0, result, c->threadContext) || c->CheckCondition() )
    {
        goto err_out;
    }

    if ( ! initPageDlgFrame(c, pcpsp) )
    {
        goto err_out;
    }

    result = c->SendMessage1(pcpsp->rexxSelf, "LOADITEMS", pcpsp->extraOpts);

    if ( ! isInt(0, result, c->threadContext) || pcdd->active == NULL )
    {
        goto err_out;
    }

    result = c->SendMessage0(pcpbd->rexxSelf, "DEFINEDIALOG");

    if ( ! c->CheckCondition() && pcdd->active != NULL )
    {
        pcpsp->pageID = (INT_PTR)pcdd->base;

        // Set the number of dialog items field in the dialog template.
        ((DLGTEMPLATEEX *)pcdd->base)->cDlgItems = (WORD)pcdd->count;

        return TheTrueObj;
    }

err_out:
    // No underlying windows dialog can be created.  We still need to clean up
    // the CSelf struct, which was allocated when the Rexx dialog object was
    // instantiated.  But I'm not sure it needs to be done here.
    stopDialog(pcpbd, c->threadContext);
    return TheFalseObj;
}

RexxObjectPtr initResTemplate(RexxMethodContext *c, pCPropertySheetPage pcpsp)
{
    pCPlainBaseDialog pcpbd = pcpsp->pcpbd;

    if ( ! loadResourceDLL(pcpbd, pcpbd->library) )
    {
        goto err_out;
    }
    pcpsp->pageID = (INT_PTR)pcpsp->resID;

    return TheTrueObj;

err_out:
    // No underlying windows dialog can be created.  We still need to clean up
    // the CSelf struct, which was allocated when the Rexx dialog object was
    // instantiated.  But I'm not sure it needs to be done here.
    stopDialog(pcpbd, c->threadContext);
    return TheFalseObj;
}

/** PropertySheetPage::init()  [Class method]
 *
 *  Used to capture the PropertySheetPage class object.  This is used for scoped
 *  look ups of the CSelf.
 */
RexxMethod1(RexxObjectPtr, psp_init_cls, OSELF, self)
{
    if ( isOfClassType(context, self, PROPERTYSHEETPAGE_CLASS) )
    {
        ThePropertySheetPageClass = (RexxClassObject)self;
    }
    return NULLOBJECT;
}


/** PropertySheetPage::propSheet()  [Attribute get]
 *
 */
RexxMethod1(RexxObjectPtr, psp_propSheet_atr, CSELF, pCSelf)
{
    pCPropertySheetPage pcpsp = (pCPropertySheetPage)pCSelf;
    return pcpsp->rexxPropSheet;
}


/** PropertySheetPage::wasActivated()  [Attribute get]
 *
 */
RexxMethod1(RexxObjectPtr, psp_wasActivated_atr, CSELF, pCSelf)
{
    pCPropertySheetPage pcpsp = (pCPropertySheetPage)pCSelf;
    return pcpsp->activated ? TheTrueObj : TheFalseObj;
}


/** PropertySheetPage::pageID()  [Attribute get]
 *
 */
RexxMethod1(POINTER, psp_pageID_atr, CSELF, pCSelf)
{
    pCPropertySheetPage pcpsp = (pCPropertySheetPage)pCSelf;
    return (POINTER)pcpsp->pageID;
}


/** PropertySheetPage::pageIndex()  [Attribute get]
 *
 */
RexxMethod1(uint32_t, psp_pageNumber_atr, CSELF, pCSelf)
{
    pCPropertySheetPage pcpsp = (pCPropertySheetPage)pCSelf;
    return pcpsp->pageNumber + 1;
}


/** PropertySheetPage::cx                [Attribute get]
 *  PropertySheetPage::cy
 *
 */
RexxMethod2(uint32_t, psp_getcx, NAME, name, CSELF, pCSelf)
{
    pCPropertySheetPage pcpsp = (pCPropertySheetPage)pCSelf;

    return *(name + 1) == 'X' ? pcpsp->cx : pcpsp->cy;
}


/** PropertySheetPage::cx                [Attribute set]
 *  PropertySheetPage::cy
 *
 */
RexxMethod3(RexxObjectPtr, psp_setcx, uint32_t, dlgUnit, NAME, name, CSELF, pCSelf)
{
    pCPropertySheetPage pcpsp = (pCPropertySheetPage)pCSelf;

    *(name + 1) == 'X' ? pcpsp->cx = dlgUnit : pcpsp->cy = dlgUnit;
    return NULLOBJECT;
}


/** PropertySheetPage::pageTitle()       [Attribute get]
 *  PropertySheetPage::headerTitle()
 *  PropertySheetPage::headerSubTitle()
 */
RexxMethod2(RexxObjectPtr, psp_getPageTitle, NAME, name, CSELF, pCSelf)
{
    pCPropertySheetPage pcpsp = (pCPropertySheetPage)pCSelf;

    switch ( *(name + 7) )
    {
        case 'L' :
            return pcpsp->pageTitle == NULL ? TheNilObj : context->String(pcpsp->pageTitle);
        case 'I' :
            return pcpsp->headerTitle == NULL ? TheNilObj : context->String(pcpsp->headerTitle);
    }
    return pcpsp->headerSubTitle == NULL ? TheNilObj : context->String(pcpsp->headerSubTitle);
}


/** PropertySheetPage::pageTitle()       [Attribute set]
 *  PropertySheetPage::headerTitle()
 *  PropertySheetPage::headerSubTitle()
 */
RexxMethod3(RexxObjectPtr, psp_setPageTitle, CSTRING, text, NAME, name, CSELF, pCSelf)
{
    pCPropertySheetPage pcpsp = (pCPropertySheetPage)pCSelf;

    switch ( *(name + 7) )
    {
        case 'L' :
            setPageText(context, pcpsp, text, pageText);
            break;

        case 'I' :
            setPageText(context, pcpsp, text, headerText);
            break;

        case 'U' :
            setPageText(context, pcpsp, text, headerSubtext);
            break;
    }
    return NULLOBJECT;
}


/** PropertySheetPage::wantAccelerators()   [Attribute]
 *  PropertySheetPage::wantGetObject()
 *
 */
RexxMethod2(RexxObjectPtr, psp_getWantNotification, NAME, methName, CSELF, pCSelf)
{
    pCPropertySheetPage pcpsp = (pCPropertySheetPage)pCSelf;
    RexxObjectPtr result = TheFalseObj;

    if ( *(methName + 4) == 'A' )
    {
        result = pcpsp->wantAccelerators ? TheTrueObj : TheFalseObj;
    }
    else
    {
        result = pcpsp->wantGetObject ? TheTrueObj : TheFalseObj;
    }
    return result;
}


RexxMethod3(RexxObjectPtr, psp_setWantNotification, logical_t, want, NAME, methName, CSELF, pCSelf)
{
    pCPropertySheetPage pcpsp = (pCPropertySheetPage)pCSelf;

    if ( *(methName + 4) == 'A' )
    {
        pcpsp->wantAccelerators = want ? true : false;
    }
    else
    {
        pcpsp->wantGetObject = want ? true : false;
    }
    return NULLOBJECT;
}


/** PropertySheetPage::resources()      [Attribute set]
 *
 */
RexxMethod2(RexxObjectPtr, psp_setResources_atr, RexxObjectPtr, resourceImage, CSELF, pCSelf)
{
    pCPropertySheetPage pcpsp = (pCPropertySheetPage)pCSelf;

    PRESOURCEIMAGE ri = rxGetResourceImage(context, resourceImage, 1);
    if ( ri != NULL )
    {
        pcpsp->hInstance = ri->hMod;
        context->SetObjectVariable("RESOURCES", resourceImage); // TODO this won't work, did you test it? ;-)
    }
    return NULLOBJECT;
}

/** PropertySheetPage::tabIcon()        [Attribute set]
 *
 *  Sets the icon used for the tab.  The user can specify the icon as either a
 *  resource ID (numeric or symbolic) or as an .Image object.
 *
 *  @remarks  If the user specifies the icon as an .Image object, then it has to
 *            be an icon image, not some other type of image, like a bitmap,
 *            etc.  The rxGetImageIcon() call will raise an exception if the
 *            image is not an icon.
 */
RexxMethod2(RexxObjectPtr, psp_setTabIcon_atr, RexxObjectPtr, icon, CSELF, pCSelf)
{
    pCPropertySheetPage pcpsp = (pCPropertySheetPage)pCSelf;

    bool    isImage;
    uint8_t type;

    INT_PTR result = getImageOrID(context, pcpsp->rexxSelf, icon, 1, &isImage, &type);
    if ( result != 0 )
    {
        if ( isImage )
        {
            if ( type != IMAGE_ICON )
            {
                wrongArgValueException(context->threadContext, 1, "Icon, Cursor", getImageTypeName(type));
                goto done_out;
            }
            pcpsp->hIcon = (HICON)result;
        }
        else
        {
            pcpsp->iconID = (uint32_t)result;
        }

        context->SetObjectVariable("TABICON", icon);
    }

done_out:
    return NULLOBJECT;
}


/** PropertySheetPage::init_propertySheetPage()
 *
 *  The property sheet page initialization.  This can only be done from native
 *  code.
 *
 *  @param cSelf  The Rexx Buffer object containing the CSelf for a property
 *                sheet page.
 */
RexxMethod1(logical_t, psp_init_propertySheetPage, RexxObjectPtr, cSelf)
{
    if ( ! context->IsBuffer(cSelf) )
    {
        wrongClassException(context->threadContext, 1, "Buffer");
        return FALSE;
    }

    context->SetObjectVariable("CSELF", cSelf);
    return TRUE;
}


/** PropertySheetPage::initTemplate()
 *
 *
 */
RexxMethod1(RexxObjectPtr, psp_initTemplate, CSELF, pCSelf)
{
    // TODO validate CSelf.
    pCPropertySheetPage pcpsp = (pCPropertySheetPage)pCSelf;

    switch ( pcpsp->pageType )
    {
        case oodUserPSPDialog :
            return initUserTemplate(context, pcpsp);
            break;

        case oodRcPSPDialog :
            return initRcTemplate(context, pcpsp);
            break;

        case oodResPSPDialog :
            return initResTemplate(context, pcpsp);
            break;

        default :
            break;

    }
    return TheFalseObj;
}


/** PropertySheetPage::setSize()
 *
 *
 */
RexxMethod2(RexxObjectPtr, psp_setSize, RexxObjectPtr, size, CSELF, pCSelf)
{
    // TODO validate CSelf.
    pCPropertySheetPage pcpsp = (pCPropertySheetPage)pCSelf;

    PSIZE s = rxGetSize(context, size, 1);
    if ( s != NULL )
    {
        pcpsp->cx = s->cx;
        pcpsp->cy = s->cy;
    }

    return TheZeroObj;
}




/**
 *  Methods for the .UserPSPDialog class.
 */
#define USERPSPDIALOG_CLASS  "UserPSPDialog"


RexxMethod9(RexxObjectPtr, userpspdlg_init, OPTIONAL_RexxObjectPtr, dlgData, OPTIONAL_RexxObjectPtr, includeFile,
            OPTIONAL_CSTRING, opts, OPTIONAL_CSTRING, title, OPTIONAL_CSTRING, fontName, OPTIONAL_uint32_t, fontSize,
            OPTIONAL_uint32_t, expected, SUPER, super, OSELF, self)
{
    RexxArrayObject newArgs = context->NewArray(2);

    if ( argumentExists(1) )
    {
        context->ArrayPut(newArgs, dlgData, 1);
    }
    if ( argumentExists(2) )
    {
        context->ArrayPut(newArgs, includeFile, 2);
    }

    RexxObjectPtr result = context->ForwardMessage(NULL, NULL, super, newArgs);

    if ( isInt(0, result, context->threadContext) )
    {
        pCPlainBaseDialog pcpbd = (pCPlainBaseDialog)context->GetCSelf();
        pCDynamicDialog pcdd = (pCDynamicDialog)context->ObjectToCSelf(self, TheDynamicDialogClass);

        // We have a 0 result.  We set result back to an error code and then
        // reset it to 0 if we get through all the error checks
        result = TheOneObj;
        pcpbd->wndBase->initCode = 1;

        if ( ! initPropertySheetPage(context, pcpbd, pcdd, self) )
        {
            goto done_out;
        }

        pCPropertySheetPage pcpsp = (pCPropertySheetPage)pcpbd->dlgPrivate;
        pcpsp->pageType = oodUserPSPDialog;

        parsePageOpts(context, pcpsp, opts);

        pcdd->expected = (expected == 0 ? DEFAULT_EXPECTED_DIALOG_ITEMS : expected);
        pcpbd->isPageDlg = true;

        if ( argumentExists(4) )
        {
            if ( ! setPageText(context, pcpsp, title, pageText) )
            {
                goto done_out;
            }
        }
        if ( argumentExists(5) )
        {
            if ( strlen(fontName) > (MAX_DEFAULT_FONTNAME - 1) )
            {
                stringTooLongException(context->threadContext, 4, MAX_DEFAULT_FONTNAME, strlen(fontName));
                goto done_out;
            }
            strcpy(pcpbd->fontName, fontName);
        }
        if ( argumentExists(6) )
        {
            if ( fontSize == 0 )
            {
                context->RaiseException2(Rexx_Error_Invalid_argument_positive, context->WholeNumber(5), TheZeroObj);
                goto done_out;
            }
            pcpbd->fontSize = fontSize;
        }
        result = TheZeroObj;
        pcpbd->wndBase->initCode = 0;
    }

done_out:
    return result;
}


/**
 *  Methods for the .RcPSPDialog class.
 */
#define RCPSPDIALOG_CLASS  "RcPSPDialog"


RexxMethod9(RexxObjectPtr, rcpspdlg_init, RexxStringObject, scriptFile, RexxObjectPtr, resource, OPTIONAL_RexxObjectPtr, dlgData,
            OPTIONAL_RexxObjectPtr, includeFile, OPTIONAL_CSTRING, pageOpts, OPTIONAL_RexxStringObject, connectOpts,
            OPTIONAL_uint32_t, expected, SUPER, super, OSELF, self)
{
    RexxArrayObject newArgs = context->NewArray(4);

    context->ArrayPut(newArgs, scriptFile, 3);
    context->ArrayPut(newArgs, resource, 4);
    if ( argumentExists(3) )
    {
        context->ArrayPut(newArgs, dlgData, 1);
    }
    if ( argumentExists(4) )
    {
        context->ArrayPut(newArgs, includeFile, 2);
    }

    RexxObjectPtr result = context->ForwardMessage(NULL, NULL, super, newArgs);

    if ( result == TheZeroObj )
    {
        pCPlainBaseDialog pcpbd = (pCPlainBaseDialog)context->GetCSelf();
        pCDynamicDialog pcdd = (pCDynamicDialog)context->ObjectToCSelf(self, TheDynamicDialogClass);

        if ( ! initPropertySheetPage(context, pcpbd, pcdd, self) )
        {
            result = TheOneObj;
            pcpbd->wndBase->initCode = 1;
            goto done_out;
        }

        pCPropertySheetPage pcpsp = (pCPropertySheetPage)pcpbd->dlgPrivate;
        pcpsp->pageType = oodRcPSPDialog;

        parsePageOpts(context, pcpsp, pageOpts);

        pcdd->expected = (expected == 0 ? DEFAULT_EXPECTED_DIALOG_ITEMS : expected);
        pcpbd->isPageDlg = true;

        pcpsp->extraOpts = (argumentExists(6) ? connectOpts : context->NullString());
    }

done_out:
    return result;
}


RexxMethod7(RexxObjectPtr, rcpspdlg_startTemplate, uint32_t, cx, uint32_t, cy, CSTRING, title, CSTRING, fontName,
            uint32_t, fontSize, uint32_t, expected, CSELF, pCSelf)
{
    RexxMethodContext *c = context;
    pCPlainBaseDialog pcpbd = (pCPlainBaseDialog)pCSelf;
    pCPropertySheetPage pcpsp = (pCPropertySheetPage)pcpbd->dlgPrivate;
    pCDynamicDialog pcdd = pcpsp->pcdd;

    pcpsp->cx = cx;
    pcpsp->cy = cy;

    if ( strlen(title) > 0 && ! setPageText(context, pcpsp, title, pageText) )
    {
        return TheOneObj;
    }

    size_t len = strlen(fontName);

    if ( len > (MAX_DEFAULT_FONTNAME - 1) )
    {
        stringTooLongException(c->threadContext, 4, MAX_DEFAULT_FONTNAME, len);
        return TheOneObj;
    }
    if ( len > 0 )
    {
        strcpy(pcpbd->fontName, fontName);
    }

    if ( fontSize != 0 )
    {
        pcpbd->fontSize = fontSize;
    }
    pcdd->expected = expected;

    return TheZeroObj;
}


/**
 *  Methods for the .ResPSPDialog class.
 */
#define RESPSPDIALOG_CLASS  "ResPSPDialog"


RexxMethod7(RexxObjectPtr, respspdlg_init, RexxStringObject, dllFile, RexxObjectPtr, resourceID, OPTIONAL_RexxObjectPtr, dlgData,
            OPTIONAL_RexxObjectPtr, includeFile, OPTIONAL_CSTRING, pageOpts, SUPER, super, OSELF, self)
{
    RexxArrayObject newArgs = context->NewArray(4);

    context->ArrayPut(newArgs, dllFile, 1);
    context->ArrayPut(newArgs, resourceID, 2);
    if ( argumentExists(3) )
    {
        context->ArrayPut(newArgs, dlgData, 3);
    }
    if ( argumentExists(4) )
    {
        context->ArrayPut(newArgs, includeFile, 4);
    }

    RexxObjectPtr result = context->ForwardMessage(NULL, NULL, super, newArgs);

    if ( result == TheZeroObj )
    {
        pCPlainBaseDialog pcpbd = (pCPlainBaseDialog)context->GetCSelf();

        int32_t resID = oodResolveSymbolicID(context->threadContext, self, resourceID, -1, 2, true);
        if ( resID == OOD_ID_EXCEPTION )
        {
            result = TheOneObj;
            pcpbd->wndBase->initCode = 1;
            goto done_out;
        }

        if ( ! initPropertySheetPage(context, pcpbd, NULL, self) )
        {
            result = TheOneObj;
            pcpbd->wndBase->initCode = 1;
            goto done_out;
        }

        pCPropertySheetPage pcpsp = (pCPropertySheetPage)pcpbd->dlgPrivate;

        pcpsp->pageType = oodResPSPDialog;
        pcpsp->resID = resID;

        parsePageOpts(context, pcpsp, pageOpts);

        pcpbd->isPageDlg = true;
    }

done_out:
    return result;
}


/**
 *  Methods for the .TabOwnerDialog class.
 */
#define TABOWNERDIALOG_CLASS        "TabOwnerDialog"


inline char *tcn2str(LPARAM lParam)
{
    switch ( ((NMHDR *)lParam)->code )
    {
        case TCN_FOCUSCHANGE :    return "TCN_FOCUSCHANGE";
        case TCN_GETOBJECT :      return "TCN_GETOBJECT";
        case TCN_KEYDOWN :        return "TCN_KEYDOWN";
        case TCN_SELCHANGE :      return "TCN_SELCHANGE";
        case TCN_SELCHANGING :    return "TCN_SELCHANGING";
        case NM_CLICK :           return "NM_CLICK";
        case NM_DBLCLK :          return "NM_DBLCLK";
        case NM_RCLICK :          return "NM_RCLICK";
        case NM_RDBLCLK :         return "NM_RDBLCLK";
        case NM_RELEASEDCAPTURE : return "NM_RELEASEDCAPTURE";
        default : return "Not a tab control notification";
    }
}

/**
 * Filter for tab control notification messages.
 *
 * @param uMsg    The windowm message id.
 * @param lParam  The LPARAM argument for the message.
 *
 * @return Return true if the message is a tab control notification message,
 *         otherwise false.
 */
inline bool isTCNMsg(uint32_t uMsg, LPARAM lParam)
{
    if ( uMsg == WM_NOTIFY )
    {
        uint32_t code = ((NMHDR *)lParam)->code;
        if ( code >= TCN_LAST && code <= TCN_FIRST )
        {
            return true;
        }

        UINT_PTR id = ((NMHDR *)lParam)->idFrom;
        if ( id == 200 && (code == NM_CLICK || code == NM_DBLCLK || code == NM_RCLICK ||
                           code == NM_RDBLCLK || code == NM_RELEASEDCAPTURE) )
        {
            return true;
        }
    }
    return false;
}


/**
 * Sets the owner dialog handle for the control dialogs managed by at tab owner
 * dialog.
 *
 * The Rexx owner is set when the control dialogs are added to the tab owner,
 * but at that time, the dialog handle is not known.  We set that here.
 *
 * @param pctod
 */
void setTabOwnerHandles(pCTabOwnerDialog pctod)
{
    HWND hOwner = pctod->hDlg;

    for ( uint32_t i = 0; i < pctod->countMTs; i++ )
    {
        pCManagedTab pmt = pctod->mts[i];
        for ( uint32_t j = 0; j < pmt->count; j++ )
        {
            pCPlainBaseDialog pcpbd = pmt->cppPages[j]->pcpbd;
            pcpbd->hOwnerDlg = hOwner;
        }
    }
}


/**
 * The dialog procedure function for TabOwnerDialog ooDialog dialogs.  Handles
 * and processes all window messages for the dialog.
 *
 * @param hDlg
 * @param uMsg
 * @param wParam
 * @param lParam
 *
 * @return LRESULT CALLBACK
 *
 * TODO rewrite comments.
 *
 * @remarks  The WM_INITDIALOG message.
 *
 *           In CreateDialogParam() / CreateDialogIndirectParam() we pass the
 *           pointer to the PlainBaseDialog CSelf as the param.  The OS then
 *           sends us this value as the LPARAM argument in the WM_INITDIALOG
 *           message. The pointer is stored in the user data field of the window
 *           words for this dialog.  We do the same thing for the child dialogs,
 *           see the WM_USER_CREATECHILD message.
 *
 *           Note that when the child dialogs of the category dialog get
 *           created, we recieve a WM_INITDIALOG for each of them.  These child
 *           dialogs are all running on the same thread as the parent category
 *           dialog.  We don't want to do a bunch of nested AttachThreads()
 *           because we only do 1 DetachThread() for each window message loop.
 *           So, we check to see if dlgProcContext is null before doing the
 *           AttachThread().
 *
 *           The WM_USER_CREATECHILD message.
 *
 *           This user message's purpose is to create a child dialog of this
 *           dialog and return its window handle. Child dialogs are only created
 *           to implement the CategoryDialog and at this time are always created
 *           dynamically (from an in-memory template.) The dialog template
 *           pointer is passed here as the LPARAM arg from
 *           DynamicDialog::startChildDialog().
 *
 *           These child dialogs do not have a backing Rexx dialog. There is no
 *           unique CPlainBaseDialog struct for them.  Instead, at this time,
 *           all interaction with the child dialogs is done through the
 *           CPlainBaseDialog struct of the parent.  For each child dialog, we
 *           set the CPlainBaseDialog struct of the parent in the window words
 *           of the child dialog.  Prior to the conversion of ooDialog to the
 *           C++ API, when a message came in for a child dialog, a search was
 *           made through the DialogTable to try and find the parent dialog.
 *           This has been disposed of and the CPlainBaseDialog struct is just
 *           pulled out of the window words.
 */
LRESULT CALLBACK RexxTabOwnerDlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    if ( uMsg == WM_INITDIALOG )
    {
        pCPlainBaseDialog pcpbd = (pCPlainBaseDialog)lParam;

        if ( pcpbd == NULL )
        {
            // Theoretically impossible.  But ... if it should happen, abort.
            return endDialogPremature(pcpbd, hDlg, NoPCPBDpased);
        }
        pCTabOwnerDialog pctod = (pCTabOwnerDialog)pcpbd->dlgPrivate;

        printf("In WM_INITDIALOG for RexxTabOwnerDlgProc() pcpbd=%p pctod=%p\n", pcpbd, pctod);
        printf("   Tab1 hwnd=%p Tab2 hwnd=%p\n", GetDlgItem(hDlg, 200), GetDlgItem(hDlg, 400));
        printf("   pcpbd->isActive=%d pcpbd->hDlg=%p\n", pcpbd->isActive, pcpbd->hDlg);
        if ( pcpbd->dlgProcContext == NULL )
        {
            RexxThreadContext *context;
            if ( ! pcpbd->interpreter->AttachThread(&context) )
            {
                // Again, this shouldn't happen ... but
                return endDialogPremature(pcpbd, hDlg, NoThreadAttach);
            }
            pcpbd->dlgProcContext = context;

            RexxSetProcessMessages(FALSE);

            pctod->dlgProcContext = context;
        }

        pctod->hDlg = hDlg;
        setTabOwnerHandles(pctod);

        setWindowPtr(hDlg, GWLP_USERDATA, (LONG_PTR)pcpbd);

        // Normally isActive is not set until later TODO
        pcpbd->isActive = true; pcpbd->hDlg = hDlg;

        RexxThreadContext *c = pctod->dlgProcContext;

        pCManagedTab      pcmt = pctod->mts[0];
        pCControlDialog   pccd = pcmt->cppPages[0];
        pCPlainBaseDialog pcpbdTabdlg = pccd->pcpbd;

        pCDynamicDialog pcdd = (pCDynamicDialog)c->ObjectToCSelf(pccd->rexxSelf, TheDynamicDialogClass);

        pcpbdTabdlg->dlgProcContext = c;  // TODO need to check that this is proper place to do this.
        RexxArrayObject args = c->ArrayOfFour(c->String(pcpbdTabdlg->library),
                                              pcpbdTabdlg->resourceID,
                                              c->NullString(),
                                              c->Int32(pcdd->count));
        c->SendMessage(pccd->rexxSelf, "LOAD", args);
        c->SendMessage0(pccd->rexxSelf, "EXECUTE");

        return TRUE;
    }

    pCPlainBaseDialog pcpbd = (pCPlainBaseDialog)getWindowPtr(hDlg, GWLP_USERDATA);
    if ( pcpbd == NULL )
    {
        // A number of messages arrive before WM_INITDIALOG, we just ignore them.
        return FALSE;
    }

    if ( pcpbd->dlgProcContext == NULL )
    {
        printf("In RexxTabOwnerDlgProc() dlgProcContext=%p isActive=%d\n",
               pcpbd->dlgProcContext, pcpbd->isActive);
        if ( ! pcpbd->isActive )
        {
            return FALSE;
        }

        // Once again, theoretically impossible ...
        return endDialogPremature(pcpbd, hDlg, NoThreadContext);
    }

    if ( uMsg == WM_DESTROY )
    {
        // Under all normal circumstances, WM_DESTROY never gets here.  But if
        // it does, it is because of some unexplained / unanticpated error.
        // PostQuitMessage() will cause the window message loop to quit and
        // things should then (hopefully) unwind cleanly.
        PostQuitMessage(3);
        return TRUE;
    }

    bool msgEnabled = IsWindowEnabled(hDlg) ? true : false;

    // Do not search message table for WM_PAINT to improve redraw.
    if ( msgEnabled && uMsg != WM_PAINT && uMsg != WM_NCPAINT )
    {
        if ( isTCNMsg(uMsg, lParam) )
        {
            printf("RexxTabOwnerDlgProc() got TCN message: %s\n", tcn2str(lParam));
        }

        MsgReplyType searchReply = searchMessageTables(uMsg, wParam, lParam, pcpbd);
        if ( searchReply != ContinueProcessing )
        {
            // Note pre 4.0.1, we always returned FALSE, (pass on to the system
            // to process.) But, post 4.0.1 we sometimes reply TRUE, the message
            // has been handled.
            return (searchReply == ReplyTrue ? TRUE : FALSE);
        }
    }

    switch ( uMsg )
    {
        case WM_PAINT:
            if ( pcpbd->bkgBitmap != NULL )
            {
                drawBackgroundBmp(pcpbd, hDlg);
            }
            break;

        case WM_DRAWITEM:
            if ( lParam != 0 )
            {
                return drawBitmapButton(pcpbd, lParam, msgEnabled);
            }
            break;

        case WM_CTLCOLORDLG:
            if ( pcpbd->bkgBrush )
            {
                return(LRESULT)pcpbd->bkgBrush;
            }
            break;

        case WM_CTLCOLORSTATIC:
        case WM_CTLCOLORBTN:
        case WM_CTLCOLOREDIT:
        case WM_CTLCOLORLISTBOX:
        case WM_CTLCOLORMSGBOX:
        case WM_CTLCOLORSCROLLBAR:
        {
            HBRUSH hbrush = NULL;

            if ( pcpbd->CT_nextIndex > 0 )
            {
                // See of the user has set the dialog item with a different
                // color.
                long id = GetWindowLong((HWND)lParam, GWL_ID);
                if ( id > 0 )
                {
                    register size_t i = 0;
                    while ( i < pcpbd->CT_nextIndex && pcpbd->ColorTab[i].itemID != id )
                    {
                        i++;
                    }
                    if ( i < pcpbd->CT_nextIndex )
                    {
                        hbrush = pcpbd->ColorTab[i].ColorBrush;
                    }

                    if ( hbrush )
                    {
                        if ( pcpbd->ColorTab[i].isSysBrush )
                        {
                            SetBkColor((HDC)wParam, GetSysColor(pcpbd->ColorTab[i].ColorBk));
                            if ( pcpbd->ColorTab[i].ColorFG != -1 )
                            {
                                SetTextColor((HDC)wParam, GetSysColor(pcpbd->ColorTab[i].ColorFG));
                            }
                        }
                        else
                        {
                            SetBkColor((HDC)wParam, PALETTEINDEX(pcpbd->ColorTab[i].ColorBk));
                            if ( pcpbd->ColorTab[i].ColorFG != -1 )
                            {
                                SetTextColor((HDC)wParam, PALETTEINDEX(pcpbd->ColorTab[i].ColorFG));
                            }
                        }
                    }
                }
            }
            if ( hbrush )
                return(LRESULT)hbrush;
            else
                return DefWindowProc(hDlg, uMsg, wParam, lParam);
        }

        case WM_COMMAND:
            switch ( LOWORD(wParam) )
            {
                case IDOK:
                case IDCANCEL:

                    // For both IDOK and IDCANCEL, the notification code
                    // (the high word value) must be 0.
                    if ( HIWORD(wParam) == 0 )
                    {
                        // We should never get here because both IDOK and
                        // IDCANCEL should have be interecepted in
                        // searchMessageTables().  But - sometimes we do, very
                        // rarely.  It is on some abnormal error. See the
                        // comments above for the WM_DESTROY message.
                        pcpbd->abnormalHalt = true;
                        DestroyWindow(hDlg);

                        return TRUE;
                    }
            }
            break;

        case WM_QUERYNEWPALETTE:
        case WM_PALETTECHANGED:
            return paletteMessage(pcpbd, hDlg, uMsg, wParam, lParam);

        case WM_USER_CREATECHILD:
        {
            HWND hChild = CreateDialogIndirectParam(MyInstance, (LPCDLGTEMPLATE)lParam, hDlg, (DLGPROC)RexxDlgProc,
                                                    (LPARAM)pcpbd);
            ReplyMessage((LRESULT)hChild);
            return TRUE;
        }

        case WM_USER_CREATECONTROL_DLG:
        {
            pCPlainBaseDialog p = (pCPlainBaseDialog)wParam;
            HWND hChild = CreateDialogIndirectParam(MyInstance, (LPCDLGTEMPLATE)lParam, p->hOwnerDlg, (DLGPROC)RexxChildDlgProc,
                                                    wParam);
            ReplyMessage((LRESULT)hChild);
            return TRUE;
        }

        case WM_USER_CREATECONTROL_RESDLG:
        {
            pCPlainBaseDialog pcpbd = (pCPlainBaseDialog)wParam;

            HWND hChild = CreateDialogParam(pcpbd->hInstance, MAKEINTRESOURCE((uint32_t)lParam), pcpbd->hOwnerDlg,
                                            (DLGPROC)RexxChildDlgProc, (LPARAM)pcpbd);

            ReplyMessage((LRESULT)hChild);
            return TRUE;
        }

        case WM_USER_CREATEPROPSHEET_DLG:
        {
            pCPropertySheetDialog pcpsd = (pCPropertySheetDialog)lParam;

            assignPSDThreadContext(pcpsd, pcpbd->dlgProcContext);

            if ( setPropSheetHook(pcpsd) )
            {
                SetLastError(0);
                INT_PTR ret = PropertySheet((PROPSHEETHEADER *)wParam);
                ReplyMessage((LRESULT)ret);
            }
            else
            {
                ReplyMessage((LRESULT)-1);
            }

            return TRUE;
        }

        case WM_USER_INTERRUPTSCROLL:
            pcpbd->stopScroll = wParam;
            return TRUE;

        case WM_USER_GETFOCUS:
            ReplyMessage((LRESULT)GetFocus());
            return TRUE;

        case WM_USER_GETSETCAPTURE:
            if ( wParam == 0 )
            {
                ReplyMessage((LRESULT)GetCapture());
            }
            else if ( wParam == 2 )
            {
                uint32_t rc = 0;
                if ( ReleaseCapture() == 0 )
                {
                    rc = GetLastError();
                }
                ReplyMessage((LRESULT)rc);
            }
            else
            {
                ReplyMessage((LRESULT)SetCapture((HWND)lParam));
            }
            return TRUE;

        case WM_USER_GETKEYSTATE:
            ReplyMessage((LRESULT)GetAsyncKeyState((int)wParam));
            return TRUE;

        case WM_USER_SUBCLASS:
        {
            SUBCLASSDATA *pData = (SUBCLASSDATA *)lParam;

            pData->dlgProcContext = pcpbd->dlgProcContext;
            pData->rexxDialog = pcpbd->rexxSelf;

            BOOL success = SetWindowSubclass(pData->hCtrl, (SUBCLASSPROC)wParam, pData->uID, (DWORD_PTR)pData);

            ReplyMessage((LRESULT)success);
            return TRUE;
        }

        case WM_USER_SUBCLASS_REMOVE:
            ReplyMessage((LRESULT)RemoveWindowSubclass(GetDlgItem(hDlg, (int)lParam), (SUBCLASSPROC)wParam, (int)lParam));
            return TRUE;

        case WM_USER_HOOK:
        {
            SUBCLASSDATA *pData = (SUBCLASSDATA *)lParam;

            pData->dlgProcContext = pcpbd->dlgProcContext;
            pData->rexxDialog = pcpbd->rexxSelf;

            ReplyMessage((LRESULT)SetWindowsHookEx(WH_KEYBOARD, (HOOKPROC)wParam, NULL, GetCurrentThreadId()));
            return TRUE;
        }

        case WM_USER_CONTEXT_MENU:
        {
            PTRACKPOP ptp = (PTRACKPOP)wParam;
            uint32_t cmd;

            SetLastError(0);
            cmd = (uint32_t)TrackPopupMenuEx(ptp->hMenu, ptp->flags, ptp->point.x, ptp->point.y,
                                             ptp->hWnd, ptp->lptpm);

            // If TPM_RETURNCMD is specified, the return is the menu item
            // selected.  Otherwise, the return is 0 for failure and
            // non-zero for success.
            if ( ! (ptp->flags & TPM_RETURNCMD) )
            {
                cmd = (cmd == 0 ? FALSE : TRUE);
                if ( cmd == FALSE )
                {
                    ptp->dwErr = GetLastError();
                }
            }
            ReplyMessage((LRESULT)cmd);
            return TRUE;
        }
    }

    return FALSE;
}

static inline pCTabOwnerDialog validateTodCSelf(RexxMethodContext *c, void *pCSelf)
{
    pCTabOwnerDialog pctod = (pCTabOwnerDialog)pCSelf;
    if ( pctod == NULL )
    {
        baseClassIntializationException(c);
    }
    return pctod;
}

static RexxObjectPtr findRexxPage(pCTabOwnerDialog pctod, int32_t id, uint32_t pageIndex)
{
    for ( uint32_t i = 0; i < pctod->countMTs; i++ )
    {
        if ( id == pctod->tabIDs[i] )
        {
            pCManagedTab pmt = pctod->mts[i];
            if ( pageIndex < pmt->count )
            {
                return pmt->rexxPages[pageIndex];
            }

            // We found the right managed tab, no sense in looking any further.
            break;
        }
    }
    return NULLOBJECT;
}

/** TabOwnerDialog::init()  [private]
 *
 *  @param cpbd       Pointer to the PlainBaseDialog CSelf.
 *  @param ownerData  Owner data.
 */
RexxMethod2(RexxObjectPtr, tod_tabOwnerDlgInit, POINTER, cpbd, OSELF, self)
{
    pCPlainBaseDialog pcpbd = (pCPlainBaseDialog)cpbd;


    RexxBufferObject pcdBuffer = context->NewBuffer(sizeof(CTabOwnerDialog));
    if ( pcdBuffer == NULLOBJECT )
    {
        return TheOneObj;
    }
    context->SetObjectVariable("CSELF", pcdBuffer);

    pCTabOwnerDialog pctod = (pCTabOwnerDialog)context->BufferData(pcdBuffer);
    memset(pctod, 0, sizeof(CTabOwnerDialog));

    pcpbd->dlgPrivate = pctod;

    pctod->pcpbd = pcpbd;
    pctod->rexxSelf = self;

    if ( pcpbd->initPrivate != NULL && pcpbd->initPrivate != TheNilObj )
    {
        RexxMethodContext *c = context;

        pCTabOwnerDlgInfo pctodi = (pCTabOwnerDlgInfo)c->ObjectToCSelf((RexxObjectPtr)pcpbd->initPrivate);

        printf("In TabOwnerDialog::init() PlainBaseDialog cSelf: %p initPrivate=%p pctodi=%p\n", pcpbd, pcpbd->initPrivate, pctodi);

        RexxObjectPtr bag = rxNewBag(context);
        context->SetObjectVariable("CONTROLDIALOGS_BAG", bag);

        pctod->countMTs = pctodi->count;

        for ( uint32_t i = 0; i < pctodi->count; i++)
        {
            pCManagedTab pmt = pctodi->mts[i];

            pctod->tabIDs[i] = pmt->tabID;
            pctod->mts[i] = pmt;

            // For each control dialog, set ourself as the ownerDialog, and
            // protect the Rexx control dialog from being garbage collected by
            // storing it in the bag.

            for ( uint32_t j = 0; j < pmt->count; j++)
            {
                pCControlDialog pccd = pmt->cppPages[j];
                pCPlainBaseDialog pcpbd = pccd->pcpbd;

                pcpbd->rexxOwner = self;
                pcpbd->isOwnedDlg = true;
                pcpbd->isManagedDlg = pccd->isManaged;

                c->SendMessage1(bag, "PUT", pmt->rexxPages[j]);
            }
            c->ReleaseGlobalReference(pmt->pages);
            pmt->pages = NULLOBJECT;
        }
    }

    return TheZeroObj;
}

/** TabOwnerDialog::getTabPage()
 *
 *
 */
RexxMethod3(RexxObjectPtr, tod_getTabPage, RexxObjectPtr, tabID, uint32_t, pageIndex, CSELF, pCSelf)
{
    RexxObjectPtr page = NULLOBJECT;

    pCTabOwnerDialog pctod = validateTodCSelf(context, pCSelf);
    if ( pctod == NULL )
    {
        goto done_out;
    }

    int32_t id = oodGlobalID(context->threadContext, tabID, 1, true);
    if ( id == OOD_ID_EXCEPTION  )
    {
        goto done_out;
    }

    if ( pageIndex == 0 || pageIndex > MAXTABPAGES )
    {
        wrongRangeException(context->threadContext, 2, 1, MAXTABPAGES, pageIndex);
        goto done_out;
    }
    pageIndex--;

    page = findRexxPage(pctod, id, pageIndex);
    if ( page == NULLOBJECT )
    {
        noSuchPageException(context, id, pageIndex);
    }

done_out:
    return page;
}


/**
 *  Methods for the .TabOwnerDlgInfo class.
 */
#define TABOWNERDLGINFO_CLASS        "TabOwnerDlgInfo"


/** TabOwnerDlgInfo::init()
 *
 * use strict arg arrayOfMts, useResourceImage = .false
 *
 */
RexxMethod3(RexxObjectPtr, todi_init, RexxArrayObject, mts, OPTIONAL_logical_t, useResourceImage, OSELF, self)
{
    uint32_t count = (uint32_t)context->ArrayItems(mts);
    if ( count == 0 )
    {
        emptyArrayException(context->threadContext, 1);
        goto done_out;
    }
    else if( count > MAXMANAGEDTABS )
    {
        arrayToLargeException(context->threadContext, count, MAXMANAGEDTABS, 1);
        goto done_out;
    }

    RexxBufferObject obj = context->NewBuffer(sizeof(CTabOwnerDlgInfo));
    if ( obj == NULLOBJECT )
    {
        goto done_out;
    }
    context->SetObjectVariable("CSELF", obj);

    RexxMethodContext *c = context;

    pCTabOwnerDlgInfo pctodi = (pCTabOwnerDlgInfo)context->BufferData(obj);
    memset(pctodi, 0, sizeof(CTabOwnerDlgInfo));

    printf("In TabOwnerDlgInfo init() pctodi=%p\n", pctodi);
    pctodi->useResourceImage = useResourceImage ? true : false;
    pctodi->count = count;

    for ( uint32_t i = 1; i <= count; i++ )
    {
        RexxObjectPtr mTab = context->ArrayAt(mts, i);
        if ( mTab == NULLOBJECT )
        {
            sparseArrayException(context->threadContext, 1, i);
            goto done_out;
        }
        if ( ! context->IsOfType(mTab, "ManagedTab") )
        {
            wrongObjInArrayException(context->threadContext, 1, i, "a ManagedTab", mTab);
            goto done_out;
        }

        RexxMethodContext *c = context;

        pCManagedTab pcmt = (pCManagedTab)c->ObjectToCSelf(mTab);
        pctodi->mts[i - 1] = pcmt;
        pctodi->pages[1 - 1] = pcmt->pages;
    }

done_out:
    return NULLOBJECT;
}

/**
 *  Methods for the .ManagedTab class.
 */
#define ManagedTab_CLASS        "ManagedTab"


/** ManagedTab::init()
 *
 * use strict arg tabID, pages, wantNotifications = .false
 *
 */
RexxMethod4(RexxObjectPtr, mt_init, RexxObjectPtr, tabID, RexxArrayObject, pages, OPTIONAL_logical_t, wantNotifications, OSELF, self)
{
    printf("In ManagedTab init()\n");
    RexxMethodContext *c = context;

    uint32_t count = (uint32_t)context->ArrayItems(pages);
    if ( count == 0 )
    {
        emptyArrayException(context->threadContext, 1);
        goto done_out;
    }
    else if( count > MAXTABPAGES )
    {
        arrayToLargeException(context->threadContext, count, MAXTABPAGES, 1);
        goto done_out;
    }

    RexxBufferObject obj = context->NewBuffer(sizeof(CManagedTab));
    if ( obj == NULLOBJECT )
    {
        goto done_out;
    }
    context->SetObjectVariable("CSELF", obj);

    pCManagedTab pcmt = (pCManagedTab)context->BufferData(obj);
    memset(pcmt, 0, sizeof(CManagedTab));

    int32_t id = oodGlobalID(context->threadContext, tabID, 1, true);
    if ( id == OOD_ID_EXCEPTION )
    {
        goto done_out;
    }

    pcmt->wantNotifications = wantNotifications ? true : false;
    pcmt->tabID = (uint32_t) id;

    pcmt->count = count;

    pCControlDialog *cppPages = (pCControlDialog *)LocalAlloc(LPTR, MAXPROPPAGES * sizeof(pCControlDialog *));
    RexxObjectPtr *rexxPages = (RexxObjectPtr *)LocalAlloc(LPTR, MAXPROPPAGES * sizeof(RexxObjectPtr *));

    if ( cppPages == NULL || rexxPages == NULL )
    {
        outOfMemoryException(context->threadContext);
        goto done_out;
    }

    pCControlDialog *pPage = cppPages;
    RexxObjectPtr *pRexxPage = rexxPages;
    for ( uint32_t i = 1; i <= count; i++, pPage++, pRexxPage++ )
    {
        RexxObjectPtr dlg = context->ArrayAt(pages, i);
        if ( dlg == NULLOBJECT )
        {
            sparseArrayException(context->threadContext, 1, i);
            goto done_out;
        }
        if ( ! context->IsOfType(dlg, "CONTROLDIALOG") )
        {
            wrongObjInArrayException(context->threadContext, 1, i, "a ControlDlg", dlg);
            goto done_out;
        }

        pCControlDialog pccd = dlgToCDCSelf(context, dlg);
        pccd->pageNumber = i;

        *pPage = pccd;
        *pRexxPage = dlg;
    }

    pcmt->cppPages = cppPages;
    pcmt->rexxPages = rexxPages;

    // Prevent the Rexx pages object from being garbage collected. This then
    // needs to be released when the TabOwner dialog retrieves them, or during
    // delDialog.
    c->RequestGlobalReference(pages);
    pcmt->pages = pages;

done_out:
    return NULLOBJECT;
}


/**
 *  Methods for the .ControlDlgInfo class.
 */
#define CONTROLDLGINFO_CLASS        "ControlDlgInfo"


static bool setCdiTitle(RexxMethodContext *c, pCControlDialogInfo pccdi, CSTRING title)
{
    safeLocalFree(pccdi->title);

    pccdi->title = (char *)LocalAlloc(LPTR, strlen(title) + 1);
    if ( pccdi->title == NULL )
    {
        outOfMemoryException(c->threadContext);
        return false;
    }

    strcpy(pccdi->title, title);
    return true;
}

static bool setCdiSize(RexxMethodContext *c, pCControlDialogInfo pccdi, RexxObjectPtr _size, bool exists, size_t argPos)
{
    if ( exists )
    {
        SIZE *s = rxGetSize(c, _size, argPos);
        if ( s == NULL )
        {
            return false;
        }
        pccdi->size.cx = s->cx;
        pccdi->size.cy = s->cy;
    }
    else
    {
        pccdi->size.cx = 200;
        pccdi->size.cy = 150;
    }
    return true;
}

/** ControlDlgInfo::init()
 *
 * use strict arg owner = .nil,
 *                managed = .false
 *                title = "",
 *                size = (.size~new(200, 150)),
 *                wantNotifications = .false,
 *                tabIcon = (-1),
 *                resources = .nil,
 *
 */
RexxMethod7(RexxObjectPtr, cdi_init, OPTIONAL_RexxObjectPtr, owner, OPTIONAL_logical_t, managed,
            OPTIONAL_CSTRING, title, OPTIONAL_RexxObjectPtr, _size, OPTIONAL_RexxObjectPtr, tabIcon,
            OPTIONAL_RexxObjectPtr, resources, OSELF, self)
{
    RexxBufferObject obj = context->NewBuffer(sizeof(CControlDialogInfo));
    if ( obj == NULLOBJECT )
    {
        goto done_out;
    }
    context->SetObjectVariable("CSELF", obj);

    RexxMethodContext *c = context;

    pCControlDialogInfo pccdi = (pCControlDialogInfo)context->BufferData(obj);
    memset(pccdi, 0, sizeof(CControlDialogInfo));

    pccdi->managed = managed ? true : false;

    if ( argumentExists(1) )
    {
        if ( ! c->IsOfType(owner, "PLAINBASEDIALOG") )
        {
            wrongClassException(c->threadContext, 1, "PlainBaseDialog");
            goto done_out;
        }
        pccdi->owner = owner;
    }

    if ( argumentExists(2) )
    {
        if ( ! setCdiTitle(context, pccdi, title) )
        {
            goto done_out;
        }
    }

    if ( ! setCdiSize(context, pccdi, _size, argumentExists(3), 3) )
    {
        goto  done_out;
    }



done_out:
    return NULLOBJECT;
}

/** ControlDlgInfo::managed()     [Attribute set]
 *
 *
 */
RexxMethod2(RexxObjectPtr, cdi_set_managed, logical_t, managed, CSELF, pCSelf)
{
    pCControlDialogInfo pccdi = (pCControlDialogInfo)pCSelf;
    pccdi->managed = managed ? true : false;
    return NULLOBJECT;
}


/** ControlDlgInfo::title()       [Attribute set]
 *
 *
 */
RexxMethod2(RexxObjectPtr, cdi_set_title, CSTRING, title, CSELF, pCSelf)
{
    pCControlDialogInfo pccdi = (pCControlDialogInfo)pCSelf;
    setCdiTitle(context, pccdi, title);
    return NULLOBJECT;
}


/** ControlDlgInfo::size()        [Attribute set]
 *
 *  Allows the size attribute of a control dialog info object to be reset after
 *  the object is instantiated.
 *
 *  @param size  [optional]  The new size value for this object.  If omitted,
 *               the size is reset to the default 200 x 150.
 */
RexxMethod2(RexxObjectPtr, cdi_set_size, RexxObjectPtr, _size, CSELF, pCSelf)
{
    pCControlDialogInfo pccdi = (pCControlDialogInfo)pCSelf;
    setCdiSize(context, pccdi, _size, true, 1);
    return NULLOBJECT;
}



/**
 *  Methods for the .ControlDialog class.
 */
#define CONTROLDIALOG_CLASS        "ControlDialog"

static inline pCControlDialog validateCdCSelf(RexxMethodContext *c, void *pCSelf)
{
    pCControlDialog pccd = (pCControlDialog)pCSelf;
    if ( pccd == NULL )
    {
        baseClassIntializationException(c);
    }
    return pccd;
}


/** ControlDialog::init()  [Class method]
 *
 *  Used to capture the ControlDialog class object.  This is used for scoped
 *  look ups of the CSelf.
 */
RexxMethod1(RexxObjectPtr, cd_init_cls, OSELF, self)
{
    if ( isOfClassType(context, self, CONTROLDIALOG_CLASS) )
    {
        TheControlDialogClass = (RexxClassObject)self;
    }
    return NULLOBJECT;
}


/** ControlDialog::controlDlgInit()  [private]
 *
 *  @param cpbd       Pointer to the PlainBaseDialog CSelf.
 *  @param ownerData  Owner data.
 */
RexxMethod2(RexxObjectPtr, cd_controlDlgInit, POINTER, cpbd, OSELF, self)
{
    pCPlainBaseDialog pcpbd = (pCPlainBaseDialog)cpbd;

    RexxBufferObject pcdBuffer = context->NewBuffer(sizeof(CControlDialog));
    if ( pcdBuffer == NULLOBJECT )
    {
        return TheOneObj;
    }
    context->SetObjectVariable("CSELF", pcdBuffer);

    pCControlDialog pccd = (pCControlDialog)context->BufferData(pcdBuffer);
    memset(pccd, 0, sizeof(CControlDialog));

    pccd->pcpbd = pcpbd;
    pccd->rexxSelf = self;
    pccd->isInitializing = true;
    pccd->pcpbd->dlgPrivate = pccd;

    if ( context->IsOfType(self, "USERCONTROLDIALOG") )
    {
        pccd->pageType = oodUserControlDialog;
    }
    else if( context->IsOfType(self, "RCCONTROLDIALOG") )
    {
        pccd->pageType = oodRcControlDialog;
    }
    else
    {
        pccd->pageType = oodResControlDialog;
    }

    if ( pcpbd->initPrivate != NULL && pcpbd->initPrivate != TheNilObj )
    {
        pCControlDialogInfo pccdi = (pCControlDialogInfo)context->ObjectToCSelf((RexxObjectPtr)pcpbd->initPrivate);

        pccd->isManaged = pccdi->managed;

        if ( pccdi->title != NULL )
        {
            pccd->pageTitle = pccdi->title;
            pccdi->title = NULL;
        }

        pccd->size.cx = pccdi->size.cx;
        pccd->size.cy = pccdi->size.cy;
    }


    return TheZeroObj;
}

/** ControlDialog::isManaged()    [Attribute get]
 */
RexxMethod1(RexxObjectPtr, cd_get_isManaged, CSELF, pCSelf)
{
    pCControlDialog pccd = validateCdCSelf(context, pCSelf);
    if ( pccd != NULL )
    {
        return pccd->isManaged ? TheTrueObj : TheFalseObj;
    }
    return NULLOBJECT;
}

/** ControlDialog::wasActivated()    [Attribute get]
 */
RexxMethod1(RexxObjectPtr, cd_get_wasActivated, CSELF, pCSelf)
{
    pCControlDialog pccd = validateCdCSelf(context, pCSelf);
    if ( pccd != NULL )
    {
        return pccd->activated ? TheTrueObj : TheFalseObj;
    }
    return NULLOBJECT;
}

/** ControlDialog::initializing()    [Attribute get]
 */
RexxMethod1(RexxObjectPtr, cd_get_initializing, CSELF, pCSelf)
{
    pCControlDialog pccd = validateCdCSelf(context, pCSelf);
    if ( pccd != NULL )
    {
        return pccd->isInitializing ? TheTrueObj : TheFalseObj;
    }
    return NULLOBJECT;
}

/** ControlDialog::initializing()    [Attribute set]
 */
RexxMethod2(RexxObjectPtr, cd_set_initializing, logical_t, initializing, CSELF, pCSelf)
{
    pCControlDialog pccd = validateCdCSelf(context, pCSelf);
    if ( pccd != NULL )
    {
        pccd->isInitializing = initializing ? true : false;
    }
    return NULLOBJECT;
}

/** ControlDialog::pageTitle()       [Attribute get]
 */
RexxMethod1(RexxObjectPtr, cd_get_pageTitle, CSELF, pCSelf)
{
    pCControlDialog pccd = validateCdCSelf(context, pCSelf);
    if ( pccd != NULL )
    {
        return pccd->pageTitle == NULL ? TheNilObj : context->String(pccd->pageTitle);
    }
    return NULLOBJECT;
}


/** ControlDialog::pageTitle()       [Attribute set]
 */
RexxMethod2(RexxObjectPtr, cd_set_pageTitle, CSTRING, text, CSELF, pCSelf)
{
    pCControlDialog pccd = validateCdCSelf(context, pCSelf);
    if ( pccd != NULL )
    {
        goto out;
    }

    char *t = (char *)LocalAlloc(LPTR, strlen(text) + 1);
    if ( t == NULL )
    {
        outOfMemoryException(context->threadContext);
        goto out;
    }

    strcpy(t, text);

    safeLocalFree(pccd->pageTitle);
    pccd->pageTitle = t;

out:
    return TheZeroObj;
}


/**
 *  Methods for the .ResControlDialog class.
 */
#define RESControlDialog_CLASS        "ResControlDialog"

/** ResControlDialog::startDialog()
 *
 *  This method over-rides the superclass (ResDialog) startDialog().
 *
 *  We only need library and id, the owner dialog we pull from the CSelf
 *  struct.
 */
RexxMethod3(RexxObjectPtr, resCtrlDlg_startDialog_pvt, CSTRING, library, RexxObjectPtr, _dlgID, CSELF, pCSelf)
{
    pCPlainBaseDialog pcpbd = getPBDCSelf(context, pCSelf);
    if ( pcpbd == NULL )
    {
        goto err_out;
    }

    if ( ! validControlDlg(context, pcpbd) )
    {
        goto err_out;
    }

    int32_t dlgID = oodResolveSymbolicID(context->threadContext, pcpbd->rexxSelf, _dlgID, -1, 2, true);
    if ( dlgID == OOD_ID_EXCEPTION )
    {
        goto err_out;
    }
    if ( dlgID == 0 )
    {
        wrongArgValueException(context->threadContext, 2, "a valid numeric resource ID or a valid symbolic ID", _dlgID);
        goto err_out;
    }

    if ( !loadResourceDLL(pcpbd, library) )
    {
        goto err_out;
    }

    HWND hChild = (HWND)SendMessage(pcpbd->hOwnerDlg, WM_USER_CREATECONTROL_RESDLG, (WPARAM)pcpbd, (LPARAM)dlgID);
    if ( hChild )
    {
        pcpbd->hDlg = hChild;
        pcpbd->isActive = true;
        ((pCControlDialog)pcpbd->dlgPrivate)->activated = true;
        pcpbd->childDlg[0] = hChild;

        setDlgHandle(context->threadContext, pcpbd);
        setFontAttrib(context->threadContext, pcpbd);

        if ( pcpbd->autoDetect )
        {
            if ( doDataAutoDetection(context, pcpbd) != OOD_NO_ERROR )
            {
                goto err_out;
            }
        }

        return TheTrueObj;
    }

err_out:
    return TheFalseObj;
}



