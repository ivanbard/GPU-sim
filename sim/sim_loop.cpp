#include <iostream>
#include "scheduler.h"
#include "instructions.h"
#include "memory.h"

int main(){
    init_memory(256);
    load_program("../kernels/test.kern");
    SimState s;
    s.warps.resize(4);
    s.cycle = 0;

    //init each warps r1 to a distinct addr so LOAD hits unique spots
    for (int i = 0; i < (int)s.warps.size(); i++){
        s.warps[i].setReg(1, i * 4); // warp 0 = adr 0, warp 1 = adr 4, etc
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
    std::cout << "IPC: " << s.stats.instructions_issued / (double)s.stats.total_cycles << "\n";

    return 0;
}