
These utilities are from Javier Pedro's website:

  <https://git.javispedro.com/cgit/vbados.git/about/>

VBADOS is a set of Terminate-and-Stay-Resident (TSR) utilities to be used
inside MS-DOS virtual machines (and Windows 3.x), and help to provide closer
integration between the guest and the host.

There are three utilities right now:

VBMOUSE.EXE is a DOS mouse driver for VirtualBox/VMware emulated mouse, but
also real PS/2 mice. It allows for seamless host/guest mouse integration, and
scroll wheel support.

VBMOUSE.DRV is a Windows 3.x mouse driver that depends on VBMOUSE.EXE (or
alternatively the builtin mouse support in DOSemu/DOSBox) to provide the same
seamless mouse integration and wheel support within Windows.  VBSF.EXE is a
Shared Folders driver for VirtualBox (not VMware yet).  It allows to map
directories from the host OS into drive letters in the MS-DOS guest.

Note: this project has absolutely no official relation with either VirtualBox
or Visual Basic. The VB stands for "Very Basic" :)


