function Write-Status {
    param(
        [parameter(Mandatory=$true)]
        [string] $output
    )

    if (!($Quiet.isPresent)) {
        if (Test-Path env:CI) {
            #Write-Host "[${ProductName}] ${output}"
            Write-Host -ForegroundColor blue "[${ProductName}] ${output}"
        } else {
            Write-Host -ForegroundColor blue "[${ProductName}] ${output}"
        }
    }
}

function Write-Info {
    param(
        [parameter(Mandatory=$true)]
        [string] $output
    )

    if (!($Quiet.isPresent)) {
        if (Test-Path env:CI) {
            #Write-Host " + ${output}"
            Write-Host -ForegroundColor DarkYellow "::warning:: + ${output}"
        } else {
            Write-Host -ForegroundColor DarkYellow " + ${output}"
        }
    }
}

function Write-Step {
    param(
        [parameter(Mandatory=$true)]
        [string] $output
    )

    if (!($Quiet.isPresent)) {
        if (Test-Path env:CI) {
            #Write-Host " + ${output}"
            Write-Host "::endgroup::"
            Write-Host "::group:: ${output}"
        } else {
            Write-Host -ForegroundColor green " + ${output}"
        }
    }
}

function Write-Error {
    param(
        [parameter(Mandatory=$true)]
        [string] $output
    )

    if (Test-Path env:CI) {
        #Write-Host " + ${output}"
        "::error::  + ${output}"
    } else {
        Write-Host -ForegroundColor red " + ${output}"
    }
}

function Test-CommandExists {
    param(
        [parameter(Mandatory=$true)]
        [string] $Command
    )

    $CommandExists = $false
    $OldActionPref = $ErrorActionPreference
    $ErrorActionPreference = "stop"

    try {
        if (Get-Command $Command) {
            $CommandExists = $true
        }
    } Catch {
        $CommandExists = $false
    } Finally {
        $ErrorActionPreference = $OldActionPref
    }

    return $CommandExists
}

function Ensure-Directory {
    param(
        [parameter(Mandatory=$true)]
        [string] $Directory
    )

    if (!(Test-Path $Directory)) {
        $null = New-Item -ItemType Directory -Force -Path $Directory
    }

    Set-Location -Path $Directory
}

$BuildDirectory = "$(if (Test-Path Env:BuildDirectory) { $env:BuildDirectory } else { $BuildDirectory })"
$BuildConfiguration = "$(if (Test-Path Env:BuildConfiguration) { $env:BuildConfiguration } else { $BuildConfiguration })"
$BuildArch = "$(if (Test-Path Env:BuildArch) { $env:BuildArch } else { $BuildArch })"
$OBSBranch = "$(if (Test-Path Env:OBSBranch) { $env:OBSBranch } else { $OBSBranch })"
$WindowsDepsVersion = "$(if (Test-Path Env:WindowsDepsVersion ) { $env:WindowsDepsVersion } else { "2019" })"
$WindowsQtVersion = "$(if (Test-Path Env:WindowsQtVersion ) { $env:WindowsQtVersion } else { "5.15.2" })"
$CmakeSystemVersion = "$(if (Test-Path Env:CMAKE_SYSTEM_VERSION) { $Env:CMAKE_SYSTEM_VERSION } else { "10.0.18363.657" })"
$OBSVersion = "$(if ( Test-Path Env:OBSVersion ) { $env:ObsVersion } else { "27.1.3" })"
$NSISVersion = "$(if ( Test-Path Env:NSISVersion ) { $env:NSISVersion } else { "3.08" })"

function Install-Windows-Dependencies {
    Write-Status "Checking Windows build dependencies"

    $ObsBuildDependencies = @(
        @("7z", "7zip"),
        @("cmake", "cmake --install-arguments 'ADD_CMAKE_TO_PATH=System'")
    )

    if(!(Test-CommandExists "choco")) {
        Set-ExecutionPolicy AllSigned
        Set-ExecutionPolicy Bypass -Scope Process -Force; [System.Net.ServicePointManager]::SecurityProtocol = [System.Net.ServicePointManager]::SecurityProtocol -bor 3072; iex ((New-Object System.Net.WebClient).DownloadString('https://chocolatey.org/install.ps1'))
    }

    Foreach($Dependency in $ObsBuildDependencies) {
        if($Dependency -is [system.array]) {
            $Command = $Dependency[0]
            $ChocoName = $Dependency[1]
        } else {
            $Command = $Dependency
            $ChocoName = $Dependency
        }

        if(!(Test-CommandExists "${Command}")) {
            Invoke-Expression "choco install -y ${ChocoName}"
        }
    }

    $env:Path = [System.Environment]::GetEnvironmentVariable("Path", "Machine") + ";" + [System.Environment]::GetEnvironmentVariable("Path", "User")
}
