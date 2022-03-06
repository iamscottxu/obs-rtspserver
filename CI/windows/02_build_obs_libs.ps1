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
# Windows libobs library build function
##############################################################################
#
# This script file can be included in build scripts for Windows or run
# directly
#
##############################################################################

$ErrorActionPreference = "Stop"

function Build-OBS-Libs {
    Param(
        [String]$BuildDirectory = $(if (Test-Path variable:BuildDirectory) { "${BuildDirectory}" }),
        [String]$BuildArch = $(if (Test-Path variable:BuildArch) { "${BuildArch}" }),
        [String]$BuildConfiguration = $(if (Test-Path variable:BuildConfiguration) { "${BuildConfiguration}" })
    )

    Write-Status "Build libobs and obs-frontend-api"

    Ensure-Directory ${ObsBuildDir}

    if ($BuildArch -eq "64-bit") {
        $QtDirectory = "${CheckoutDir}/../obs-build-dependencies/Qt_${WindowsQtVersion}/msvc2019_64"
        $DepsDirectory = "${CheckoutDir}/../obs-build-dependencies/windows-deps-${WindowsDepsVersion}/win64"
        
        Set-Msvc-Environment-And-Run-Cmake -Arch "amd64" -WinsdkVersion "${CmakeSystemVersion}" -CmakeArgumentList @(
            "-S", ".", "-B", """plugin_${BuildDirectory}64""", "-G", "Ninja",
            "-DCMAKE_SYSTEM_VERSION=""${CmakeSystemVersion}""",
            "-DCMAKE_VS_WINDOWS_TARGET_PLATFORM_VERSION=""${CmakeSystemVersion}""",
            "-DQTDIR=""${QtDirectory}""",
            "-DDepsPath=""${DepsDirectory}""",
            "-DCMAKE_C_COMPILER=cl.exe",
            "-DCMAKE_CXX_COMPILER=cl.exe",
            "-DENABLE_PLUGINS=OFF",
            "-DENABLE_UI=ON",
            "-DENABLE_SCRIPTING=OFF",
            "-DBUILD_BROWSER=OFF",
            "-DCMAKE_BUILD_TYPE=Release",
            "$(if (Test-Path Variable:$Quiet) { "-Wno-deprecated -Wno-dev --log-level=ERROR" })")

        Set-Msvc-Environment-And-Run-Cmake -Arch "amd64" -WinsdkVersion "${CmakeSystemVersion}" -CmakeArgumentList @(
            "--build", """plugin_${BuildDirectory}64""", "-t", "obs-frontend-api", "--config ${BuildConfiguration}")
    } else {
        $QtDirectory = "${CheckoutDir}/../obs-build-dependencies/Qt_${WindowsQtVersion}/msvc2019"
        $DepsDirectory = "${CheckoutDir}/../obs-build-dependencies/windows-deps-${WindowsDepsVersion}/win32"

        Set-Msvc-Environment-And-Run-Cmake -Arch "x86" -WinsdkVersion "${CmakeSystemVersion}" -CmakeArgumentList @(
            "-S", ".", "-B", """plugin_${BuildDirectory}32""", "-G", "Ninja",
            "-DCMAKE_SYSTEM_VERSION=""${CmakeSystemVersion}""",
            "-DCMAKE_VS_WINDOWS_TARGET_PLATFORM_VERSION=""${CmakeSystemVersion}""",
            "-DQTDIR=""${QtDirectory}""",
            "-DDepsPath=""${DepsDirectory}""",
            "-DCMAKE_C_COMPILER=cl.exe",
            "-DCMAKE_CXX_COMPILER=cl.exe",
            "-DENABLE_PLUGINS=OFF",
            "-DENABLE_UI=ON",
            "-DENABLE_SCRIPTING=OFF",
            "-DBUILD_BROWSER=OFF",
            "-DCMAKE_BUILD_TYPE=Release",
            "$(if (Test-Path Variable:$Quiet) { "-Wno-deprecated -Wno-dev --log-level=ERROR" })")

            Set-Msvc-Environment-And-Run-Cmake -Arch "x86" -WinsdkVersion "${CmakeSystemVersion}" -CmakeArgumentList @(
                "--build", """plugin_${BuildDirectory}32""", "-t", "obs-frontend-api", "--config ${BuildConfiguration}")
    }

    Ensure-Directory ${CheckoutDir}
}


function Build-OBS-Libs-Standalone {
    $CheckoutDir = git rev-parse --show-toplevel

    if (Test-Path ${CheckoutDir}/CI/include/build_environment.ps1) {
        . ${CheckoutDir}/CI/include/build_environment.ps1
    }

    $ObsBuildDir = "${CheckoutDir}/../obs-studio"

    . ${CheckoutDir}/CI/include/build_support_windows.ps1

    Build-OBS-Libs
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

    Build-OBS-Libs-Standalone
}
