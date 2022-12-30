# TweakPCI
TweakPCI is a command line utility for DOS to view/modify PCI configuration registers. It will show the current contents of the requested register and will attempt to modify any requested bits. To know which bits do what, track down the technical reference / specification of your PCI device. TweakPCI requires the device and vendor identifiers and currently only supports configuring PCI devices through BIOS calls. (PCI BIOS v2.0c+ is required)

## Usage
```
tweakpci [switch]... device_id vendor_id [register [bit_changes...]]...
switch = -b show binary representation only
         -h show hexadecimal representation only
         -c compact output, shows final value only
device_id = hexadecimal id of the PCI device (required)
vendor_id = hexadecimal id of the PCI vendor (required)
register = size 'b'/'w'/'d' (for 8/16/32-bit) + hexdec regnr
bit_changes = index of lowest bit (decimal) + '=' + binary digits
```

## Examples
```
tweakpci -h 0496 1039 d0
tweakpci 0496 1039 b40 0=10 5=1 6=1
tweakpci -c -b 0496 1039 b40 2=010 b81 2=010
```

If you don't specify any bit changes, the program will just show the current value of the register. If you do specify any bit changes, the program will read and display the current value, apply your requested changes, show the result and write the new value to the register. Finally it will read and display the (new) value of the register.

## Example output
```
C:\>tweakpci 0496 1039 dc8 12=1010 8=1110
TweakPCI - PCI configuration tool v1.0 by TimmermanV
Tweaking device: DeviceId 0x0496, VendorId 0x1039, DeviceIdx 0x0
Device found: BusNr 0x00, DeviceNr 0x05, FunctionNr 0x0

Register 0xC8 (32-bit)
Cur value: 0x00FF0000  0000_0000 1111_1111 0000_0000 0000_0000
Set value: 0x00FFAE00  0000_0000 1111_1111 1010_1110 0000_0000
New value: 0x00FFAE00  0000_0000 1111_1111 1010_1110 0000_0000
```

## How to build
Run `MAKE.BAT` or more directly `WCL -3 -os TWEAKPCI.C INT386R.C` to build TweakPCI. Currently, this was tested on Open Watcom 1.9 only. Other versions of (Open) Watcom may work as well.

## Related software
* [PCISET](https://www.uwe-sieber.de/util_e.html) - A similar utility by Uwe Sieber, written in Pascal.
* [PCICFG](https://www.cs.cmu.edu/~ralf/files.html) - A utility by Ralf Brown that provides a detailed list of your PCI devices. (RBpci)
