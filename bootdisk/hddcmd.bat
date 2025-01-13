@echo off

set WINDRV=C
if exist %WINDRV%:\WINDOWS\COMMAND\START.EXE goto drvfound
set WINDRV=D
if exist %WINDRV%:\WINDOWS\COMMAND\START.EXE goto drvfound
set WINDRV=E
if exist %WINDRV%:\WINDOWS\COMMAND\START.EXE goto drvfound
set WINDRV=F
if exist %WINDRV%:\WINDOWS\COMMAND\START.EXE goto drvfound
echo.
echo Windows install not found in C:, D:, E: or F: - error!
echo.
goto end

:drvfound
echo.
echo ===============================================================
echo.
echo Windows install found in %WINDRV%:\WINDOWS
echo Running Windows COMMAND.COM, type EXIT to return to FreeDOS:

set PATH=%WINDRV%:\WINDOWS;%WINDRV%:\WINDOWS\COMMAND
set TEMP=%WINDRV%:\WINDOWS\TEMP
set DIRCMD=/on/p
%WINDRV%:
cd \
PROMPT $p$g
%WINDRV%:\WINDOWS\COMMAND.COM %WINDRV%:
echo.
ver

:end
