#pragma once
#include <vector>
#include "scheduler.h"
#include <string>

enum Op { ALU=0, LD=1, ST=2, EXIT=3, BEQ=4, ELSE_OP=5, ENDIF_OP=6 };

struct Instruction {
    Op op;
    int dst = 0; // destination register
    int src1 = 0; //source register 1
    int src2 = 0; //source register 2
};

extern std::vector<Instruction> program;
extern int L;
extern int ST_L;

const char* opToStr(Op op);
void exec_inst(SimState& s, int warp_idx);
void load_program(const std::string& filepath);