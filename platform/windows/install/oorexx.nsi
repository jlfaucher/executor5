;
; ooRexx Install Script, based on Modern Example Script Written by Joost Verburg
; Requires: ${NSISDIR}\Plugins\FindProcDll.dll - http://nsis.sourceforge.net/FindProcDLL_plug-in
; Requires: ${NSISDIR}\Plugins\KillProcDll.dll - http://nsis.sourceforge.net/KillProcDLL_plug-in
; Requires: ${NSISDIR}\Plugins\services.dll    - http://nsis.sourceforge.net/File:Services.zip
; Run as:
;  makensis /DVERSION=x.x /DNODOTVER=xx /DSRCDIR=xxx /DBINDIR=xxx oorexx.nsi
;  eg
;  makensis /DVERSION=3.1.2 /DNODOTVER=312 /DSRCDIR=d:\oorexx\oorexx /DBINDIR=d:\oorexx\oorexx\win32rel oorexx.nsi
; Note:
;  oorexx.nsi MUST be in the current directory!

!define LONGNAME "Open Object Rexx"  ;Long Name (for descriptions)
!define SHORTNAME "ooRexx" ;Short name (no slash) of package
!define DISPLAYICON "$INSTDIR\rexx.exe,0"
!define UNINSTALLER "uninstall.exe"
!define KEYFILE     "rexx.exe"

!define MUI_ICON "${SRCDIR}\platform\windows\rexx.ico"
!define MUI_UNICON "${SRCDIR}\platform\windows\install\uninstall.ico"

Name "${LONGNAME} ${VERSION}"

!include "MUI.nsh"
!include "Library.nsh"

!define MUI_CUSTOMPAGECOMMANDS
!define MUI_WELCOMEFINISHPAGE_BITMAP "orange.bmp"
!define MUI_UNWELCOMEFINISHPAGE_BITMAP "orange-uninstall.bmp"

!define MUI_LICENSEPAGE
!define MUI_COMPONENTSPAGE
!define MUI_DIRECTORYPAGE
!define MUI_FINISHPAGE
!define MUI_FINISHPAGE_NOAUTOCLOSE

!define MUI_ABORTWARNING

!define MUI_UNINSTALLER
!define MUI_UNCONFIRMPAGE
!define MUI_UNFINISHPAGE_NOAUTOCLOSE

;define MUI_CUSTOMFUNCTION_GUIINIT myGuiInit

;--------------------------------
;Configuration

  ;General
  OutFile "${SHORTNAME}${NODOTVER}.exe"
  ShowInstdetails show
  SetOverwrite on
  SetPluginUnload alwaysoff

  ;Folder-select dialog
  InstallDir "$PROGRAMFILES\${SHORTNAME}"

  LangString TEXT_IO_PAGETITLE_RXAPI ${LANG_ENGLISH} "The ooRexx rxapi process"
  LangString TEXT_IO_SUBTITLE_RXAPI ${LANG_ENGLISH} "Install rxapi as a Windows Service"
;--------------------------------
;Pages

  !insertmacro MUI_PAGE_WELCOME
  !insertmacro MUI_PAGE_LICENSE "${SRCDIR}\CPLv1.0.txt"
  !define MUI_PAGE_CUSTOMFUNCTION_PRE CheckForRxAPI
  !insertmacro MUI_PAGE_COMPONENTS
  !insertmacro MUI_PAGE_DIRECTORY
;  Page custom SetCustomAssoc
;  Page custom SetCustomLanguage
  Page custom SetCustomRxAPI
  !insertmacro MUI_PAGE_INSTFILES
  !insertmacro MUI_PAGE_FINISH

  !insertmacro MUI_UNPAGE_WELCOME
  !define MUI_PAGE_CUSTOMFUNCTION_LEAVE un.CheckForRxAPI
  !insertmacro MUI_UNPAGE_CONFIRM
  !insertmacro MUI_UNPAGE_INSTFILES
  !insertmacro MUI_UNPAGE_FINISH
;--------------------------------
;Language
!insertmacro MUI_LANGUAGE "English"

;--------------------------------
;Reserved files

  ReserveFile "oorexx_ss.ini"
  !insertmacro MUI_RESERVEFILE_INSTALLOPTIONS

;--------------------------------
; Variables
Var rxapichk
Var IsAdminUser

;========================================================================
;Installer Sections

;------------------------------------------------------------------------
; Core

Section "${LONGNAME} Core (required)" SecMain
  SectionIn 1 RO
  ; Set output path to the installation directory.
  SetOutPath $INSTDIR
  ; Set the REXX_HOME environment variable
  Push "REXX_HOME"
  Push $INSTDIR
  Push $IsAdminUser ; "true" or "false"
  Call WriteEnvStr
  ; Distribution executables...
  File "${BINDIR}\rexx.exe"
  File "${BINDIR}\rexx.img"
  File "${BINDIR}\rexxc.exe"
  File "${BINDIR}\rxsubcom.exe"
  File "${BINDIR}\rxqueue.exe"
  File "${BINDIR}\rxapi.exe"
  File "${BINDIR}\rexxhide.exe"
  File "${BINDIR}\rexxpaws.exe"
  ; Distribution DLLs...
  File "${BINDIR}\rexx.dll"
  File "${BINDIR}\rexxapi.dll"
  File "${BINDIR}\rexxutil.dll"
  File "${BINDIR}\rxmath.dll"
  File "${BINDIR}\rxsock.dll"
  File "${BINDIR}\rxregexp.dll"
  File "${BINDIR}\rxwinsys.dll"
  File "${BINDIR}\oodialog.dll"
  File "${BINDIR}\orexxole.dll"
  ; CLASS files...
  File "${BINDIR}\winsystm.cls"
  File "${BINDIR}\rxregexp.cls"
  File "${BINDIR}\rxftp.cls"
  File "${BINDIR}\orexxole.cls"
  File "${BINDIR}\oodialog.cls"
  File "${BINDIR}\oodwin32.cls"
  File "${BINDIR}\oodplain.cls"
  CreateDirectory "$SMPROGRAMS\${LONGNAME}"
  ; rexxtry is technically a sample, but it is heavily used, so add it to
  ; the executables.
  File "${SRCDIR}\samples\rexxtry.rex"
  CreateShortCut "$SMPROGRAMS\${LONGNAME}\Try Rexx.lnk" "$INSTDIR\rexx.exe" '"$INSTDIR\rexxtry.rex"' "$INSTDIR\rexx.exe"
  ; Other files...
  File "${SRCDIR}\platform\windows\rexx.ico"
  File "${SRCDIR}\CPLv1.0.txt"
  ; readmes
  ; Set output path to the installation directory.
  SetOutPath $INSTDIR\doc
  File "${SRCDIR}\doc\readme.pdf"
  File /oname=CHANGES.txt "${SRCDIR}\CHANGES"
  CreateDirectory "$SMPROGRAMS\${LONGNAME}\Documentation"
  CreateShortCut "$SMPROGRAMS\${LONGNAME}\Documentation\ooRexx README.lnk" "$INSTDIR\doc\readme.pdf" "" "$INSTDIR\doc\readme.pdf" 0
  CreateShortCut "$SMPROGRAMS\${LONGNAME}\Documentation\ooRexx CHANGES.lnk" "$INSTDIR\doc\CHANGES.txt" "" "$INSTDIR\doc\CHANGES.txt" 0

;;;;  Comment out orxscrpt stuff temporarily
  ; Set output path to the installation directory just in case
;;  SetOutPath $INSTDIR
  ; orxscrpt.dll needs to be registered
;;  !insertmacro InstallLib REGDLL NOTSHARED REBOOT_PROTECTED "${BINDIR}\orxscrpt.dll" "$INSTDIR\orxscrpt.dll" "$INSTDIR"
  ;
  ; Stop rxapi.exe (again!) the registration process starts rxapi.exe GRRRR!!!
;;  KillProcDLL::KillProc "rxapi.exe"

  ; add the Install directory to the PATH env variable; either system wide or user-specific
  Push $INSTDIR
  Push $IsAdminUser ; "true" or "false"
  Push "PATH"
  Call AddToPath
  ; Add Start Menu items
  CreateShortCut "$SMPROGRAMS\${LONGNAME}\Uninstall ${SHORTNAME}.lnk" "$INSTDIR\uninstall.exe" "" "$INSTDIR\uninstall.exe" 0
  CreateShortCut "$SMPROGRAMS\${LONGNAME}\LICENSE.lnk" "$INSTDIR\CPLv1.0.txt" "" "$INSTDIR\CPLv1.0.txt" 0
  WriteINIStr "$SMPROGRAMS\${LONGNAME}\ooRexx Home Page.url" "InternetShortcut" "URL" "http://www.oorexx.org/"
  ; Write the uninstall keys
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${SHORTNAME}" "DisplayName" "${LONGNAME}"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${SHORTNAME}" "DisplayIcon" "${DISPLAYICON}"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${SHORTNAME}" "HelpLink" "http://www.rexxla.org/support.html"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${SHORTNAME}" "URLUpdateInfo" "http://sourceforge.net/project/showfiles.php?group_id=119701"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${SHORTNAME}" "URLInfoAbout" "http://www.rexxla.org/"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${SHORTNAME}" "DisplayVersion" "${VERSION}"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${SHORTNAME}" "Publisher" "Rexx Language Association"
  WriteRegExpandStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${SHORTNAME}" "UninstallString" '"$INSTDIR\${UNINSTALLER}"'
  WriteRegExpandStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${SHORTNAME}" "InstallLocation" '"$INSTDIR"'
  WriteRegExpandStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${SHORTNAME}" "UnInstallLocation" "$INSTDIR" ; dont quote it
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${SHORTNAME}" "NoModify" 0x00000001
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${SHORTNAME}" "NoRepair" 0x00000001
  WriteUninstaller "$INSTDIR\${UNINSTALLER}"

  ; associate .REX with ooRexx (REXXScript)
  StrCpy $5 0
  Push 1
  Push ".REX"
  Call DoFileAssociation
  Call DoFileAssociationDetails

  ; read the result of the custom rxapi page if an administrator
  StrCmp $IsAdminUser "false" NotAdmin
  ReadIniStr $R0 "$PLUGINSDIR\oorexx_ss.ini" "Field 2" State
  ReadIniStr $R1 "$PLUGINSDIR\oorexx_ss.ini" "Field 3" State
  Push $R0
  Push $R1
  Call Installrxapi
  NotAdmin:
SectionEnd

;------------------------------------------------------------------------
; Demos

Section "${LONGNAME} Samples" SecDemo
  DetailPrint "********** Samples **********"
  ; Set output path to the installation directory.
  SetOutPath $INSTDIR\samples
  File "${SRCDIR}\samples\rexxcps.rex"
  File "${SRCDIR}\samples\ccreply.rex"
  File "${SRCDIR}\samples\complex.rex"
  File "${SRCDIR}\samples\factor.rex"
  File "${SRCDIR}\samples\greply.rex"
  File "${SRCDIR}\samples\guess.rex"
  File "${SRCDIR}\samples\ktguard.rex"
  File "${SRCDIR}\samples\makestring.rex"
  File "${SRCDIR}\samples\month.rex"
  File "${SRCDIR}\samples\windows\philfork.rex"
  File "${SRCDIR}\samples\pipe.rex"
  File "${SRCDIR}\samples\properties.rex"
  File "${SRCDIR}\samples\qdate.rex"
  File "${SRCDIR}\samples\qtime.rex"
  File "${SRCDIR}\samples\scclient.rex"
  File "${SRCDIR}\samples\scserver.rex"
  File "${SRCDIR}\samples\semcls.rex"
  File "${SRCDIR}\samples\sfclient.rex"
  File "${SRCDIR}\samples\sfserver.rex"
  File "${SRCDIR}\samples\stack.rex"
  File "${SRCDIR}\samples\usecomp.rex"
  File "${SRCDIR}\samples\usepipe.rex"
  File "${SRCDIR}\samples\windows\rexutils\usewmgr.rex"
  File "${SRCDIR}\samples\windows\rexutils\deskicon.rex"
  File "${SRCDIR}\samples\windows\rexutils\desktop.rex"
  File "${SRCDIR}\samples\windows\rexutils\drives.rex"
  File "${SRCDIR}\samples\windows\rexutils\eventlog.rex"
  File "${SRCDIR}\samples\windows\rexutils\registry.rex"
  ; Set output path to the installation directory.
  SetOutPath $INSTDIR\samples\ole\adsi
  ; Distribution files...
  File "${SRCDIR}\samples\windows\ole\adsi\*.rex"
  ; Set output path to the installation directory.
  SetOutPath $INSTDIR\samples\ole\adsi
  ; Distribution files...
  File "${SRCDIR}\samples\windows\ole\adsi\*.rex"
  ; Set output path to the installation directory.
  SetOutPath $INSTDIR\samples\ole\apps
  ; Distribution files...
  File "${SRCDIR}\samples\windows\ole\apps\*.rex"
  File "${SRCDIR}\samples\windows\ole\apps\*.mwp"
  ; Set output path to the installation directory.
  SetOutPath $INSTDIR\samples\ole\methinfo
  ; Distribution files...
  File "${SRCDIR}\samples\windows\ole\methinfo\*.rex"
  File "${SRCDIR}\samples\windows\ole\methinfo\*.cls"
  ; Set output path to the installation directory.
  SetOutPath $INSTDIR\samples\ole\oleinfo
  ; Distribution files...
  File "${SRCDIR}\samples\windows\ole\oleinfo\*.rex"
  File "${SRCDIR}\samples\windows\ole\oleinfo\*.txt"
  File "${SRCDIR}\samples\windows\ole\oleinfo\*.bmp"
  File "${SRCDIR}\samples\windows\ole\oleinfo\*.rc"
  ; Set output path to the installation directory.
  SetOutPath $INSTDIR\samples\ole\wmi
  ; Distribution files...
  File "${SRCDIR}\samples\windows\ole\wmi\*.rex"
  ; Set output path to the installation directory.
  SetOutPath $INSTDIR\samples\ole\wmi\sysinfo
  ; Distribution files...
  File "${SRCDIR}\samples\windows\ole\wmi\sysinfo\*.rex"
  File "${SRCDIR}\samples\windows\ole\wmi\sysinfo\*.rc"
;;; Temporarily block out the orxscrpt samples
;;;   ; Set output path to the installation directory.
;;;   SetOutPath $INSTDIR\samples\wsh
;;;   ; Distribution files...
;;;   File "${SRCDIR}\samples\windows\wsh\*.rex"
;;;   File "${SRCDIR}\samples\windows\wsh\*.htm"
;;;   File "${SRCDIR}\samples\windows\wsh\*.wsf"
;;;   File "${SRCDIR}\samples\windows\wsh\*.wsc"
  ; Create start menu shortcuts
  SetOutPath $INSTDIR\samples
  CreateDirectory "$SMPROGRAMS\${LONGNAME}\${SHORTNAME} Samples"
  CreateShortCut "$SMPROGRAMS\${LONGNAME}\${SHORTNAME} Samples\RexxCPS.lnk" "$INSTDIR\rexxpaws.exe" '"$INSTDIR\samples\rexxcps.rex"' "$INSTDIR\rexx.exe"
  CreateShortCut "$SMPROGRAMS\${LONGNAME}\${SHORTNAME} Samples\Quick Date.lnk" "$INSTDIR\rexxpaws.exe" '"$INSTDIR\samples\qdate.rex"' "$INSTDIR\rexx.exe"
  CreateShortCut "$SMPROGRAMS\${LONGNAME}\${SHORTNAME} Samples\Quick Time.lnk" "$INSTDIR\rexxpaws.exe" '"$INSTDIR\samples\qtime.rex"' "$INSTDIR\rexx.exe"
  CreateShortCut "$SMPROGRAMS\${LONGNAME}\${SHORTNAME} Samples\Display Event Log.lnk" "$INSTDIR\rexxpaws.exe" '"$INSTDIR\samples\registry.rex"' "$INSTDIR\rexx.exe"
  CreateShortCut "$SMPROGRAMS\${LONGNAME}\${SHORTNAME} Samples\Display Drive Info.lnk" "$INSTDIR\rexxpaws.exe" '"$INSTDIR\samples\drives.rex"' "$INSTDIR\rexx.exe"
  CreateShortCut "$SMPROGRAMS\${LONGNAME}\${SHORTNAME} Samples\Windows Manager.lnk" "$INSTDIR\rexxpaws.exe" '"$INSTDIR\samples\usewmgr.rex"' "$INSTDIR\rexx.exe"
  CreateShortCut "$SMPROGRAMS\${LONGNAME}\${SHORTNAME} Samples\MS Access.lnk" "$INSTDIR\rexxpaws.exe" '"$INSTDIR\samples\ole\apps\MSAccessDemo.rex"' "$INSTDIR\rexx.exe"
  ;
  ; OOdialog samples
  ;
  SetOutPath $INSTDIR\samples\oodialog
  ; Distribution files...
  File "${SRCDIR}\extensions\platform\windows\oodialog\oodialog.ico"
  File "${BINDIR}\oodialog.cls"
  File "${BINDIR}\oodwin32.cls"
  File "${BINDIR}\oodplain.cls"
  File "${SRCDIR}\samples\windows\oodialog\*.rex"
  File "${SRCDIR}\samples\windows\oodialog\*.inp"
  File "${SRCDIR}\samples\windows\oodialog\*.ico"
  ; Set output path to the installation directory.
  SetOutPath $INSTDIR\samples\oodialog\bmp
  ; Distribution files...
  File "${SRCDIR}\samples\windows\oodialog\bmp\*.bmp"
  ; Set output path to the installation directory.
  SetOutPath $INSTDIR\samples\oodialog\rc
  ; Distribution files...
  File "${SRCDIR}\samples\windows\oodialog\rc\*.rc"
  ; Set output path to the installation directory.
  SetOutPath $INSTDIR\samples\oodialog\res
  ; Distribution files...
  File "${SRCDIR}\samples\windows\oodialog\res\*.res"
  File "${SRCDIR}\samples\windows\oodialog\res\*.dll"
  ; Set output path to the installation directory.
  SetOutPath $INSTDIR\samples\oodialog\wav
  ; Distribution files...
  File "${SRCDIR}\samples\windows\oodialog\wav\*.wav"
  File "${SRCDIR}\samples\windows\oodialog\wav\*.txt"
  ; Set output path to the installation directory.
  SetOutPath $INSTDIR\samples\oodialog\examples
  ; Distribution files...
  File "${SRCDIR}\samples\windows\oodialog\examples\*.rex"
  File "${SRCDIR}\samples\windows\oodialog\examples\*.txt"
  ; Set output path to the installation directory.
  SetOutPath $INSTDIR\samples\oodialog\tutorial
  ; Distribution files...
  File "${SRCDIR}\samples\windows\oodialog\tutorial\*.rex"
  File "${SRCDIR}\samples\windows\oodialog\tutorial\*.bmp"
  File "${SRCDIR}\samples\windows\oodialog\tutorial\*.rc"
  ; Set output path to the installation directory.
  SetOutPath $INSTDIR\samples\oodialog\source
  ; Distribution files...
  File "${BINDIR}\oodialog.cls"
  File "${BINDIR}\oodwin32.cls"
  File "${BINDIR}\oodplain.cls"
  File "${SRCDIR}\extensions\platform\windows\oodialog\advctrl.cls"
  File "${SRCDIR}\extensions\platform\windows\oodialog\anibuttn.cls"
  File "${SRCDIR}\extensions\platform\windows\oodialog\basedlg.cls"
  File "${SRCDIR}\extensions\platform\windows\oodialog\build.rex"
  File "${SRCDIR}\extensions\platform\windows\oodialog\catdlg.cls"
  File "${SRCDIR}\extensions\platform\windows\oodialog\dialog.cls"
  File "${SRCDIR}\extensions\platform\windows\oodialog\dlgext.cls"
  File "${SRCDIR}\extensions\platform\windows\oodialog\dyndlg.cls"
  File "${SRCDIR}\extensions\platform\windows\oodialog\makedll.bat"
  File "${SRCDIR}\extensions\platform\windows\oodialog\msgext.cls"
  File "${SRCDIR}\extensions\platform\windows\oodialog\oodutils.cls"
  File "${SRCDIR}\extensions\platform\windows\oodialog\plbdlg.cls"
  File "${SRCDIR}\extensions\platform\windows\oodialog\pludlg.cls"
  File "${SRCDIR}\extensions\platform\windows\oodialog\propsht.cls"
  File "${SRCDIR}\extensions\platform\windows\oodialog\resdlg.cls"
  File "${SRCDIR}\extensions\platform\windows\oodialog\stddlg.cls"
  File "${SRCDIR}\extensions\platform\windows\oodialog\stdext.cls"
  File "${SRCDIR}\extensions\platform\windows\oodialog\userdlg.cls"
  ; Create start menu shortcuts
  SetOutPath $INSTDIR\samples\oodialog
  CreateDirectory "$SMPROGRAMS\${LONGNAME}\${SHORTNAME} Samples\OODialog"
  CreateShortCut "$SMPROGRAMS\${LONGNAME}\${SHORTNAME} Samples\OODialog\Samples.lnk" "$INSTDIR\rexxhide.exe" '"$INSTDIR\samples\oodialog\sample.rex"' "$INSTDIR\samples\oodialog\oodialog.ico"
  CreateShortCut "$SMPROGRAMS\${LONGNAME}\${SHORTNAME} Samples\OODialog\Calculator.lnk" "$INSTDIR\rexxhide.exe" '"$INSTDIR\samples\oodialog\calculator.rex"' "$INSTDIR\samples\oodialog\oodialog.ico"
  CreateShortCut "$SMPROGRAMS\${LONGNAME}\${SHORTNAME} Samples\OODialog\Change Editor.lnk" "$INSTDIR\rexxhide.exe" '"$INSTDIR\samples\oodialog\editrex.rex"' "$INSTDIR\samples\oodialog\oodialog.ico"
  CreateShortCut "$SMPROGRAMS\${LONGNAME}\${SHORTNAME} Samples\OODialog\FTYPE Changer.lnk" "$INSTDIR\rexxhide.exe" '"$INSTDIR\samples\oodialog\ftyperex.rex"' "$INSTDIR\samples\oodialog\oodialog.ico"
SectionEnd

;------------------------------------------------------------------------
; Development tools

Section "${LONGNAME} Development Kit" SecDev
  DetailPrint "********** Development Kit **********"
  ; Set output path to the installation directory.
  SetOutPath $INSTDIR\api
  ; Distribution files...
  File "${BINDIR}\rexx.lib"
  File "${BINDIR}\rexxapi.lib"
  File "${SRCDIR}\api\oorexxapi.h"
  File "${SRCDIR}\api\rexx.h"
  File "${SRCDIR}\api\oorexxerrors.h"
  File "${SRCDIR}\api\rexxapidefs.h"
  File "${SRCDIR}\api\platform\windows\rexxapitypes.h"
  File "${SRCDIR}\api\platform\windows\rexxplatformapis.h"
  File "${SRCDIR}\api\platform\windows\rexxplatformdefs.h"
  File "${SRCDIR}\samples\windows\readme.txt"
  ; Set output path to the installation directory.
  SetOutPath $INSTDIR\api\callrxnt
  ; Distribution files...
  File "${SRCDIR}\samples\windows\api\callrxnt\*.fnc"
  File "${SRCDIR}\samples\windows\api\callrxnt\*.c"
  File "${SRCDIR}\samples\windows\api\callrxnt\*.ico"
  File "${SRCDIR}\samples\windows\api\callrxnt\*.mak"
  File "${SRCDIR}\samples\windows\api\callrxnt\*.exe"
  ; Set output path to the installation directory.
  SetOutPath $INSTDIR\api\callrxwn
  ; Distribution files...
  File "${SRCDIR}\samples\windows\api\callrxwn\*.fnc"
  File "${SRCDIR}\samples\windows\api\callrxwn\*.c"
  File "${SRCDIR}\samples\windows\api\callrxwn\*.h"
  File "${SRCDIR}\samples\windows\api\callrxwn\*.ico"
  File "${SRCDIR}\samples\windows\api\callrxwn\*.mak"
  File "${SRCDIR}\samples\windows\api\callrxwn\*.exe"
  File "${SRCDIR}\samples\windows\api\callrxwn\*.rc"
  ; Set output path to the installation directory.
  SetOutPath $INSTDIR\api\rexxexit
  ; Distribution files...
  File "${SRCDIR}\samples\windows\api\rexxexit\*.c"
  File "${SRCDIR}\samples\windows\api\rexxexit\*.ico"
  File "${SRCDIR}\samples\windows\api\rexxexit\*.mak"
  File "${SRCDIR}\samples\windows\api\rexxexit\*.exe"
  File "${SRCDIR}\samples\rexxtry.rex"
  ; Set output path to the installation directory.
  SetOutPath $INSTDIR\api\wpipe
  ; Distribution files...
  File "${SRCDIR}\samples\windows\api\wpipe\readme.txt"
  ; Set output path to the installation directory.
  SetOutPath $INSTDIR\api\wpipe\wpipe1
  ; Distribution files...
  File "${SRCDIR}\samples\windows\api\wpipe\wpipe1\*.c"
  File "${SRCDIR}\samples\windows\api\wpipe\wpipe1\*.def"
  File "${SRCDIR}\samples\windows\api\wpipe\wpipe1\*.rex"
  File "${SRCDIR}\samples\windows\api\wpipe\wpipe1\*.mak"
  File "${SRCDIR}\samples\windows\api\wpipe\wpipe1\*.dll"
  ; Set output path to the installation directory.
  SetOutPath $INSTDIR\api\wpipe\wpipe2
  ; Distribution files...
  File "${SRCDIR}\samples\windows\api\wpipe\wpipe2\*.c"
  File "${SRCDIR}\samples\windows\api\wpipe\wpipe2\*.def"
  File "${SRCDIR}\samples\windows\api\wpipe\wpipe2\*.rex"
  File "${SRCDIR}\samples\windows\api\wpipe\wpipe2\*.mak"
  File "${SRCDIR}\samples\windows\api\wpipe\wpipe2\*.dll"
  ; Set output path to the installation directory.
  SetOutPath $INSTDIR\api\wpipe\wpipe3
  ; Distribution files...
  File "${SRCDIR}\samples\windows\api\wpipe\wpipe3\*.c"
  File "${SRCDIR}\samples\windows\api\wpipe\wpipe3\*.def"
  File "${SRCDIR}\samples\windows\api\wpipe\wpipe3\*.rex"
  File "${SRCDIR}\samples\windows\api\wpipe\wpipe3\*.mak"
  File "${SRCDIR}\samples\windows\api\wpipe\wpipe3\*.dll"
  ; Create start menu shortcuts
  SetOutPath $INSTDIR\api
  CreateDirectory "$SMPROGRAMS\${LONGNAME}\${SHORTNAME} Samples\API"
  CreateShortCut "$SMPROGRAMS\${LONGNAME}\${SHORTNAME} Samples\API\Call ooRexx in a Console.lnk" "$INSTDIR\api\callrxnt\callrxnt.exe" "" "$INSTDIR\api\callrxnt\callrxnt.ico"
  CreateShortCut "$SMPROGRAMS\${LONGNAME}\${SHORTNAME} Samples\API\Call ooRexx in a Window.lnk" "$INSTDIR\api\callrxwn\callrxwn.exe" "" "$INSTDIR\api\callrxwn\callrxwn.ico"
  CreateShortCut "$SMPROGRAMS\${LONGNAME}\${SHORTNAME} Samples\API\Call ooRexx with Exits.lnk" "$INSTDIR\api\rexxexit\rexxexit.exe" "rexxtry.rex" "$INSTDIR\api\rexxexit\rexxexit.ico"
SectionEnd

;------------------------------------------------------------------------
; Doco

Section "${LONGNAME} Documentation" SecDoc
  DetailPrint "********** Documentation **********"
  ; Set output path to the installation directory.
  SetOutPath $INSTDIR\doc
  File "${SRCDIR}\doc\rexxpg.pdf"
  File "${SRCDIR}\doc\rexxref.pdf"
  File "${SRCDIR}\doc\rxmath.pdf"
  File "${SRCDIR}\doc\rxsock.pdf"
  File "${SRCDIR}\doc\rxftp.pdf"
  File "${SRCDIR}\doc\oodialog.pdf"
  ; Create start menu shortcuts
  CreateDirectory "$SMPROGRAMS\${LONGNAME}\Documentation"
  CreateShortCut "$SMPROGRAMS\${LONGNAME}\Documentation\ooRexx Reference.lnk" "$INSTDIR\doc\rexxref.pdf" "" "$INSTDIR\doc\rexxref.pdf" 0
  CreateShortCut "$SMPROGRAMS\${LONGNAME}\Documentation\ooRexx Programming Guide.lnk" "$INSTDIR\doc\rexxpg.pdf" "" "$INSTDIR\doc\rexxpg.pdf" 0
  CreateShortCut "$SMPROGRAMS\${LONGNAME}\Documentation\ooRexx Mathematical Functions Reference.lnk" "$INSTDIR\doc\rxmath.pdf" "" "$INSTDIR\doc\rxmath.pdf" 0
  CreateShortCut "$SMPROGRAMS\${LONGNAME}\Documentation\ooRexx TCP-IP Sockets Functions Reference.lnk" "$INSTDIR\doc\rxsock.pdf" "" "$INSTDIR\doc\rxsock.pdf" 0
  CreateShortCut "$SMPROGRAMS\${LONGNAME}\Documentation\ooRexx rxFTP Class Reference.lnk" "$INSTDIR\doc\rxftp.pdf" "" "$INSTDIR\doc\rxftp.pdf" 0
  CreateShortCut "$SMPROGRAMS\${LONGNAME}\Documentation\ooRexx OODIalog Method Reference.lnk" "$INSTDIR\doc\oodialog.pdf" "" "$INSTDIR\doc\oodialog.pdf" 0
SectionEnd


Section ""

  ;Invisible section to display the Finish header
; !insertmacro MUI_FINISHHEADER

SectionEnd

;========================================================================
;Installer Functions

Function .onInit
;  !insertmacro MUI_INSTALLOPTIONS_EXTRACT "oorexx_fa.ini"
  !insertmacro MUI_INSTALLOPTIONS_EXTRACT "oorexx_ss.ini"
;  !insertmacro MUI_INSTALLOPTIONS_EXTRACT "oorexx_mt.ini"
  ;
  ; Uninstall previous version if present
  ;
  ReadRegStr $R1 HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${SHORTNAME}" "UninstallString"
  StrCmp $R1 "" NoUninstall
    ;
    ; ask the user to run the uninstaller
    ;
    ReadRegStr $R2 HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${SHORTNAME}" "UnInstallLocation"
    StrCmp $R2 "" NoUninstallLocation
    Goto StartUninstall
    NoUninstallLocation:
      StrCpy $R2 $INSTDIR
    StartUninstall:
    MessageBox MB_YESNOCANCEL|MB_ICONEXCLAMATION|MB_TOPMOST "A version of ${LONGNAME} is currently installed.$\nIt is recommended that it be uninstalled before proceeding.$\nUninstall previous version?" /SD IDYES IDNO NoUninstall IDCANCEL DoAbort
    HideWindow
    ClearErrors
    ExecWait '$R1 _?=$R2'
    IfErrors no_remove_uninstaller
    IfFileExists "$INSTDIR\${KEYFILE}" no_remove_uninstaller
      RMDir /r "$R2"
    no_remove_uninstaller:
    BringToFront

    Goto NoUninstall
  DoAbort:
    Abort
  NoUninstall:
  ;
  ; Install as All Users if an admin
  ;
  Call IsUserAdmin
  Pop $IsAdminUser
  StrCmp $IsAdminUser "false" DefaultUser
  SetShellVarContext all
  DefaultUser:
  ;
  ; Initialise rxapichk
  ;
  StrCpy $rxapichk 0
  ;
FunctionEnd

; checks if the user is an admin and skips the page if not
Function SetCustomRxAPI
  StrCmp $IsAdminUser "true" NoAbort
  Abort
  NoAbort:
  !insertmacro MUI_HEADER_TEXT "$(TEXT_IO_PAGETITLE_RXAPI)" "$(TEXT_IO_SUBTITLE_RXAPI)"
  !insertmacro MUI_INSTALLOPTIONS_DISPLAY "oorexx_ss.ini"
FunctionEnd

Function CheckForRxAPI
  ;
  ; Determines if rxapi.exe is running
  StrCmp $rxapichk 1 NotRunning
  StrCpy $rxapichk 1
  FindProcDLL::FindProc "rxapi.exe"
  DetailPrint "rc from FindProcDll $R0"
  StrCmp $R0 0 NotRunning
  ;
  ; rxapi.exe is running, we need to stop it
  MessageBox MB_OKCANCEL|MB_ICONEXCLAMATION|MB_TOPMOST "The Open Object Rexx memory manager (RXAPI) is currently active.$\nSelect OK to stop it (possible loss of data) and continue.$\nSelect CANCEL to continue with the installation without stopping the memory manager." /SD IDOK IDCANCEL NotRunning
  ;
  ; Stop rxapi.exe
  KillProcDLL::KillProc "rxapi.exe"
  DetailPrint "rc from KillProcDll $R0"

  NotRunning:
FunctionEnd

Function .onMouseOverSection

  !insertmacro MUI_DESCRIPTION_BEGIN
    !insertmacro MUI_DESCRIPTION_TEXT ${SecMain} "Installs the core components of ${LONGNAME} to the application folder."
    !insertmacro MUI_DESCRIPTION_TEXT ${SecDev} "Installs the files required to embed ${LONGNAME} into you C/C++ application."
    !insertmacro MUI_DESCRIPTION_TEXT ${SecDemo} "Install sample ${LONGNAME} programs."
    !insertmacro MUI_DESCRIPTION_TEXT ${SecDoc} "Install ${LONGNAME} documentation."
 !insertmacro MUI_DESCRIPTION_END

FunctionEnd

Function DoFileAssociation
  Pop $R1
  Pop $R0
  Strcmp $R0 0 exitfa
  Strcmp $R1 "" exitfa
  ; do the association
  DetailPrint "Registering $R1 extension to run with ooRexx"
  WriteRegStr HKCR $R1 "" "REXXScript"
  ;
  ; Append $R1 to .PATHEXT for NT-based systems, for Administrators only
  ;
  Call IsNT
  Pop $1
  StrCmp $1 0 exitfa
  StrCmp $IsAdminUser "false" exitfa
  Push $R1
  Push $IsAdminUser ; should only be "true" at this point
  Push "PATHEXT"
  Call AddToPath
  exitfa:
  Return
FunctionEnd

Function DoFileAssociationDetails
  ; do the association details
  WriteRegStr HKCR "REXXScript" "" "ooRexx Rexx Program"
  WriteRegStr HKCR "REXXScript\shell" "" "open"
  WriteRegStr HKCR "REXXScript\DefaultIcon" "" "$INSTDIR\rexx.exe,0"
  WriteRegStr HKCR "REXXScript\shell\open" "" "Run"
  WriteRegStr HKCR "REXXScript\shell\open\command" "" '"$INSTDIR\rexx.exe" "%1" %*'
  WriteRegStr HKCR "REXXScript\shell\edit" "" "Edit"
  WriteRegStr HKCR "REXXScript\shell\edit\command" "" 'notepad.exe "%1"'
  WriteRegStr HKCR "REXXScript\shellex\DropHandler" "" "{60254CA5-953B-11CF-8C96-00AA00B8708C}"
  System::Call 'Shell32::SHChangeNotify(i ${SHCNE_ASSOCCHANGED}, i ${SHCNF_IDLIST}, i 0, i 0)'
  Return
FunctionEnd

Function Installrxapi
  ; $R1 is start flag
  ; $R0 is install flag
  Pop $R1
  Pop $R0
  Strcmp $R0 0 exitss
    ; do the install of rxapi
    DetailPrint "Installing rxapi as a Windows Service"
    nsExec::ExecToLog "$INSTDIR\rxapi /i /s"
    Pop $R0
    StrCmp $R0 0 dostart
      MessageBox MB_OK|MB_ICONEXCLAMATION|MB_TOPMOST "Failed to install rxapi as a Windows Service:$\n$R0" /SD IDOK
      Goto exitss
    dostart:
    DetailPrint "rxapi successfully installed as a Windows Service"
    StrCmp $R1 0 exitss
      ; start the service
      Services::SendServiceCommand 'start' 'RXAPI'
      Pop $R0
      StrCmp $R0 'Ok' exitss
        MessageBox MB_OK|MB_ICONEXCLAMATION|MB_TOPMOST "Failed to start the ooRexx rxapi service:$\n$0" /SD IDOK
  exitss:
FunctionEnd

;========================================================================
;Uninstaller Section

Section "Uninstall"

;;;; temporarily comment out orxscrpt stuff while it is disabled in the build.
  ; orxscrpt.dll needs to be degistered
;;  !insertmacro UnInstallLib REGDLL NOTSHARED REBOOT_PROTECTED "$INSTDIR\orxscrpt.dll"
  ;
  ; Stop rxapi.exe (again!) the de-registration process starts rxapi.exe GRRRR!!!
  KillProcDLL::KillProc "rxapi.exe"

  ; get rid of file association
  Push ".REX"
  Call un.DeleteFileAssociation

  ; removes the rxapi service - ignore if we get errors
  StrCmp $IsAdminUser "false" NotAdmin
    Services::IsServiceInstalled 'RXAPI'
    Pop $R0
    StrCmp $R0 'No' NotAdmin
    DetailPrint "Uninstalling ooRexx rxapi Service"
    Services::SendServiceCommand 'stop' 'RXAPI'
    Pop $R0
    StrCmp $R0 'Ok' StoppedOK
      ; rxapi.exe used to have a bug where after the Service stopped, rxapi would restart as a
      ; normal process.  This code is a hold over from that, probably not needed anymore.
      DetailPrint "Service Control Manager failed to stop rxapi, forcing termination"
      KillProcDLL::KillProc "rxapi.exe"
    StoppedOK:
    nsExec::ExecToLog "$INSTDIR\rxapi /u /s"
    Pop $R0
    StrCmp $R0 0 doprintok
      DetailPrint "Failed to uninstall rxapi as a service"
      goto NotAdmin
    doprintok:
      DetailPrint "Uninstalled rxapi as a service"
  NotAdmin:

  ; Stop rxapi.exe (again!) just in case
  KillProcDLL::KillProc "rxapi.exe"

  ; remove directory from PATH
  Push $INSTDIR
  Push $IsAdminUser ; pushes "true" or "false"
  Push "PATH"
  Call un.RemoveFromPath

  ;
  DeleteRegKey HKCR "REXXScript"

  ; remove the REXX_HOME environment variable
  Push "REXX_HOME"
  Push $IsAdminUser ; "true" or "false"
  Call un.DeleteEnvStr

  ; remove all installed files
  RMDir /r "$INSTDIR"

  ; Remove the installation stuff
  DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${SHORTNAME}"
  DeleteRegKey HKLM "SOFTWARE\${LONGNAME}"

  ; remove shortcuts directory and everything in it
  RMDir /r "$SMPROGRAMS\${LONGNAME}"

; !insertmacro MUI_UNFINISHHEADER

SectionEnd

;========================================================================
;Uninstaller Functions

Function un.onInit
  ;
  ; UnInstall as All Users if an admin
  ;
  Call un.IsUserAdmin
  Pop $IsAdminUser
  StrCmp $IsAdminUser "false" DefaultUser
  SetShellVarContext all
  Goto DefaultUser
  DefaultUser:
FunctionEnd

Function un.DeleteFileAssociation
  Pop $R0
  ReadRegStr $R1 HKCR "$R0" ""
  StrCmp $R1 "REXXScript" 0 NoOwn ; only delete key if we own it
  DeleteRegKey HKCR "$R0"
  DetailPrint "Deleting file association for $R0"
  NoOwn:
  ;
  ; Remove $R0 from PATHEXT for NT-based systems
  ;
  Call un.IsNT
  Pop $1
  StrCmp $1 0 NoAdmin
  StrCmp $IsAdminUser "false" NoAdmin
  Push $R0
  Push $IsAdminUser ; should only be "true" at this point
  Push "PATHEXT"
  Call un.RemoveFromPath
  System::Call 'Shell32::SHChangeNotify(i ${SHCNE_ASSOCCHANGED}, i ${SHCNF_IDLIST}, i 0, i 0)'
  NoAdmin:
FunctionEnd

Function un.CheckForRxAPI
  ;
  ; Determines if rxapi.exe is running
  StrCmp $rxapichk 1 NotRunning
  StrCpy $rxapichk 1
  FindProcDLL::FindProc "rxapi.exe"
  DetailPrint "rc from FindProcDll $R0"
  StrCmp $R0 0 NotRunning
  ;
  ; rxapi.exe is running, we need to stop it
  MessageBox MB_OKCANCEL|MB_ICONEXCLAMATION|MB_TOPMOST "The Open Object Rexx memory manager (RXAPI) is currently active.$\nSelect OK to stop it (possible loss of data) and continue.$\nSelect CANCEL to continue with the uninstall without stopping the service." /SD IDOK IDCANCEL NotRunning
  ;
  ; Stop rxapi.exe
  KillProcDLL::KillProc "rxapi.exe"
  DetailPrint "rc from KillProcDll $R0"

  NotRunning:
FunctionEnd

!include "admin.nsh"
!include "isnt.nsh"
!include "newpath.nsh"
!include "WriteEnv.nsh"

;eof
