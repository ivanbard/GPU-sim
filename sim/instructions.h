#pragma once
#include <vector>
#include "scheduler.h"
#include <string>

enum Op { ALU=0, LD=1, ST=2, EXIT=3 };

extern std::vector<Op> program;
extern int L;

const char* opToStr(Op op);
void exec_inst(SimState& s, int warp_idx);
void load_program(const std::string& filepath);