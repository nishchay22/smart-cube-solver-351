#include "constants.hpp"
#include "cube.hpp"
#include "indexing.hpp"
#include "tables.hpp"
#include "search.hpp"

#include <vector>
#include <string>
#include <queue>
#include <algorithm>
#include <iostream>
#include <sstream>
#include <array>
#include <cstring>
#include <tuple>

using namespace std;

// --- 1. DEFINITIONS ---
const string MOVE_STR[] = {"U", "U2", "U'", "R", "R2", "R'", "F", "F2", "F'", "D", "D2", "D'", "L", "L2", "L'", "B", "B2", "B'"};
const int N_CO = 2187;
const int N_EO = 2048;
const int N_SL = 495;
const int N_CP = 40320;
const int N_EP8 = 40320;
const int N_EP4 = 24;
const int FACT[] = {1, 1, 2, 6, 24, 120, 720, 5040, 40320};

int C_nk[13][5];

// --- 3. MATH HELPER ---
void init_math() {
    for (int n=0; n<=12; ++n) {
        C_nk[n][0] = 1;
        if(n<=4) C_nk[n][n] = 1;
        for (int k=1; k<n && k<=4; ++k)
            C_nk[n][k] = C_nk[n-1][k-1] + C_nk[n-1][k];
    }
}

// --- 4. CORE MOVE LOGIC ---
void apply_move_struct(Cube &c, int move) {
    int face = move / 3;
    int power = (move % 3) + 1;
    
    static const int U_C[] = {0, 1, 2, 3}; static const int U_E[] = {0, 1, 2, 3};
    static const int R_C[] = {2, 1, 6, 5}; static const int R_E[] = {1, 10, 5, 9};
    static const int F_C[] = {3, 2, 5, 4}; static const int F_E[] = {2, 9, 4, 8};
    static const int D_C[] = {4, 5, 6, 7}; static const int D_E[] = {4, 5, 6, 7};
    static const int L_C[] = {0, 3, 4, 7}; static const int L_E[] = {3, 8, 7, 11};
    static const int B_C[] = {1, 0, 7, 6}; static const int B_E[] = {0, 11, 6, 10};

    const int* c_cycle;
    const int* e_cycle;

    switch(face) {
        case 0: c_cycle = U_C; e_cycle = U_E; break;
        case 1: c_cycle = R_C; e_cycle = R_E; break;
        case 2: c_cycle = F_C; e_cycle = F_E; break;
        case 3: c_cycle = D_C; e_cycle = D_E; break;
        case 4: c_cycle = L_C; e_cycle = L_E; break;
        case 5: c_cycle = B_C; e_cycle = B_E; break;
    }

    for(int p=0; p<power; ++p) {
        uint8_t c0 = c.cp[c_cycle[3]];
        c.cp[c_cycle[3]] = c.cp[c_cycle[2]]; c.cp[c_cycle[2]] = c.cp[c_cycle[1]];
        c.cp[c_cycle[1]] = c.cp[c_cycle[0]]; c.cp[c_cycle[0]] = c0;

        uint8_t co0 = c.co[c_cycle[3]];
        c.co[c_cycle[3]] = c.co[c_cycle[2]]; c.co[c_cycle[2]] = c.co[c_cycle[1]];
        c.co[c_cycle[1]] = c.co[c_cycle[0]]; c.co[c_cycle[0]] = co0;

        if (face != 0 && face != 3) {
            c.co[c_cycle[0]] = (c.co[c_cycle[0]] + 1) % 3; c.co[c_cycle[1]] = (c.co[c_cycle[1]] + 2) % 3;
            c.co[c_cycle[2]] = (c.co[c_cycle[2]] + 1) % 3; c.co[c_cycle[3]] = (c.co[c_cycle[3]] + 2) % 3;
        }

        uint8_t e0 = c.ep[e_cycle[3]];
        c.ep[e_cycle[3]] = c.ep[e_cycle[2]]; c.ep[e_cycle[2]] = c.ep[e_cycle[1]];
        c.ep[e_cycle[1]] = c.ep[e_cycle[0]]; c.ep[e_cycle[0]] = e0;

        uint8_t eo0 = c.eo[e_cycle[3]];
        c.eo[e_cycle[3]] = c.eo[e_cycle[2]]; c.eo[e_cycle[2]] = c.eo[e_cycle[1]];
        c.eo[e_cycle[1]] = c.eo[e_cycle[0]]; c.eo[e_cycle[0]] = eo0;

        if (face == 2 || face == 5) {
            c.eo[e_cycle[0]] ^= 1; c.eo[e_cycle[1]] ^= 1; c.eo[e_cycle[2]] ^= 1; c.eo[e_cycle[3]] ^= 1;
        }
    }
}

// --- 5. INDEXING HELPERS ---
int get_co(const Cube &c) {
    int sum = 0;
    for (int i=0; i<7; ++i) sum = sum*3 + c.co[i];
    return sum;
}

int get_eo(const Cube &c) {
    int sum = 0;
    for (int i=0; i<11; ++i) sum = sum*2 + c.eo[i];
    return sum;
}

int get_slice(const Cube &c) {
    int idx = 0, k = 4;
    for (int i=11; i>=0; --i) {
        if (c.ep[i] >= 8 && c.ep[i] <= 11) idx += C_nk[i][k--];
    }
    return idx;
}

int get_cp(const Cube &c) {
    int idx = 0;
    for (int i=0; i<7; ++i) {
        int count = 0;
        for (int j=i+1; j<8; ++j) if (c.cp[j] < c.cp[i]) count++;
        idx += count * FACT[7-i];
    }
    return idx;
}

int get_ep8(const Cube &c) {
    int idx = 0;
    int eps[8], x=0;
    for(int i=0; i<12; ++i) if(c.ep[i] < 8) eps[x++] = c.ep[i];
    for (int i=0; i<7; ++i) {
        int count = 0;
        for (int j=i+1; j<8; ++j) if (eps[j] < eps[i]) count++;
        idx += count * FACT[7-i];
    }
    return idx;
}

int get_ep4(const Cube &c) {
    int idx = 0;
    int eps[4], x=0;
    for(int i=0; i<12; ++i) if(c.ep[i] >= 8) eps[x++] = c.ep[i];
    for(int i=0; i<4; ++i) eps[i] -= 8;
    for (int i=0; i<3; ++i) {
        int count = 0;
        for (int j=i+1; j<4; ++j) if (eps[j] < eps[i]) count++;
        idx += count * FACT[3-i];
    }
    return idx;
}

// --- 6. GLOBAL TABLES ---
unsigned short move_co[2187][18];
unsigned short move_eo[2048][18];
unsigned short move_sl[495][18];
unsigned short move_cp[40320][18];
unsigned short move_ep8[40320][18];
unsigned short move_ep4[24][18];

int8_t prune_co[2187];
int8_t prune_eo[2048];
int8_t prune_sl[495];
int8_t prune_cp[40320];
int8_t prune_ep[40320];
int8_t prune_ep4[24];

bool tables_built = false;

template<int SIZE, typename Func>
void build_table(unsigned short move_table[][18], int8_t* prune_table, Func get_idx, bool phase2=false) {
    memset(prune_table, -1, SIZE);
    vector<Cube> states(SIZE);
    vector<bool> vis(SIZE, false);
    
    Cube solved; 
    int start_node = get_idx(solved);
    states[start_node] = solved;
    vis[start_node] = true;
    prune_table[start_node] = 0;
    
    queue<int> q;
    q.push(start_node);

    while(!q.empty()) {
        int u = q.front(); q.pop();
        Cube cur = states[u];

        for(int m=0; m<18; ++m) {
            if(phase2) {
                int f = m/3; int p = (m%3)+1;
                if(f!=0 && f!=3 && p!=2) continue;
            }

            Cube next = cur;
            apply_move_struct(next, m);
            int v = get_idx(next);
            move_table[u][m] = v;

            if(!vis[v]) {
                vis[v] = true;
                states[v] = next;
                prune_table[v] = prune_table[u] + 1;
                q.push(v);
            }
        }
    }
}

void build_tables() {
    if(tables_built) return;
    init_math();
    cout << "Generating Tables (may take 2s)..." << endl;

    build_table<2187>(move_co, prune_co, get_co);
    build_table<2048>(move_eo, prune_eo, get_eo);
    build_table<495>(move_sl, prune_sl, get_slice);
    build_table<40320>(move_cp, prune_cp, get_cp, true);
    build_table<40320>(move_ep8, prune_ep, get_ep8, true);
    build_table<24>(move_ep4, prune_ep4, get_ep4, true);

    cout << "Tables Ready." << endl;
    tables_built = true;
}

// --- 7. THREAD-SAFE SOLVER CLASS ---
int Solver::h_p1(int co, int eo, int sl) {
    return max({(int)prune_co[co], (int)prune_eo[eo], (int)prune_sl[sl]});
}

int Solver::h_p2(int cp, int ep, int ep4) {
    return max({(int)prune_cp[cp], (int)prune_ep[ep], (int)prune_ep4[ep4]});
}

void Solver::search_p2(int cp, int ep, int ep4, int g, int bound) {
    if(solved_found) return;
    int h = h_p2(cp, ep, ep4);
    if(g + h > bound) return;

    if(cp == 0 && ep == 0 && ep4 == 0) {
        solved_found = true;
        for(auto s : moves) solution_str += s + " ";
        return;
    }

    for(int m=0; m<18; ++m) {
        int f = m/3; int p = (m%3)+1;
        if(f!=0 && f!=3 && p!=2) continue;

        if(!moves.empty() && moves.back()[0] == MOVE_STR[m][0]) continue;

        int next_cp = move_cp[cp][m];
        int next_ep = move_ep8[ep][m];
        int next_ep4 = move_ep4[ep4][m];
        
        moves.push_back(MOVE_STR[m]);
        search_p2(next_cp, next_ep, next_ep4, g+1, bound);
        if(solved_found) return;
        moves.pop_back();
    }
}

void Solver::search_p1(int co, int eo, int sl, int g, int bound, Cube c_tracker) {
    if(solved_found) return;
    int h = h_p1(co, eo, sl);
    if(g + h > bound) return;

    if(h == 0) {
        int cp = get_cp(c_tracker);
        int ep = get_ep8(c_tracker);
        int ep4 = get_ep4(c_tracker);
        
        int b2 = h_p2(cp, ep, ep4);
        while(!solved_found && b2 < 14) {
            search_p2(cp, ep, ep4, 0, b2);
            b2++;
        }
        if(solved_found) return;
    }

    for(int m=0; m<18; ++m) {
        if(!moves.empty() && moves.back()[0] == MOVE_STR[m][0]) continue;

        int next_co = move_co[co][m];
        int next_eo = move_eo[eo][m];
        int next_sl = move_sl[sl][m];
        
        Cube next_c = c_tracker;
        apply_move_struct(next_c, m);

        moves.push_back(MOVE_STR[m]);
        search_p1(next_co, next_eo, next_sl, g+1, bound, next_c);
        if(solved_found) return;
        moves.pop_back();
    }
}

Cube Solver::execute_scramble(string s) {
    Cube c;
    stringstream ss(s);
    string seg;
    while(ss >> seg) {
        for(int i=0; i<18; ++i) if(seg == MOVE_STR[i]) { apply_move_struct(c, i); break; }
    }
    return c;
}

string Solver::solve(string scramble) {
    moves.clear();
    solution_str = "";
    solved_found = false;

    Cube c = execute_scramble(scramble);
    
    int co = get_co(c);
    int eo = get_eo(c);
    int sl = get_slice(c);
    
    int bound = h_p1(co, eo, sl);
    while(!solved_found && bound < 20) {
        search_p1(co, eo, sl, 0, bound, c);
        bound++;
    }
    return solution_str;
}
