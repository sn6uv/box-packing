#ifndef gui_h
#define gui_h

#include "box.h"

struct box;
struct cover;
struct box_corners;

int setup_gui();
int window_redraw_required();

void draw_boxes(struct box_corners *c);
void draw_cover(cover *cov);
void clear_window();
void draw_boarder();
void flush_window();
int handle_button_presses();
int blocking_handle_button_presses();

void draw_pause_button(int pause_state);
void draw_reset_button(int reset_state);

void add_score(int gen, float score);
void reset_scores();
void draw_scores();

void close_gui();
#endif
