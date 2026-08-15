:: based on https://github.com/es3n1n/defendnot/issues/49 comments
@echo off
setlocal

net session >nul 2>&1
if %errorlevel% neq 0 (
    echo This script needs administrator rights
    pause
    exit /b 1
)

reg.exe ADD "HKLM\SOFTWARE\Policies\Microsoft\Windows Defender" /v DisableAntiSpyware /t REG_DWORD /d 00000001 /f
reg.exe ADD "HKLM\SOFTWARE\Policies\Microsoft\Windows Defender" /v DisableAntiVirus /t REG_DWORD /d 00000001 /f
reg.exe ADD "HKLM\SOFTWARE\Policies\Microsoft\Windows Defender" /v DisableRoutinelyTakingAction /t REG_DWORD /d 00000001 /f
reg.exe ADD "HKLM\SOFTWARE\Policies\Microsoft\Windows Defender" /v ServiceKeepAlive /t REG_DWORD /d 00000000 /f
reg.exe ADD "HKLM\SOFTWARE\Policies\Microsoft\Windows Defender\Real-Time Protection" /v DisableBehaviorMonitoring /t REG_DWORD /d 00000001 /f
reg.exe ADD "HKLM\SOFTWARE\Policies\Microsoft\Windows Defender\Real-Time Protection" /v DisableIntrusionPreventionSystem /t REG_DWORD /d 00000001 /f
reg.exe ADD "HKLM\SOFTWARE\Policies\Microsoft\Windows Defender\Real-Time Protection" /v DisableIOAVProtection /t REG_DWORD /d 00000001 /f
reg.exe ADD "HKLM\SOFTWARE\Policies\Microsoft\Windows Defender\Real-Time Protection" /v DisableOnAccessProtection /t REG_DWORD /d 00000001 /f
reg.exe ADD "HKLM\SOFTWARE\Policies\Microsoft\Windows Defender\Real-Time Protection" /v DisableRawWriteNotification /t REG_DWORD /d 00000001 /f
reg.exe ADD "HKLM\SOFTWARE\Policies\Microsoft\Windows Defender\Real-Time Protection" /v DisableRealtimeMonitoring /t REG_DWORD /d 00000001 /f
reg.exe ADD "HKLM\SOFTWARE\Policies\Microsoft\Windows Defender\Real-Time Protection" /v DisableScanOnRealtimeEnable /t REG_DWORD /d 00000001 /f
reg.exe ADD "HKLM\SOFTWARE\Policies\Microsoft\Windows Defender\Signature Updates" /v DisableUpdateOnStartupWithoutEngine /t REG_DWORD /d 00000001 /f
reg.exe ADD "HKLM\SOFTWARE\Policies\Microsoft\Windows Defender\Signature Updates" /v UpdateOnStartup /t REG_DWORD /d 00000000 /f
reg.exe ADD "HKLM\SOFTWARE\Policies\Microsoft\Windows Defender\SpyNet" /v DisableBlockAtFirstSeen /t REG_DWORD /d 00000001 /f
reg.exe ADD "HKLM\SOFTWARE\Policies\Microsoft\Windows Defender\SpyNet" /v SpynetReporting /t REG_DWORD /d 00000000 /f

echo All done, please restart your machine to apply these changes
pause
endlocal
