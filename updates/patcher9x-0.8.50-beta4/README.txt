Patch for Windows 9x for run on newest CPUs - AMD ZEN 2+, Intel Tiger Lake+
Version: 0.8.50-BETA4

Usage:
patcher9x.exe [options] [path]
  or
patcher9x.exe [options] <batch function> <batch arguments>
path: path to installed windows directory or directory with windows instalation
options:
	-h,/?: print this help
	-v: print program version
	-auto,-y: use default actions (if path given) and don't bother user
	-no-backup: don't backup files before modify
	-force-w3: when patching VMM32.VXD, leave it as W3 file
	-force-w4: when patching VMM32.VXD, always compress to W4 file
	-force-tlb: always try to apply TLB patch set
	-force-cpupatch: always try to apply CPU SPEED patch set
	-force-cpupatch-ndis: always try to apply CPU SPEED patch for NDIS.VXD
	-no-tlb: do not apply TLB patches
	-no-cpupatch: do not apply CPU SPEED patches
	-no-cpupatch-ndis: do not apply CPU SPEED patches for NDIS.VXD
	-millennium: ignored, Me patch is included in TLB set

When running without arguments, program operate in interactive mode (ask user)

Batch mode functions:
	--cab-list                archive.cab
	--cab-extract             archive.cab file1 [file2 [...]]
	--cabs-extract            dir-to-search file1 [file2 [...]]
	--vxd-list                archive.vxd
	--vxd-extract             archive.vxd file1 [file2 [...]]
	--vxd-convert             archive.vxd
	--vxd-extract-all         archive.vxd [destination-dir]
	--patch-all               file.vxd [file2.vxd [...]]
	--patch-tlb               file.vxd [file2.vxd [...]]
	--patch-cpuspeed          file.vxd [file2.vxd [...]]
	--patch-cpuspeed-ndis     file.vxd [file2.vxd [...]]
