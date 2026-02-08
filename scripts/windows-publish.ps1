[CmdletBinding()]
param (
    [string] $archiveName, 
    [string] $targetName, 
    [string] $targetQtDir,       # path to target Qt root (e.g. msvc2019_arm64)
    [string] $windeployqtPath    # path to windeployqt.exe (e.g. from msvc2019_64/bin)
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
    
    # 确定 windeployqt 执行路径
    $windeployqt = "windeployqt"
    if (-not [string]::IsNullOrEmpty($windeployqtPath)) {
        $windeployqt = $windeployqtPath
        Write-Host "Using explicit windeployqt: $windeployqt"
    } else {
         Write-Host "Using windeployqt from PATH"
    }

    # 运行 windeployqt (可能拷贝错误的架构，稍后覆盖)
    # 注意: 不使用 --compiler-runtime 因为我们后面会手动处理 VC 运行时
    & $windeployqt --qmldir . --plugindir dist\plugins --no-translations --no-opengl-sw dist\$targetName
    
    # 核心步骤: 如果指定了 targetQtDir，遍历 dist 目录下的所有 DLL，
    # 并尝试从 targetQtDir 对应的位置找到同名文件进行覆盖。
    if (-not [string]::IsNullOrEmpty($targetQtDir)) {
        Write-Host "Overwriting dependencies from Target Qt Dir: $targetQtDir"
        
        $files = Get-ChildItem -Path dist -Filter "*.dll" -Recurse
        foreach ($file in $files) {
            $relPath = $file.FullName.Substring((Get-Item dist).FullName.Length + 1)
            $targetFile = $null
            
            # Case 1: Root DLLs -> target/bin
            if (-not $relPath.Contains("\")) {
                 $candidate = Join-Path $targetQtDir "bin\$relPath"
                 if (Test-Path $candidate) { $targetFile = $candidate }
            }
            # Case 2: Plugins -> target/plugins
            elseif ($relPath.StartsWith("plugins\")) {
                 $subPath = $relPath.Substring(8) # len("plugins\")
                 $candidate = Join-Path $targetQtDir "plugins\$subPath"
                 if (Test-Path $candidate) { $targetFile = $candidate }
            }
            # Case 3: Other folders (assume QML) -> target/qml
            else {
                 $candidate = Join-Path $targetQtDir "qml\$relPath"
                 if (Test-Path $candidate) { $targetFile = $candidate }
            }
            
            if ($null -ne $targetFile) {
                Write-Host "Overwriting $relPath with $targetFile"
                Copy-Item $targetFile $file.FullName -Force
            } else {
                Write-Host "Warning: Could not find overwrite source for $relPath"
            }
        }

        # 显式处理 OpenSSL 库 (通常命名包含架构后缀，导致上面的同名覆盖失效)
        Write-Host "Explicitly copying OpenSSL DLLs from Target Qt Dir..."
        $opensslPattern = @("libssl*.dll", "libcrypto*.dll")
        if (Test-Path (Join-Path $targetQtDir "bin")) {
            Get-ChildItem -Path (Join-Path $targetQtDir "bin") -Include $opensslPattern -Recurse | ForEach-Object {
                Write-Host "Copying OpenSSL: $($_.Name)"
                Copy-Item $_.FullName -Destination "dist\" -Force
            }
        }

        # 如果我们在构建 ARM64，清理掉 windeployqt带进来的 x64 OpenSSL 库
        if ($targetQtDir -match "arm64") {
            Write-Host "Cleaning up x64 specific artifacts from dist..."
            Remove-Item "dist\*-x64.dll" -Force -ErrorAction SilentlyContinue
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

