[CmdletBinding()]
param (
    [string] $archiveName, [string] $targetName, [string] $qtBinDir
)
# 外部环境变量包括:
# archiveName: ${{ matrix.qt_ver }}-${{ matrix.qt_arch }}
# winSdkDir: ${{ steps.build.outputs.winSdkDir }}
# winSdkVer: ${{ steps.build.outputs.winSdkVer }}
# vcToolsInstallDir: ${{ steps.build.outputs.vcToolsInstallDir }}
# vcToolsRedistDir: ${{ steps.build.outputs.vcToolsRedistDir }}
# msvcArch: ${{ matrix.msvc_arch }}


# winSdkDir: C:\Program Files (x86)\Windows Kits\10\ 
# winSdkVer: 10.0.19041.0\ 
# vcToolsInstallDir: C:\Program Files (x86)\Microsoft Visual Studio\2019\Enterprise\VC\Tools\MSVC\14.28.29333\ 
# vcToolsRedistDir: C:\Program Files (x86)\Microsoft Visual Studio\2019\Enterprise\VC\Redist\MSVC\14.28.29325\ 
# archiveName: 5.9.9-win32_msvc2015
# msvcArch: x86

$scriptDir = $PSScriptRoot
$currentDir = Get-Location
Write-Host "currentDir" $currentDir
Write-Host "scriptDir" $scriptDir

function Main() {

    New-Item -ItemType Directory dist
    # 拷贝exe
    Copy-Item bin\Release\* dist -Force -Recurse | Out-Null
    
    # 确定 windeployqt 路径
    $windeployqt = "windeployqt"
    if (-not [string]::IsNullOrEmpty($qtBinDir)) {
        $windeployqt = Join-Path $qtBinDir "windeployqt.exe"
        Write-Host "Using explicit windeployqt: $windeployqt"
    } else {
        Write-Host "Using windeployqt from PATH"
    }

    # 拷贝依赖
    # 注意: 如果手动拷贝了 DLL，windeployqt 应该会跳过它们，只处理插件和 QML
    & $windeployqt --qmldir . --plugindir dist\plugins --no-translations --no-opengl-sw dist\$targetName
    
    if (-not [string]::IsNullOrEmpty($qtBinDir)) {
        # 强制覆盖核心 DLL，防止 windeployqt 拷贝错误架构的文件
        Write-Host "Manually copying Core DLLs from $qtBinDir to dist AFTER windeployqt..."
        Get-ChildItem -Path $qtBinDir -Filter "Qt6*.dll" | ForEach-Object {
            Copy-Item $_.FullName -Destination "dist\" -Force
        }
        # 同时也拷贝 d3dcompiler_47.dll 如果存在
        if (Test-Path "$qtBinDir\d3dcompiler_47.dll") {
            Copy-Item "$qtBinDir\d3dcompiler_47.dll" -Destination "dist\" -Force
        }
    }

    # 删除不必要的文件
    $excludeList = @("*.qmlc", "*.ilk", "*.exp", "*.lib", "*.pdb")
    Remove-Item -Path dist -Include $excludeList -Recurse -Force
    # 拷贝vcRedist dll
    # 使用 Trim() 去除可能存在的环境变量尾部空格 (由于 cmd echo >> 导致)
    $vcToolsRedistDir = if ($env:vcToolsRedistDir) { $env:vcToolsRedistDir.Trim() } else { "" }
    $msvcArch = if ($env:msvcArch) { $env:msvcArch.Trim() } else { "" }
    $winSdkDir = if ($env:winSdkDir) { $env:winSdkDir.Trim() } else { "" }
    $winSdkVer = if ($env:winSdkVer) { $env:winSdkVer.Trim() } else { "" }

    $vcRedistPath = Join-Path $vcToolsRedistDir $msvcArch
    $vcRedistGlob = Join-Path $vcRedistPath "*.CRT\*.dll"
    if (Test-Path $vcRedistPath) {
        Write-Host "Copying VC Redist from $vcRedistGlob"
        Copy-Item $vcRedistGlob dist\
    } else {
        Write-Host "Warning: VC Redist path not found: $vcRedistPath"
    }

    # 拷贝WinSDK dll
    if (-not [string]::IsNullOrEmpty($winSdkDir) -and -not [string]::IsNullOrEmpty($winSdkVer)) {
        $sdkPath = Join-Path $winSdkDir "Redist"
        $sdkPath = Join-Path $sdkPath $winSdkVer
        $sdkPath = Join-Path $sdkPath "ucrt\DLLs"
        $sdkPath = Join-Path $sdkPath $msvcArch
        
        if (Test-Path $sdkPath) {
            Write-Host "Copying SDK Redist from $sdkPath"
            Copy-Item "$sdkPath\*.dll" dist\
        } else {
            Write-Host "Warning: SDK Redist path not found: $sdkPath"
        }
    } else {
        Write-Host "Warning: WinSDK environment variables missing."
    }
    
    # 打包zip
    Compress-Archive -Path dist $archiveName'.zip'
}

if ($null -eq $archiveName || $null -eq $targetName) {
    Write-Host "args missing, archiveName is" $archiveName ", targetName is" $targetName
    return
}
Main

