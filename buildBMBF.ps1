# Builds a .zip file for loading with BMBF
$NDKPath = Get-Content $PSScriptRoot/ndkpath.txt
$Name = "introSkip_v0.3.0"

$buildScript = "$NDKPath/build/ndk-build"
if (-not ($PSVersionTable.PSEdition -eq "Core")) {
    $buildScript += ".cmd"
}

& $buildScript NDK_PROJECT_PATH=$PSScriptRoot APP_BUILD_SCRIPT=$PSScriptRoot/Android.mk NDK_APPLICATION_MK=$PSScriptRoot/Application.mk
Compress-Archive -Path "./libs/arm64-v8a/libIntroSkip.so","./mod.json","./extern/libbeatsaber-hook_2_2_5.so" -DestinationPath "./$Name.zip" -Update
$FileName = "./$Name.qmod"
if(Test-Path $FileName) {
  Remove-Item $FileName
}
Rename-Item -Path "./$Name.zip" "$Name.qmod"