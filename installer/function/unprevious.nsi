;Uninstall Previous
Function UninstallPrevious

    ; Check for uninstaller.
    ReadRegStr $R0 HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPNAME}" "UninstallString"
	StrCpy $R1 $INSTDIR
	ReadRegStr $R1 HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPNAME}" "InstallLocation"

    ${If} $R0 == ""
        Return
    ${EndIf}

    DetailPrint $(LANGTEXT_REMOVING_PREV)

    ; Run the uninstaller silently.
    ExecWait '"$R0" /S _?=$R1' $0

	${If} $0 != 0
		Abort $(LANGTEXT_REMOVING_PREV_FAILED)
    ${EndIf}

FunctionEnd

; eof
