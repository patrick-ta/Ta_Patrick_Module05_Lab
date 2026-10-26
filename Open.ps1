param([Parameter(Mandatory=$true)][string]$EngineRoot)
$ErrorActionPreference='Stop'
$exe=Join-Path $EngineRoot 'Engine/Binaries/Win64/UnrealEditor.exe'
if(!(Test-Path -LiteralPath $exe)){throw "Engine executable not found: $exe"}
$project=Join-Path $PSScriptRoot 'CourseGame.uproject'
& $exe $project
