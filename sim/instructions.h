#pragma once
#include <vector>
#include "scheduler.h"

enum Op { ALU=0, LD=1, ST=2, EXIT=3 };

extern std::vector<Op> program;
extern int L;

const char* opToStr(Op op);
void exec_inst(SimState& s, int warp_idx);
