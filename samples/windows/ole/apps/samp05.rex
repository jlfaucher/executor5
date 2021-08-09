/*----------------------------------------------------------------------------*/
/*                                                                            */
/* Copyright (c) 1995, 2004 IBM Corporation. All rights reserved.             */
/* Copyright (c) 2005-2014 Rexx Language Association. All rights reserved.    */
/*                                                                            */
/* This program and the accompanying materials are made available under       */
/* the terms of the Common Public License v1.0 which accompanies this         */
/* distribution. A copy is also available at the following address:           */
/* https://www.oorexx.org/license.html                         */
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
/**********************************************************************/
/*                                                                    */
/* SAMP05.REX: OLE Automation with Object REXX - Sample 5             */
/*                                                                    */
/* Create a new document in WordPro 97, enter some text with          */
/* different attributes and finally save and print the document.      */
/*                                                                    */
/* Since no check is done do ensure the new document does not already */
/* exist you will get a popup message from WordPro asking to          */
/* overwrite an already existing document when this sample is run     */
/* multiple times.                                                    */
/*                                                                    */
/**********************************************************************/

WordProApp = .OLEObject~New("WordPro.Application")
WordProApp~NewDocument("AutomatedDocument.lwp","","default.mwp")
WPDoc = WordProApp~ActiveDocument
WPDocText = WordProApp~Text

WordProApp~Type("This is the first paragraph entered from REXX via ")
WordProApp~Type("the OLE automation classes.[Enter]")

WordProApp~Type("The second paragraph will be changed in its ")
WordProApp~Type("appearance.")
WordProApp~SelectParagraph
WPDocText~Font~Name = "Arial"
WPDocText~Font~Bold = .True
WPDocText~Font~Italic = .True
WPDocText~Font~Size = 15

WordProApp~Type("[End][Enter][End]Document created at:" Time("N") "on" Date("N"))

WPDoc~Save

/* if you want this document printed, comment in the next line */
--WordProApp~PrintOut(1, 1, 1, .True)

say "Created" WPDoc~Path"\"WPDoc~Name

WPDoc~Close
WordProApp~Quit


