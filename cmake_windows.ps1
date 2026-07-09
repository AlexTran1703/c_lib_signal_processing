param(
    [string]$BuildDir = "build",

    [string]$OutputDir = "output",

    [string]$Compiler = "clang",

    [string]$Mode = "main",

    [string]$BuildType = "Release",

    # Single example file to build, relative to ./example (e.g. example_narray1d.c).
    # If empty, all examples are built.
    [string]$Example_File = "",

    [bool]$RemoveBuildDir = $true


)

$build_config = @{
    BuildDir = $BuildDir
    OutputDir = $OutputDir
    Compiler = $Compiler
    Mode     = $Mode
    BuildType = $BuildType
    Example_File = $Example_File
    RemoveBuildDir = $RemoveBuildDir
}

if ($build_config["RemoveBuildDir"] -eq $true) {
    $params = @{
    Path        = $build_config["BuildDir"]
    Recurse     = $true
    Force       = $true
    ErrorAction = "SilentlyContinue"
}
    Remove-Item @params

        $params = @{
    Path        = $build_config["OutputDir"]
    Recurse     = $true
    Force       = $true
    ErrorAction = "SilentlyContinue"
}
    Remove-Item @params
}

$build_command = "cmake -S . -B $($build_config["BuildDir"]) -DCMAKE_BUILD_TYPE=$($build_config["BuildType"]) -DBUILD_MODE=$($build_config["Mode"])"

if ($build_config["Compiler"] -eq "clang") {
    Write-Host "Using Clang Compiler"
    $build_command += " -G Ninja -DCMAKE_C_COMPILER=clang"
}

if ($build_config["Mode"] -eq "example" -and -not [string]::IsNullOrWhiteSpace($build_config["Example_File"])) {
    $build_command += " -DEXAMPLE_SOURCE=`"$($build_config["Example_File"])`""
}

Invoke-Expression $build_command
Invoke-Expression "cmake --build $($build_config["BuildDir"])"


# ------------------------------------------------------------
# Copy build outputs into ./output
# ------------------------------------------------------------

$output_root = Join-Path $PWD $build_config["OutputDir"]
$output_bin  = Join-Path $output_root "bin"
$output_lib  = Join-Path $output_root "lib"

New-Item -ItemType Directory -Force -Path $output_bin | Out-Null
New-Item -ItemType Directory -Force -Path $output_lib | Out-Null

$build_root = Join-Path $PWD $build_config["BuildDir"]

# Visual Studio generator usually outputs to build/Debug or build/Release.
# Ninja usually outputs directly to build/.
$config_dir = Join-Path $build_root $build_config["Config"]

if (Test-Path $config_dir) {
    $search_root = $config_dir
}
else {
    $search_root = $build_root
}

Write-Host "Searching build outputs in: $search_root"

$built_files = Get-ChildItem -Path $search_root -Recurse -File -Include *.exe, *.lib, *.a |
    Where-Object {
        $_.FullName -notmatch "\\CMakeFiles\\" -and
        $_.FullName -notmatch "\\Testing\\"
    }

foreach ($file in $built_files) {
    switch ($file.Extension) {
        ".exe" {
            Copy-Item -Path $file.FullName -Destination $output_bin -Force
            Write-Host "Copied EXE: $($file.Name) -> $($output_bin)"
        }

        ".lib" {
            Copy-Item -Path $file.FullName -Destination $output_lib -Force
            Write-Host "Copied LIB: $($file.Name) -> $($output_lib)"
        }

        ".a" {
            Copy-Item -Path $file.FullName -Destination $output_lib -Force
            Write-Host "Copied static archive: $($file.Name) -> $($output_lib)"
        }
    }
}