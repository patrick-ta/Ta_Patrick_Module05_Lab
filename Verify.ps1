param([Parameter(Mandatory=$true)][string]$EngineRoot,[switch]$Baseline)
$ErrorActionPreference='Stop'
$exe=Join-Path $EngineRoot 'Engine/Binaries/Win64/UnrealEditor-Cmd.exe'
if(!(Test-Path -LiteralPath $exe)){throw "Engine executable not found: $exe"}
$out=Join-Path $PSScriptRoot 'Saved/Verification'
New-Item -ItemType Directory -Path $out -Force | Out-Null
$csv=Join-Path $out 'Week05.csv'
if(Test-Path -LiteralPath $csv){Remove-Item -LiteralPath $csv}
$project=Join-Path $PSScriptRoot 'CourseGame.uproject'
$log=Join-Path $out 'Verify.log'
$runArgs=@(('"'+$project+'"'),'/Game/Course/Maps/L_Week05','-game','-nullrhi','-unattended','-nosound','-nosplash','-Week05Verify',('-abslog="'+$log+'"'))
if($Baseline){$runArgs+='-LabBaseline'}
$proc=Start-Process -FilePath $exe -ArgumentList $runArgs -WorkingDirectory $PSScriptRoot -WindowStyle Hidden -PassThru -RedirectStandardOutput (Join-Path $out 'Verify-stdout.log') -RedirectStandardError (Join-Path $out 'Verify-stderr.log')
$null=$proc.Handle
if(!$proc.WaitForExit(600000)){Stop-Process -Id $proc.Id;throw 'Verification timed out. Read Saved/Verification/Verify.log.'}
if(!(Test-Path -LiteralPath $csv)){throw "Verification produced no report. Read $log"}
$rows=Import-Csv -LiteralPath $csv
$failed=@($rows | Where-Object {$_.result -ne 'PASS'})
$overall=@($rows | Where-Object {$_.check -eq 'Overall' -and $_.result -eq 'PASS'})
$rows | Format-Table -AutoSize
if($proc.ExitCode -ne 0 -or $failed.Count -gt 0 -or $overall.Count -ne 1){throw 'Verification FAILED. Unfinished starter TODOs should fail the completed-lab test. -Baseline only tests the initial unfinished state.'}
Write-Host "PASS. Report: $csv"

