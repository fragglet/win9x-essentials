Bochs and QEMU support emulating these Cirrus Logic cards, so the drivers
are included here.

--

Since Bochs 2.2 it is possible to use Cirrus SVGA graphics. The Cirrus device
supports both ISA and PCI depending on the bochsrc settings. If PCI is
disabled or the Cirrus card is not assigned to a PCI slot, it appears as a
CL-GD5430 ISA with 2MB VRAM. If you assign the Cirrus card to a PCI slot, it
appears as a CL-GD5446 PCI with 4MB VRAM.

In order to use Cirrus SVGA, you need to compile Bochs using the
--enable-clgd54xx option and enable it in your bochsrc by setting the vga
option to cirrus. Finally, you need to use the Cirrus version of the LGPL'd
VGABIOS as vgaromimage option for applications to correctly detect Cirrus
support.

  # Enable CL-GD5446 PCI
  vga: extension=cirrus
  vgaromimage: file=$BXSHARE/VGABIOS-lgpl-latest-cirrus
  pci: enabled=1, chipset=i440fx, slot1=cirrus
  
Note: The VGABIOS is already included in the Bochs release, so no separate
download is necessary.

