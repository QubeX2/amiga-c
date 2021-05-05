#include <hardware/cia.h>
#include <hardware/custom.h>

/* Helpers */
#define GETBYTE_HI(var) (*(volatile UBYTE*)&var)
#define GETBYTE_LO(var) (*((volatile UBYTE*)&var+1))
#define GETWORD(var) (*(volatile UWORD*)&var)
#define GETLONG(var) (*(volatile ULONG*)&var)

/* Copperlist helpers */
#define CMOVE(addr, data) addr, data
#define CWAIT(vhpos, flags) vhpos, flags
#define CEND 0xffff, 0xfffe

struct ExecBase *SysBase;
volatile struct Custom *custom = (struct Custom *)0xdff000;
volatile struct CIA *ciaa = (struct CIA *)0xbfe001;

int main() {
    while(ciaa->ciapra & CIAF_GAMEPORT0) {
        while(GETBYTE_HI(custom->vhposr) != 0xac) {            
        }
        custom->color[0] = 0xfff;

        while(GETBYTE_HI(custom->vhposr) == 0xac) {            
        }
        custom->color[0] = 0x116;

    }
    return 0;
}