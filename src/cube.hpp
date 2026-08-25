#ifndef CUBE_HPP
#define CUBE_HPP
#include <cstdint>

struct Cube {
    uint8_t cp[8];
    uint8_t co[8];
    uint8_t ep[12];
    uint8_t eo[12];

    Cube() {
        for(int i=0; i<8; ++i) { cp[i]=i; co[i]=0; }
        for(int i=0; i<12; ++i) { ep[i]=i; eo[i]=0; }
    }
};

void apply_move_struct(Cube &c, int move);

#endif
