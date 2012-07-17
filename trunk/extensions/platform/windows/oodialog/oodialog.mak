#/*----------------------------------------------------------------------------*/
#/*                                                                            */
#/* Copyright (c) 1995, 2004 IBM Corporation. All rights reserved.             */
#/* Copyright (c) 2005-2012 Rexx Language Association. All rights reserved.    */
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

# If OOD_INDEPENDENT is defined we are operating outside of the interpreter
# build.  In that case, OOD_OUTDIR, OOD_OODIALOGSRC, and OOD_INCLUDE_File are
# defined in the parent make file that calls us.  When called in the interpreter
# build process, we set those macros using the OR_xx values set by the build
# batch files.

!ifdef OOD_INDEPENDENT

REXXAPI_LIBS = $(REXX_LIBS)

!else

OOD_OUTDIR=$(OR_OUTDIR)
OOD_OODIALOGSRC=$(OR_OODIALOGSRC)
REXXAPI_LIBS = $(OR_OUTDIR)\rexx.lib $(OR_OUTDIR)\rexxapi.lib
OOD_INCLUDE_FILE = "$(OR_LIBSRC)\ORXWIN32.MAK"

!endif

# Generate the version information.  Quit if there is an error.
!IF [generateVersionFile.bat] != 0
!  ERROR Build error: could not gerate version file, ooDialog.ver.incl
!ENDIF

!include ooDialog.ver.incl
!include $(OOD_INCLUDE_FILE)

# The ooDialog specific version definition
ood_ver_def = -DOOD_VER=$(OOD_MAJOR) -DOOD_REL=$(OOD_MINOR) -DOOD_MOD=$(OOD_MOD_LVL) -DOOD_BLD=$(OOD_BLD_LVL) -DOOD_COPY_YEAR=\"$(OOD_COPY_YEAR)\"

# We use our own rc flags version.
rcflags_oodialog = rc /NOLOGO /DWIN32 -dOODIALOG_VER=$(OOD_MAJOR) -dOODIALOG_REL=$(OOD_MINOR) -dOODIALOG_SUB=$(OOD_MOD_LVL) -dOODIALOG_BLD=$(OOD_BLD_LVL) -dOODIALOG_VER_STR=\"$(OOD_VER_STR)\" -dOODIALOG_COPY_YEAR=\"$(OOD_COPY_YEAR)\" -dMANIFEST_FILE=$(M_FILE)

C=cl
OPTIONS= $(cflags_common) $(ood_ver_def) $(cflags_dll) $(OR_ORYXINCL)

SOURCEF= $(OOD_OUTDIR)\oovutil.obj $(OOD_OUTDIR)\oovdata.obj $(OOD_OUTDIR)\oovtext.obj $(OOD_OUTDIR)\oovtools.obj \
         $(OOD_OUTDIR)\oovmsg.obj $(OOD_OUTDIR)\oovscrll.obj $(OOD_OUTDIR)\oovdeskt.obj $(OOD_OUTDIR)\oovdraw.obj \
         $(OOD_OUTDIR)\oovuser.obj $(OOD_OUTDIR)\oovbmp.obj $(OOD_OUTDIR)\oovother.obj $(OOD_OUTDIR)\menu.obj \
         $(OOD_OUTDIR)\oodialog.res

all:  $(OOD_OUTDIR)\oodialog.dll

.c{$(OOD_OUTDIR)}.obj:
    $(C) $(OPTIONS)  /DINCL_32  -c $(@B).c /DCREATEDLL /Fo$(OOD_OUTDIR)\$(@B).obj

#
# *** .cpp -> .obj rules
#
{$(OOD_OODIALOGSRC)}.cpp{$(OOD_OUTDIR)}.obj:
    @ECHO .
    @ECHO Compiling $(@B).cpp
    $(OR_CC) $(cflags_common) $(ood_ver_def) $(cflags_dll) /DCREATEDLL /Fo$(OOD_OUTDIR)\$(@B).obj $(OR_ORYXINCL)  $(OOD_OODIALOGSRC)\$(@B).cpp


{$(OOD_OODIALOGSRC)}.c{$(OOD_OUTDIR)}.obj:
    @ECHO .
    @ECHO Compiling $(@B).c
    $(OR_CC) $(cflags_common) $(ood_ver_def)  $(cflags_dll) /DCREATEDLL /Fo$(OOD_OUTDIR)\$(@B).obj $(OR_ORYXINCL)  $(OOD_OODIALOGSRC)\$(@B).c


$(OOD_OUTDIR)\oodialog.dll: $(SOURCEF)
    @ECHO .
    @ECHO Linking $(OOD_OUTDIR)\oodialog.dll
    $(OR_LINK) \
    $(SOURCEF)  \
    $(lflags_common) $(lflags_dll) \
    $(REXXAPI_LIBS) \
    WINMM.LIB \
    COMDLG32.LIB \
    COMCTL32.LIB \
    shlwapi.lib \
    -def:$(OOD_OODIALOGSRC)\oovutil.def \
    -out:$(OOD_OUTDIR)\$(@B).dll


# Update the version information block
$(OOD_OUTDIR)\oodialog.res: $(OOD_OODIALOGSRC)\oodialog.rc
    @ECHO .
    @ECHO ResourceCompiling $(@B).res
    $(rc) $(rcflags_oodialog) /i $(OOD_OODIALOGSRC) /i $(OR_WINKERNELSRC) -r -fo$(OOD_OUTDIR)\$(@B).res $(OOD_OODIALOGSRC)\$(@B).rc
