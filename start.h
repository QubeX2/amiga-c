#pragma once
#ifndef __START_H__
#define __START_H__

/**
 * QubeX2
 * 
 */
#include "includes.h"
#include "hardware.h"
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

BOOL init_display(void);
void close_system(STRPTR message);
BOOL init_system(ULONG coplist);
void wait_raster(ULONG raster);

#endif /* __START_H__ */