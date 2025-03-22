# build.ps1
param (
    [string]$BuildType = "Debug"
)

# 切换到 Conan 生成目录
Set-Location "conan_${BuildType}/build/${BuildType}/generators"

# 从 conanrun.bat 获取环境变量
$envOutput = cmd.exe /c "call conanrun.bat && set"
foreach ($line in $envOutput) {
    if ($line -match "^(.*?)=(.*)$") {
        [System.Environment]::SetEnvironmentVariable($matches[1], $matches[2], "Process")
    }
}

# 设置额外的 PATH、INCLUDE 和 LIB 环境变量
$msvcPath = "C:/Program Files/Microsoft Visual Studio/2022/Enterprise/VC/Tools/MSVC/14.43.34808"
$windowsSdkPath = "C:/Program Files (x86)/Windows Kits/10"
$windowsSdkVersion = "10.0.22621.0"
$vcpkgPath = "C:/vcpkg/installed/x64-windows"

# PATH：追加 CMake、Tools 和现有 PATH
$additionalPath = "C:/Program Files/CMake/bin;C:/Tools"
$env:PATH = "$additionalPath;$env:PATH"

# INCLUDE：MSVC、Windows SDK 和 vcpkg 的头文件路径
$env:INCLUDE = "$msvcPath/include;" +
               "$windowsSdkPath/Include/$windowsSdkVersion/ucrt;" +
               "$windowsSdkPath/Include/$windowsSdkVersion/um;" +
               "$windowsSdkPath/Include/$windowsSdkVersion/shared;" +
               "$vcpkgPath/include"

# LIB：MSVC、Windows SDK 和 vcpkg 的库路径
$env:LIB = "$msvcPath/lib/x64;" +
           "$windowsSdkPath/Lib/$windowsSdkVersion/ucrt/x64;" +
           "$windowsSdkPath/Lib/$windowsSdkVersion/um/x64;" +
           "$vcpkgPath/debug/lib"

# 回到项目根目录
Set-Location "..\..\..\.."

# 执行 CMake 构建
& ninja -C build