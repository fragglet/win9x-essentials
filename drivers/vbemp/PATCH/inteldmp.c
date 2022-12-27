#ifdef _MSC_VER
#include "windows.h"
#else
#define TRUE 0
#define FALSE -1
#endif
#include <stdio.h>
#include <memory.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

// compile: cl /Ox /Zp1 aaaa.c

#define ATI_SIGNATURE1 "ATI MOBILITY RADEON"
#define ATI_SIGNATURE2 "ATI Technologies Inc"
#define NVIDIA_SIGNATURE "NVIDIA Corp"
#define INTEL_SIGNATURE "Intel Corp"

#define NEW(a) ((a *)(calloc(1, sizeof(a))))
#define FREE(a) (free(a))

#define VBIOS_START         0xc0000
#define VBIOS_SIZE          0x10000

typedef unsigned char * address;
typedef unsigned char byte;
typedef unsigned short word;
typedef unsigned char boolean;
typedef unsigned int cardinal;

typedef enum {
    CT_UNKWN, CT_830, CT_845G, CT_855GM, CT_865G, CT_915G, CT_915GM, CT_945G, CT_945GM,
    CT_946GZ, CT_G965, CT_Q965, CT_965GM, CT_G33, CT_Q33, CT_Q35, CT_Cedarview, CT_500GMA,
    CT_GM45, CT_GMA3150, CT_HDGraphics, CT_HD3000, CT_HD4000
} chipset_type;

char * chipset_type_names[] = {
    "UNKNOWN", "830",  "845G", "855GM", "865G", "915G", "915GM", "945G", "945GM",
    "946GZ",   "G965", "Q965", "965GM", "G33", "Q33", "Q35", "Cedarview", "500GMA",
    "GM45", "GMA3150", "HDGraphics", "HD3000", "HD4000"
};

typedef enum {
    BT_UNKWN, BT_1, BT_2, BT_3, BT_0
} bios_type;

char * bios_type_names[] = {"UNKNOWN", "TYPE 1", "TYPE 2", "TYPE 3", "TYPE 0"};

int freqs[] = { 60, 75, 85 };

typedef struct {
    byte mode;
    byte bits_per_pixel;
    word resolution;
    byte unknown; 
} vbios_mode, *pvbios_mode;

typedef struct {
    word clock;		/* Clock frequency in 10 kHz */
    byte x1;
    byte x_total;
    byte x2;
    byte y1;
    byte y_total;
    byte y2;
    byte unk0; // H.Front porch
    byte unk1; // H.Sync pulse
}  vbios_resolution_type0; // HD Series

typedef struct {
    word clock;		/* Clock frequency in 10 kHz */
    byte x1;
    byte x_total;
    byte x2;
    byte y1;
    byte y_total;
    byte y2;
}  vbios_resolution_type1;

typedef struct {
    unsigned long clock;

    word x1;
    word htotal;
    word x2;
    word hblank;
    word hsyncstart;
    word hsyncend;

    word y1;
    word vtotal;
    word y2;
    word vblank;
    word vsyncstart;
    word vsyncend;
}  vbios_modeline_type2;

typedef struct {
    byte xchars;
    byte ychars;
    byte unknown[4];

    vbios_modeline_type2 modelines[];
} vbios_resolution_type2;

typedef struct {
    unsigned long clock;

    word x1;
    word htotal;
    word x2;
    word hblank;
    word hsyncstart;
    word hsyncend;

    word y1;
    word vtotal;
    word y2;
    word vblank;
    word vsyncstart;
    word vsyncend;

    word timing_h;
    word timing_v;

    byte unknown[6];
} vbios_modeline_type3;

typedef struct {
    unsigned char unknown[6];

    vbios_modeline_type3 modelines[];
} vbios_resolution_type3;

typedef struct {
    cardinal chipset_id;
    chipset_type chipset;
    bios_type bios;
    
    int bios_fd;
    address bios_ptr;

    vbios_mode * mode_table;
    cardinal mode_table_size;

    byte b1, b2;

    boolean unlocked;
} vbios_map;
/*
cardinal get_chipset_id(void) {
    outl(0x80000000, 0xcf8);
    return inl(0xcfc);
} */

chipset_type get_chipset(cardinal id) {
    chipset_type type;

    //printf("0x%08X",id);

    switch (id) {
    case 0x35758086:
        type = CT_830;
        break;

    case 0x25608086:
        type = CT_845G;
        break;
        
    case 0x35808086:
        type = CT_855GM;
        break;
        
    case 0x25708086:
        type = CT_865G;
        break;

    case 0x25808086:
	type = CT_915G;
	break;

    case 0x25908086:
        type = CT_915GM;
        break;

    case 0x27708086:
        type = CT_945G;
        break;

    case 0x27a08086:
        type = CT_945GM;
        break;

    case 0x29708086:
        type = CT_946GZ;
        break;

    case 0x29a08086:
	type = CT_G965;
	break;

    case 0x29908086:
        type = CT_Q965;
        break;

    case 0x2a008086:
        type = CT_965GM;
        break;

    case 0x29c08086:
        type = CT_G33;
        break;

    case 0x29b08086:
        type = CT_Q35;
        break;

    case 0x29d08086:
        type = CT_Q33;
        break;

    case 0xbf38086:
        type = CT_Cedarview;
        break;    

    case 0xa0008086:
        type = CT_GMA3150;
        break;

    case 0xa0108086:
        type = CT_GMA3150;
        break;

    case 0x2a408086:
        type = CT_GM45;
        break;

    case 0x2a018086:
        type = CT_965GM;
        break;

    case 0x2a028086:
        type = CT_965GM;
        break;

    case 0x00408086:
    case 0x00448086:
    case 0x00488086:
    case 0x00698086:
        type = CT_HDGraphics;
        break;

    case 0x01008086:
    case 0x01048086:
    case 0x01088086:
    case 0x010C8086:
        type = CT_HD3000;
        break;

    case 0x01508086:
    case 0x01548086:
    case 0x01588086:
    case 0x015C8086:
        type = CT_HD4000;
        break;

    default:
        type = CT_UNKWN;
        break;
    }

    return type;
}

vbios_resolution_type0 * map_type0_resolution(vbios_map * map, word res) {
    vbios_resolution_type0 * ptr = ((vbios_resolution_type0*)(map->bios_ptr + res)); 
    return ptr;
}

vbios_resolution_type1 * map_type1_resolution(vbios_map * map, word res) {
    vbios_resolution_type1 * ptr = ((vbios_resolution_type1*)(map->bios_ptr + res)); 
    return ptr;
}

vbios_resolution_type2 * map_type2_resolution(vbios_map * map, word res) {
    vbios_resolution_type2 * ptr = ((vbios_resolution_type2*)(map->bios_ptr + res)); 
    return ptr;
}

vbios_resolution_type3 * map_type3_resolution(vbios_map * map, word res) {
    vbios_resolution_type3 * ptr = ((vbios_resolution_type3*)(map->bios_ptr + res)); 
    return ptr;
}

boolean detect_bios_type(vbios_map * map, boolean modeline, int entry_size) {
    int i;
    short r1, r2;
    float f;
    
    r1 = r2 = 32000;

    for (i=0; i < map->mode_table_size; i++) {
        if (map->mode_table[i].resolution <= r1) {
            r1 = map->mode_table[i].resolution;
    	}
        else {
            if (map->mode_table[i].resolution <= r2) {
            	r2 = map->mode_table[i].resolution;
            }
    	}

        //printf("r1 = %d  r2 = %d\n", r1, r2);
    }

    
    f = ((float) (r2-r1-6)) / entry_size;

    return f == (int) f;
}

/*

void unlock_vbios(vbios_map * map) {

    assert(!map->unlocked);
        
    map->unlocked = TRUE;
    
    switch (map->chipset) {
    case CT_UNKWN:
        break;
    case CT_830:
    case CT_855GM:
        outl(0x8000005a, 0xcf8);
        map->b1 = inb(0xcfe);
        
        outl(0x8000005a, 0xcf8);
        outb(0x33, 0xcfe);
        break;
    case CT_845G:
    case CT_865G:
    case CT_915G:
    case CT_915GM:
    case CT_945G:
    case CT_945GM:
    case CT_946GZ:
    case CT_G965:
    case CT_Q965:
    case CT_965GM:
    case CT_G33:
    case CT_Q35:
    case CT_Q33:
    case CT_Cedarview:
    case CT_500GMA:
    case CT_GM45:
    case CT_GMA3150:
        outl(0x80000090, 0xcf8);
        map->b1 = inb(0xcfd);
        map->b2 = inb(0xcfe);
        
        outl(0x80000090, 0xcf8);
        outb(0x33, 0xcfd);
        outb(0x33, 0xcfe);
        break;
    case CT_HDGraphics:
    case CT_HD3000:
    case CT_HD4000:
        outl(0x80000080, 0xcf8);
        map->b1 = inb(0xcfd);
        map->b2 = inb(0xcfe);
        
        outl(0x80000080, 0xcf8);
        outb(0x33, 0xcfd);
        outb(0x33, 0xcfe);
        break;
    }

#if DEBUG
    {
        cardinal t = inl(0xcfc);
        printf("unlock PAM: (0x%08x)\n", t);
    }
#endif
}

void relock_vbios(vbios_map * map) {

    assert(map->unlocked);
    map->unlocked = FALSE;
    
    switch (map->chipset) {
    case CT_UNKWN:
        break;
    case CT_830:
    case CT_855GM:
        outl(0x8000005a, 0xcf8);
        outb(map->b1, 0xcfe);
        break;
    case CT_845G:
    case CT_865G:
    case CT_915G:
    case CT_915GM:
    case CT_945G:
    case CT_945GM:
    case CT_946GZ:
    case CT_G965:
    case CT_Q965:
    case CT_965GM:
    case CT_G33:
    case CT_Q35:
    case CT_Q33:
    case CT_Cedarview:
    case CT_500GMA:
    case CT_GM45:
    case CT_GMA3150:
        outl(0x80000090, 0xcf8);
        outb(map->b1, 0xcfd);
        outb(map->b2, 0xcfe);
        break;
    case CT_HDGraphics:
    case CT_HD3000:
    case CT_HD4000:
        outl(0x80000080, 0xcf8);
        outb(map->b1, 0xcfd);
        outb(map->b2, 0xcfe);
        break;
    }

#if DEBUG
    {
        cardinal t = inl(0xcfc);
        printf("relock PAM: (0x%08x)\n", t);
    }
#endif
}

*/

void list_modes(vbios_map *map, cardinal raw) {
    cardinal i, x, y;

    for (i=0; i < map->mode_table_size; i++) {
        switch(map->bios) {
        case BT_0:
            {
                vbios_resolution_type0 * res = map_type0_resolution(map, map->mode_table[i].resolution);
                printf("Offset $%x, ",((cardinal)res) - ((cardinal)map->bios_ptr));
                
                x = ((((cardinal) res->x2) & 0xf0) << 4) | res->x1;
                y = ((((cardinal) res->y2) & 0xf0) << 4) | res->y1;

                if (x != 0 && y != 0) {
                    printf("Mode %02x : %dx%d, %d bits/pixel\n", map->mode_table[i].mode, x, y, map->mode_table[i].bits_per_pixel);
                } else printf("\n");

		if (raw)
		{
                    printf("Mode %02x (raw) :\n\t%04x\n\t%02x\n\t%02x\n\t%02x\n\t%02x\n\t%02x\n\t%02x\n\t%02x\n\t%02x\n", map->mode_table[i].mode, res->clock, res->x1,res->x_total,res->x2,res->y1,res->y_total,res->y2,res->unk0,res->unk1);
		}

            }
            break;
        case BT_1:
            {
                vbios_resolution_type1 * res = map_type1_resolution(map, map->mode_table[i].resolution);
                printf("Offset $%x, ",((cardinal)res) - ((cardinal)map->bios_ptr));
                
                x = ((((cardinal) res->x2) & 0xf0) << 4) | res->x1;
                y = ((((cardinal) res->y2) & 0xf0) << 4) | res->y1;

                if (x != 0 && y != 0) {
                    printf("Mode %02x : %dx%d, %d bits/pixel\n", map->mode_table[i].mode, x, y, map->mode_table[i].bits_per_pixel);
                } else printf("\n");

		if (raw)
		{
                    printf("Mode %02x (raw) :\n\t%04x\n\t%02x\n\t%02x\n\t%02x\n\t%02x\n\t%02x\n\t%02x\n", map->mode_table[i].mode, res->clock, res->x1,res->x_total,res->x2,res->y1,res->y_total,res->y2);
		}

            }
            break;
        case BT_2:
            {
                vbios_resolution_type2 * res = map_type2_resolution(map, map->mode_table[i].resolution);
                printf("Offset $%x, ",((cardinal)res) - ((cardinal)map->bios_ptr));
                
                x = res->modelines[0].x1+1;
                y = res->modelines[0].y1+1;

                if (x != 0 && y != 0) {
                    printf("Mode %02x : %dx%d, %d bits/pixel\n", map->mode_table[i].mode, x, y, map->mode_table[i].bits_per_pixel);
                } else printf("\n");
            }
            break;
        case BT_3:
            {
                vbios_resolution_type3 * res = map_type3_resolution(map, map->mode_table[i].resolution);
                printf("Offset $%x, ",((cardinal)res) - ((cardinal)map->bios_ptr));
                
                x = res->modelines[0].x1+1;
                y = res->modelines[0].y1+1;

                if (x != 0 && y != 0) {
                    printf("Mode %02x : %dx%d, %d bits/pixel\n", map->mode_table[i].mode, x, y, map->mode_table[i].bits_per_pixel);
                } else printf("\n");
            }
            break;
        case BT_UNKWN:
            break;
        }
    }
}

static void gtf_timings(int x, int y, int freq,
        unsigned long *clock,
        word *hsyncstart, word *hsyncend, word *hblank,
        word *vsyncstart, word *vsyncend, word *vblank)
{
    int hbl, vbl, vfreq;

    vbl = y + (y+1)/(20000.0/(11*freq) - 1) + 1.5;
    vfreq = vbl * freq;
    hbl = 16 * (int)(x * (30.0 - 300000.0 / vfreq) /
            (70.0 + 300000.0 / vfreq) / 16.0 + 0.5);

    *vsyncstart = y;
    *vsyncend = y + 3;
    *vblank = vbl - 1;
    *hsyncstart = x + hbl / 2 - (x + hbl + 50) / 100 * 8 - 1;
    *hsyncend = x + hbl / 2 - 1;
    *hblank = x + hbl - 1;
    *clock = (x + hbl) * vfreq / 1000;
}

static void cvt_timings(int x, int y, int freq,
        unsigned long *clock,
        word *hsyncstart, word *hsyncend, word *hblank,
        word *vsyncstart, word *vsyncend, word *vblank, boolean reduced)
{
	int hbl, hbp, vbl, vsync, hperiod;
	
	if (!(y % 3) && ((y * 4 / 3) == x))
        vsync = 4;
    else if (!(y % 9) && ((y * 16 / 9) == x))
        vsync = 5;
    else if (!(y % 10) && ((y * 16 / 10) == x))
        vsync = 6;
    else if (!(y % 4) && ((y * 5 / 4) == x))
        vsync = 7;
    else if (!(y % 9) && ((y * 15 / 9) == x))
        vsync = 7;
    else /* Custom */
        vsync = 10;
	
	if (!reduced) {
		hperiod = (1000000/freq - 550) / (y + 3);
		vbl = y + (550/hperiod) + 3;
		hbp = 30 - ((300*hperiod)/1000);
		hbl = (x * hbp) / (100 - hbp);
		
		*vsyncstart = y + 6;
		*vsyncend = *vsyncstart + vsync;
		*vblank = vbl - 1;	
		*hsyncstart = x + hbl / 2 - (x + hbl + 50) / 100 * 8 - 1;	
		*hsyncend = x + hbl / 2 - 1;	
		*hblank = x + hbl - 1;
		
	} else {
		hperiod = (1000000/freq - 460) / y;
		vbl = y + 460/hperiod + 1;
		hbl = 160;
		
		*vsyncstart = y + 3;
		*vsyncend = *vsyncstart + vsync;
		*vblank = vbl - 1;	
		*hsyncstart = x + hbl / 2 - 32;	
		*hsyncend = x + hbl / 2 - 1;	
		*hblank = x + hbl - 1;
		
	}
	*clock = (x + hbl) * 1000 / hperiod;
}

int rint(float x)
{
return ((int)(x < 0 ? x - 0.5f : x + 0.5f));
};

void set_mode(vbios_map * map, cardinal mode, cardinal x, cardinal y, cardinal bp, cardinal htotal, cardinal vtotal) {
    int xprev, yprev;
    cardinal i, j;

    for (i=0; i < map->mode_table_size; i++) {
        if (map->mode_table[i].mode == mode) {
            switch(map->bios) {
            case BT_0:
                {
                    vbios_resolution_type0 * res = map_type0_resolution(map, map->mode_table[i].resolution);
		    unsigned long clock;
		    word hsyncstart, hsyncend, hblank;
		    word vsyncstart, vsyncend, vblank;
                    
                    if (bp) {
                        map->mode_table[i].bits_per_pixel = bp;
                    }

		    gtf_timings(x, y, freqs[0], &clock,
				&hsyncstart, &hsyncend, &hblank,
				&vsyncstart, &vsyncend, &vblank);
		    
		    //printf("x = %d, y = %d, clock = %lu, h = %d %d %d, v = %d %d %d\n",
			  //x, y, clock,
			  //hsyncstart, hsyncend, hblank,
			  //vsyncstart, vsyncend, vblank);

		    htotal = htotal ? htotal : (unsigned int)hblank+1;
		    vtotal = vtotal ? vtotal : (unsigned int)vblank+1;

		    res->clock = clock/10; /* Units appear to be 10 kHz */
                    res->x2 = (((htotal-x) >> 8) & 0x0f) | ((x >> 4) & 0xf0);
                    res->x1 = (x & 0xff);
                    
                    res->y2 = (((vtotal-y) >> 8) & 0x0f) | ((y >> 4) & 0xf0);
                    res->y1 = (y & 0xff);
		    if (htotal)
			res->x_total = ((htotal-x) & 0xff);

		    if (vtotal)
			res->y_total = ((vtotal-y) & 0xff);
	    //printf("u0 = %d, u1 = %d (before)\n", res->unk0, res->unk1);
	    //res->unk1 = rint(8.0f/100.0f * htotal / 8); // H.Sync pulse
  	  //res->unk0 = (hblank / 2.0f) - res->unk1; // H.Front porch
	    //printf("u0 = %d, u1 = %d\n", res->unk0, res->unk1);
                }
                break; 
            case BT_1:
                {
                    vbios_resolution_type1 * res = map_type1_resolution(map, map->mode_table[i].resolution);
		    unsigned long clock;
		    word hsyncstart, hsyncend, hblank;
		    word vsyncstart, vsyncend, vblank;
                    
                    if (bp) {
                        map->mode_table[i].bits_per_pixel = bp;
                    }

		    gtf_timings(x, y, freqs[0], &clock,
				&hsyncstart, &hsyncend, &hblank,
				&vsyncstart, &vsyncend, &vblank);
		    
		    //printf("x = %d, y = %d, clock = %lu, h = %d %d %d, v = %d %d %d\n",
			  //x, y, clock,
			  //hsyncstart, hsyncend, hblank,
			  //vsyncstart, vsyncend, vblank);

		    htotal = htotal ? htotal : (unsigned int)hblank+1;
		    vtotal = vtotal ? vtotal : (unsigned int)vblank+1;

		    res->clock = clock/10; /* Units appear to be 10 kHz */
                    res->x2 = (((htotal-x) >> 8) & 0x0f) | ((x >> 4) & 0xf0);
                    res->x1 = (x & 0xff);
                    
                    res->y2 = (((vtotal-y) >> 8) & 0x0f) | ((y >> 4) & 0xf0);
                    res->y1 = (y & 0xff);
		    if (htotal)
			res->x_total = ((htotal-x) & 0xff);

		    if (vtotal)
			res->y_total = ((vtotal-y) & 0xff);
                }
                break; 
           case BT_2:
                {
                    vbios_resolution_type2 * res = map_type2_resolution(map, map->mode_table[i].resolution);

                    res->xchars = x / 8;
                    res->ychars = y / 16 - 1;
                    xprev = res->modelines[0].x1;
                    yprev = res->modelines[0].y1;

                    for(j=0; j < 3; j++) {
                        vbios_modeline_type2 * modeline = &res->modelines[j];
                        
                        if (modeline->x1 == xprev && modeline->y1 == yprev) {
                            modeline->x1 = modeline->x2 = x-1;
                            modeline->y1 = modeline->y2 = y-1;

                            gtf_timings(x, y, freqs[j], &modeline->clock,
                                    &modeline->hsyncstart, &modeline->hsyncend,
                                    &modeline->hblank, &modeline->vsyncstart,
                                    &modeline->vsyncend, &modeline->vblank);

                            if (htotal)
                                modeline->htotal = htotal;
                            else
                                modeline->htotal = modeline->hblank;

                            if (vtotal)
                                modeline->vtotal = vtotal;
                            else
                                modeline->vtotal = modeline->vblank;
                        }
                    }
                }
                break;
            case BT_3:
                {
                    vbios_resolution_type3 * res = map_type3_resolution(map, map->mode_table[i].resolution);
                    
                    xprev = res->modelines[0].x1;
                    yprev = res->modelines[0].y1;

                    for (j=0; j < 3; j++) {
                        vbios_modeline_type3 * modeline = &res->modelines[j];
                        
                        if (modeline->x1 == xprev && modeline->y1 == yprev) {
                            modeline->x1 = modeline->x2 = x-1;
                            modeline->y1 = modeline->y2 = y-1;
                            
                            gtf_timings(x, y, freqs[j], &modeline->clock,
                                    &modeline->hsyncstart, &modeline->hsyncend,
                                    &modeline->hblank, &modeline->vsyncstart,
                                    &modeline->vsyncend, &modeline->vblank);
                            if (htotal)
                                modeline->htotal = htotal;
                            else
                                modeline->htotal = modeline->hblank;
                            if (vtotal)
                                modeline->vtotal = vtotal;
                            else
                                modeline->vtotal = modeline->vblank;

                            modeline->timing_h   = y-1;
                            modeline->timing_v   = x-1;
                        }
                    }
                }
                break;
            case BT_UNKWN:
                break;
            }
        }
    }
}   

void display_map_info(vbios_map * map) {
    printf("Chipset: %s\n", chipset_type_names[map->chipset]);
    printf("BIOS: %s\n", bios_type_names[map->bios]);

    printf("Mode Table Offset: $C0000 + $%x\n", ((cardinal)map->mode_table) - ((cardinal)map->bios_ptr));
    printf("Mode Table Entries: %u\n", map->mode_table_size);
}

void usage(char *name) {
    printf("Usage: %s <inputfile> [outputfile] [mode X Y] [bits/pixel] [htotal] [vtotal]\n", name);
    printf("  Set the resolution to XxY for a video mode\n");
    printf("  Bits per pixel are optional.  htotal/vtotal settings are additionally optional.\n");
    printf("  Options:\n");
    printf("    mode       BIOS mode number, MUST replace existing mode!\n");
    printf("    inputfile  source BIOS binary file, bin or exe\n");
    printf("    outputfile target BIOS binary file, bin or exe, optional\n");
}

int main(int argc, char* argv[])
{
	unsigned long bhead, readcnt, size, size2, s, offset = 0;
	address p, limit;
	vbios_map * map;
	FILE * fIn, * fOut;
  unsigned long* sign;
	char vbios[65536], fn[256], raw = 0;
	
	printf("Intel(r) VBIOS DUMP/PATCH Utility, build 20140214 (based on 915resolution)\n");
	if (argc < 2) 
	{ 
   	usage(argv[0]); return 0; 
	} 
	strcpy((char*)fn,argv[1]);
  printf("Processing: %s\n",fn);
					fIn = fopen(fn,"rb");
  			if (fIn)
  			{
					fseek(fIn, 0L, SEEK_END);
					size2 = ftell(fIn);
          printf("File size: %d (0x%X) bytes\n", size2, size2);
					fseek(fIn, 0x00, SEEK_SET);
					fread(&bhead,4,1,fIn);
					if ((bhead & 0xFFFF) == 0x5A4D) { // 'MZ' header
          for (s = 0; s < size2; s++)
          {
          fseek(fIn, s, SEEK_SET); fread(&bhead,4,1,fIn);
          if ((bhead & 0xFFFF) == 0xAA55) {
          printf("Signature 0xAA55 found at: %d (0x%X) bytes\n", s, s);
          offset = s;
          break; }
          }
          }
					if ((bhead & 0xFFFF) != 0xAA55) {
        	printf("Signature 0xAA55 not found: this file is not a BIOS image!\n"); return 0; }
					size = (bhead & 0x00FF0000) >> 7;
          printf("BIOS signature: 0x%02X, size: %d (0x%X) bytes\n",(unsigned short)bhead,
          size,size);
					fseek(fIn, 0x00, SEEK_SET);
					fread(&vbios[0],size2,1,fIn);
					close(fIn);
// *****************************************
    /*
     * Figure out where the mode table is 
     */
         map = NEW(vbios_map);

         //map->chipset_id = get_chipset_id();
         map->chipset = get_chipset(map->chipset_id);
 
         map->bios_ptr = (char *)(vbios + offset);

        p = map->bios_ptr + 16;
        limit = map->bios_ptr + VBIOS_SIZE - (3 * sizeof(vbios_mode));
        while (p < limit && map->mode_table == 0) {
            sign = (unsigned long *) p;
            if (*sign == 0x65746E49) {  // "Inte"
                printf("%s\n",(char *)p); break;}
            p++;
        }

        p = map->bios_ptr + 16;
        limit = map->bios_ptr + VBIOS_SIZE - (3 * sizeof(vbios_mode));
        while (p < limit && map->mode_table == 0) {
            vbios_mode * mode_ptr = (vbios_mode *) p;
            if (((mode_ptr[0].mode & 0xf0) == 0x30) && ((mode_ptr[1].mode & 0xf0) == 0x30) &&
                ((mode_ptr[2].mode & 0xf0) == 0x30) && ((mode_ptr[3].mode & 0xf0) == 0x30)) {

                map->mode_table = mode_ptr; 
								//printf("Found0: %x %x %x\n",mode_ptr[0].mode,mode_ptr[0].bits_per_pixel,mode_ptr[0].resolution);
            }
            p++;
        }

        if (map->mode_table == 0) {
            printf("Unable to locate the mode table.\n");
            printf("Please run the program 'dump_bios' as root and\n");
            printf("email the file 'vbios.dmp' to stomljen@yahoo.com.\n");
            
            printf("Chipset: %s\n", chipset_type_names[map->chipset]);
            //exit(2);
        }
       //return 0;
    /*
     * Determine size of mode table
     */
    
    {
        vbios_mode * mode_ptr = map->mode_table;
        
        while (mode_ptr->mode != 0xff) {
            map->mode_table_size++;
            mode_ptr++;
        }
    }

    /*
     * Figure out what type of bios we have
     *  order of detection is important
     */

    if (detect_bios_type(map, TRUE, sizeof(vbios_modeline_type3))) {
        map->bios = BT_3;
    }
    else if (detect_bios_type(map, TRUE, sizeof(vbios_modeline_type2))) {
        map->bios = BT_2;
    }
    else if (detect_bios_type(map, FALSE, sizeof(vbios_resolution_type1))) {
        map->bios = BT_1;
    }
    else if (detect_bios_type(map, FALSE, sizeof(vbios_resolution_type0))) {
        map->bios = BT_0;
    }
    else {
        printf("Unable to determine bios type.\n");
        printf("Please run the program 'dump_bios' as root and\n");
        printf("email the file 'vbios.dmp' to stomljen@yahoo.com.\n");
        printf("Chipset: %s\n", chipset_type_names[map->chipset]);
        printf("Mode Table Offset: $C0000 + $%x\n", ((cardinal)map->mode_table) - ((cardinal)map->bios_ptr));
        printf("Mode Table Entries: %u\n", map->mode_table_size);
        // exit(2);
       	return(0); // bw
    }
    display_map_info(map);
    raw = 0; list_modes(map, raw);
// ******************************************
				if (argc > 2)
				{
    //unlock_vbios(map);
    word am = 0, ax = 0, ay = 0;
    long i, j;
    am = atoi(argv[3]);
    ax = atoi(argv[4]);
    ay = atoi(argv[5]);
    printf("Setting: mode %u X %u Y %u\n", am, ax, ay);
    set_mode(map, am, ax, ay, 0, 0, 0); // ... bp, ht, vt = 0!
/*
    set_mode(map, 0x60, 800, 480, 0, 0, 0); // ... bp, ht, vt = 0!
    set_mode(map, 0x63, 720, 576, 0, 0, 0);
    set_mode(map, 0x66, 1024, 600, 0, 0, 0);
    set_mode(map, 0x69, 1366, 768, 0, 0, 0);
    set_mode(map, 0x6c, 1440, 900, 0, 0, 0);

    set_mode(map, 0x3a, 832, 624, 0, 0, 0);
    set_mode(map, 0x3c, 512, 384, 0, 0, 0);

    set_mode(map, 0x32, 640, 400, 0, 832, 445); // 75Hz
    set_mode(map, 0x34, 720, 576, 0, 864, 625);
    set_mode(map, 0x38, 800, 480, 0, 1032, 560);
    set_mode(map, 0x3a, 832, 624, 0, 1152, 667); // 85Hz
    set_mode(map, 0x3c, 1024, 600, 0, 1312, 622);
    set_mode(map, 0x4d, 1152, 864, 0, 1520, 895);

    set_mode(map, 0x32, 640, 400, 0, 0, 0); // 75Hz
    set_mode(map, 0x34, 720, 576, 0, 0, 0);
    set_mode(map, 0x38, 800, 480, 0, 0, 0);
    set_mode(map, 0x3a, 832, 624, 0, 0, 0); // 85Hz
    set_mode(map, 0x3c, 1024, 600, 0, 0, 0);
*/
    raw = 0; list_modes(map, raw);
    //relock_vbios(map);
        vbios[offset+size-1] = 0;
re_check:
        j = 0; for (i = offset; i < (offset+size); i++) j += vbios[i];
        j &= 0xFF;
        printf("BIOS Checksum is 0x%02X", j);
        if (j != 0) { printf(" BAD, CORRECTING ...\n"); vbios[offset+size-1] = 0x100 - j; goto re_check; }
        else printf(" OK\n");
			  printf("Processing: %s (WRITE)\n",argv[2]);
				fOut = fopen(argv[2],"wb");
				fseek(fOut, 0x00, SEEK_SET);
				fwrite(&vbios[0],size2,1,fOut);
				close(fOut);
				}
    FREE(map);
				}
	return(0);
}
