#ifndef TABLES_HPP
#define TABLES_HPP
#include <cstdint>

extern unsigned short move_co[2187][18];
extern unsigned short move_eo[2048][18];
extern unsigned short move_sl[495][18];
extern unsigned short move_cp[40320][18];
extern unsigned short move_ep8[40320][18];
extern unsigned short move_ep4[24][18];

extern int8_t prune_co[2187];
extern int8_t prune_eo[2048];
extern int8_t prune_sl[495];
extern int8_t prune_cp[40320];
extern int8_t prune_ep[40320];
extern int8_t prune_ep4[24];

extern bool tables_built;
void build_tables();

#endif
