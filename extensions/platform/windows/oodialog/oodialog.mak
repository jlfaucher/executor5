#/*----------------------------------------------------------------------------*/
#/*                                                                            */
#/* Copyright (c) 1995, 2004 IBM Corporation. All rights reserved.             */
#/* Copyright (c) 2005-2008 Rexx Language Association. All rights reserved.    */
#/*                                                                            */
#/* This program and the accompanying materials are made available under       */
#/* the terms of the Common Public License v1.0 which accompanies this         */
#/* distribution. A copy is also available at the following address:           */
#/* http://www.oorexx.org/license.html                                         */
#/*                                                                            */
#/* Redistribution and use in source and binary forms, with or                 */
#/* without modification, are permitted provided that the following            */
#/* conditions are met:                                                        */
#/*                                                                            */
#/* Redistributions of source code must retain the above copyright             */
#/* notice, this list of conditions and the following disclaimer.              */
#/* Redistributions in binary form must reproduce the above copyright          */
#/* notice, this list of conditions and the following disclaimer in            */
#/* the documentation and/or other materials provided with the distribution.   */
#/*                                                                            */
#/* Neither the name of Rexx Language Association nor the names                */
#/* of its contributors may be used to endorse or promote products             */
#/* derived from this software without specific prior written permission.      */
#/*                                                                            */
#/* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS        */
#/* "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT          */
#/* LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS          */
#/* FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT   */
#/* OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,      */
#/* SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED   */
#/* TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,        */
#/* OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY     */
#/* OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING    */
#/* NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS         */
#/* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.               */
#/*                                                                            */
#/*----------------------------------------------------------------------------*/

# NOTE:  /OPT:REF in linker flags eliminates unreferenced functions and data.
#        Need to use /Gy when compiling to use /OPT:REF.

# NMAKE-compatible MAKE file for ooDialog
all:  $(OR_OUTDIR)\oodialog.dll

!include "$(OR_LIBSRC)\ORXWIN32.MAK"
C=cl
OPTIONS= $(cflags_common) $(cflags_dll) $(OR_ORYXINCL)
OR_LIB=$(OR_OUTDIR)

# All Source Files
SOURCEF= $(OR_OUTDIR)\APICommon.obj $(OR_OUTDIR)\oodCommon.obj $(OR_OUTDIR)\oodControl.obj $(OR_OUTDIR)\oodData.obj \
         $(OR_OUTDIR)\ooDialog.obj $(OR_OUTDIR)\oodMenu.obj $(OR_OUTDIR)\oodMessaging.obj $(OR_OUTDIR)\oodPackageEntry.obj \
         $(OR_OUTDIR)\oodRoutines.obj $(OR_OUTDIR)\oodUser.obj $(OR_OUTDIR)\oodUtilities.obj $(OR_OUTDIR)\oovbmp.obj \
         $(OR_OUTDIR)\oovdeskt.obj $(OR_OUTDIR)\oovdraw.obj $(OR_OUTDIR)\oovother.obj $(OR_OUTDIR)\oovscrll.obj \
         $(OR_OUTDIR)\oodText.obj $(OR_OUTDIR)\oodialog.res

# All Source files that include oodCommon.hpp
COMMON_SOURCEF = $(OR_OUTDIR)\oodCommon.obj $(OR_OUTDIR)\oodData.obj $(OR_OUTDIR)\oodMenu.obj $(OR_OUTDIR)\oodMessaging.obj \
                 $(OR_OUTDIR)\oodRoutines.obj $(OR_OUTDIR)\oodUser.obj $(OR_OUTDIR)\oodUtilities.obj $(OR_OUTDIR)\oovbmp.obj \
                 $(OR_OUTDIR)\oovdeskt.obj $(OR_OUTDIR)\oovdraw.obj $(OR_OUTDIR)\oovother.obj $(OR_OUTDIR)\oovscrll.obj \
                 $(OR_OUTDIR)\oodText.obj

# All Source files that include APICommon.hpp
APICOMMON_SOURCEF = $(OR_OUTDIR)\APICommon.obj $(OR_OUTDIR)\oodCommon.obj $(OR_OUTDIR)\oodControl.obj $(OR_OUTDIR)\oodData.obj \
                    $(OR_OUTDIR)\ooDialog.obj $(OR_OUTDIR)\oodMenu.obj $(OR_OUTDIR)\oodMessaging.obj $(OR_OUTDIR)\oodText.obj \
                    $(OR_OUTDIR)\oodUser.obj $(OR_OUTDIR)\oodUtilities.obj $(OR_OUTDIR)\oovother.obj

# All Source files that include oodText.hpp
OODTEXT_SOURCEF = $(OR_OUTDIR)\oodControl.cpp $(OR_OUTDIR)\ooDialog.cpp $(OR_OUTDIR)\oodText.cpp $(OR_OUTDIR)\oodUser.obj \
                  $(OR_OUTDIR)\oodUtilities.cpp $(OR_OUTDIR)\oovother.cpp

# All Source files that include oodData.hpp
OODDATA_SOURCEF = $(OR_OUTDIR)\ooDialog.cpp $(OR_OUTDIR)\oodData.obj $(OR_OUTDIR)\oodUser.obj

# All Source files that include oodControl.hpp
OODCONTROL_SOURCEF = $(OR_OUTDIR)\oodControl.obj $(OR_OUTDIR)\oodData.obj $(OR_OUTDIR)\ooDialog.cpp $(OR_OUTDIR)\oodUser.obj \
                     $(OR_OUTDIR)\oovother.obj

.c{$(OR_OUTDIR)}.obj:
    $(C) $(OPTIONS)  /DINCL_32  -c $(@B).c /Fo$(OR_OUTDIR)\$(@B).obj

#
# *** .cpp -> .obj rules
#
{$(OR_OODIALOGSRC)}.cpp{$(OR_OUTDIR)}.obj:
    @ECHO .
    @ECHO Compiling $(@B).cpp
    $(OR_CC) $(cflags_common) $(cflags_dll) /Fo$(OR_OUTDIR)\$(@B).obj $(OR_ORYXINCL)  $(OR_OODIALOGSRC)\$(@B).cpp


{$(OR_OODIALOGSRC)}.c{$(OR_OUTDIR)}.obj:
    @ECHO .
    @ECHO Compiling $(@B).c
    $(OR_CC) $(cflags_common) $(cflags_dll) /Fo$(OR_OUTDIR)\$(@B).obj $(OR_ORYXINCL)  $(OR_OODIALOGSRC)\$(@B).c


$(OR_OUTDIR)\oodialog.dll:     $(SOURCEF)
    $(OR_LINK) \
        $(SOURCEF)  \
    $(lflags_common) $(lflags_dll) \
    $(OR_LIB)\rexx.lib \
    $(OR_LIB)\rexxapi.lib \
    WINMM.LIB \
    COMDLG32.LIB \
    COMCTL32.LIB \
    shlwapi.lib \
    -def:$(OR_OODIALOGSRC)\ooDialog.def \
    -out:$(OR_OUTDIR)\$(@B).dll


# Update the version information block
$(OR_OUTDIR)\oodialog.res: $(OR_OODIALOGSRC)\oodialog.rc
    @ECHO .
    @ECHO ResourceCompiling $(@B).res
        $(rc) $(rcflags_common) /i $(OR_OODIALOGSRC) /i $(OR_WINKERNELSRC) -r -fo$(OR_OUTDIR)\$(@B).res $(OR_OODIALOGSRC)\$(@B).rc

# Source .obj files that should be recompiled when header file(s) change.
$(SOURCEF) : ooDialog.hpp
$(COMMON_SOURCEF) : oodCommon.hpp
$(APICOMMON_SOURCEF) : APICommon.hpp
$(OODTEXT_SOURCEF) : oodText.hpp
$(OODDATA_SOURCEF) : oodData.hpp
$(OODCONTROL_SOURCEF) : oodControl.hpp
$(OR_OUTDIR)\oodMenu.obj : oodMenu.hpp
