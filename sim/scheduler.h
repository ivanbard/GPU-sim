#pragma once
#include <vector>
#include "stats.h"

class Warp {
private:
    int pc = 0;
    int stall_until = 0;
    bool done = false;

public:
    int  getPC() const { return pc; }
    int  getStallUntil() const { return stall_until; }
    bool isDone() const { return done; }

    void setPC(int p) { pc = p; }
    void setStallUntil(int s) { stall_until = s; }
    void setDone(bool d) { done = d; }
};

struct SimState {
    int cycle = 0;
    int rr_ptr = 0;
    std::vector<Warp> warps;
    SimStats stats;
};

int scheduler(SimState& s);