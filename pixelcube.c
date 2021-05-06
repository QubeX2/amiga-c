/**
 * QubeX2
 * 
 */
#include "start.h"
#include "line.h"

#define SCREEN_WIDTH (320)
#define SCREEN_HEIGHT (256)
#define SCREEN_HALF_WIDTH SCREEN_WIDTH/2
#define SCREEN_HALF_HEIGHT SCREEN_HEIGHT/2
#define BYTES_PER_ROW (SCREEN_WIDTH / 8)

volatile struct CIA *ciaa = (struct CIA *)0xbfe001;
static struct Custom *custom = (struct Custom*)0xdff000;
static UWORD angle_z = 0;
static UWORD angle_y = 0;
static UWORD angle_x = 0;
extern WORD sintable[];

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

static PLANEPTR __chip display_buffer1a;
static PLANEPTR __chip display_buffer1b;
static PLANEPTR __chip display_tmp;


#define POINT_COUNT (8)
#define MAX (16<<8)
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
    LONG sinz = sintable[angle_z];
    LONG cosz = sintable[angle_z + 90];
    LONG siny = sintable[angle_y];
    LONG cosy = sintable[angle_y + 90];
    LONG sinx = sintable[angle_x];
    LONG cosx = sintable[angle_x + 90];

    for(UBYTE i=0;i<POINT_COUNT;i++) {
        LONG xi = (LONG)points[i].x;
        LONG yi = (LONG)points[i].y;
        LONG zi = (LONG)points[i].z;
        // ROT Z
        // x' = x*cos q - y*sin q
		// y' = x*sin q + y*cos q
        LONG xz = xi * cosz - yi * sinz;
        xz >>= 8;
        LONG yz = xi * sinz + yi * cosz;
        yz >>= 8;

        // ROT X
        // y' = y*cos q - z*sin q
	    // z' = y*sin q + z*cos q
        LONG yx = yz * cosx - zi * sinx;
        yx >>= 8;
        LONG zx = yz * sinx + zi * cosx;
        zx >>= 8;

        // ROT Y
        // z' = z*cos q - x*sin q
		// x' = z*sin q + x*cos q
        LONG zy = zx * cosy - xz * siny;
        zy >>= 8;
        LONG xy = zx * siny + xz * cosy;
        xy >>= 8;

        zy = -zy;
        zy >>= 8;
        zy += 78;

        coords[i].x = xy / zy + SCREEN_HALF_WIDTH;
        coords[i].y = yx / zy + SCREEN_HALF_HEIGHT;
    }

    angle_z = angle_z >= 360 ? angle_z - 360 : angle_z + 1;
    angle_y = angle_y >= 360 ? angle_y - 360 : angle_y + 4;
    angle_x = angle_x >= 360 ? angle_x - 360 : angle_x + 2;
}

void draw_cube(PLANEPTR plane)
{
    for(UBYTE i=0;i<LINE_COUNT;i++) {
        struct LineAB line = lines[i];
        struct CoordXY c1 = coords[line.a];
        struct CoordXY c2 = coords[line.b];
        draw_line(c1.x, c1.y, c2.x, c2.y, plane);
    }
}

void swap_buffers()
{
    display_tmp = display_buffer1a;
    display_buffer1a = display_buffer1b;
    display_buffer1b = display_tmp;
}

void clear_screen(PLANEPTR plane, ULONG size)
{
    for(ULONG* i=(ULONG*)plane;i<(ULONG*)plane+size;i++) {
        *i=0;
    }
}

/**
 * 
 */
int main() 
{

    if(init_system((ULONG)coplist) == TRUE) {
        custom->dmacon = (DMAF_SETCLR | DMAF_COPPER | DMAF_BLITTER | DMAF_RASTER | DMAF_AUDIO | DMAF_MASTER);
        custom->intena = (INTF_SETCLR | INTF_EXTER | INTF_INTEN | INTF_AUD0 | INTF_AUD1| INTF_AUD2| INTF_AUD3);

        ULONG size = BYTES_PER_ROW * SCREEN_HEIGHT;
        display_buffer1a = AllocMem(size, MEMF_CHIP | MEMF_CLEAR);
        display_buffer1b = AllocMem(size, MEMF_CHIP | MEMF_CLEAR);

        while(ciaa->ciapra & CIAF_GAMEPORT0) {
            wait_raster(303);
            clear_screen(display_buffer1b, 10*256);
            rotate_points();
            draw_cube(display_buffer1b);
            custom->cop1lc = (ULONG)coplist;
            custom->bplpt[0] = display_buffer1a;
            UWORD c = custom->copjmp1;
            swap_buffers();
        }

        FreeMem(display_buffer1a, size);
        FreeMem(display_buffer1b, size);
    }
      
	close_system("Exited!");
    return 0;
}