#pragma once
#include <vector>
#include "stats.h"

class Warp {
private:
    int pc = 0;
    int stall_until = 0;
    bool done = false;
    static const int NUM_REGS = 8;
    int regs[NUM_REGS] = {};

public:
    int getPC() const { return pc; }
    int getStallUntil() const { return stall_until; }
    bool isDone() const { return done; }
    int getReg(int r) const { return regs[r]; }

    void setPC(int p) { pc = p; }
    void setStallUntil(int s) { stall_until = s; }
    void setDone(bool d) { done = d; }
    void setReg(int r, int val) { regs[r] = val; }
};

struct SimState {
    int cycle = 0;
    int rr_ptr = 0;
    std::vector<Warp> warps;
    SimStats stats;
};

int scheduler(SimState& s);