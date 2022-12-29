@ECHO OFF
SET REGSVR32=%WINDIR%\SYSTEM\REGSVR32.EXE

ECHO Please make sure to have *SUCCESSFULLY* installed VCREDIST.EXE (and rebooted) or else this installation may fail!

%REGSVR32% VDMConfig.dll

%REGSVR32% /s DiskWriter.dll
%REGSVR32% /s DMAController.dll
%REGSVR32% /s DSoundDevice.dll
%REGSVR32% /s EmuAdLib.dll
%REGSVR32% /s EmuJoystick.dll
%REGSVR32% /s EmuMPU401.dll
%REGSVR32% /s EmuPPDAC.dll
%REGSVR32% /s EmuSBCompat.dll
%REGSVR32% /s MIDIDevice.dll
%REGSVR32% /s MIDIIndicator.dll
%REGSVR32% /s MIDIToolkit.dll
%REGSVR32% /s VDDLoader.dll
%REGSVR32% /s WaveDevice.dll

ECHO Installation complete.
ECHO !!! IMPORTANT !!! IMPORTANT !!! IMPORTANT !!! IMPORTANT !!! IMPORTANT !!!
ECHO Please edit C:\AUTOEXEC.BAT and add the following line:
ECHO   SET VDMSPATH=C:\VDMSOUND
ECHO ...where C:\VDMSOUND is the directory where VDMSound was installed (i.e. the current directory)
