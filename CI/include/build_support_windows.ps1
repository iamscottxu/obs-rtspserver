function Write-Status {
    param(
        [parameter(Mandatory=$true)]
        [string] $output
    )

    if (!($Quiet.isPresent)) {
        if (Test-Path env:CI) {
            Write-Output "`e[33;34m[${ProductName}] ${output}`e[33;0m"
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
            Write-Output "`e[33;33m + ${output}`e[33;0m"
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
            Write-Output "`e[33;32m + ${output}`e[33;0m"
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
        Write-Output "e[33;31m + ${output}`e[33;0m"
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
#$WindowsDepsVersion = "$(if (Test-Path Env:WindowsDepsVersion ) { $env:WindowsDepsVersion } else { "2022-02-13" })"
$WindowsDepsVersion = "$(if (Test-Path Env:WindowsDepsVersion ) { $env:DEPS_VERSION_WIN } else { "2022-02-13" })"
#$WindowsQtVersion = "$(if (Test-Path Env:WindowsQtVersion ) { $env:WindowsQtVersion } else { "5.15.2" })"
$WindowsQtVersion = "$(if (Test-Path Env:WindowsQtVersion ) { $env:QT_VERSION_WIN } else { "5.15.2" })"
$CmakeSystemVersion = "$(if (Test-Path Env:CMAKE_SYSTEM_VERSION) { $Env:CMAKE_SYSTEM_VERSION } else { "10.0.20348.0" })"
#$OBSVersion = "$(if ( Test-Path Env:OBSVersion ) { $env:ObsVersion } else { "27.2.3" })"
$OBSVersion = "$(if ( Test-Path Env:OBSVersion ) { $env:OBS_VERSION } else { "27.2.3" })"
#$NSISVersion = "$(if ( Test-Path Env:NSISVersion ) { $env:NSISVersion } else { "3.08" })"
$NSISVersion = "$(if ( Test-Path Env:NSISVersion ) { $env:NSIS_VERSION_WIN } else { "3.08" })"

if ($env:GITHUB_ACTIONS -eq "true")
{
    $env:CI = "true"
}

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

function Find-Msvc-Vcvarsall-Path {
    param(
        [string]$Version = ""
    )
    $vswherePath = "${env:ProgramFiles}\Microsoft Visual Studio\Installer\vswhere.exe"
    if (-not (Test-Path $vswherePath)) { $vswherePath = "${env:ProgramFiles(x86)}\Microsoft Visual Studio\Installer\vswhere.exe" }
    $argumentList = "-products", "*", "-requires", "Microsoft.VisualStudio.Component.VC.Tools.x86.x64", "-property", "installationPath", "-latest"
    if ($Version -ne "") {
        $argumentList += "-version", $Version
    }
    $output = (&$vswherePath $argumentList)
    $VcvarsallPath = "$output\VC\Auxiliary\Build\vcvarsall.bat"
    return $VcvarsallPath
}

function Set-Msvc-Environment-And-Run-Cmake {
    param(
        [string]$VsVersion = "",
        [ValidateSet("x86", "amd64", "x86_amd64", "x86_arm", "x86_arm64", "amd64_x86", "amd64_arm", "amd64_arm64")]
        [String]$Arch,
        [ValidateSet("", "store", "uwp")]
        [String]$PlatformType = "",
        [string]$WinsdkVersion = "",
        [string]$VcvarsVer = "",
        [string]$VcvarsSpectreLibs = "",
        [string[]]$CmakeArgumentList
    )
    $VcvarsallPath = Find-Msvc-Vcvarsall-Path($VsVersion)
    $argumentList = "/C", "call", """$VcvarSallPath""", $Arch, $PlatformType, $WinsdkVersion
    if (-not $VcvarsVer -eq '') { $argumentList += "-vcvars_ver=$VcvarsVer" }
    if (-not $VcvarsSpectreLibs -eq '') {  $argumentList += "-vcvars_spectre_libs=$VcvarsSpectreLibs" }
    $argumentList += "&&call", "cmake.exe"
    $argumentList += $CmakeArgumentList
    &"$env:windir\System32\cmd.exe" $argumentList
} 