#include <math.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>

#include "box.h"
#include "const.h"
#include "genetic.h"


void check_genetic_params() {
    assert(ngen > 0);
    assert(gen_size > 0);
}

float randf() {
    float randf;
    randf = rand() / (float) RAND_MAX;
    return randf;
}

float min(float x, float y) {
    if (x < y) return x;
    return y;
}

int sign(float x) {
    if (x >= 0) return 1;
    return -1;
}

float mutate(float x, float xmax) {
    float tmp;
    tmp = 2*randf();
    if (tmp < 1) return x*(1-pow(tmp,8));
    return x + (xmax-x)*pow(tmp-1,8);
}

/*
float mutate(float x, float xmax) {
    float tmp;
    tmp = 2*(randf()-0.5);
    return x + sign(tmp)*min(xmax-x,x)*pow(tmp,8);
}
*/

/*
float mutate(float x, float xmax) {
    float tmp;
    tmp = randf();
    if (randf() < x) 
        return x*(1-pow(tmp,8));
    return x + (xmax-x)*pow(tmp,8);
}
*/

int iscollide(cover *cov) {
    box_corners corners[nboxes];
    float *corner;
    float *icorner;
    float *jcorner;

    float ldiag;
    float denom,p1,p2;      // Collision paramters
    float x1,x2,x3,x4,y1,y2,y3,y4; // tmp collision paramaters

    int i,j;

    ldiag = cov->l/sqrt(2);  // Length of half diagonal

    // Find Corners
    for (i=0; i<nboxes; i++)  corners[i] = get_corners(cov->l, & cov->boxes[i]);

    for (i=0; i<nboxes; i++) {
        // Collisions with edges
        corner = &(corners[i].x0);
        for (j=0;j<8;j++) {
            if (corner[j] > 1.0 || corner[j] < 0.0) {
                #if VVERBOSE
                printf("Edge Collision %i, corner %i\n",i,j);
                printf("Box:    %e %e \n", cov->boxes[i].x, cov->boxes[i].y);
                printf("Corner: %e %e \n", corner[j], corner[j]);
                #endif
                return 1;
            }
        }
    }

    // Trim based on absolute positions only (not rotations)
    for (i=0; i<nboxes; i++) {
        for (j=0; j<nboxes; j++) 
            if (i != j) {
            if (pow(cov->boxes[i].x-cov->boxes[j].x,2)
            + pow(cov->boxes[i].y-cov->boxes[j].y,2) < pow(ldiag,2)) {
                #if VVERBOSE 
                printf("Colision %i %i\n", i, j);
                printf("Box1    %f %f \n", cov->boxes[i].x, cov->boxes[i].y);
                printf("Box2    %f %f \n", cov->boxes[j].x, cov->boxes[j].y);
                #endif
                return 1;
            }
            }
    }

    // Full collision detection
    for (i=0; i<nboxes; i++) {
        for (j=0; j<nboxes; j++) {
            for (int ic=0; ic<4; ic++) {
                for (int jc=0; jc<4; jc++) {
                    icorner = &(corners[i].x0);
                    jcorner = &(corners[j].x0);

                    x1 = icorner[2*ic];
                    x2 = icorner[2*((ic+1)%4)];

                    x3 = jcorner[2*jc];
                    x4 = jcorner[2*((jc+1)%4)];

                    y1 = icorner[2*ic+1];
                    y2 = icorner[2*((ic+1)%4)+1];

                    y3 = jcorner[2*jc+1];
                    y4 = jcorner[2*((jc+1)%4)+1];


                    denom = 1.0/((y4-y3)*(x2-x1) - (x4-x3)*(y2-y1));
                    p1 = denom*((x4-x3)*(y1-y3) - (y4-y3)*(x1-x3));
                    p2 = denom*((x2-x1)*(y1-y3) - (y2-y1)*(x1-x3));
                    if (p1 < 1 && p1 > 0 && p2 < 1 && p2 > 0) {
                        // Lines intersect
                        #if VVERBOSE
                        printf("Box collision %i, %i\n", i, j);
                        #endif
                        return 1;
                    }
                    
                }
            }

            /*
            denom=1.0/
            ((corners[j].y1-corners[j].y0)*(corners[i].x1-corners[i].x0)
            -(corners[j].x1-corners[j].x0)*(corners[i].y1-corners[i].y0));

            if (abs(denom) < 1.0E-10) continue;      // lines are parallel 

            p1 = denom*(
            (corners[j].x1-corners[j].x0)*(corners[i].y0-corners[j].y0)-
            (corners[j].y1-corners[j].y0)*(corners[i].x0-corners[j].x0));
            p2 = denom*(
            (corners[i].x1-corners[i].x0)*(corners[i].y0-corners[j].y0)-
            (corners[i].y1-corners[i].y0)*(corners[i].x0-corners[j].x0));
            printf("%f %f\n", p1, p2);
            if (p1 < 1 && p1 > 0 && p2 < 1 && p2 > 0) {     // lines intersect
                printf("YAY!\n");
                #if VVERBOSE
                printf("Box collision %i, %i\n", i, j);
                #endif
                return 1;
            }
            */
        }
    }
    return 0;
}

void optimise(int nsteps, cover *cov) {
    int i;

    float lmin = 0.0;
    float lmax = 1.0;

    for (i=0;i<nsteps;i++) {
        cov->l = (lmax+lmin)/2.0;
        if (iscollide(cov) == 1) {
            lmax = cov->l;
        } else {
            lmin = cov->l;
        }
    }
    cov->l = (lmax+lmin)/2.0;
}

/*
void optimise(int nsteps, cover *cov) {
    // Two step optimisation - much slower
    int i;

    float lmin = 0.0;
    float lmax = 1.0;

    for (i=0;i<nsteps;i++) {
        cov->l = (lmax+lmin)/2.0;
        if (iscollide(cov) == 1) {
            lmax = cov->l;
        } else {
            lmin = cov->l;
        }
    }
    cov->l = lmin;

    // Now jiggle boxes around randomly but do not collide
    box tmpbox;
    for (int i=0; i<2; i++) {
        for (int j=0; j<nboxes; j++) {
            tmpbox = cov->boxes[j];
            cov->boxes[j].x =  mutate(tmpbox.x, 1.0);
            cov->boxes[j].y =  mutate(tmpbox.y, 1.0);
            cov->boxes[j].r =  mutate(tmpbox.r, 0.5*M_PI);
            if (iscollide(cov) == 1) cov->boxes[j] = tmpbox; // Unsucessful
        }
    }

    // ReOptimise the 'jiggled' boxes
    lmax = 1.0;
    for (i=0;i<nsteps;i++) {
        cov->l = (lmax+lmin)/2.0;
        if (iscollide(cov) == 1) {
            lmax = cov->l;
        } else {
            lmin = cov->l;
        }
    }
    cov->l = lmin;
}
*/

void cpcov(cover sourcecov,cover destcov) {
        destcov.l = sourcecov.l;
        for (int i = 0; i < nboxes; i++) destcov.boxes[i] = sourcecov.boxes[i];
}


void mate(cover *mother, cover *father, cover *child) {
    float tmpfrac;
    child->l = 0.0;
    for (int i=0; i<nboxes; i++) {
        // Each box is a (randomised) linear combination of its parents 
        tmpfrac = randf();
        child->boxes[i].x =
        tmpfrac*(mother->boxes[i].x) + (1.0-tmpfrac)*(father->boxes[i].x);
        child->boxes[i].y =
        tmpfrac*(mother->boxes[i].y) + (1.0-tmpfrac)*(father->boxes[i].y);
        child->boxes[i].r =
        tmpfrac*(mother->boxes[i].r) + (1.0-tmpfrac)*(father->boxes[i].r);
        // With some mutation
        child->boxes[i].x = mutate(child->boxes[i].x, 1.0);
        child->boxes[i].y = mutate(child->boxes[i].y, 1.0);
        child->boxes[i].r = mutate(child->boxes[i].r, 0.5*M_PI);
    }
}

cover * create_gen(int gen_size) {
    cover *gen;
    int i;

    gen = malloc(gen_size*sizeof(cover));

    for (i=0; i<gen_size; i++) {
        gen[i].boxes = malloc(nboxes*sizeof(box));
    }
    return gen;
}

void destroy_gen(int gen_size, cover *gen) {
    for (int i=0; i<gen_size; i++)  free(gen[i].boxes);
    free(gen);
}

void randomise_box(box *b) {
    b->x = randf();
    b->y = randf();
    b->r = (M_PI/2.0)*randf();
}

void randomise_gen(cover *gen) {
    // Creates a random generation
    for (int i=0; i<gen_size; i++) 
        for (int j=0; j< nboxes; j++) 
            randomise_box(& gen[i].boxes[j]);
}

int fittest_cov(cover *gen) {
    float lmax = 0.0;
    int imax = -1;

    for (int i=0; i< gen_size; i++) {
        //optimise(20, & gen[i]);
        if (gen[i].l > lmax) {
            lmax = gen[i].l;
            imax = i;
        }
    }

    assert(imax >= 0);
    return imax;
}

void score_gen(cover *gen) {
    for (int i=0; i< gen_size; i++) optimise(40, & gen[i]);
}

void new_gen(cover *gen) {
    assert(gen_size > 0);
    float sum_score;
    float * sub_score;

    // Parent paramaters
    float tmpx, tmpy;
    int ix = 0, iy = 0;

    sub_score = malloc(gen_size*sizeof(float));

    // Select gene pool
    cover *gene_pool;
    gene_pool = create_gen(gen_size);
    for (int i=0; i<gen_size; i++) cpcov(gen[i], gene_pool[i]);


    // Score each organism in the current generation and calculate sum(scores)
    for (int i=0; i< gen_size; i++) {
        sum_score += pow(gen[i].l,2);
        sub_score[i] = sum_score;
    }

    // Randomly select mating pairs
    for (int i=0; i<gen_size; i++) {
        if (i != fittest_cov(gen)) { // Keep the best performer
            tmpx = sum_score*randf();
            tmpy = sum_score*randf();
            for (ix=0; sub_score[ix] < tmpx; ix++);
            for (iy=0; sub_score[iy] < tmpy; iy++);
            assert(ix < gen_size);
            assert(iy < gen_size);

            mate(&gene_pool[ix], &gene_pool[iy], &gen[i]);
        }
    }

    destroy_gen(gen_size, gene_pool);
}

