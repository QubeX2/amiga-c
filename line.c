/**
 * 
 */
#include "line.h"
#include <stdio.h>
#include <stdlib.h>
#include "includes/hardware.h"

static struct Custom *custom = (struct Custom*)0xdff000;


#define SPACE (0x40)
#define LF_COOKIE_CUT (0xca)
#define LF_XOR (0x4a)

void draw_line(UWORD x1, UWORD y1, UWORD x2, UWORD y2, APTR* bitplane)
{
    UWORD width = 320;
    UWORD dx = abs(x2 - x1);
    UWORD dy = abs(y2 - y1);
    UWORD dmin, dmax;
    UWORD bpl = width/8;
    UBYTE octant;

    if(y1 >= y2) {
        if(x1 <= x2) {
            octant = dx >= dy ? 6 : 1;
        } else {
            octant = dx <= dy ? 3: 7;
        }
    } else {
        if(x1 >= x2) {
            octant = dx >= dy ? 5 : 2;
        } else {
            octant = dx <= dy ? 0 : 4;
        }
    }

    if(dx <= dy) {
        dmin = dx;
        dmax = dy;
    } else {
        dmin = dy;
        dmax = dx;
    }

    WORD aptlval = 4 * dmin - 2 * dmax;
    UWORD startx = (x1 & 0xf) << 12; // x1 mod 16
    UWORD texture = (x1 & 0xf) << 12;
    UWORD sign = (aptlval < 0 ? 1 : 0) << 6;
    UWORD bltcon1val = texture | sign | (octant << 2) | 0x01;

    WaitBlit();
    custom->bltapt = (APTR) ((UWORD) aptlval);
    custom->bltcpt = bitplane;
    custom->bltdpt = bitplane;
    custom->bltamod = 4 * (dmin - dmax);    
    custom->bltbmod = 4 * dmin;
    custom->bltcmod = width / 8;
    custom->bltdmod = width / 8;
    custom->bltadat = 0x8000;
    custom->bltbdat = 0xffff;
    custom->bltafwm = 0xffff;
    custom->bltalwm = 0xffff;
    custom->bltcon0 = 0x0b00 | LF_COOKIE_CUT | startx;
    custom->bltcon1 = bltcon1val;
    custom->bltsize = ((dmax + 1) << 6) + 2;
}