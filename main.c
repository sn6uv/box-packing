#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <X11/Xlib.h>
#include <assert.h>
#include <unistd.h>

#include "gui.h"
#include "box.h"
#include "const.h"
#include "genetic.h"

#define VERBOSE 1
#define VVERBOSE 0

#if VVERBOSE
#define VERBOSE 1
#endif

int ifit, jfit;
cover *box_fam;

void print_best() {
    score_gen(box_fam);

    ifit = fittest_cov(box_fam);

    printf("%f%% Cover\n", 100*nboxes*pow(box_fam[ifit].l,2));
    printf("Best Cover:\n");
    printf("i x        y        r\n");

    for (int i=0; i<nboxes; i++) printf("%i %f %f %f\n", i,
        box_fam[ifit].boxes[i].x, box_fam[ifit].boxes[i].y,
        box_fam[ifit].boxes[i].r);
}

int main() {
    int button_code;
    int pause_state = 0;
    int reset_state = 0;

    setup_gui();

    // Seed random num generator
    srand (time(NULL));

    box_fam = create_gen(gen_size);
    randomise_gen(box_fam);    

    ifit = -1;

    for (int i=0; i<ngen; i++) {
        // Score generation
        score_gen(box_fam);

        printf("gen %i\r", i);
        fflush(stdout);
        // Draw fittest of the fit
        jfit = fittest_cov(box_fam);
        if (ifit != jfit) {
            ifit = jfit;
            clear_window();
            draw_boarder();
            draw_cover(& box_fam[ifit]);
            draw_pause_button(pause_state);
            draw_reset_button(reset_state);
            flush_window();
            printf("gen %i, %f\n", i, nboxes*pow(box_fam[ifit].l,2));
            add_score(i, nboxes*pow(box_fam[ifit].l,2));
        } 
        if (window_redraw_required()) {
            clear_window();
            draw_boarder();
            draw_cover(& box_fam[ifit]);
            draw_pause_button(pause_state);
            draw_reset_button(reset_state);
            draw_scores();
            flush_window();
        }

        button_code = handle_button_presses();
        while (button_code != 0 || pause_state || reset_state) {
            if (pause_state || reset_state)
                button_code = blocking_handle_button_presses();

            switch (button_code) {
                case -1:
                    // Redraw Required
                    clear_window();
                    draw_boarder();
                    draw_cover(& box_fam[ifit]);
                    draw_pause_button(pause_state);
                    draw_reset_button(reset_state);
                    draw_scores();
                    flush_window();
                    break;
                case 1:
                    // Play/Pause Button
                    if (pause_state) {
                        printf("\rPlay!         \n");
                        pause_state = 0;
                    } else {
                        printf("\rPause!        \n");
                        pause_state = 1;
                    }

                    draw_pause_button(pause_state);
                    flush_window();
                    break;
                case 2:
                    // Reset/Start Button  
                    if (reset_state) {
                        printf("\rStart!             \n");
                        reset_state = 0;
                    } else {
                        printf("\rReset!             \n");
                        print_best();
                        i = 0;
                        randomise_gen(box_fam);
                        score_gen(box_fam);            
                        reset_state = 1;
                        reset_scores();
                        flush_window();
                    }
                    clear_window();
                    draw_boarder();
                    draw_cover(& box_fam[ifit]);

                    draw_pause_button(pause_state);
                    draw_reset_button(reset_state);
                    flush_window();
                    break;
                default:
                    printf("\rButton Error %i\n         ", button_code);
                    exit(button_code);
            }
            button_code = handle_button_presses();
        }

        // Evolve Generation
        new_gen(box_fam); 
    }
    // Clear last gen n line
    printf("                  \r");
    fflush(stdout);

    close_gui();
    print_best();

    printf("Press enter to close\n");
    while( getchar() != '\n' );
    return 0;
}

