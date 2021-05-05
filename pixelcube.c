/**
 * QubeX2
 * 
 */
#include "start.h"

volatile struct CIA *ciaa = (struct CIA *)0xbfe001;
static struct Custom *custom = (struct Custom*)0xdff000;

static UWORD __chip coplist[] = {
    COPMOVE(BPLCON0, (1 << 9)),
    COPMOVE(COLOR00, 0x000),
    COPWAIT(0x7c07, 0xfffe),
    COPMOVE(COLOR00, 0xf00),
    COPWAIT(0xda07, 0xfffe),
    COPMOVE(COLOR00, 0xff0),
    COPEND()
};


int main() 
{
   if(init_system((ULONG)coplist) == TRUE) {
        custom->dmacon = (DMAF_SETCLR | DMAF_COPPER | DMAF_BLITTER | DMAF_RASTER | DMAF_MASTER);
        custom->intena = (INTF_SETCLR | INTF_EXTER);

        while(ciaa->ciapra & CIAF_GAMEPORT0) {
            wait_raster(303);
        }
    }
      
	close_system("Exited!");
    return 0;
}