#include <stdio.h>
#include <stdlib.h>
#include <X11/Xlib.h>
#include <assert.h>

#include "gui.h"
#include "box.h"
#include "const.h"
#include "genetic.h"

int blackColour, whiteColour;

char *font_name = "-misc-dejavu serif-medium-r-normal--0-0-0-0-p-0-iso8859-15";
XFontStruct* font_info;

Display *dpy;
Window w, pause_win, reset_win;
GC gc;

int imgx = 500;
int imgy = 500;
int pltx = 500;

int cscore = 1; // Current score
float *scores;  // Previous scores
int *iscores;   // Previous Generations


int rgb(int r, int g, int b) {
    assert(r < 256);
    assert(g < 256);
    assert(b < 256);

    assert(r >= 0);
    assert(g >= 0);
    assert(b >= 0);
    return (r << 16) + (g<<8) + b;
}

void reset_scores() {
    cscore = 1;
}

void draw_scores() {
    for(int i=0; i<cscore-1; i++) 
        XDrawLine(dpy, w, gc, imgx+iscores[i]*pltx/ngen+1, imgy-imgy*scores[i], 
        imgx+iscores[i+1]*pltx/ngen+1, imgy-imgy*scores[i+1]);
}

void add_score(int gen, float score) {
    scores[cscore] = score;
    iscores[cscore] = gen;

    cscore++;
    assert(cscore < pltx);

    draw_scores();
}

int setup_gui() {

      assert(gen_size > -1); // Removes compiler warning message

      scores = malloc(sizeof(float)*pltx);
      assert(scores != NULL);

      iscores = malloc(sizeof(int)*pltx);
      assert(iscores != NULL);

      iscores[0] = 0;
      scores[0] = 0.0;

      /* Open the display */
      dpy = XOpenDisplay(NULL);
      assert(dpy);

      /* Get some colors */
      blackColour = BlackPixel(dpy, DefaultScreen(dpy));
      whiteColour = WhitePixel(dpy, DefaultScreen(dpy));
        
      assert(blackColour == 0);
      assert(whiteColour == 16777215); // 2**24-1

      /* try to load the given font. */
      font_info = XLoadQueryFont(dpy, font_name);

      if (!font_info) {
          fprintf(stderr,
          "XLoadQueryFont: failed loading font '%s'\n", font_name);
          exit(1);
      }

      /* Create the window */
      w = XCreateSimpleWindow(dpy, DefaultRootWindow(dpy), 0, 0,
                     imgx+pltx+1, imgy+51, 0, blackColour, blackColour);

      /* We want to get MapNotify events */
      XSelectInput(dpy, w, StructureNotifyMask|ExposureMask);

      /* "Map" the window (that is, make it appear on the screen) */
      XMapWindow(dpy, w);

      /* Create a "Graphics Context" */
      gc = XCreateGC(dpy, w, 0, NULL);

      /* Tell the GC we draw using the white color */
      XSetForeground(dpy, gc, rgb(140,140,140));

      // Draw pause/start button
      pause_win = XCreateSimpleWindow(dpy, w, 0, imgy+1,
                     100, 50, 0, blackColour, blackColour);

      assert(pause_win != 0);
      XSelectInput(dpy, pause_win, ButtonPressMask);
      XMapWindow(dpy, pause_win);

      // Draw reset button
      reset_win = XCreateSimpleWindow(dpy, w, imgx-100, imgy+1,
                     100, 50, 0, blackColour, blackColour);

      assert(reset_win != 0);
      XSelectInput(dpy, reset_win, ButtonPressMask);
      XMapWindow(dpy, reset_win);

      /* Wait for the MapNotify event */
      for(;;) {
        XEvent e;
        XNextEvent(dpy, &e);
        if (e.type == MapNotify)
          break;
      }
    return 0;
}

int draw_box(box_corners *c) {
    // Draw the lines
    XDrawLine(dpy, w, gc, imgx*c->x0, imgy*c->y0, imgx*c->x1, imgy*c->y1);
    XDrawLine(dpy, w, gc, imgx*c->x1, imgy*c->y1, imgx*c->x2, imgy*c->y2);
    XDrawLine(dpy, w, gc, imgx*c->x2, imgy*c->y2, imgx*c->x3, imgy*c->y3);
    XDrawLine(dpy, w, gc, imgx*c->x3, imgy*c->y3, imgx*c->x0, imgy*c->y0);

    // Send the "DrawLine" request to the server
    // XFlush(dpy);
    return 0;
}

void draw_cover(cover *cov) {
    box_corners c;

    for(int i=0; i<nboxes; i++) {
        c = get_corners(cov->l, & cov->boxes[i]);
        draw_box(&c);
    }
}
void flush_window() {
    XFlush(dpy);
}

void draw_boarder() {
    XDrawLine(dpy, w, gc, 0, 0, imgx, 0);
    XDrawLine(dpy, w, gc, imgx, 0, imgx, imgy);
    XDrawLine(dpy, w, gc, imgx, imgy, 0, imgy);
    XDrawLine(dpy, w, gc, 0, imgy, 0, 0);
    
    // Graph Boarder
    XDrawLine(dpy, w, gc, imgx, 0, imgx+pltx, 0);
    XDrawLine(dpy, w, gc, imgx+pltx, 0, imgx+pltx, imgy);
    XDrawLine(dpy, w, gc, imgx+pltx, imgy, imgx, imgy);

}

void draw_pause_button(int pause_state) {
    GC pause_gc;

    pause_gc = XCreateGC(dpy, w, 0, NULL);

    XSetFont(dpy, pause_gc, font_info->fid);
    XSetForeground(dpy, pause_gc, rgb(150,150,150));

    XClearWindow(dpy, pause_win);
    
    XDrawLine(dpy, pause_win, pause_gc, 0, 0, 99, 0);
    XDrawLine(dpy, pause_win, pause_gc, 99, 0, 99, 49);
    XDrawLine(dpy, pause_win, pause_gc, 99, 49, 0, 49);
    XDrawLine(dpy, pause_win, pause_gc, 0, 49, 0, 0);

    if (pause_state) {
        XDrawString(dpy, pause_win, pause_gc, 20, 30, " Play ",5);
    } else {
        XDrawString(dpy, pause_win, pause_gc, 20, 30, "Pause",5);
    }
    XFreeGC(dpy, pause_gc);
}

void draw_reset_button(int reset_state) {
    GC reset_gc;

    reset_gc = XCreateGC(dpy, w, 0, NULL);

    XSetFont(dpy, reset_gc, font_info->fid);
    XSetForeground(dpy, reset_gc, rgb(150,150,150));

    XClearWindow(dpy,reset_win);

    XDrawLine(dpy, reset_win, reset_gc, 0, 0, 99, 0);
    XDrawLine(dpy, reset_win, reset_gc, 99, 0, 99, 49);
    XDrawLine(dpy, reset_win, reset_gc, 99, 49, 0, 49);
    XDrawLine(dpy, reset_win, reset_gc, 0, 49, 0, 0);


    if (reset_state) {
        XDrawString(dpy, reset_win, reset_gc, 20, 30, "Start",5);
    } else {
        XDrawString(dpy, reset_win, reset_gc, 20, 30, "Reset",5);
    }
    XFreeGC(dpy, reset_gc);
}

void clear_window() {
    XClearWindow(dpy,w);
}

int window_redraw_required() {
    XEvent event;
    return XCheckTypedWindowEvent(dpy, w, Expose, &event);
}

int handle_button_presses() {
    XEvent event;

    // Play/Pause Button
    if (XCheckTypedWindowEvent(dpy, pause_win, ButtonPress, &event)) {
        return 1;
    }

    // Reset Button
    if (XCheckTypedWindowEvent(dpy, reset_win, ButtonPress, &event)) {
        return 2;
    }

    return 0;
}

int blocking_handle_button_presses() {
    XEvent event;
    XNextEvent(dpy, &event);

    if (event.type == ButtonPress) {
        if (event.xbutton.window == pause_win) return 1;
        if (event.xbutton.window == reset_win) return 2;
    }

    return -1;
}

void close_gui(){
    XFreeFont(dpy, font_info); 
    XFreeGC(dpy, gc);

    XUnmapWindow(dpy, pause_win);
    XUnmapWindow(dpy, reset_win);
    XUnmapWindow(dpy, w);

    XDestroyWindow(dpy, pause_win);
    XDestroyWindow(dpy, reset_win);
    XDestroyWindow(dpy, w);

    flush_window();
}
