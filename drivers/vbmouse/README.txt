
This driver is from the following site, credit to Javier Pedro:

  <https://git.javispedro.com/cgit/vbmouse.git/about/>

This is a mouse driver for Windows 3.x with VirtualBox mouse integration
support.

I have tested it with Windows 3.0 in real and 386 enhanced modes, Windows 3.11
in 386 enhanced mode with paging on, as well as Windows 95 (Windows 9x can use
16-bit mouse drivers).

In the Windows Setup program (accessible either via SETUP.EXE on an installed
Windows or via the corresponding icon in Program Manager), go to Options ->
Change system configuration -> Mouse -> Select "Other mouse..." -> Search in
"A:" -> "VirtualBox PS/2 Mouse". Select the "VirtualBox PS/2 Mouse" in the
Mouse section again.

Alternatively, you can copy vbmouse.drv to your WINDOWS\SYSTEM directory and
edit WINDOWS\SYSTEM.INI 's mouse.drv line to point to it, e.g.

  [boot]
  mouse.drv = vbmouse.drv

This later option also works with Windows 9x.

