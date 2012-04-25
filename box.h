#ifndef box_h
#define box_h

static int nboxes = 9;

typedef struct {
    float x;    // x position
    float y;    // y position
    float r;    // rotation
} box;

typedef struct {
    box *boxes; // The boxes
    float l;    // Edge Length
} cover;

typedef struct {
    float x0;
    float y0;
    float x1;
    float y1;
    float x2;
    float y2;
    float x3;
    float y3;
} box_corners;

box_corners get_corners(float l, box *p_box);
#endif
