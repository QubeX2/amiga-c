#pragma once
#ifndef __LINE_H__
#define __LINE_H__

#include "includes/includes.h"
#include "includes/hardware.h"

struct PointXYZ {
    WORD x;
    WORD y;
    WORD z;
};

struct LineAB {
    UWORD a;
    UWORD b;
};

struct CoordXY {
    UWORD x;
    UWORD y;
};

void draw_line(UWORD x1, UWORD y1, UWORD x2, UWORD y2, APTR* bitplane);

#endif /* __LINE_H__ */