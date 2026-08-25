#ifndef SEARCH_HPP
#define SEARCH_HPP
#include <vector>
#include <string>
#include "cube.hpp"

class Solver {
private:
    std::vector<std::string> moves;
    bool solved_found = false;
    std::string solution_str = "";

    int h_p1(int co, int eo, int sl);
    int h_p2(int cp, int ep, int ep4);
    void search_p2(int cp, int ep, int ep4, int g, int bound);
    void search_p1(int co, int eo, int sl, int g, int bound, Cube c_tracker);
    Cube execute_scramble(std::string s);

public:
    std::string solve(std::string scramble);
};

#endif
