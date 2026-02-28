#include <iostream>
#include "scheduler.h"
#include "instructions.h"
#include "memory.h"

int main(){
    init_memory(256);
    load_program("../kernels/diverge.kern");
    SimState s;
    s.warps.resize(4);
    s.cycle = 0;

    //r1 = warp_id * 4 (distinct memory addresses for LD/ST)
    //r7 stays 0; BEQ R0 R7 would compare R0 vs 0 in a full model
    for (int i = 0; i < (int)s.warps.size(); i++){
        s.warps[i].setReg(1, i * 4);
    }

    int max_cycles = 1000;
    for (s.cycle = 0; s.cycle < max_cycles; s.cycle++){
        int w = scheduler(s);
        if (w == -1){
            // all warps finished, stop the sim
            if (s.stats.warps_completed == (int)s.warps.size()) break;
            // otherwise SM is idle waiting on memory stalls
            std::cout << "[cycle " << s.cycle << "] IDLE\n";
            s.stats.stall_cycles++;
        } else {
            // track cycles where the warp is executing with a partial active mask
            if (s.warps[w].getActiveMask() != 0xFFFFFFFF && s.warps[w].getActiveMask() != 0x0){
                s.stats.diverged_cycles++;
            }
            exec_inst(s, w);
            s.stats.instructions_issued++;
        }
        s.stats.total_cycles++;
    }

    std::cout << "\n--Simulation Results--\n";
    std::cout << "Total cycles: " << s.stats.total_cycles << "\n";
    std::cout << "Instructions issued: " << s.stats.instructions_issued << "\n";
    std::cout << "Stall cycles: " << s.stats.stall_cycles << "\n";
    std::cout << "Warps completed: " << s.stats.warps_completed << "\n";
    std::cout << "Diverged cycles: " << s.stats.diverged_cycles << "\n";
    std::cout << "IPC: " << s.stats.instructions_issued / (double)s.stats.total_cycles << "\n";

    return 0;
}