Unicode true

; Define your application name
!define APPNAME "obs-rtspserver"
!ifndef APPVERSION
!define APPVERSION ${VERSION}
!define SHORTVERSION ${VERSION}
!endif
!define APPNAMEANDVERSION "OBS RTSP Server ${SHORTVERSION}"

; Main Install settings
Name "${APPNAMEANDVERSION}"
InstallDir "$PROGRAMFILES64\obs-studio"
InstallDirRegKey HKLM "Software\OBS Studio" ""
OutFile "..\build-package\obs-rtspserver-${SHORTVERSION}-Windows-installer.exe"

; Use compression
SetCompressor LZMA

; Modern interface settings
!include "MUI.nsh"
!include nsDialogs.nsh

; Include library for dll stuff
!include Library.nsh


!define MUI_ICON "obs.ico"
;!define MUI_HEADERIMAGE
;!define MUI_HEADERIMAGE_BITMAP ""
;!define MUI_HEADERIMAGE_RIGHT
!define MUI_ABORTWARNING

!define MUI_PAGE_HEADER_TEXT "License Information"
!define MUI_PAGE_HEADER_SUBTEXT "Please review the license terms before installing OBS RTSP Server."
!define MUI_LICENSEPAGE_TEXT_TOP "Press Page Down or scroll to see the rest of the license."
!define MUI_LICENSEPAGE_TEXT_BOTTOM " "
!define MUI_LICENSEPAGE_BUTTON "&Next >"

!insertmacro MUI_PAGE_WELCOME
!insertmacro MUI_PAGE_LICENSE "..\LICENSE"
!insertmacro MUI_PAGE_INSTFILES
!insertmacro MUI_PAGE_FINISH



!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES

; Set languages (first is default language)
!insertmacro MUI_LANGUAGE "English"
!insertmacro MUI_RESERVEFILE_LANGDLL

Section "obs-rtspserver" Section1

	; Set Section properties
	SetOverwrite on
	; Set Section Files and Shortcuts
	SetOutPath "$INSTDIR\data\obs-plugins\obs-virtualoutput\"
	File "..\build-package\bin\32bit\obs-rtspserver.dll"
	File "..\build-package\bin\32bit\obs-rtspserver.pdb"
	File "..\build-package\bin\32bit\avutil-56.dll"
	File "..\build-package\bin\32bit\swscale-5.dll"
	SetOutPath "$INSTDIR\data\obs-plugins\obs-rtspserver\locale\"
	File /r "..\build-package\data\obs-plugins\obs-rtspserver\locale\"
	SetOutPath "$INSTDIR\obs-plugins\64bit\"
	File "..\build-package\obs-plugins\64bit\obs-rtspserver.dll"
	File "..\build-package\obs-plugins\64bit\obs-rtspserver.pdb"
	SetOutPath "$INSTDIR\bin\64bit\"
	File "..\build-package\bin\64bit\obs-rtspserver.dll"
	File "..\build-package\bin\64bit\obs-rtspserver.pdb"
	
SectionEnd

Section -FinishSection

	WriteRegStr HKLM "Software\${APPNAME}" "" "$INSTDIR"
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPNAME}" "DisplayName" "${APPNAME}"
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPNAME}" "UninstallString" "$INSTDIR\uninstall_obs-rtspserver.exe"
	WriteUninstaller "$INSTDIR\uninstall_obs-rtspserver.exe"
	
SectionEnd

;Uninstall section
Section Uninstall

	;Remove from registry...
	DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPNAME}"
	DeleteRegKey HKLM "SOFTWARE\${APPNAME}"

	; Delete self
	Delete "$INSTDIR\uninstall_obs-rtspserver.exe"

	; Clean up obs-rtspserver
	Delete "$INSTDIR\obs-plugins\64bit\obs-rtspserver.dll"
	Delete "$INSTDIR\obs-plugins\64bit\obs-rtspserver.pdb"
	Delete "$INSTDIR\bin\64bit\obs-rtspserver.dll"
	Delete "$INSTDIR\bin\64bit\obs-rtspserver.pdb"

	; Remove data directory
	RMDir /r "$INSTDIR\data\obs-plugins\obs-rtspserver\"

SectionEnd

; eof