Unicode true

; Define your application name
!define APPNAME "obs-rtspserver"
!define DISPLAYNAME $(LANGTEXT_DISPLAYNAME)
!ifndef APPVERSION
!define APPVERSION ${VERSION}
!define SHORTVERSION ${VERSION}
!endif
!define APPNAMEANDVERSION "${DISPLAYNAME} ${SHORTVERSION}"

; Main Install settings
Name "${APPNAMEANDVERSION}"
InstallDir "$PROGRAMFILES64\obs-studio"
InstallDirRegKey HKLM "Software\OBS Studio" ""
OutFile "..\build-package\obs-rtspserver-${SHORTVERSION}-windows-installer.exe"

; Use compression
SetCompressor LZMA

; Modern interface settings
!include MUI2.nsh

; Include library for dll stuff
!include Library.nsh


!define MUI_ICON "obs.ico"
!define MUI_UNICON "obs.ico"
;!define MUI_HEADERIMAGE
;!define MUI_HEADERIMAGE_BITMAP ""
;!define MUI_HEADERIMAGE_RIGHT
!define MUI_ABORTWARNING

!define MUI_PAGE_HEADER_TEXT $(LANGTEXT_HEADER_TEXT)
!define MUI_PAGE_HEADER_SUBTEXT $(LANGTEXT_HEADER_SUBTEXT)
!define MUI_LICENSEPAGE_TEXT_TOP $(LANGTEXT_LICENSEPAGE_TEXT_TOP)
!define MUI_LICENSEPAGE_TEXT_BOTTOM " "
!define MUI_LICENSEPAGE_BUTTON $(LANGTEXT_LICENSEPAGE_BUTTON)
!define MUI_FINISHPAGE_NOAUTOCLOSE
!define MUI_HEADER_TRANSPARENT_TEXT

!insertmacro MUI_PAGE_WELCOME
!insertmacro MUI_PAGE_LICENSE "LICENSE"
!insertmacro MUI_PAGE_INSTFILES
!insertmacro MUI_PAGE_FINISH



!insertmacro MUI_UNPAGE_WELCOME
!insertmacro MUI_UNPAGE_COMPONENTS
!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES
!insertmacro MUI_UNPAGE_FINISH

!include .\locale.nsi

!insertmacro MUI_RESERVEFILE_LANGDLL

Section -SecUninstallPrevious

    Call UninstallPrevious
	
SectionEnd

Section "!OBS RTSP Server"

	SectionIn RO
	; Set Section properties
	SetOverwrite on
	; Set Section Files and Shortcuts
	SetOutPath "$INSTDIR\"
	File /r "..\build-package\"
	
SectionEnd

Section -FinishSection

	WriteRegStr HKLM "Software\${APPNAME}" "" "$INSTDIR"
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPNAME}" "DisplayName" "${DISPLAYNAME}"
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPNAME}" "UninstallString" "$INSTDIR\uninstall_obs-rtspserver.exe"
	WriteUninstaller "$INSTDIR\uninstall_obs-rtspserver.exe"
	
SectionEnd

;Uninstall section
Section "!un.OBS RTSP Server"

	SectionIn RO
	
	;Remove from registry...
	DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPNAME}"
	DeleteRegKey HKLM "SOFTWARE\${APPNAME}"

	; Delete self
	Delete "$INSTDIR\uninstall_obs-rtspserver.exe"

	; Clean up obs-rtspserver
	Delete /REBOOTOK "$INSTDIR\obs-plugins\64bit\obs-rtspserver.dll"
	Delete /REBOOTOK "$INSTDIR\obs-plugins\64bit\obs-rtspserver.pdb"
	Delete /REBOOTOK "$INSTDIR\obs-plugins\32bit\obs-rtspserver.dll"
	Delete /REBOOTOK "$INSTDIR\obs-plugins\32bit\obs-rtspserver.pdb"

	; Remove data directory
	RMDir /r "$INSTDIR\data\obs-plugins\obs-rtspserver\"

SectionEnd

Function .onInit

  !insertmacro MUI_LANGDLL_DISPLAY

FunctionEnd

;Uninstall Previous
Function UninstallPrevious

    ; Check for uninstaller.
    ReadRegStr $R0 HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPNAME}" "UninstallString"

    ${If} $R0 == ""
        Return
    ${EndIf}

    DetailPrint $(LANGTEXT_REMOVING_PREV)

    ; Run the uninstaller silently.
    ExecWait '"$R0" /S _?=$INSTDIR' $0

	${If} $0 != 0
		Abort $(LANGTEXT_REMOVING_PREV_FAILED)
    ${EndIf}

FunctionEnd

; eof
