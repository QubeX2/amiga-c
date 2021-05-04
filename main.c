/**
 * QubeX2
 * 
 */
#include "includes.h"
#include <intuition/intuition.h>
#include <graphics/gfxbase.h>
#include <hardware/dmabits.h>
#include <hardware/intbits.h>
#include <hardware/custom.h>
#include <graphics/gfxmacros.h>

#define TASK_PRIORITY (20)
#define COPMOVE(addr, data) addr, data
#define COPEND() 0xffff, 0xfffe
#define COPWAIT(VP, HP) VP, HP
#define COLOR00 (0x180)
#define BPLCON0_CC (1 << 9)
#define BPLCON0 (0x100)

struct IntuitionBase *IntuitionBase;
struct GfxBase *GfxBase;
extern struct ExecBase *SysBase;
extern struct DosLibrary *DOSBase;
volatile struct Custom *custom = (struct Custom *)0xdff000;
//extern struct Custom *custom;
volatile struct CIA *ciaa = (struct CIA *)0xbfe001;
struct View my_view;
struct View *my_old_view;


struct UCopList* cl;
static UWORD __chip coplist[] = {
    COPMOVE(BPLCON0, BPLCON0_CC),
    COPMOVE(COLOR00, 0x000),
    COPWAIT(0x7c07, 0xfffe),
    COPMOVE(COLOR00, 0xf00),
    COPWAIT(0xda07, 0xfffe),
    COPMOVE(COLOR00, 0xff0),
    COPEND()
};

BOOL init_display(void)
{
    LoadView(NULL);
    WaitTOF();
    WaitTOF();
    return (((struct GfxBase *) GfxBase)->DisplayFlags & PAL) == PAL;
}

void close(STRPTR message) 
{
    // Close the Graphics library:
	if(GfxBase)
		CloseLibrary((struct Library *)GfxBase);

	// C Close the Intuition library:
	if(IntuitionBase)
		CloseLibrary((struct Library *)IntuitionBase);

	// Restore the old View:
	LoadView( my_old_view );
    WaitTOF();
    WaitTOF();
    custom->cop1lc = (ULONG) ((struct GfxBase *) GfxBase)->copinit;
    RethinkDisplay();

	// Print the message and leave: 
	printf( "%s\n", message );

	exit(0);
}

int main() 
{
    printf("%x\n", custom->vposr);
    // Open the Intuition library:
	IntuitionBase = (struct IntuitionBase *)
	OpenLibrary( "intuition.library", 0 );
	if( !IntuitionBase )
		close( "Could NOT open the Intuition library!" );

    // Open the Graphics library:
	GfxBase = (struct GfxBase *)
	OpenLibrary( "graphics.library", 0 );
	if( !GfxBase )
		close( "Could NOT open the Graphics library!" );

    // Save the current View, so we can restore it later:
	my_old_view = GfxBase->ActiView;

    SetTaskPri(FindTask(NULL), TASK_PRIORITY);
    BOOL is_pal = init_display();
    printf("PAL display: %d\n", is_pal);

    custom->cop1lc = (ULONG)coplist;
    
	Forbid();
	Disable();
	WaitBlit();

    while(ciaa->ciapra & CIAF_GAMEPORT0) {

    }
        
    Enable();
	Permit();

	close("Exited!");

    return 0;
}