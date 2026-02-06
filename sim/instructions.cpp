#include <iostream>
#include <vector>
#include "scheduler.h"

// instruction encoding
enum Op {ALU=0, LD=1, ST=2, EXIT=3};
std::vector<Op> program = { LD, EXIT };

int L = 5; //stall for L cycles

void exec_inst(SimState& s, int warp_idx){
    int pc_val = s.warps[warp_idx].getPC();
    Op op = program[pc_val];

    if (pc_val < 0 || pc_val >= (int)program.size()){
        s.warps[warp_idx].setDone(true); //if pc out of range
        return; //say warp is done, and go back to sim loop
    }

    std::cout << "PC= " << pc_val << "op= " << op << "\n";
    if (op == LD){ // pc_val = LD?
        s.warps[warp_idx].setStallUntil(s.cycle + L);
        s.warps[warp_idx].setPC(pc_val + 1);
    } else if (op == EXIT){ //pc_val = exit?
        s.warps[warp_idx].setDone(true);
    } else {
        s.warps[warp_idx].setPC(pc_val +1);
    }
}