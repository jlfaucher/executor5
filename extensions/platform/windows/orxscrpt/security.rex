/*----------------------------------------------------------------------------*/
/*                                                                            */
/* Copyright (c) 1995, 2004 IBM Corporation. All rights reserved.             */
/* Copyright (c) 2005-2014 Rexx Language Association. All rights reserved.    */
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
::CLASS engineSecurity

/********************************************************************/
/* Security manager methods must return a value of either 0 or 1 to */
/* the language processor. A  value of 0 indicates that the  program*/
/* is authorized to perform the indicated action. In this case,     */
/* processing continues as usual. A value of 1 indicates that the   */
/* security  manager performed the action itself. The security      */
/* manager sets entries in the  information directory to pass       */
/* results for the action back to the language  processor.          */
/********************************************************************/

::METHOD new CLASS
  SIGNAL ON syntax
  if arg() < 1 then RAISE SYNTAX 93.903 ARRAY(1) Description "security flags are needed"
  FORWARD CLASS (super)

syntax:
  RAISE PROPAGATE

::METHOD init
  expose untrustedCaller untrustedData
  use strict arg flags

  untrustedCaller = .false
  untrustedData = .false

  flags = flags~d2x~x2b    -- convert into a bit pattern
  if flags~subchar(flags~length) == '1' then untrustedData = .true
  if flags~subchar(flags~length - 1) == '1' then untrustedCaller = .true


-- sent for all external function calls
::METHOD call
   expose untrustedCaller
  if untrustedCaller then
    RAISE SYNTAX 48.1 ARRAY ('External function call not allowed in sandbox mode!')
--  directory~setentry('RESULT', "ACCESS DENIED")*/
  return 0

-- sent for all host command instructions
::METHOD command
   expose untrustedCaller
  if untrustedCaller then
    RAISE SYNTAX 48.1 ARRAY ('Host command invocation not allowed in sandbox mode!')
  return 0

-- sent whenever a ::REQUIRES directive in the file is processed
::METHOD requires
   expose untrustedCaller
  use arg directory
  if untrustedCaller then do
-- set the security manager for the required file as well
    directory~setentry("SECURITYMANAGER", self)
  end
-- say we didn't handle so the usual routines resolve to full path for us
  return 0

-- sent whenever REXX is going to access an entry in the .LOCAL
-- directory as part of the resolution of the environment symbol name
::METHOD local
   expose untrustedCaller
  use arg directory
  if untrustedCaller then do
    request = directory~name
    if (request == "OUTPUT") | (request == "STDOUT") | (request == "STDQUE") |,
       (request == "STDERR") | (request == "ERROR") then
      RAISE SYNTAX 48.1 ARRAY ('.LOCAL access ('request') limited in sandbox mode!')
  end
  return 0

-- sent whenever REXX is going to access an entry in the .ENVIRONMENT
-- directory as part of the resolution of the environment symbol name.
::METHOD environment
  expose untrustedCaller
  use arg directory
  if untrustedCaller then do
    request = directory~name
    if (request == "QUEUE") | (request == "STREAM") | (request == "RX_QUEUE") then
      RAISE SYNTAX 48.1 ARRAY ('.ENVIRONMENT access ('request') limited in sandbox mode!')
  end
  return 0

-- sent whenever one of the REXX input and output built-in functions
-- (CHARIN, CHAROUT, CHARS, LINEIN, LINEOUT, LINES, or STREAM) needs
-- to resolve a stream name
::METHOD stream
  expose untrustedCaller
  use arg directory
  if untrustedCaller then
    RAISE SYNTAX 48.1 ARRAY ('File I/O operation not allowed in sandbox mode!')
  return 0

-- sent whenever a secure program attempts to send a message for a
-- protected method (see the ::METHOD directive ::METHOD) to an object.
::METHOD method
  expose untrustedCaller
  use arg directory
  if untrustedCaller then do
    request = directory~name
    if request == "SETSECURITYMANAGER" then
      RAISE SYNTAX 48.1 ARRAY ('Change of the security manager not allowed in sandbox mode!')
  end
  return 0
