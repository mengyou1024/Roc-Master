$VC2022_PATH="C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"
$VC2019_PATH="C:\Program Files\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvars64.bat"

if (Test-Path $VC2022_PATH) {
    $VC_PATH = $VC2022_PATH
} elseif (Test-Path $VC2019_PATH) {
    $VC_PATH = $VC2022_PATH
}

cmd /S /C "`"$VC_PATH`"  x64 && powershell -NoLogo -ExecutionPolicy Bypass -File script/PackInstaller.ps1"
