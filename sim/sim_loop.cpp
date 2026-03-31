#include <iostream>
#include <exception>
#include <string>
#include "scheduler.h"
#include "instructions.h"
#include "memory.h"

int main(int argc, char** argv){
    const std::string kernel_path = (argc > 1) ? argv[1] : "../kernels/diverge.kern";
    const int memory_size = 256;
    const int num_warps = 4;
    const int max_cycles = 1000;

    try {
        init_memory(memory_size);
        load_program(kernel_path);
        SimState s;
        s.warps.resize(num_warps);
        s.cycle = 0;

        // r1 = warp_id * 4 (distinct memory addresses for LD/ST)
        // BEQ remains a synthetic divergence marker for now.
        for (int i = 0; i < (int)s.warps.size(); i++){
            s.warps[i].setReg(1, i * 4);
        }

        for (s.cycle = 0; s.cycle < max_cycles; s.cycle++){
            int w = scheduler(s);
            if (w == -1){
                if (s.stats.warps_completed == (int)s.warps.size()) break;
                std::cout << "[cycle " << s.cycle << "] IDLE\n";
                s.stats.stall_cycles++;
            } else {
                if (s.warps[w].getActiveMask() != 0xFFFFFFFF && s.warps[w].getActiveMask() != 0x0){
                    s.stats.diverged_cycles++;
                }
                exec_inst(s, w);
                s.stats.instructions_issued++;
            }
            s.stats.total_cycles++;
        }

        std::cout << "\n--Simulation Results--\n";
        std::cout << "Kernel: " << kernel_path << "\n";
        std::cout << "Total cycles: " << s.stats.total_cycles << "\n";
        std::cout << "Instructions issued: " << s.stats.instructions_issued << "\n";
        std::cout << "Stall cycles: " << s.stats.stall_cycles << "\n";
        std::cout << "Warps completed: " << s.stats.warps_completed << "\n";
        std::cout << "Diverged cycles: " << s.stats.diverged_cycles << "\n";
        if (s.stats.total_cycles == 0){
            std::cout << "IPC: 0\n";
        } else {
            std::cout << "IPC: " << s.stats.instructions_issued / (double)s.stats.total_cycles << "\n";
        }

        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Simulation error: " << e.what() << "\n";
        return 1;
    }
}