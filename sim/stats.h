#pragma once

// struct to track metrics throughout the sim loop
struct SimStats{
    int total_cycles = 0;
    int instructions_issued = 0;
    int stall_cycles = 0;
    int warps_completed = 0;
    int diverged_cycles = 0; // cycles where a warp executed with a partial active mask
};