#ifndef INDEXING_HPP
#define INDEXING_HPP
#include "cube.hpp"

extern int C_nk[13][5];
void init_math();
int get_co(const Cube &c);
int get_eo(const Cube &c);
int get_slice(const Cube &c);
int get_cp(const Cube &c);
int get_ep8(const Cube &c);
int get_ep4(const Cube &c);

#endif
