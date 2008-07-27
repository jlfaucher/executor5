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
/* Object REXX OODialog                                              oovmsg.c */
/*                                                                            */
/*  OODialog Messaging function                                               */
/*                                                                            */

#include <windows.h>
#define INCL_REXXSAA
#include <rexx.h>
#include <stdio.h>
#include <dlgs.h>
#include "oovutil.h"
#include <commctrl.h>


extern CRITICAL_SECTION crit_sec;  /* defined in OOVUTIL.C */
extern BOOL DialogInAdminTable(DIALOGADMIN * Dlg);


BOOL AddDialogMessage(CHAR * msg, CHAR * Qptr)
{
   if (strlen(Qptr) + strlen(msg) + 1 < MAXLENQUEUE)
   {
      strcat(Qptr, msg);
      strcat(Qptr, ";");
      return 1;
   } else return 0;
}


CHAR * GetDlgMessage(DIALOGADMIN * addressedTo, CHAR * buffer, BOOL remove)
{
   int i = 0, l;
   MSG msg;

   if (addressedTo->pMessageQueue)
   {
       CHAR * QPtr = addressedTo->pMessageQueue;
       l = strlen(QPtr);
       if (!l && !PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE) && remove) Sleep(1);   /* don't sleep for just a Peek */

       /* copy up to ; */
       while ((i<l) && (QPtr[i] != ';'))
       {
          buffer[i] = QPtr[i];
          i++;
       }
       buffer[i]='\0';

       if (l && remove)   {
           if (i>=l) QPtr[0] = '\0';
           else memmove(&QPtr[0], &QPtr[i+1], l-i);
       }
   }
   return buffer;
}


#define SelectionDidChange(p) ((p->uNewState & LVIS_SELECTED) != (p->uOldState & LVIS_SELECTED))
#define FocusDidChange(p)     ((p->uNewState & LVIS_FOCUSED) != (p->uOldState & LVIS_FOCUSED))

/* MatchSelectFocus
 * Check that: (a) tag is for select change and focuse change, and (b) that
 * either the selection or the focus actually changed.
 */
#define MatchSelectFocus(tag, p)    \
( ((tag & TAG_SELECTCHANGED) && (tag & TAG_FOCUSCHANGED)) && (SelectionDidChange(p) || FocusDidChange(p)) )

/* MatchSelect
 * Check that: (a) tag is only for selection change and not focuse change, and (b)
 * that the selection actually changed.
 */
#define MatchSelect(tag, p)    \
( ((tag & TAG_SELECTCHANGED) && !(tag & TAG_FOCUSCHANGED)) && (SelectionDidChange(p)) )

/* MatchFocus
 * Check that: (a) tag is only for focus change and not selection change, and (b)
 * that the focus actually changed.
 */
#define MatchFocus(tag, p)    \
( ((tag & TAG_FOCUSCHANGED) && !(tag & TAG_SELECTCHANGED)) && (FocusDidChange(p)) )


BOOL SearchMessageTable(ULONG message, WPARAM param, LPARAM lparam, DIALOGADMIN * addressedTo)
{
   register LONG i = 0;
   MESSAGETABLEENTRY * m = addressedTo->MsgTab;

   if (m)
   for (i=0; i<addressedTo->MT_size; i++)
      if (((message & m[i].filterM) == m[i].msg)
      && ( (ULONG)(param & m[i].filterP) == m[i].wParam)
      && ( ((message == WM_NOTIFY) && ((ULONG)(((NMHDR *)lparam)->code & m[i].filterL) == m[i].lParam))
         || ((message != WM_NOTIFY) && ( (ULONG)(lparam & m[i].filterL) == m[i].lParam)) ) )
      {
         if (param || lparam)  /* if one of the params is <> 0, build argument string */
         {
            char msgstr[512];
            CHAR tmp[20];
            PCHAR np = NULL;
            ULONG item;

                /* do we have a notification where we have to extract some information ? */
            if (message == WM_NOTIFY)
            {
                UINT code = ((NMHDR *)lparam)->code;

                /* do we have a left mouse click */
                if ( code == NM_CLICK )
                {
                    /* on a tagged List-View control? */
                    if ( (m[i].tag & TAG_CTRLMASK) == TAG_LISTVIEW )
                    {
                        LPNMITEMACTIVATE pIA = (LPNMITEMACTIVATE)lparam;

                        if ( pIA->uKeyFlags == 0 )
                        {
                            strcpy(tmp, "NONE");
                        }
                        else
                        {
                            tmp[0] = '\0';

                            if ( pIA->uKeyFlags & LVKF_SHIFT )
                                strcpy(tmp, "SHIFT");
                            if ( pIA->uKeyFlags & LVKF_CONTROL )
                                tmp[0] == '\0' ? strcpy(tmp, "CONTROL") : strcat(tmp, " CONTROL");
                            if ( pIA->uKeyFlags & LVKF_ALT )
                                tmp[0] == '\0' ? strcpy(tmp, "ALT") : strcat(tmp, " ALT");
                        }
                        np = tmp;

                        /* Don't drop through, use AddDialogMessage here and
                         * return because we need to send 4 args to ooRexx.
                         */

                        _snprintf(msgstr, 511, "%s(%u,%u,%u,\"%s\")", m[i].rexxProgram,
                                  pIA->hdr.idFrom, pIA->iItem, pIA->iSubItem, np);
                        AddDialogMessage((char *)msgstr, addressedTo->pMessageQueue);
                        return 1;
                    }
                }
                else if ( code == LVN_ITEMCHANGED )
                {
                    if ( (m[i].tag & TAG_CTRLMASK) == TAG_LISTVIEW )
                    {
                        LPNMLISTVIEW pLV = (LPNMLISTVIEW)lparam;

                        if ( (m[i].tag & TAG_STATECHANGED) && (pLV->uChanged == LVIF_STATE) )
                        {
                            item = pLV->iItem;
                            param = pLV->hdr.idFrom;

                            if ( (m[i].tag & TAG_CHECKBOXCHANGED) && (pLV->uNewState & LVIS_STATEIMAGEMASK) )
                            {
                                np = pLV->uNewState == INDEXTOSTATEIMAGEMASK(2) ? "CHECKED" : "UNCHECKED";
                            }
                            else if ( MatchSelectFocus(m[i].tag, pLV) )
                            {
                                tmp[0] = '\0';

                                if ( SelectionDidChange(pLV) )
                                {
                                    (pLV->uNewState & LVIS_SELECTED) ?
                                        strcpy(tmp, "SELECTED") : strcpy(tmp, "UNSELECTED");
                                }

                                if ( FocusDidChange(pLV) )
                                {
                                    if ( (pLV->uNewState & LVIS_FOCUSED) )
                                        tmp[0] == '\0' ? strcpy(tmp, "FOCUSED") : strcat(tmp, " FOCUSED");
                                    else
                                        tmp[0] == '\0' ? strcpy(tmp, "UNFOCUSED") : strcat(tmp, " UNFOCUSED");
                                }
                                np = tmp;
                            }

                            /* We continue in the 2 following cases to allow a
                             * user to have separate method connections for
                             * selected and focused.
                             */
                            else if ( MatchSelect(m[i].tag, pLV) )
                            {
                                np = (pLV->uNewState & LVIS_SELECTED) ? "SELECTED" : "UNSELECTED";
                                _snprintf(msgstr, 511, "%s(%u,%u,\"%s\")", m[i].rexxProgram, param, item, np);
                                AddDialogMessage((char *)msgstr, addressedTo->pMessageQueue);
                                continue;
                            }
                            else if ( MatchFocus(m[i].tag, pLV) )
                            {
                                np = (pLV->uNewState & LVIS_FOCUSED) ? "FOCUSED" : "UNFOCUSED";
                                _snprintf(msgstr, 511, "%s(%u,%u,\"%s\")", m[i].rexxProgram, param, item, np);
                                AddDialogMessage((char *)msgstr, addressedTo->pMessageQueue);
                                continue;
                            }
                            else
                            {
                                /* This message in the message table does not
                                 * match, keep searching.
                                 */
                                continue;
                            }
                        }
                    }
                }
                /* do we have an end label edit for tree or list view? */
                else if ((code == TVN_ENDLABELEDIT) && ((TV_DISPINFO *)lparam)->item.pszText)
                {
                    np = ((TV_DISPINFO *)lparam)->item.pszText;
                    item = (ULONG)((TV_DISPINFO *)lparam)->item.hItem;
                }
                else if ((code == LVN_ENDLABELEDIT) && ((LV_DISPINFO *)lparam)->item.pszText)
                {
                    np = ((LV_DISPINFO *)lparam)->item.pszText;
                    item = ((LV_DISPINFO *)lparam)->item.iItem;
                }
                /* do we have a tree expand/collapse? */
                else if ((code == TVN_ITEMEXPANDED) || (code == TVN_ITEMEXPANDING))
                {
                    item = (ULONG)((NM_TREEVIEW *)lparam)->itemNew.hItem;
                    if (((NM_TREEVIEW *)lparam)->itemNew.state & TVIS_EXPANDED) np = "EXPANDED";
                    else np = "COLLAPSED";
                }
                /* do we have a key_down? */
                else if ((code == TVN_KEYDOWN) || (code == LVN_KEYDOWN) || (code == TCN_KEYDOWN))
                {
                    lparam = (ULONG)((TV_KEYDOWN *)lparam)->wVKey;
                }
                /* do we have a list drag and drop? */
                else if ((code == LVN_BEGINDRAG) || (code == LVN_BEGINRDRAG))
                {
                    item = (ULONG)((NM_LISTVIEW *)lparam)->iItem;
                    param = ((NMHDR *)lparam)->idFrom;
                    sprintf(tmp, "%d %d", ((NM_LISTVIEW *)lparam)->ptAction.x, ((NM_LISTVIEW *)lparam)->ptAction.y);
                    np = tmp;
                }
                /* do we have a tree drag and drop? */
                else if ((code == TVN_BEGINDRAG) || (code == TVN_BEGINRDRAG))
                {
                    item = (ULONG)((NM_TREEVIEW *)lparam)->itemNew.hItem;
                    param = ((NMHDR *)lparam)->idFrom;
                    sprintf(tmp, "%d %d", ((NM_TREEVIEW *)lparam)->ptDrag.x, ((NM_TREEVIEW *)lparam)->ptDrag.y);
                    np = tmp;
                }
                /* do we have a column click in a report? */
                else if (code == LVN_COLUMNCLICK)
                {
                    param = ((NMHDR *)lparam)->idFrom;
                    lparam = (ULONG)((NM_LISTVIEW *)lparam)->iSubItem;  /* which column is pressed */
                }
            }
            else if ( m[i].tag )
            {
                /* There is only 1 message in this category now, so this could
                 * have been all one test.
                 */
                if ( ((m[i].tag & TAG_CTRLMASK) == TAG_DIALOG) && (m[i].tag & TAG_HELP) && (message == WM_HELP))
                {
                    LPHELPINFO phi = (LPHELPINFO)lparam;

                    if ( phi->iContextType == HELPINFO_WINDOW )
                        np = "WINDOW";
                    else
                        np = "MENU";

                    /* Use AddDialogMessage directely to send 4 args to ooRexx. */
                    _snprintf(msgstr, 511, "%s(%u,\"%s\",%d,%d)", m[i].rexxProgram,
                              phi->iCtrlId, np, phi->MousePos.x, phi->MousePos.y);
                    AddDialogMessage((char *)msgstr, addressedTo->pMessageQueue);
                    return 1;
                }
            }

            if (np)
                _snprintf(msgstr, 511, "%s(%u,%u,\"%s\")", m[i].rexxProgram, param, item, np);
            else
                sprintf(msgstr, "%s(%u,%u)", m[i].rexxProgram, param, lparam);
            AddDialogMessage((char *)msgstr, addressedTo->pMessageQueue);
         }
         else
            AddDialogMessage((char *)m[i].rexxProgram, addressedTo->pMessageQueue);
         return 1;
      }
   return 0;
}


BOOL AddTheMessage(DIALOGADMIN * aDlg, ULONG message, ULONG filt1, ULONG param, ULONG filt2, ULONG lparam, ULONG filt3, RXSTRING prog, ULONG ulTag)
{
   if (!prog.strlength) return 0;
   if (!(message | param | lparam))
   {
       MessageBox(0,"Message passed is invalid","Error",MB_OK | MB_ICONHAND);
       return 0;
   }
   if (!aDlg->MsgTab)
   {
      aDlg->MsgTab = LocalAlloc(LPTR, sizeof(MESSAGETABLEENTRY) * MAX_MT_ENTRIES);
      if (!aDlg->MsgTab)
      {
          MessageBox(0,"No memory available","Error",MB_OK | MB_ICONHAND);
          return 0;
      }

      aDlg->MT_size = 0;
   }

   if (aDlg->MT_size < MAX_MT_ENTRIES)
   {
      aDlg->MsgTab[aDlg->MT_size].msg = message;
      aDlg->MsgTab[aDlg->MT_size].filterM = filt1;
      aDlg->MsgTab[aDlg->MT_size].wParam = param;
      aDlg->MsgTab[aDlg->MT_size].filterP = filt2;
      aDlg->MsgTab[aDlg->MT_size].lParam = lparam;
      aDlg->MsgTab[aDlg->MT_size].filterL = filt3;
      aDlg->MsgTab[aDlg->MT_size].tag = ulTag;
      aDlg->MsgTab[aDlg->MT_size].rexxProgram = LocalAlloc(LMEM_FIXED, prog.strlength+1);
      if (aDlg->MsgTab[aDlg->MT_size].rexxProgram) rxstrlcpy(aDlg->MsgTab[aDlg->MT_size].rexxProgram, prog);
      aDlg->MT_size ++;
      return 1;
   }
   else
   {
      MessageBox(0, "Messages have exceeded the maximum number of allocated\n"
                    "table entries. No message can be added.\n",
                 "Error",MB_OK | MB_ICONHAND);
   }
   return 0;
}


#define NARG 7

ULONG APIENTRY AddUserMessage(
  PUCHAR funcname,
  ULONG argc,
  RXSTRING argv[],
  PUCHAR qname,
  PRXSTRING retstr )

{
   ULONG n[NARG];
   INT i;
   DEF_ADM;

   CHECKARGL(NARG+1);

   GET_ADM;

   if (!dlgAdm) return 1;

   for (i=1;i<NARG;i++)
   {
      if (ISHEX(argv[i].strptr))
         n[i-1] = strtoul(argv[i].strptr,'\0',16);
      else
         n[i-1] = (ULONG)atol(argv[i].strptr);
   }

   if ( argc == 9 )
   {
      if ( ISHEX(argv[8].strptr) )
         n[NARG-1] = strtoul(argv[8].strptr,'\0',16);
      else
         n[NARG-1] = (ULONG)atol(argv[8].strptr);
   }
   else
      n[NARG-1] = 0;

   RETC(!AddTheMessage(dlgAdm, n[0], n[1], n[2], n[3], n[4], n[5], argv[7], n[NARG-1]))
}



ULONG APIENTRY SendWinMsg(
  PUCHAR funcname,
  ULONG argc,
  RXSTRING argv[],
  PUCHAR qname,
  PRXSTRING retstr )
{
    LONG i;
    ULONG n[5];

    CHECKARGL(5);
    if (!strcmp(argv[0].strptr,"DLG"))
    {
        CHECKARG(6);

        for (i=0; i<5; i++)
        {
           if (ISHEX(argv[i+1].strptr))
               n[i] = strtol(argv[i+1].strptr,'\0',16);
           else
               n[i] = atol(argv[i+1].strptr);
        }
        ltoa(SendDlgItemMessage((HWND)n[0], n[1], n[2], (WPARAM)n[3], (LPARAM)n[4]), retstr->strptr, 10);
        retstr->strlength = strlen(retstr->strptr);
        return 0;
    }
    else
    if (!strcmp(argv[0].strptr,"PTR"))
    {
        LONG ret, lP, lBuffer;

        CHECKARG(6);

        for (i=0; i<4; i++)
        {
           if (ISHEX(argv[i+1].strptr))
               n[i] = strtol(argv[i+1].strptr,'\0',16);
           else
               n[i] = atol(argv[i+1].strptr);
        }
        if (ISHEX(argv[5].strptr)) lP = (LPARAM) strtol(argv[5].strptr,'\0',16);
        else
        if (argv[5].strptr[0] == 'T') lP = (LPARAM) &argv[5].strptr[1];
        else
        if (argv[5].strptr[0] == 'L')  /* e.g. used to set tab stops for edit control */
        {
            lBuffer = atol(&argv[5].strptr[1]);
            lP = (LONG)&lBuffer;
        }
        else
        if (argv[5].strptr[0] == 'G')     /* buffered get e.g. to get a text line of an edit control */
        {
            ULONG len = atoi(&argv[5].strptr[1]);
            if (len > retstr->strlength) {
                lP = (LONG)GlobalAlloc(GMEM_FIXED, len+1);
                if (!lP) return GetLastError();
                retstr->strptr = (PCHAR)lP;
            }
            else lP = (LONG)retstr->strptr;
            memcpy(retstr->strptr, (char *)&len, sizeof(INT));  /* set the buffer size at the beginning of the buffer */
        }
        else
           lP = (LPARAM) atol(argv[5].strptr);

        /* Special handling for this message because it do not wotk for multiple selection lb's */
        if ( n[2] == LB_SETCURSEL )
        {
          // at first check if it is an multiple selection lb
          LONG style;
          style = GetWindowLong(GetDlgItem( (HWND)n[0], n[1] ), GWL_STYLE);

          if ( style & LBS_MULTIPLESEL )
            if ( argv[5].strptr[0] == 'D' )
              // deselect item in muliple selection lb
              ret = SendDlgItemMessage((HWND)n[0], n[1], LB_SETSEL, 0, (LPARAM)n[3]);
            else
              // select item in muliple selection lb
              ret = SendDlgItemMessage((HWND)n[0], n[1], LB_SETSEL, 1, (LPARAM)n[3]);
          else
            // select item in single selection lb
            ret = SendDlgItemMessage((HWND)n[0], n[1], n[2], (WPARAM)n[3], lP);
        }
        else

          ret = SendDlgItemMessage((HWND)n[0], n[1], n[2], (WPARAM)n[3], lP);

       if (argv[5].strptr[0] != 'G')
       {
           ltoa(ret, retstr->strptr, 10);
           retstr->strlength = strlen(retstr->strptr);
       }
       else retstr->strlength = ret;  /* for get text because \0 isn't copied always */
       if (retstr->strlength < 0) retstr->strlength = 0;   /* could be LB_ERR = -1 */
       return 0;
    }
    else
    if (!strcmp(argv[0].strptr,"ANY"))
    {
       for (i=0; i<4; i++)
       {
          if (ISHEX(argv[i+1].strptr))
              n[i] = strtoul(argv[i+1].strptr,'\0',16);
          else
              n[i] = strtoul(argv[i+1].strptr,'\0',10);
       }

       ltoa(SendMessage((HWND)n[0], n[1], (WPARAM)n[2], (LPARAM)n[3]), retstr->strptr, 10);
       retstr->strlength = strlen(retstr->strptr);
       return 0;
    }
    return 0;
}



ULONG APIENTRY GetDlgMsg(
  PUCHAR funcname,
  ULONG argc,
  RXSTRING argv[],
  PUCHAR qname,
  PRXSTRING retstr )

{
   BOOL remove = TRUE;
   HWND hDlg = NULL;
   DEF_ADM;

   CHECKARGL(1);

   GET_ADM;

   if (!dlgAdm) RETERR

   EnterCriticalSection(&crit_sec);
   if (argc == 2) remove = FALSE;

   if (!DialogInAdminTable(dlgAdm))   /* Is the dialog admin valid? */
   {
       strcpy(retstr->strptr, MSG_TERMINATE);
       retstr->strlength = strlen(retstr->strptr);
       LeaveCriticalSection(&crit_sec);
       return 0;
   }
   GetDlgMessage(dlgAdm, retstr->strptr, remove);
   retstr->strlength = strlen(retstr->strptr);
   LeaveCriticalSection(&crit_sec);
   return 0;
}


ULONG APIENTRY SetLBTabStops(
  PUCHAR funcname,
  ULONG argc,
  RXSTRING argv[],
  PUCHAR qname,
  PRXSTRING retstr )

{
   ULONG i;
   INT tabs[20];

   CHECKARGL(3);

   for (i=0; (i<argc-2) && (i < 20) ; i++)
   {
      tabs[i] = atoi(argv[i+2].strptr);
   }

   i = SendDlgItemMessage((HWND)atol(argv[0].strptr), atoi(argv[1].strptr), LB_SETTABSTOPS, (WPARAM)(argc-2), (LPARAM)tabs);
   RETC(!i)
}


