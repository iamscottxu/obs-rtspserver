Param(
    [Switch]$Help = $(if (Test-Path variable:Help) { $Help }),
    [Switch]$Quiet = $(if (Test-Path variable:Quiet) { $Quiet }),
    [Switch]$Verbose = $(if (Test-Path variable:Verbose) { $Verbose }),
    [Switch]$BuildInstaller = $(if ($BuildInstaller.isPresent) { $true }),
    [String]$ProductName = $(if (Test-Path variable:ProductName) { "${ProductName}" } else { "obs-plugin" }),
    [Switch]$CombinedArchs = $(if ($CombinedArchs.isPresent) { $true }),
    [String]$BuildDirectory = $(if (Test-Path variable:BuildDirectory) { "${BuildDirectory}" } else { "build" }),
    [String]$BuildArch = $(if (Test-Path variable:BuildArch) { "${BuildArch}" } else { (Get-WmiObject Win32_OperatingSystem).OSArchitecture}),
    [String]$BuildConfiguration = $(if (Test-Path variable:BuildConfiguration) { "${BuildConfiguration}" } else { "RelWithDebInfo" })
)

##############################################################################
# Windows libobs plugin package function
##############################################################################
#
# This script file can be included in build scripts for Windows or run
# directly with the -Standalone switch
#
##############################################################################

$ErrorActionPreference = "Stop"

function Package-OBS-Plugin {
    Param(
        [String]$BuildDirectory = $(if (Test-Path variable:BuildDirectory) { "${BuildDirectory}" }),
        [String]$BuildArch = $(if (Test-Path variable:BuildArch) { "${BuildArch}" }),
        [String]$BuildConfiguration = $(if (Test-Path variable:BuildConfiguration) { "${BuildConfiguration}" })
    )

    Write-Status "Package plugin ${ProductName}"
    Ensure-Directory ${CheckoutDir}

    $makensis_path = "${CheckoutDir}/../obs-build-dependencies/nsis-$NSISVersion/makensis.exe"

    if ($CombinedArchs.isPresent) {
        if (!(Test-Path ${CheckoutDir}/release/obs-plugins/64bit)) {
            cmake --build ${BuildDirectory}64 --config ${BuildConfiguration} -t install
        }

        if (!(Test-Path ${CheckoutDir}/release/obs-plugins/32bit)) {
            cmake --build ${BuildDirectory}32 --config ${BuildConfiguration} -t install
        }

        $CompressVars = @{
            Path = "${CheckoutDir}/release/*"
            CompressionLevel = "Optimal"
            DestinationPath = "${FileName}-windows-all.zip"
        }

        Write-Step "Creating zip archive..."

        Compress-Archive -Force @CompressVars
        if(($BuildInstaller.isPresent) -And (Test-Path $makensis_path)) {
            Write-Step "Creating installer..."
            Set-Location "${CheckoutDir}/installer"
            Copy-Item "${CheckoutDir}/LICENSE" "${CheckoutDir}/installer/LICENSE"
            & $makensis_path /V4 /DVERSION=${ProductVersion} /DFVERSION=${ProductFileVersion} /DPLANTFORM=all .\installer.nsi
            Ensure-Directory ${CheckoutDir}
        }
    } elseif ($BuildArch -eq "64-bit") {
        cmake --build ${BuildDirectory}64 --config ${BuildConfiguration} -t install

        $CompressVars = @{
            Path = "${CheckoutDir}/release/*"
            CompressionLevel = "Optimal"
            DestinationPath = "${FileName}-windows-x64.zip"
        }

        Write-Step "Creating zip archive..."

        Compress-Archive -Force @CompressVars
        if(($BuildInstaller.isPresent) -And (Test-Path $makensis_path)) {
            Write-Step "Creating installer..."
            Set-Location "${CheckoutDir}/installer"
            Copy-Item "${CheckoutDir}/LICENSE" "${CheckoutDir}/installer/LICENSE"
            & $makensis_path /V4 /DVERSION=${ProductVersion} /DFVERSION=${ProductFileVersion} /DPLANTFORM=x64 .\installer.nsi
            Ensure-Directory ${CheckoutDir}
        }
    } elseif ($BuildArch -eq "32-bit") {
        cmake --build ${BuildDirectory}32 --config ${BuildConfiguration} -t install

        $CompressVars = @{
            Path = "${CheckoutDir}/release/*"
            CompressionLevel = "Optimal"
            DestinationPath = "${FileName}-windows-x86.zip"
        }

        Write-Step "Creating zip archive..."

        Compress-Archive -Force @CompressVars
        if(($BuildInstaller.isPresent) -And (Test-Path $makensis_path)) {
            Write-Step "Creating installer..."
            Set-Location "${CheckoutDir}/installer"
            Copy-Item "${CheckoutDir}/LICENSE" "${CheckoutDir}/installer/LICENSE"
            & $makensis_path /V4 /DVERSION=${ProductVersion} /DFVERSION=${ProductFileVersion} /DPLANTFORM=x86 .\installer.nsi
            Ensure-Directory ${CheckoutDir}
        }
    }
}

function Package-Plugin-Standalone {
    $CheckoutDir = git rev-parse --show-toplevel

    if (Test-Path ${CheckoutDir}/CI/include/build_environment.ps1) {
        . ${CheckoutDir}/CI/include/build_environment.ps1
    }

    . ${CheckoutDir}/CI/include/build_support_windows.ps1

    Ensure-Directory ${CheckoutDir}
    $GitBranch = git rev-parse --abbrev-ref HEAD
    $GitHash = git rev-parse --short HEAD
    $ErrorActionPreference = "SilentlyContiue"
    $GitTag = git describe --tags --abbrev=0
    $ErrorActionPreference = "Stop"

    if ($null -eq $GitTag) {
        $GitTag="v$ProductVersion"
    } elseif ($GitTag -match "[0-9]+.[0-9]+.[0-9]+(-[a-z0-9]+)+$") {
        $ProductVersion = $Matches[0]
    }

    if ($ProductVersion -match "^[0-9]+.[0-9]+.[0-9]+")
    {
        $ProductFileVersion = $Matches[0] + ".0"
    }
    $FileName = "${ProductName}-v${ProductVersion}"

    Package-OBS-Plugin
}

function Print-Usage {
    $Lines = @(
        "Usage: ${MyInvocation.MyCommand.Name}",
        "-Help                    : Print this help",
        "-Quiet                   : Suppress most build process output",
        "-Verbose                 : Enable more verbose build process output",
        "-CombinedArchs           : Create combined architecture package",
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

    Package-Plugin-Standalone
}
