README

By Ross Ridge
Public Domain

@(#) MySLOW README 1.1 01/06/24 22:59:54


MySLOW is a public domain utility for slowing down MS-DOS programmes run
under Window 95, Windows 98 or Window ME.  MySLOW is currently in the early
stages of beta testing.  This utility is only recommended for Pentium or
faster CPUs.  It consists of two main components, a Windows Virtual Device
Driver (VxD) that implements the slowing down of programmes, and a DLL that
provides a new PIF property sheet page through which the the user can
configure the slow down settings on a per application basis.  The MySLOW VxD
implements basic and advanced slow down techniques which allow it have a
great range of effective CPU speeds (from 100% to 0.01%) without making the
game or application choppy or unresponsive.  Settings can be an accessed in
a familiar and non-intrusive manner using MySLOW's PIF property sheet
extension.  This allows settings to be stored in the MS-DOS application's
PIF file and automatically used whenever the application is run.


HOW TO INSTALL MYSLOW

To install MySLOW, just unpack the distribution .ZIP file in a new
directory.  In addition to this README file, the distribution contains three
files, MYSLOW.VXD, the MySLOW virtual device driver (VxD), SLOWPIF.DLL, the
PIF extension DLL, and MYSLOW.EXE, a "launch" programme used to load and
unload the VxD and DLL.  To have MySLOW automatically loaded everytime your
computer starts, create a shortcut to the MYSLOW.EXE in the StartUp folder
in the Programs menu (Start -> Programs -> StartUp).  Don't copy the
MYSLOW.EXE file to the StartUp folder, MYSLOW.EXE must be in the same
directory (folder) as MYSLOW.VXD and SLOWPIF.DLL. 

To uninstall MySLOW, make sure the VxD and DLL are unloaded, and just delete
the directory you unpacked the files into.  Delete any shortcuts to
MYSLOW.EXE you created.


HOW TO USE MYSLOW

Load the MySLOW VxD and DLL by running MYSLOW.EXE.  Right click on the PIF
file used to launch the game or application you want to slow down and select
"Properties" from the menu.  Click on the "MySLOW" tab, which will bring up
the MySLOW property sheet page.  From this window you can change all the
MySLOW settings for the application.  The "Enable MySLOW" checkbox on this
page acts as a master on/off switch, if it's not enabled then MySLOW is
disabled and the application will run at full speed.  Clicking this checkbox
will both enable MySLOW for this application and the the rest of the
controls on this page.  You can change the settings for an already running
MS-DOS application by using selecting properties in the system menu of its
window (press ALT-SPACE to access the system menu in full screen
applications). 


BASIC SETTINGS

MySLOW has a fair number of settings that can be changed to both change the
ammount of slow down that MySLOW provides as well controlling advanced
features that can be used to eliminate jerkiness if necessary.  Currently
the PIF controls provide direct control over the parameters that MySLOW uses
to slow down progammess.  While this gives the user as complete control as
possible, it means the interface isn't as simple or easy to use as it could
be.  In normal operation MySLOW simply adjusts how Windows schedules the
MS-DOS application.  Windows normally will give the application nearly 100%
of available CPU if it's the foreground task.  MySLOW can change this
behaviour and instead will schedule alternating "run" and "sleep" periods
for the application.  To use this mode of operation, select "Run" as the Run
Mode and "Sleep" as the Sleep Mode, and enter the length in millisecond (ms)
for MySLOW to schedule each period.  Changing the ratio of Run Mode time
(normally 1 ms) and Sleep Mode time will change effective CPU speed
accordingly.  MySLOW's estimate of the effect the current settings
will have on CPU speed for the application are displayed in the upper right
of the property page.  This basic mode of operation provides the highest
level application compatibility but can result in games and applications
appearing jerky and/or unresponsive. 


ADVANCED SETTINGS

The "Virtualize Timer" checkbox allows MySLOW to virtualize the hardware
timer if the application tries to use it.  Normally Windows will give full
control of the hardware timer to MS-DOS applications if they try to use it.
Unfortunately both Windows and MySLOW use the hardware timer to schedule
processes, and changing the hardware timer prevents from MySLOW from
scheduling the application with a 1 ms accuracy.  While MySLOW automatically
adjusts to this and maintains the same ratio of run and sleep times, these
times are greatly increased, often making the application choppy and
unresponsive.  Enabling the "Virtualize Timer" checked will prevent the
application from changing the real hardware timer, and MySLOW will emulate
one instead.  While the emulated hardware timer isn't as accurate as the
real hardware timer would've been, it's usually sufficient.

Selecting "Trace" as either the Run or Sleep Mode will cause MySLOW to use
the CPU's Trace feature to single step each CPU instruction in the game or
application.  Single stepping the application slows it down immensely.  How
much so depends a lot on the application being slowed down, it can be
anywhere from 3% to 0.1% of its normal speed.  Using Trace as the Run Mode
allows you to slow down the CPU to 1% or less without increasing the sleep
time so much that it causes jerkiness.  Using Trace as the Sleep Mode allows
you to keep the program running, albiet very very slowly, when MySLOW
schedules it to go to sleep.  This is useful for games that repeatly poll
for events like mouse clicks, key presses and the vertical retrace interval
that it might miss while sleeping normally.

If "No Cache" is selected as either the Run or Sleep Mode will cause MySLOW
to disable the CPU's cache when it schedules the programme to run or sleep
respective.  This has a similar affect to disabling the CPU cache from most
computers BIOS setup menu, but it only affects the MS-DOS program being
slowed, it doesn't slow down all of Windows.  Like the Trace option, the No
Cache option slows down programmes immensely, from 3% to 0.1% of normal
depending on the application (though usually by a different amount than
using Trace would).  No Cache mode can be used much like Trace mode.

Trace and No Cache mode provide much the same functionality, the slow down
programmes a lot, but they have different advantages and disavantages.
Trace mode is a lot more intelligent than No Cache mode.  Trace mode is
automatically disabled during system and time critical operations like disk
accesses and interrupt handlers.  No Cache mode however is more transparent
then Trace mode.  Programmes can't reenable the CPU cache, nor can they
detect that the CPU cache is disabled.  Programmes can however detect that
they're being single stepped and deliberately or inadvertantly disable trace
mode.  Some games deliberately crash if they detect they're being traced,
believing that a debugger is being used to try crack the game.  It's
possible that even some extreamly paraniod Windows games might think MySLOW
was a debugger trying to crack them.  In general, using Trace mode is
preferable to using No Cache mode.  In particular, if you want to use a
joystick with the game being slowed, don't use No Cache as the Sleep Mode.

Selecting "Busy Wait" as the Sleep Mode causes MySLOW to busy wait keeping the
process active while sleeping instead of putting the process to sleep.
Aside from hogging the CPU this mode has very little effect except for
making the length of the sleep period more accurate.

The "Off" Sleep Mode stops MySLOW from scheduling alternating run and sleep
periods.  MySLOW uses the Run Mode exclusively.  The Run Mode should be
either be Trace or No Cache, if it's Run then MySLOW is effectively
disabled.


STATISTICS

Clicking the "Statistics" button on the MySLOW property page brings up a
window listing various statistics for the currently running program.  This
button is only enabled if you accessed the propreties through the system
menu.  Most of the information listed isn't very useful, but it can
sometimes give insight on what are the best settings to use.

The "Average Scheduled Times" and "Average Actual Times" show how well
MySLOW has been able to schedule the application as requested.  If the
average scheduled times are a significantly different from what has been
requested its probably because the application has reprogrammed the hardware
timer (see below).

The "Trace" section show how many instructions that MySLOW has single
stepped through (it doesn't take to long for this value to wrap however),
and the first two bytes last instruction to be traced.  If Trace mode fails
to work the last instruction traced may give a clue as to why.

The "Information" section shows the total CPU time the application has
used, and what kind of application it is.  The "HW Timer" value in
this section is the most useful of all the statistics.  This value is
the period that the application has tried to set the hardware timer
to.  If it says "n/a" then the application hasn't tried to reprogram
the hardware timer.  If it gives a time that's more than a few
milliseconds, then you may need to use the "Virtualize Timer" option
to prevent jerkiness.  If the value displayed in red then the
application has tried to set a timer period of less than 1
millisecond.  Windows won't let MS-DOS applications set a period of
less than 1 millisecond and will program the timer to use a period of
1 millisecond instead.  This may or may not cause the application to
not work correctly.  In either case the "Virtualize Timer" option will
have no effect.  If the period programmed by the application is 17 ms
(60Hz) or 14 ms (70Hz) then the application may be trying to
synchronize the timer with the display's verticle retrace period.  In
this case you will probably need to set the "Virtualize Timer" option
and set the Sleep Mode to Trace in order to prevent jerkiness.

LEGAL NOTICES

The files MYSLOW.EXE and SLOWPIF.DLL contain Borland C++ 3.1 runtime
library code.  "Copyright (C) 1987, 1991, 1992 Borland International,
Inc. All Rights Reserved."

FEEDBACK

If you have any questions, comments or bug reports about MySLOW please
e-mail me at rridge@csclub.uwaterloo.ca.  Please remember that MySLOW
is in beta, that it's public domain (ie. free) and that I don't get
paid to work on or support it.


                                        Ross Ridge






