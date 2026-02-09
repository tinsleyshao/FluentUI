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

function Get-FileArch {
    param([string]$filePath)
    try {
        $stream = [System.IO.File]::OpenRead($filePath)
        $reader = [System.IO.BinaryReader]::new($stream)
        
        # Read DOS header
        if ($reader.ReadUInt16() -ne 0x5A4D) { 
            $reader.Close(); $stream.Close(); return "Unknown" 
        } # MZ
        
        $stream.Seek(60, [System.IO.SeekOrigin]::Begin)
        $peOffset = $reader.ReadInt32()
        
        $stream.Seek($peOffset, [System.IO.SeekOrigin]::Begin)
        if ($reader.ReadUInt32() -ne 0x00004550) { 
            $reader.Close(); $stream.Close(); return "Unknown" 
        } # PE\0\0
        
        $machine = $reader.ReadUInt16()
        
        $reader.Close()
        $stream.Close()
        
        switch ($machine) {
            0x8664 { return "x64" }
            0xAA64 { return "ARM64" }
            0x014c { return "x86" }
            default { return "0x{0:X}" -f $machine }
        }
    } catch {
        return "Error"
    }
}

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
    # 移除 --no-opengl-sw 以便让 OpenGL 程序有软解库可用 (随后会被覆盖为 arm64 版本)
    & $windeployqt --qmldir . --plugindir dist\plugins --no-translations dist\$targetName
    
    # 核心步骤: 如果指定了 targetQtDir，遍历 dist 目录下的所有 DLL，
    # 并尝试从 targetQtDir 对应的位置找到同名文件进行覆盖。
    if (-not [string]::IsNullOrEmpty($targetQtDir)) {
        Write-Host "Overwriting dependencies from Target Qt Dir: $targetQtDir"
        if (-not (Test-Path $targetQtDir)) {
            Write-Error "Target Qt Dir does not exist: $targetQtDir"
        }
        
        $distDir = (Get-Item dist).FullName
        $files = Get-ChildItem -Path dist -Filter "*.dll" -Recurse
        foreach ($file in $files) {
            $relPath = $file.FullName.Substring($distDir.Length + 1)
            $targetFile = $null
            
            # 策略1: 根目录 DLL -> target/bin
            if (-not $relPath.Contains("\") -and -not $relPath.Contains("/")) {
                 $candidate = Join-Path $targetQtDir "bin\$relPath"
                 if (Test-Path $candidate) { $targetFile = $candidate }
            }
            # 策略2: Plugins -> target/plugins
            elseif ($file.FullName.Contains("\plugins\")) {
                 # 提取 plugins 之后的路径
                 # dist\plugins\imageformats\qjpeg.dll -> imageformats\qjpeg.dll
                 $index = $relPath.IndexOf("plugins\")
                 if ($index -ge 0) {
                    $subPath = $relPath.Substring($index + 8)
                    $candidate = Join-Path $targetQtDir "plugins\$subPath"
                    if (Test-Path $candidate) { $targetFile = $candidate }
                 }
            }
            
            # 策略3: 其他带子目录的 (通常是 QML) -> target/qml
            elseif ($relPath.Contains("\")) {
                 # 如果 windeployqt 生成的目录结构已经是 qml\ 开头 (例如 dist\qml\QtQuick\...)
                 # 那么直接在 targetQtDir 下找对应路径 (target\qml\QtQuick\...)
                 if ($relPath.StartsWith("qml\")) {
                     $candidate = Join-Path $targetQtDir $relPath
                 } else {
                     # 否则假设 windeployqt 把模块放在了根目录 (dist\QtQuick\...)
                     # 我们需要到 target 的 qml 目录找 (target\qml\QtQuick\...)
                     $candidate = Join-Path $targetQtDir "qml\$relPath"
                 }

                 if (Test-Path $candidate) { $targetFile = $candidate }
            }

            # 策略4: 尝试在 bin 下寻找 (解决 D3DCompiler 等)
            if ($null -eq $targetFile) {
                 $candidate = Join-Path $targetQtDir "bin\$($file.Name)"
                 if (Test-Path $candidate) { $targetFile = $candidate }
            }

            if ($null -ne $targetFile) {
                Write-Host "Overwriting $relPath from $targetFile"
                Copy-Item $targetFile $file.FullName -Force
            } else {
                Write-Host "Warning: Could not find overwrite source for $relPath in $targetQtDir"
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

        # 如果我们在构建 ARM64，验证架构
        if ($targetQtDir -match "arm64") {
            Write-Host "Cleaning up x64 specific artifacts from dist..."
            Remove-Item "dist\*-x64.dll" -Force -ErrorAction SilentlyContinue

            Write-Host "Validating DLL Architectures..."
            $hasError = $false
            Get-ChildItem -Path dist -Filter "*.dll" -Recurse | ForEach-Object {
                $arch = Get-FileArch $_.FullName
                if ($arch -eq "x64" -or $arch -eq "x86") {
                     Write-Host "ERROR: File $($_.Name) is $arch (Expected ARM64) - DELETING" -ForegroundColor Red
                     Remove-Item $_.FullName -Force
                     $hasError = $true
                } else {
                     Write-Host "OK: $($_.Name) is $arch" -ForegroundColor Green
                }
            }
            if ($hasError) {
                Write-Warning "Cleaned up incorrect architecture files. Please verify if application can run."
            }
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
        $ucrtPath = Join-Path $sdkPath "ucrt\DLLs"
        $ucrtPath = Join-Path $ucrtPath $msvcArch
        
        if (Test-Path $ucrtPath) {
            Write-Host "Copying SDK UCRT Redist from $ucrtPath"
            Copy-Item "$ucrtPath\*.dll" dist\
        } else {
            Write-Host "Warning: SDK UCRT Redist path not found: $ucrtPath"
        }
        
        # 显式查找并拷贝 D3Dcompiler_47.dll
        # 通常位置: C:\Program Files (x86)\Windows Kits\10\bin\10.0.19041.0\arm64\d3dcompiler_47.dll
        $d3dPath = Join-Path $winSdkDir "bin"
        $d3dPath = Join-Path $d3dPath $winSdkVer
        $d3dPath = Join-Path $d3dPath $msvcArch
        $d3dDll = Join-Path $d3dPath "d3dcompiler_47.dll"
        
        if (Test-Path $d3dDll) {
            Write-Host "Found D3DCompiler at $d3dDll. Copying..."
            Copy-Item $d3dDll dist\
        } else {
             Write-Host "Warning: D3DCompiler not found at $d3dDll"
        }

        # 显式查找并拷贝 opengl32sw.dll (如果 windeployqt 没带或者为了确保是 arm64)
        $openglDll = Join-Path $targetQtDir "bin\opengl32sw.dll"
        if (Test-Path $openglDll) {
             Write-Host "Found opengl32sw.dll at $openglDll. Copying..."
             Copy-Item $openglDll dist\
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

