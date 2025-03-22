# configure.ps1
param (
    [string]$BuildType = "Debug",
    [string[]]$CMakeFlags = @(
        "-DCMAKE_CXX_FLAGS=/EHsc /W3 /MP6 /Zi /FS /MDd /D_ITERATOR_DEBUG_LEVEL=2",
        "-DCMAKE_C_FLAGS=/EHsc /W3 /MP6 /Zi /FS /MDd /D_ITERATOR_DEBUG_LEVEL=2"
    )
)

Set-Location "conan_${BuildType}/build/${BuildType}/generators"
$envOutput = cmd.exe /c "call conanrun.bat && set"
foreach ($line in $envOutput) {
    if ($line -match "^(.*?)=(.*)$") {
        [System.Environment]::SetEnvironmentVariable($matches[1], $matches[2], "Process")
    }
}

$msvcPath = "C:/Program Files/Microsoft Visual Studio/2022/Enterprise/VC/Tools/MSVC/14.43.34808"
$windowsSdkPath = "C:/Program Files (x86)/Windows Kits/10"
$windowsSdkVersion = "10.0.22621.0"
$vcpkgPath = "C:/vcpkg/installed/x64-windows"

$additionalPath = "C:/Program Files/CMake/bin;C:/Tools"
$env:PATH = "$additionalPath;$env:PATH"
$env:INCLUDE = "$msvcPath/include;$windowsSdkPath/Include/$windowsSdkVersion/ucrt;$windowsSdkPath/Include/$windowsSdkVersion/um;$windowsSdkPath/Include/$windowsSdkVersion/shared;$vcpkgPath/include"
$env:LIB = "$msvcPath/lib/x64;$windowsSdkPath/Lib/$windowsSdkVersion/ucrt/x64;$windowsSdkPath/Lib/$windowsSdkVersion/um/x64;$vcpkgPath/debug/lib"

Set-Location "..\..\..\.."
Write-Host "Configuring with BuildType: $BuildType"

& cmake -S . -B build `
    -G "Ninja" `
    -DCMAKE_EXPORT_COMPILE_COMMANDS=ON `
    -DCMAKE_POLICY_DEFAULT_CMP0091=NEW `
    -DCMAKE_BUILD_TYPE="$BuildType" `
    -DCMAKE_TOOLCHAIN_FILE="${PWD}/conan_${BuildType}/build/${BuildType}/generators/conan_toolchain.cmake" `
    @CMakeFlags `
    -DCMAKE_RUNTIME_OUTPUT_DIRECTORY="${PWD}/build/bin" `
    -DCMAKE_LIBRARY_OUTPUT_DIRECTORY="${PWD}/build/lib" `
    -DCMAKE_ARCHIVE_OUTPUT_DIRECTORY="${PWD}/build/lib" `
    -DCMAKE_INSTALL_PREFIX="${PWD}/install_dir"