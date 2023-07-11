#include <stdio.h>
#include <conio.h>
#include <string.h>
#include <fcntl.h>
#include <dos.h>

#define	MAX_BIOS_SIZE	0xE000

char rambios[MAX_BIOS_SIZE + 0x10];

//=============================================
unsigned GetfarWord(char far *p, unsigned location)
{
unsigned i;

  i = p[location] + (p[location + 1] << 8);
  return( i);
}
//=============================================
main(argc, argv)

int  argc;
char **argv;
{
unsigned busdev, chksum;
unsigned char  errcode;
char     input_v[20], *input_fp, input_fno;
unsigned PCIpointer, vendor_v, deviceid_v;

unsigned far *romfcn;
unsigned rambiosseg, biossize, runtimesize, adj;
char     far *rambios_p;
unsigned i, j;

  rambios_p = rambios;
  i = j = FP_OFF(rambios_p);
  i += 0x0F;
  i &= ~0xF;
  adj = i - j;
  i >>= 4;
  rambiosseg = FP_SEG(rambios_p) + i;
  FP_SEG(rambios_p) = rambiosseg;
  FP_OFF(rambios_p) = 0;
  biossize = MAX_BIOS_SIZE;

  sscanf(argv[1], "%s", &input_v);
  input_fp = input_v;

  input_fno = open(input_fp, O_BINARY);
  if (input_fno == -1)
  {
    cputs("Input Rom Image not found !!!!!");
    return(0);
  }
  runtimesize = biossize = read(input_fno, &rambios[adj], biossize);
  close(input_fno);

  if ((rambios_p[2] << 9) > biossize)
  {
    printf("\nMaxinu. size is 56K, cannot load");
    return(0);
  }

  chksum = 0;
  for (i = 0; i < runtimesize; ++i)
    chksum += rambios_p[i];

  if ((chksum & 0xFF) != 0)
  {
    printf("\nInvalid checksum, cannot load");
    return(0);
  }

  PCIpointer = GetfarWord(rambios_p, 0x18);

  vendor_v = GetfarWord(rambios_p, PCIpointer + 4);
  deviceid_v = GetfarWord(rambios_p, PCIpointer + 6);
  FP_SEG(romfcn) = rambiosseg;
  FP_OFF(romfcn) = 0x0003;
  errcode = 0;

  _asm
  {
      mov  dx,vendor_v
      mov  cx,deviceid_v
      mov  si,0           ;index
      mov  ah,0B1h        ;PCI_FUNCTION_ID
      mov  al,002h        ;FIND_PCI_DEVICE
      int  1ah
      mov  errcode,ah
      or   ah,ah
      jnz  ex_onerror

      mov  di,4
      mov  ah,0B1h
      mov  al,008h	;READ_CONFIG_BYTE
      int  1ah

      or   cl,03h	;enale io and memory
      mov  di,4
      mov  ah,0B1h
      mov  al,00Bh	;WRITE_CONFIG_BYTE
      int  1ah

      mov  ax,bx
    found_bus_dev:

      call romfcn	;Call SEG:0003h to BIOS init
    ex_onerror:

  }
  if (errcode) 
  {
    printf("\nInvalid device, cannot start");
    return(0);
  }

  runtimesize = rambios_p[2] << 9;

  printf("\nRAMBIOS loaded at %04X:0000", rambiosseg);

  _asm
  {
    mov  ax,cs
    mov  bx,ds
    cmp  ax,bx
    jb   def
    mov  ax,bx
  def:
    mov  bx,rambiosseg
    cmp  bx,ax
    ja   abc
    xchg bx,ax
  abc:
    sub  bx,ax
    mov  dx,biossize
    shr  dx,1
    shr  dx,1
    shr  dx,1
    shr  dx,1
    add  dx,bx
    inc  dx
    add  dx,10h

    mov  ax,3100h	;Left BIOS as DOS TSR
    int  21h
  }
}
//=============================================
