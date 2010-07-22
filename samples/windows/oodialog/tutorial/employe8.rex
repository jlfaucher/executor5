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
/****************************************************************************/
/* Name: EMPLOYE8.REX                                                       */
/* Type: Object REXX Script                                                 */
/*                                                                          */
/*                                                                          */
/****************************************************************************/


signal on any name CleanUp

dlg = .MyDialogClass~new
if dlg~InitCode <> 0 then exit
dlg~Execute("SHOWTOP")
dlg~deinstall
exit

/* ------- signal handler to destroy dialog if condition trap happens  -----*/
CleanUp:
   call errorDialog "Error" rc "occurred at line" sigl":" errortext(rc),
                     || "a"x || condition("o")~message
   if dlg~isDialogActive then do
      dlg~finished = .true
      dlg~stopIt
   end


::requires "ooDialog.cls"

::class MyDialogClass subclass UserDialog

::method Employees attribute
::method Emp_count attribute
::method Emp_current attribute

::method Init
    expose font
    font = 0
    ret = self~init:super;
    if ret = 0 then ret = self~Load("EMPLOYE5.RC", 100)
    if ret = 0 then self~Employees = .array~new(10)
    if ret = 0 then do
        self~Emp_count = 1
        self~Emp_current = 1
        self~connectButtonEvent(10, "CLICKED", "Print")   /* connect button 10 with a method */
        self~connectButtonEvent(12, "CLICKED", "Add")     /* connect button 12 with a method */
        self~connectButtonEvent(13, "CLICKED", "Emp_List")
        self~ConnectDraw(1, "DrawOK")
    end
    self~InitCode = ret
    return ret

::method InitDialog
    expose font
    self~City = "New York"
    self~Male = 1
    self~Female = 0
    self~AddComboEntry(22, "Munich")
    self~AddComboEntry(22, "New York")
    self~AddComboEntry(22, "San Francisco")
    self~AddComboEntry(22, "Stuttgart")
    self~AddListEntry(23, "Business Manager")
    self~AddListEntry(23, "Software Developer")
    self~AddListEntry(23, "Broker")
    self~AddListEntry(23, "Police Man")
    self~AddListEntry(23, "Lawyer")
    self~connectEachSBEvent(11, "Emp_Previous", "Emp_Next")
    self~DisableItem(11)

    additional = .directory~new
    additional~weight = 600
    font = self~createFontEx("Script", 16, additional)
    self~setControlFont(1, font)
    self~disableItem(13)

::method Print
    self~GetData
    if self~Male = 1 then title = "Mr."; else title = "Ms."
    if self~Married = 1 then addition = " (married) "; else addition = ""
    call infoDialog title self~Name addition || "A"x || "City:" self~City || "A"x ||,
                     "Profession:" self~Profession

::method Add
    self~Employees[self~Emp_count] = .directory~new
    self~Employees[self~Emp_count]['NAME'] = self~getControlData(21)
    self~Employees[self~Emp_count]['CITY'] = self~getControlData(22)
    self~Employees[self~Emp_count]['PROFESSION'] = self~getControlData(23)
    if self~getControlData(31) = 1 then sex = 1; else sex = 2
    self~Employees[self~Emp_count]['SEX'] = sex
    self~Employees[self~Emp_count]['MARRIED'] = self~getControlData(41)
    self~Emp_count = self~Emp_count +1
    self~Emp_current = self~Emp_count
    self~setControlData(21, "");
    self~SetSBRange(11, 1, self~Emp_count)
    self~SetSBPos(11, self~Emp_count)
    self~EnableItem(11)
    self~EnableItem(13)


::method Set
    self~setControlData(21, self~Employees[self~Emp_current]['NAME'])
    self~setControlData(22, self~Employees[self~Emp_current]['CITY'])
    self~setControlData(23, self~Employees[self~Emp_current]['PROFESSION'])
    if self~Employees[self~Emp_current]['SEX'] = 1 then do
       self~setControlData(31, 1);self~setControlData(32, 0); end
    else do
       self~setControlData(31, 0);self~setControlData(32, 1); end
    self~setControlData(41, self~Employees[self~Emp_current]['MARRIED'])

::method Emp_Previous
   if self~Emp_count = 1 then return
   if self~Emp_current > 1 then do
       self~Emp_current = self~Emp_current - 1
       self~SetSBPos(11, self~Emp_current)
       self~Set
   end; else
       call TimedMessage "You reached the top!","Info",1000

::method Emp_Next
   if self~Emp_count = 1 then return
   if self~Emp_current < self~Emp_count-1 then do
       self~Emp_current = self~Emp_current + 1
       self~SetSBPos(11, self~Emp_current)
       self~Set
   end; else
       call TimedMessage "You reached the bottom!","Info",1000

::method Emp_List
   ldlg = .EmployeeListClass~new(self)
   ldlg~Execute("SHOWTOP")


::method FillList
   use arg subdlg, id
   column1 = 24;column2 = 29
   do i = 1 to self~Emp_count-1
       if self~Employees[i]['SEX'] = 1 then title = "Mr."; else title = "Ms."
       addstring = title self~Employees[i]['NAME']
       spacebetween = column1 - self~Employees[i]['NAME']~length - 5
       if spacebetween > 0 then addstring = addstring || " "~copies(spacebetween)
       addstring = addstring || " "self~Employees[i]['PROFESSION']
       spacebetween = column2 - self~Employees[i]['PROFESSION']~length - 5
       if spacebetween > 0 then addstring = addstring || " "~copies(spacebetween)
       addstring = addstring || " "self~Employees[i]['CITY']
       subdlg~AddListEntry(id, addstring)
   end

::method Leaving
   expose font
   if font \= 0 then self~DeleteFont(font)


::class EmployeeListClass subclass UserDialog

::method Parent attribute

::method Init
   expose font
   use arg ParentDlg
   self~Parent = ParentDlg
   font = 0
   ret = self~init:super
   if ret = 0 then ret = self~Load("EMPLOYE5.RC", 101)
   self~InitCode = ret
   return ret

::method InitDialog
   expose font
   font = self~createFontEx("Courier", 14)
   self~setControlFont(101, font)
   self~parent~FillList(self, 101)

::method Leaving
   expose font
   if font \= 0 then self~DeleteFont(font)