#include <iostream>
#include <vector>
#include "scheduler.h"

// instruction encoding
enum Op {ALU=0, LD=1, ST=2, EXIT=3};
std::vector<Op> program = { LD, EXIT };

const char* opToStr(Op op){
    switch(op){
        case ALU: return "ALU";
        case LD: return "LD";
        case ST: return "ST";
        case EXIT: return "EXIT";
        default: return "UNKNOWN"; //for any errors
    }
}

int L = 5; //stall for L cycles

void exec_inst(SimState& s, int warp_idx){
    int pc_val = s.warps[warp_idx].getPC();

    if (pc_val < 0 || pc_val >= (int)program.size()){
        s.warps[warp_idx].setDone(true); //if pc out of range
        return; //say warp is done, and go back to sim loop
    }

    Op op = program[pc_val]; // safe to access now
    std::cout << "[cycle " << s.cycle << "] warp " << warp_idx << " PC=" << pc_val << " op=" << opToStr(op) << " stall_until=" << s.warps[warp_idx].getStallUntil() << "\n";
    if (op == LD){
        s.warps[warp_idx].setStallUntil(s.cycle + L);
        s.warps[warp_idx].setPC(pc_val + 1);
    } else if (op == EXIT){
        s.warps[warp_idx].setDone(true);
        s.stats.warps_completed++;
    } else {
        s.warps[warp_idx].setPC(pc_val +1);
    }
}