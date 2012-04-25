#ifndef genetic_h
#define genetic_h

#include "box.h"
#include "const.h"


static int ngen = 100000;
static int gen_size = 50;

int iscollide(cover *cov);
void optimise(int nsteps, cover *cov);
void cpcov(cover sourcecov,cover destcov);
void mate(cover *mother, cover *father, cover *child);
cover * create_gen(int gen_size);
void destroy_gen(int gen_size, cover *gen);
void randomise_box(box *b);
void randomise_gen(cover *gen);
int fittest_cov(cover *gen);
void score_gen(cover *gen);
void new_gen(cover *gen);

#endif
