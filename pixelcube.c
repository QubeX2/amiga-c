/**
 * QubeX2
 * 
 */
#include "start.h"
#include "line.h"

volatile struct CIA *ciaa = (struct CIA *)0xbfe001;
static struct Custom *custom = (struct Custom*)0xdff000;

static UWORD __chip coplist[] = {
    COPMOVE(BPLCON0, 0x1200),
    COPMOVE(BPLCON1, 0x0000),
    COPMOVE(BPLCON2, 0x0024),
    COPMOVE(BPLCON3, 0x0000),
    COPMOVE(FMODE, 0x0000),
    COPMOVE(BPL1MOD, 0x0000),
    COPMOVE(BPL2MOD, 0x0000),
    COPMOVE(DIWSTRT, 0x2c81),
    COPMOVE(DIWSTOP, 0x2cc1),
    COPMOVE(DIWSTOP, 0x2cc1),
    COPMOVE(DDFSTRT, 0x0038),
    COPMOVE(DDFSTOP, 0x00d0),
    COPMOVE(COLOR00, 0x0000),
    COPMOVE(COLOR01, 0x0f00),
    COPMOVE(COLOR02, 0x00f0),
    COPMOVE(COLOR03, 0x000f),
    COPEND()
};

static ULONG __chip bitplane[10*256];

#define POINT_COUNT (8)
#define MAX (18<<8)
struct PointXYZ points[POINT_COUNT] = {
    { -MAX, -MAX, -MAX },
    { MAX, -MAX, -MAX },
    { MAX, MAX, -MAX },
    { -MAX, MAX, -MAX },
    { -MAX, -MAX, MAX },
    { MAX, -MAX, MAX },
    { MAX, MAX, MAX },
    { -MAX, MAX, MAX }
};

struct CoordXY coords[POINT_COUNT] = {
    { 0, 0 },
    { 0, 0 },
    { 0, 0 },
    { 0, 0 },
    { 0, 0 },
    { 0, 0 },
    { 0, 0 },
    { 0, 0 }
};

#define LINE_COUNT (12)
struct LineAB lines[LINE_COUNT] = {
    { 0,3 },
    { 3,2 },
    { 2,1 },
    { 1,0 },
    { 5,6 },
    { 6,7 },
    { 7,4 },
    { 4,5 },
    { 1,5 },
    { 2,6 },
    { 3,7 },
    { 0,4 }
};

void rotate_points()
{
    for(UBYTE i=0;i<POINT_COUNT;i++) {
        WORD z = -points[i].z;
        z >>= 8;
        z += 78;
        coords[i].x = (UWORD)points[i].x / z + 320;
        coords[i].y = (UWORD)points[i].y / z + 256;
        printf("Coords: X %d, Y %d\n", coords[i].x, coords[i].y);
    }
    printf("\n");
}

void draw_cube()
{
    for(UBYTE i=0;i<LINE_COUNT;i++) {
        struct LineAB line = lines[i];
        struct CoordXY c1 = coords[line.a];
        struct CoordXY c2 = coords[line.b];
        //printf("C1: %d, %d\n", c1.x, c1.y);
        //printf("C2: %d, %d\n", c2.x, c2.y);
        //draw_line(c1.x, c1.y, c2.x, c2.y, (APTR)&bitplane);
        draw_line(320,256,1,1, (APTR)&bitplane);
    }
    //printf("\n");
}

int main() 
{
   if(init_system((ULONG)coplist) == TRUE) {
        custom->dmacon = (DMAF_SETCLR | DMAF_COPPER | DMAF_BLITTER | DMAF_RASTER | DMAF_AUDIO | DMAF_MASTER);
        custom->intena = (INTF_SETCLR | INTF_EXTER | INTF_INTEN | INTF_AUD0 | INTF_AUD1| INTF_AUD2| INTF_AUD3);

        while(ciaa->ciapra & CIAF_GAMEPORT0) {
            wait_raster(303);
            custom->bplpt[0] = &bitplane;

            for(int i=0;i<10*256;i++) {
                bitplane[i] = 0x00000000;
            }


            rotate_points();

            draw_cube();

        }
    }
      
	close_system("Exited!");
    return 0;
}