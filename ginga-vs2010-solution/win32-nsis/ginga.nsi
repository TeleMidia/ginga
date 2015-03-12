; ginga.nsi
; ginga.nsi
;
; This script is used to create the ginga and gingagui package installer for
; Windows.
;


!include "FileAssociation.nsh"

;--------------------------------


!define BASEDIR "..\Ginga"
!define VERSION "0.13.6"

Name "Ginga ${VERSION}"
Caption "Ginga Installer"
OutFile "ginga-win32-${VERSION}-installer.exe"
Icon "${BASEDIR}\icon.ico"

; The default installation directory
InstallDir "$PROGRAMFILES\TeleMidia\Ginga"
; InstallDir "C:\"

; License informations
LicenseText "Please review the license terms before installing Ginga."
LicenseData "..\COPYING"

; Request application privileges for Windows Vista
RequestExecutionLevel admin

;--------------------------------

; Pages
Page license
Page components
Page directory
Page instfiles

UninstPage uninstConfirm
UninstPage instfiles

;--------------------------------
; Install Types

InstType "Full"
InstType "Minimal"

;--------------------------------
; The stuffs to install

Section "Microsoft Visual C++ 2012 Redist" SEC_CRT2012

  ; Make this required on the web installer, since it has a fully reliable check to
  ; see if it needs to be downloaded and installed or not.
  SectionIn RO

  ; Detection made easy: Unlike previous redists, VC2010 now generates a platform
  ; independent key for checking availability.

  ; Downloaded from:
  ;   http://download.microsoft.com/download/C/6/D/C6D0FD4E-9E53-4897-9B91-836EBA2AACD3/vcredist_x86.exe

  ClearErrors
  ReadRegDword $R0 HKLM "SOFTWARE\Classes\Installer\Dependencies\{33d1fd90-4274-48a1-9bc1-97e33d9c2d6f}" "Version"
  ;IfErrors done
  ;DetailPrint "Visual C++ 2012 Redistributable registry key was not found; assumed to no be installed."
  StrCmp $R0 "11.0.61030.0" 0 +3
    DetailPrint "Visual C++ 2012 Redistributable is already installed; skipping!"
    Goto done

  SetOutPath "$TEMP"

  DetailPrint "Downloading Visual C++ 2012 Redistributable Setup..."
  DetailPrint "Contacting Microsoft.com..."
  NSISdl::download /TIMEOUT=15000 "http://download.microsoft.com/download/1/6/B/16B06F60-3B20-4FF2-B699-5E9B7962F9AE/VSU_4/vcredist_x86.exe" "vcredist_x86.exe"

  Pop $R0 ;Get the return value
  StrCmp $R0 "success" OnSuccess
  ;DetailPrint "Cound not contact Microsoft.com, or the file has been (re)moved!"
  ;DetailPrint "Contacting Googlecode.com..."
  ;NSISdl::download /TIMEOUT=20000 "http://pcsx2.googlecode.com/files/vcredist_2010_sp1_x86.exe" "vcredist_2010_sp1_x86.exe"

  ; [TODO] Provide a mirror for this file hosted from pcsx2.net .. ?  or emudev.net .. ?
  ;Pop $R0 ;Get the return value
  ;StrCmp $R0 "success" +2
  ;NSISdl::download /TIMEOUT=30000 "http://www.pcsx2.net/vcredist_x86.exe" "vcredist_2010_x86.exe"

  ;Pop $R0 ;Get the return value
  StrCmp $R0 "success" +2
    MessageBox MB_OK "Could not download Visual Studio 2012 Redist; none of the mirrors appear to be functional."
    Goto done

OnSuccess:
  DetailPrint "Running Visual C++ 2012 Redistributable Setup..."
  ExecWait '"$TEMP\vcredist_x86.exe" /qb'
  DetailPrint "Finished Visual C++ 2012 Redistributable Setup"

  Delete "$TEMP\vcredist_x86.exe"

done:
SectionEnd

Section "Ginga (Command Line Binary + GUI)" ; No components page, name is not important
  SectionIn RO
  ; Set output path to the installation directory.
  SetOutPath $INSTDIR

  ; Put file there
  File /r ${BASEDIR}\*

  ; Associate .ncl files with Ginga-GUI
  ${registerExtension} $INSTDIR\gingagui.exe ".ncl" "Nested Context Language document"

  ; delete old gingagui.ini
  Delete $APPDATA\telemidia\gingagui.ini
SectionEnd ; end the section

Section "-Ginga Settings" ; No components page, name is not important
  SectionIn RO
  ; Set output path to the installation directory.
  SetOutPath C:\ProgramData\Ginga

  ; Put file there
  File ${BASEDIR}\files\contextmanager\*

  WriteUninstaller "Uninstall Ginga.exe"
SectionEnd ; end the section

; Optional section (can be disabled by the user)
Section "Start Menu Shortcuts"
  SectionIn 1
  CreateDirectory "$SMPROGRAMS\TeleMidia\Ginga"
  CreateShortCut "$SMPROGRAMS\TeleMidia\Ginga\Uninstall Ginga.lnk" "$INSTDIR\Uninstall Ginga.exe" "" "$INSTDIR\Uninstall Ginga.exe" 0
  ;CreateShortCut "$SMPROGRAMS\TeleMidia\Ginga\ginga.lnk" "$INSTDIR\ginga.exe" "" "$INSTDIR\ginga.exe" 0
  CreateShortCut "$SMPROGRAMS\TeleMidia\Ginga\Ginga.lnk" "$INSTDIR\gingagui.exe" "" "$INSTDIR\gingagui.exe" 0
  ;CreateShortCut "$SMPROGRAMS\TeleMidia\Ginga\ (MakeNSISW).lnk" "$INSTDIR\ginga.nsi" "" "$INSTDIR\ginga.nsi" 0
SectionEnd

;--------------------------------
; Uninstaller
UninstallText "This will uninstall Ginga. Hit next to continue"
Section "Uninstall"
  Delete "$INSTDIR\*"
  RMDir /r /REBOOTOK "$INSTDIR\imageformats"
  RMDir /r /REBOOTOK "$INSTDIR\locales"
  RMDir /r /REBOOTOK "$INSTDIR\nclua"
  RMDir /r /REBOOTOK $APPDATA\telemidia

  ;Shortcuts
  Delete "$SMPROGRAMS\TeleMidia\Ginga\*"
  RMDir "$SMPROGRAMS\TeleMidia\Ginga\"
  RMDir "$SMPROGRAMS\TeleMidia"

  ;Remove file association
  ${unregisterExtension} ".ncl" "Nested Context Language document"
SectionEnd
