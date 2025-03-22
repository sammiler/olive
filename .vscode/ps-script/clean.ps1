# clean.ps1
param (
    [string]$BuildType = "Debug"
)

Set-Location "conan_${BuildType}/build/${BuildType}/generators"
$envOutput = cmd.exe /c "call conanrun.bat && set"
foreach ($line in $envOutput) {
    if ($line -match "^(.*?)=(.*)$") {
        [System.Environment]::SetEnvironmentVariable($matches[1], $matches[2], "Process")
    }
}
Set-Location "..\..\..\.."
& ninja -C build -t clean