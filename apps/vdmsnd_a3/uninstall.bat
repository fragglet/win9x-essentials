@ECHO OFF
SET REGSVR32=%WINDIR%\SYSTEM\REGSVR32.EXE

%REGSVR32% /s /u DiskWriter.dll
%REGSVR32% /s /u DMAController.dll
%REGSVR32% /s /u DSoundDevice.dll
%REGSVR32% /s /u EmuAdLib.dll
%REGSVR32% /s /u EmuJoystick.dll
%REGSVR32% /s /u EmuMPU401.dll
%REGSVR32% /s /u EmuPPDAC.dll
%REGSVR32% /s /u EmuSBCompat.dll
%REGSVR32% /s /u MIDIDevice.dll
%REGSVR32% /s /u MIDIIndicator.dll
%REGSVR32% /s /u MIDIToolkit.dll
%REGSVR32% /s /u VDDLoader.dll
%REGSVR32% /s /u VDMConfig.dll
%REGSVR32% /s /u WaveDevice.dll

ECHO Done uninstalling.
