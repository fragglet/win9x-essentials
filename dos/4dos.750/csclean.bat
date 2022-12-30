@echo off
rem
rem 4DOS uninstall -- CONFIG.SYS cleanup
rem Copyright (c) 2003, JP Software Inc., All Rights Reserved.
rem
rem DO NOT delete this file!  It may be required when uninstalling 4DOS.
rem
rem DO NOT run this file manually.  It is for use by the automated
rem uninstall software.
rem
cls
echo.
echo This window is running a small batch file to restore your
echo CONFIG.SYS file to its state before 4DOS was installed.
echo.
echo To perform this restore, press the space bar or Enter key at the
echo prompt below.  To skip this step and clean up CONFIG.SYS manually,
echo press Ctrl-C or Ctrl-Break at the prompt.  
echo.
echo Depending on your Windows settings, this window may not close
echo automatically after CONFIG.SYS is restored.  If it does not, simply
echo click the "close" button in the upper right corner.
echo.
echo Thanks!
echo.
pause
if not exist %1\config.jps goto nobackup
copy %1\CONFIG.JPS C:\CONFIG.SYS
echo.
echo CONFIG.SYS restored from pre-install backup.
goto done
:nobackup
echo.
echo.
echo The backup file for CONFIG.SYS is missing.  You will need
echo to edit CONFIG.SYS manually to remove the SHELL= line for
echo 4DOS.
echo.
pause
:done
exit

