/**
 * QubeX2
 * 
 */
#include "start.h"

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
    if(init_system(coplist) == TRUE) {
        while(ciaa->ciapra & CIAF_GAMEPORT0) {

        }
    }
      
	close_system("Exited!");

    return 0;
}