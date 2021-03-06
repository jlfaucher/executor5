/*----------------------------------------------------------------------------*/
/*                                                                            */
/* Copyright (c) 1995, 2004 IBM Corporation. All rights reserved.             */
/* Copyright (c) 2005-2014 Rexx Language Association. All rights reserved.    */
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
 * Windows Dialog Interface for Open Object Rexx (ooRexx.)
 *
 * Dialog class extensions.
 */

::class 'DialogExtensions' public mixinclass Object

-- Like execute(), but does not wait for the dialog to close. Do not document
-- modeless, it is an implementation detail that may be removed if true modal
-- dialog are ever implemented. sleepTime on the other hand *was* documented.
::method executeAsync unguarded
   use strict arg sleepTime = 1, sh = "NORMAL", icon = 0, modeless = .false

   if self~startIt(icon, modeless) \= 0 then do
      if self~useStem then self~setDataStem(self~dlgData)
      else self~setData
      self~show(sh)

      do while self~automaticMethods~Items > 0
         p = self~automaticMethods~pull
         m = self~automaticMethods~pull
         c = self~automaticMethods~pull
         o = c~new(p, self)
         o~start(m)
      end
      return 0
   end
   else do
      self~finished = .true
      self~stopit
      return 1
   end

-- Used to wait for a dialog started through executeAsync() to end.
::method endAsyncExecution unguarded

   self~waitForDialog

   if self~initCode = 1 then do
      if self~useStem then self~getDataStem(self~dlgData)
      self~getData
   end
   self~stopit
   return self~initCode

-- Run a modeless dialog
::method popup
   use strict arg sh = "NORMAL", sleepTime = 1, icon = 0
   return self~start("execute", sh, icon, sleeptime, .true)

-- Run a modeless dialog that will be automatically closed when its parent closes.
::method popupAsChild
   use strict arg parent, sh = "NORMAL", sleepTime = 1, icon = 0

   if \ parent~isA(.PlainBaseDialog) then do
       call errorDialog "Specified parent is not a dialog!"
       return -1
   end

   parent~childDialogs~insert(self)  /* add to the parents child list (last item) */
   self~parentDlg = parent

   return self~start("execute", sh, icon, sleeptime, .true)

::method setSBRange unguarded
   use strict arg id, min, max, redraw = .true
   sb = self~newScrollBar(id)
   if sb == .nil then return -1
   return sb~setRange(min, max, redraw)

::method getSBRange unguarded   -- Old method, returns a string.  Not going to add an enhanced method.
   use strict arg id
   sb = self~newScrollBar(id)
   if sb == .nil then return -1
   return sb~range

::method setSBPos unguarded
   use strict arg id, pos, redraw = .true
   sb = self~newScrollBar(id)
   if sb == .nil then return -1
   return sb~setPos(pos, redraw)

::method getSBPos unguarded
   use strict arg id
   sb = self~newScrollBar(id)
   if sb == .nil then return -1
   return sb~getPos

::method determineSBPosition unguarded
   use strict arg id, posdata, single = 1, page = 10
   sb = self~newScrollBar(id)
   if sb == .nil then return -1
   return sb~determinePosition

::method getListWidthPx unguarded
   use strict arg id
   listBox = self~newListBox(id)
   if listBox == .nil then return -1
   return listBox~widthPx

::method setListColumnWidthPx unguarded
   use strict arg id, pixels
   listBox = self~newListBox(id)
   if listBox == .nil then return -1
   listBox~columnWidthPx = pixels
   return 0

::method setListItemHeightPx unguarded
   use strict arg id, pixels
   listBox = self~newListBox(id)
   if listBox == .nil then return -1
   listBox~listItemHeightPx = pixels
   return 0

::method setListWidthPx unguarded
   use strict arg id, pixels
   listBox = self~newListBox(id)
   if listBox == .nil then return -1
   listBox~setWidthPx(pixels)
   return 0

::method getListItemHeightPx unguarded
   use strict arg id
   listBox = self~newListBox(id)
   if listBox == .nil then return -1
   return listBox~itemHeightPx

::method setListWidth unguarded         -- Old method uses dialog units - not accurate
   use strict arg id, dlgunits
   pixels = dlgunits * self~factorX
   return self~setListWidthPx(id, pixels)

::method getListWidth unguarded         -- Old method uses dialog units - not accurate
   use strict arg id
   pixels = self~getListWidthPx(id)
   if pixels <= 0 then return pixels
   else return pixels / self~factorX

::method setListColumnWidth unguarded   -- Old method uses dialog units - not accurate
   use strict arg id, dlgunits
   pixels = dlgunits * self~factorX
   return self~setListColumnWidthPx(id, pixels)

::method setListItemHeight unguarded    -- Old method uses dialog units - not accurate
   use strict arg id, dlgunits
   pixels = dlgunits * self~factorY
   return self~setListItemHeightPx(id, pixels)

::method getListItemHeight unguarded    -- Old method uses dialog units - not accurate
   use strict arg id
   pixels = self~getListItemHeightPx(id)
   if pixes <= 0 then return pixels
   else return pixels / self~factorY

::method setControlFont unguarded
   use strict arg id, hFont, bRedraw = .true
   hCtrl = self~getControlHandle(id)
   return self~sendWinHandleMsg(self~WM_SETFONT, hFont, bRedraw, hCtrl)

::method setControlSysColor external "LIBRARY oodialog dlgext_setControlColor"
::method setControlColor external "LIBRARY oodialog dlgext_setControlColor"

::method setWindowRect unguarded external "LIBRARY oodialog dlgext_setWindowRect"
::method getControlRect unguarded external "LIBRARY oodialog dlgext_getControlRect"
::method resizeControl unguarded external "LIBRARY oodialog dlgext_resizeMoveControl"
::method moveControl unguarded external "LIBRARY oodialog dlgext_resizeMoveControl"

::method clearRect unguarded external "LIBRARY oodialog dlgext_clearRect"
::method clearWindowRect unguarded external "LIBRARY oodialog dlgext_clearWindowRect"
::method clearControlRect unguarded external "LIBRARY oodialog dlgext_clearControlRect"
::method redrawRect unguarded external "LIBRARY oodialog dlgext_redrawRect"
::method redrawWindowRect unguarded external "LIBRARY oodialog dlgext_redrawWindowRect"
::method redrawControl unguarded external "LIBRARY oodialog dlgext_redrawControl"
::method setForegroundWindow unguarded external "LIBRARY oodialog dlgext_setForgroundWindow"

::method drawButton unguarded external "LIBRARY oodialog dlgext_drawButton"
::method createBrush unguarded external "LIBRARY oodialog dlgext_createBrush"

::method redrawWindow unguarded
   use arg hwnd
   return .Window~new(hwnd)~redraw

-- DEPRECATED START
::method combineELwithSB  -- Use UpDown control, this method works very poorly
   use strict arg elid, sbID, step = 0, poscode = 0
   sb = self~newScrollBar(sbID)
   if sb == .nil then return -1
   pos = self~getEditData(elid)
   if \ pos~DataType("W") then return -1

   r = sb~getRange
   if .SystemErrorCode <> 0 then return -1

   if arg(4, "E") then do
       sbc = .DlgUtil~loWord(poscode)
       if sbc = 0 & step > 0 then step = -step
       else if sbc = 1 & step < 0 then step = +step
   end

   if step > 0 then do
      if pos < r~max then pos = pos + step
   end
   else if step < 0 then do
      if pos > r~min then pos = pos + step
   end
   else if arg(4, 'E') then do
      /* calc the position returned by message */
      pos = .DlgUtil~hiWord(poscode)
   end
   sb~setPos(pos)
   self~setEditData(elid, pos)
   return 0

::method connectAnimatedButton
   forward message "INSTALLANIMATEDBUTTON"
::method connectBitmapButton
   forward message "INSTALLBITMAPBUTTON"
::method clearButtonRect unguarded
   forward message 'CLEARCONTROLRECT'
::method freeButtonDC unguarded
   forward message 'FREECONTROLDC'
::method getButtonDC unguarded
   forward message 'GETCONTROLDC'
::method getButtonRect unguarded -- old method returns coordinates as a string
   forward message "GETCONTROLRECT" continue
   rect = result
   if \ rect~isA(.Rect) then return 1  -- mimic old behavior
   return rect~left rect~top rect~right rect~bottom

::method getWindowRect unguarded        -- Old method returns coordinates as a string.
   forward message "WINDOWRECT" continue
   rect = result
   return rect~left rect~top rect~right rect~bottom

::method moveItem unguarded      -- old method, this is not accurate
   use strict arg id, xPos, yPos, showOptions = ""
   p = .Point~new(trunc(xPos * self~factorX), trunc(yPos * self~factorY))
   return self~moveControl(id, p, showOptions)

::method redrawButton unguarded
   forward message 'REDRAWCONTROL'
::method resizeItem unguarded    -- old method, this is not accurate
   use strict arg id, width, height, showOptions = ""
   s = .Size~new(trunc(width * self~factorX), trunc(height * self~factorY))
   return self~resizeControl(id, s, showOptions)

::method scrollInButton unguarded
   forward message 'SCROLLINCONTROL'
::method setItemFont unguarded
   forward message 'SETCONTROLFONT'
::method setItemColor
   forward message "SETCONTROLCOLOR"
::method setItemSysColor
   forward message "SETCONTROLSYSCOLOR"
::method writeToButton unguarded
   forward message "WRITETOCONTROL"

-- END DEPRECATED


/******************************************************************************/
/* Bitmap extensions */

::method installBitmapButton external "LIBRARY oodialog dlgext_installBitmapButton"
::method changeBitmapButton unguarded external "LIBRARY oodialog dlgext_changeBitmapButton"
::method getBitmapPosition unguarded external "LIBRARY oodialog dlgext_getBitmapPosition"
::method setBitmapPosition unguarded external "LIBRARY oodialog dlgext_setBitmapPosition"
::method getBitmapSize unguarded external "LIBRARY oodialog dlgext_getBitmapSize"
::method getBitmapSizeX unguarded external "LIBRARY oodialog dlgext_getBitmapSize"
::method getBitmapSizeY unguarded external "LIBRARY oodialog dlgext_getBitmapSize"
::method drawBitmap unguarded external "LIBRARY oodialog dlgext_drawBitmap"

::method scrollText unguarded external "LIBRARY oodialog dlgext_scrollText"
::method scrollInControl unguarded external "LIBRARY oodialog dlgext_scrollText"

::method displaceBitmap unguarded external "LIBRARY oodialog dlgext_setBitmapPosition"
::method getBmpDisplacement unguarded
   use strict arg id
   pos = .Point~new
   if self~getBmpPosition(id, pos) then return pos~x pos~y
   else return -1

-- This method will add a method that will be started automatically and run concurrently when
-- the dialog is executed. That is the  method 'InClass'~'MethodName' will be started, when
-- the dialog receives the message 'execute' or execute async. You can use this method to animate
-- buttons.
::method addAutoStartMethod
   use arg inClass = (self), methodName, parameters
   self~automaticMethods~push(inClass)
   if arg(2,"E") then self~automaticMethods~push(methodName)
   if arg(3,"E") then self~automaticMethods~push(parameters)

-- Install an animated button and run it concurrently to the main activity.
--
-- The only way to get the bitmap(s) into the bitmap table is through this
-- method.  Therefore msgToRaise has to be optional.  The empty string will
-- eventually cause the bitmaps to be added to the bitmap table and the addition
-- of a method being added to the message table will be skipped.
::method installAnimatedButton
   use arg buttonID, msgToRaise = "", autoClass = (.AnimatedButton), bmpFrom, bmpTo = 0, moveX, moveY,   -
           sizeX, sizeY, delay, xNow = 0, yNow = 0

   -- Note documentation for bmpfrom, it is *not* a bitmap handle.
   if arg(5, "o") | \ bmpFrom~datatype("N") then
       ret = self~installBitmapButton(buttonID, msgToRaise, .local[bmpFrom][1])
   else
       ret = self~installBitmapButton(buttonID, msgToRaise, bmpFrom)

   if ret <> 0 then return ret

   if arg(8, "O") then sizeX = self~getBitmapSizeX(buttonID)
   if arg(9, "O") then sizeY = self~getBitmapSizeY(buttonID)

   self~addAutoStartMethod(autoClass, "RUN", buttonID || "," || bmpFrom ||"," || bmpTo ||"," || moveX    -
     || "," || moveY || "," || sizeX || "," || sizeY || "," || delay || "," || xNow || "," || yNow)

-- Draw the bitmap step by step.
::method dimBitmap unguarded external "LIBRARY oodialog dlgext_dimBitmap"

-- This method moves the rectangle within a button.  TODO be sure can take a .Rect for arg
::method scrollButton unguarded external "LIBRARY oodialog dlgext_scrollButton"

-- Scroll a bitmap from one position to another within a button.
::method scrollBitmapFromTo unguarded
   use arg id, fromX, fromY, toX, toY, stepX = 0, stepY = 0, delay = 0, displace = .true

   xl = self~getBitmapSizeX(id)
   yl = self~getBitmapSizeY(id)

   if xl <= 0 | yl <= 0 then return 1
   if stepX = 0 & stepY = 0 then return 1

   curX=fromx
   curY=fromy
   if (stepx > 0) then cursx = -curX; else cursx = 0
   if (stepy > 0) then cursy = -curY; else cursy = 0
   if (cursx > xl) then cursx = xl
   if (cursy > yl) then cursy = yl

   button = self~newPushButton(id)
   if button == .nil then return -1
   hwnd = button~hwnd

   parse value self~getWindowRect(hwnd) with r.left r.top r.right r.bottom
   r.right = r.right - r.left
   r.bottom = r.bottom - r.top
   drawx = 1
   drawy = 1

   if (stepx >= 0) then do
      left = max(fromx, 0)
      lenx = xl - cursx
   end
   else do
      left = fromx
      lenx = min(r.right-fromx, xl)
   end
   if (stepy >= 0) then do
      top = max(fromy, 0)
      leny = yl - cursy
   end
   else do
      top = fromy
      leny = min(r.bottom-fromy, yl)
   end

   self~drawBitmap(hwnd, id, left, top, cursx, cursy, lenx, leny)
   if (stepx < 0) then cursx = min(lenx, xl); else cursx = cursx-stepx;
   if (stepy < 0) then cursy = min(leny, yl); else cursy = cursy-stepy;

   worktodo = 1
   if (delay > 0) then timer = winTimer("START", delay); else timer = 0
   do while worktodo = 1

      sleft = curX
      srgt = curX+xl
      if srgt > r.right then srgt = r.right
      if sleft < 0 then sleft = 0

      s_top = curY
      sbot = curY+yl
      if sbot > r.bottom-stepy then sbot = r.bottom-stepy
      if s_top < 0 then s_top = 0

      ret = button~scroll(stepx, stepy, trunc(sleft), trunc(s_top), trunc(srgt), trunc(sbot))
      if ret = 1 then do
         if (timer \== 0) then ret = winTimer("STOP", timer)
         return 1
      end

      if (stepx < 0) then left = r.right + stepx; else left = curX
      rgt = curX+xl

      if rgt > r.right then rgt = r.right
      if left < 0 then left = 0

      if (stepy < 0) then top = r.bottom+stepy; else top = curY
      bot = curY+yl
      if bot > r.bottom then bot = r.bot
      if top < 0 then top = 0

      if stepx \= 0 & drawx=1 then self~drawBitmap(hwnd, id, left, top, cursx, cursy, abs(stepx), yl)
      if stepy \= 0 & drawy=1 then self~drawBitmap(hwnd, id, left, top, cursx, cursy, xl, abs(stepy))
      if (timer \== 0) then ret = winTimer("WAIT", timer)

      drawx = 0
      if (stepx <= 0) then do
         if cursx < xl then do
            cursx = cursx - stepx
            if cursx < xl then drawx = 1
         end
      end
      else do
         if cursx > 0 then do
            cursx = cursx - stepx
            if cursx > 0 then drawx = 1
         end
      end
      curX = curX + stepx

      drawy = 0
      if (stepy <= 0) then do
         if cursy < yl then do
            cursy = cursy - stepy
            if cursy < yl then drawy = 1
         end
      end
      else do
         if cursy > 0 then do
            cursy = cursy - stepy
            if cursy > 0 then drawy = 1
         end
      end
      curY = curY + stepy

                 /* so repaint will draw the cursor at the current position */
      if displace then self~setBitmapPosition(id, trunc(curX), trunc(curY));

      worktodo = 0
      if (stepx>0) & (curX+stepx <= tox) then worktodo = 1
      if (stepy>0) & (curY+stepy <= toy) then worktodo = 1
      if (stepx<0) & (curX+stepx >= tox) then worktodo = 1
      if (stepy<0) & (curY+stepy >= toy) then worktodo = 1
   end
   if (timer \== 0) then ret = winTimer("STOP", timer)
   self~setBitmapPosition(id, tox, toy);
   self~redrawButton(id,0)
   return 0


/******************************************************************************/
/* Graphics and drawing extensions */

::method writeToWindow unguarded external "LIBRARY oodialog dlgext_writeToWindow"

::method writeToControl unguarded
   argArray = arg(1,"A")
   argArray[1] = self~getControlHandle(arg(1))
   forward message 'writeToWindow' arguments (argArray)

::method getWindowDC unguarded external "LIBRARY oodialog dlgext_getWindowDC"
::method getControlDC unguarded
   use strict arg id
   return self~getWindowDC(self~getControlHandle(id))

::method freeWindowDC unguarded external "LIBRARY oodialog dlgext_freeWindowDC"
::method freeControlDC unguarded
   use strict arg id, dc
   hwnd = self~getControlHandle(id)
   return self~freeWindowDC(hwnd, dc)



/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*\
  These menu bar methods are needed to provide backward compatibility for the
  deprecated menu methods.  The Rexx programmer is *strongly* discouraged from
  using them.  If the deprecated methods are removed, these will be removed with
  them.

  In addition, it is highly likely that the results will be unpredictable.

  To obtain the menuBar attached to the dialog, use the getMenuBar() method.
\* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

::attribute menuBar private
::attribute isLinked private

::method linkMenu
  use strict arg menu

  signal on syntax

  if \ menu~isA(.MenuBar) then raise syntax 93.948 array(1, "MenuBar")

  oldMenu = self~menuBar
  self~menuBar = menu
  self~isLinked = .true
  return oldMenu

syntax:
  c = condition('O')
  if c~traceBack~isA(.List) then c~traceBack~empty
  raise propagate

::method unlinkMenu
  menu = self~menuBar
  self~menuBar = .nil
  self~isLinked = .false
  return menu

-- Deprecated to end of file.
::method captureMouse
  use strict arg
  mouse = .Mouse~new(self)
  return mouse~capture

::method getMouseCapture
  use strict arg
  mouse = .Mouse~new(self)
  return mouse~getCapture

::method isMouseButtonDown
  use strict arg whichButton = "LEFT"
  mouse = .Mouse~new(self)
  return mouse~isButtonDown(whichButton)

::method releaseMouseCapture
  use strict arg
  mouse = .Mouse~new(self)
  return mouse~releaseCapture

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*\
  The following menu bar methods are deprecated, instead, use the Menu classes
  directly.
\* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

::method enableMenuItem unguarded
    use strict arg id

    signal on syntax

    if self~menuBar == .nil then raise syntax 93.900 array("there is no MenuBar attached to this dialog")
    if self~menuBar~isA(.UserMenuBar), \self~menuBar~isComplete then raise syntax 93.900 array("the UserMenuBar is not complete")
    return self~menuBar~enable(id)

syntax:
  c = condition('O')
  if c~traceBack~isA(.List) then c~traceBack~empty
  raise propagate

::method disableMenuItem unguarded
    use strict arg id

    signal on syntax

    if self~menuBar == .nil then raise syntax 93.900 array("There is no MenuBar attached to this dialog")
    if self~menuBar~isA(.UserMenuBar), \self~menuBar~isComplete then raise syntax 93.900 array("the UserMenuBar is not complete")
    return self~menuBar~disable(id)

syntax:
  c = condition('O')
  if c~traceBack~isA(.List) then c~traceBack~empty
  raise propagate

::method checkMenuItem unguarded
    use strict arg id

    signal on syntax

    if self~menuBar == .nil then raise syntax 93.900 array("There is no MenuBar attached to this dialog")
    if self~menuBar~isA(.UserMenuBar), \self~menuBar~isComplete then raise syntax 93.900 array("the UserMenuBar is not complete")
    return self~menuBar~check(id)

syntax:
  c = condition('O')
  if c~traceBack~isA(.List) then c~traceBack~empty
  raise propagate

::method uncheckMenuItem unguarded
    use strict arg id

    signal on syntax

    if self~menuBar == .nil then raise syntax 93.900 array("There is no MenuBar attached to this dialog")
    if self~menuBar~isA(.UserMenuBar), \self~menuBar~isComplete then raise syntax 93.900 array("the UserMenuBar is not complete")
    return self~menuBar~uncheck(id)

syntax:
  c = condition('O')
  if c~traceBack~isA(.List) then c~traceBack~empty
  raise propagate

::method grayMenuItem unguarded
    use strict arg id

    signal on syntax

    if self~menuBar == .nil then raise syntax 93.900 array("There is no MenuBar attached to this dialog")
    if self~menuBar~isA(.UserMenuBar), \self~menuBar~isComplete then raise syntax 93.900 array("the UserMenuBar is not complete")
    return self~menuBar~gray(id)

syntax:
  c = condition('O')
  if c~traceBack~isA(.List) then c~traceBack~empty
  raise propagate

::method setMenuItemRadio unguarded
    use strict arg idstart, idend, idset

    signal on syntax

    if self~menuBar == .nil then raise syntax 93.900 array("There is no MenuBar attached to this dialog")
    if self~menuBar~isA(.UserMenuBar), \self~menuBar~isComplete then raise syntax 93.900 array("the UserMenuBar is not complete")
    return self~menuBar~checkRadio(idstart, idend, idset)

syntax:
  c = condition('O')
  if c~traceBack~isA(.List) then c~traceBack~empty
  raise propagate

::method getMenuItemState unguarded
    use strict arg id

    signal on syntax

    if self~menuBar == .nil then raise syntax 93.900 array("There is no MenuBar attached to this dialog")
    if self~menuBar~isA(.UserMenuBar), \self~menuBar~isComplete then raise syntax 93.900 array("the UserMenuBar is not complete")
    return self~menuBar~getItemState(id)

syntax:
  c = condition('O')
  if c~traceBack~isA(.List) then c~traceBack~empty
  raise propagate

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*\
  End deprecated menu bar methods.
\* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
