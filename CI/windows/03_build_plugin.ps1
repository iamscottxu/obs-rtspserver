Param(
    [Switch]$Help = $(if (Test-Path variable:Help) { $Help }),
    [Switch]$Quiet = $(if (Test-Path variable:Quiet) { $Quiet }),
    [Switch]$Verbose = $(if (Test-Path variable:Verbose) { $Verbose }),
    [String]$ProductName = $(if (Test-Path variable:ProductName) { "${ProductName}" } else { "obs-plugin" }),
    [String]$BuildDirectory = $(if (Test-Path variable:BuildDirectory) { "${BuildDirectory}" } else { "build" }),
    [String]$BuildArch = $(if (Test-Path variable:BuildArch) { "${BuildArch}" } else { (Get-WmiObject Win32_OperatingSystem).OSArchitecture}),
    [String]$BuildConfiguration = $(if (Test-Path variable:BuildConfiguration) { "${BuildConfiguration}" } else { "RelWithDebInfo" })
)

##############################################################################
# Windows libobs plugin build function
##############################################################################
#
# This script file can be included in build scripts for Windows or run
# directly
#
##############################################################################

$ErrorActionPreference = "Stop"

function Build-OBS-Plugin {
    Param(
        [String]$BuildDirectory = $(if (Test-Path variable:BuildDirectory) { "${BuildDirectory}" }),
        [String]$BuildArch = $(if (Test-Path variable:BuildArch) { "${BuildArch}" }),
        [String]$BuildConfiguration = $(if (Test-Path variable:BuildConfiguration) { "${BuildConfiguration}" })
    )

    Write-Status "Build plugin ${ProductName}"
    Ensure-Directory ${CheckoutDir}

    if ($BuildArch -eq "64-bit") {
        $QtDirectory = "${CheckoutDir}/../obs-build-dependencies/windows-deps-qt6-${WindowsDepsVersion}-x64"
        $DepsDirectory = "${CheckoutDir}/../obs-build-dependencies/windows-deps-${WindowsDepsVersion}-x64"
        
        Set-Msvc-Environment-And-Run-Cmake -Arch "amd64" -WinsdkVersion "${CmakeSystemVersion}" -CmakeArgumentList @(
            "-S", ".", "-B", """${BuildDirectory}64""", "-G", "Ninja",
            "-DCMAKE_SYSTEM_VERSION=""${CmakeSystemVersion}""",
            "-DQTDIR=""${QtDirectory}""",
            "-DDepsPath=""${DepsDirectory}""",
            "-DCMAKE_C_COMPILER=cl.exe",
            "-DCMAKE_CXX_COMPILER=cl.exe",
            "-DOBS_SOURCE_DIR=""${ObsBuildDir}""",
            "-DCMAKE_BUILD_TYPE=""${BuildConfiguration}""",
            "$(if (Test-Path Variable:$Quiet) { "-Wno-deprecated -Wno-dev --log-level=ERROR" })")

        Set-Msvc-Environment-And-Run-Cmake -Arch "amd64" -WinsdkVersion "${CmakeSystemVersion}" -CmakeArgumentList @(
            "--build", """${BuildDirectory}64""", "--config", "${BuildConfiguration}")
    } else {
        $QtDirectory = "${CheckoutDir}/../obs-build-dependencies/windows-deps-qt6-${WindowsDepsVersion}-x86"
        $DepsDirectory = "${CheckoutDir}/../obs-build-dependencies/windows-deps-${WindowsDepsVersion}-x86"

        Set-Msvc-Environment-And-Run-Cmake -Arch "x86" -WinsdkVersion "${CmakeSystemVersion}" -CmakeArgumentList @(
            "-S", ".", "-B", """${BuildDirectory}32""", "-G", "Ninja",
            "-DCMAKE_SYSTEM_VERSION=""${CmakeSystemVersion}""",
            "-DQTDIR=""${QtDirectory}""",
            "-DDepsPath=""${DepsDirectory}""",
            "-DCMAKE_C_COMPILER=cl.exe",
            "-DCMAKE_CXX_COMPILER=cl.exe",
            "-DOBS_SOURCE_DIR=""${ObsBuildDir}""",
            "-DCMAKE_BUILD_TYPE=""${BuildConfiguration}""",
            "$(if (Test-Path Variable:$Quiet) { "-Wno-deprecated -Wno-dev --log-level=ERROR" })")

        Set-Msvc-Environment-And-Run-Cmake -Arch "x86" -WinsdkVersion "${CmakeSystemVersion}" -CmakeArgumentList @(
            "--build", """${BuildDirectory}32""", "--config", "${BuildConfiguration}")
    }

    Ensure-Directory ${CheckoutDir}
}

function Build-Plugin-Standalone {
    $CheckoutDir = git rev-parse --show-toplevel

    if (Test-Path ${CheckoutDir}/CI/include/build_environment.ps1) {
        . ${CheckoutDir}/CI/include/build_environment.ps1
    }

    $ObsBuildDir = "${CheckoutDir}/../obs-studio"

    . ${CheckoutDir}/CI/include/build_support_windows.ps1

    Build-OBS-Plugin
}

function Print-Usage {
    $Lines = @(
        "Usage: ${MyInvocation.MyCommand.Name}",
        "-Help                    : Print this help",
        "-Quiet                   : Suppress most build process output",
        "-Verbose                 : Enable more verbose build process output",
        "-BuildDirectory          : Directory to use for builds - Default: build64 on 64-bit systems, build32 on 32-bit systems",
        "-BuildArch               : Build architecture to use (32bit or 64bit) - Default: local architecture",
        "-BuildConfiguration      : Build configuration to use - Default: RelWithDebInfo"
    )

    $Lines | Write-Host
}

if(!(Test-Path variable:_RunObsBuildScript)) {
    if($Help.isPresent) {
        Print-Usage
        exit 0
    }

    Build-Plugin-Standalone
 }
