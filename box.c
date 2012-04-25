#include <math.h>
#include <assert.h>


#include "box.h"
#include "const.h"

void check_box_params() {
    assert(nboxes > 0);
}

box_corners get_corners(float l, box *p_box) {
    box_corners c;
    float ldiag;

    ldiag = l/sqrt(2);  // Length of half diagonal

    c.x0 = p_box->x + ldiag*sin(1*M_PI/4 + p_box->r);
    c.y0 = p_box->y + ldiag*cos(1*M_PI/4 + p_box->r);

    c.x1 = p_box->x + ldiag*sin(3*M_PI/4 + p_box->r);
    c.y1 = p_box->y + ldiag*cos(3*M_PI/4 + p_box->r);

    c.x2 = p_box->x + ldiag*sin(5*M_PI/4 + p_box->r);
    c.y2 = p_box->y + ldiag*cos(5*M_PI/4 + p_box->r);

    c.x3 = p_box->x + ldiag*sin(7*M_PI/4 + p_box->r);
    c.y3 = p_box->y + ldiag*cos(7*M_PI/4 + p_box->r);
    
    return c;
}
