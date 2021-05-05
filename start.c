#include "start.h"
#include <stdio.h>

static struct IntuitionBase *IntuitionBase;
static struct GfxBase *GfxBase;
static struct ExecBase *SysBase;
static struct DosLibrary *DosBase;

static struct Custom *custom = (struct Custom*)0xdff000;

static struct View *my_old_view;
static UWORD old_dmacon;
static UWORD old_intena;
static UWORD old_adkcon;
static ULONG level2_vector;
static ULONG level3_vector;

static UBYTE *ICRA=(UBYTE *)0xbfed01;

/**
 * 
 */
void wait_raster(ULONG raster)
{
	raster <<= 8;
	raster &= 0x1ff00;
	// not pretty, but creates pretty much same code as the asm source (MOVE.L)
	while(raster != ((*(volatile ULONG*) &custom->vposr) & 0x1ff00))
	{}
}

/**
 * 
 */
#define MOD_SIZE (339546)
static UBYTE __chip mod_data[MOD_SIZE];

void init_music(STRPTR modpath, BOOL is_pal)
{
    FILE *fp = fopen(modpath, "rb");
    int bytes_read = fread(mod_data, sizeof(UBYTE), MOD_SIZE, fp);
    fclose(fp);
    void *p_samples = NULL;
    UBYTE start_pos = 0;
    mt_install_cia(custom, NULL, is_pal);
    mt_init(custom, mod_data, p_samples, start_pos);
    mt_Enable = 1;
}

/**
 * 
 */
BOOL init_display()
{
    LoadView(NULL);
    WaitTOF();
    WaitTOF();
    return (((struct GfxBase *) GfxBase)->DisplayFlags & PAL) == PAL;
}

/**
 * 
 */
BOOL init_system(ULONG coplist)
{
    old_dmacon = custom->dmaconr;
    old_dmacon |= (DMAF_SETCLR | DMAF_MASTER);
    old_intena = custom->intenar;
    old_adkcon = custom->adkconr;

    // save current vectors
    level2_vector = *(ULONG *)0x68;
    level3_vector = *(ULONG *)0x6c;

    // Open the Intuition library:
	IntuitionBase = (struct IntuitionBase *) OpenLibrary("intuition.library", 0 );
	if( !IntuitionBase ) 
		close_system("Could NOT open the Intuition library!");

    // Open the Graphics library:
	GfxBase = (struct GfxBase *) OpenLibrary("graphics.library", 0 );
	if( !GfxBase )
		close_system("Could NOT open the Graphics library!");

    DosBase = (struct DosLibrary *) OpenLibrary("dos.library", 0);
    if( !DosBase )
        close_system("Could NOT open the Dos library!");

    // Save the current View, so we can restore it later:
	my_old_view = GfxBase->ActiView;

    SetTaskPri(FindTask(NULL), TASK_PRIORITY);
    BOOL is_pal = init_display();
    init_music("assets/modules/OroIncenso.mod", is_pal);
    printf("PAL display: %d\n", is_pal);

    custom->cop1lc = coplist;
    
	Forbid();
	Disable();
	WaitBlit();
    OwnBlitter(); 

    wait_raster(303);

    // disable dma and interupts
    custom->dmacon = 0x7fff;
    custom->intena = 0x7fff;
    custom->intreq = 0x7fff;
    custom->intreq = 0x7fff;

    *ICRA=0x7f;

    return TRUE;
}

/**
 * 
 */
void close_system(STRPTR message) 
{
    mt_Enable = 0;
    mt_end(custom);
    mt_remove_cia(custom);    

    *(ULONG *)0x68 = level2_vector;
    *(ULONG *)0x6c = level3_vector;

    // disable dma and interupts
    custom->dmacon = 0x7fff;
    custom->intena = 0x7fff;
    custom->intreq = 0x7fff;

    custom->adkcon = old_adkcon;
    custom->dmacon = old_dmacon | (DMAF_MASTER | DMAF_RASTER);
    custom->intena = old_intena | (INTF_SETCLR | INTF_INTEN);

    *ICRA = 0x9b;

    Enable();
	Permit();

    DisownBlitter();

    // Close the Graphics library:
	if(GfxBase)
		CloseLibrary((struct Library *)GfxBase);

	// C Close the Intuition library:
	if(IntuitionBase)
		CloseLibrary((struct Library *)IntuitionBase);

    if(DosBase)
        CloseLibrary((struct Library *)DosBase);

	// Restore the old View:
	LoadView(my_old_view);
    WaitTOF();
    WaitTOF();
    custom->cop1lc = (ULONG) GfxBase->copinit;
    custom->cop2lc = (ULONG) GfxBase->LOFlist;
    RethinkDisplay();

	// Print the message and leave: 
	printf( "%s\n", message );

	exit(0);
}
