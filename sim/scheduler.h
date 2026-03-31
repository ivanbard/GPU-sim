#pragma once
#include <vector>
#include <stack>
#include <cstdint>
#include <stdexcept>
#include "stats.h"

class Warp {
private:
    int pc = 0;
    int stall_until = 0;
    bool done = false;
    static const int NUM_REGS = 8;
    int regs[NUM_REGS] = {};
    uint32_t active_mask = 0xFFFFFFFF; // 1 bit per thread, all 32 active by default
    std::stack<uint32_t> mask_stack; // saved masks for nested branches

    static void requireRegIndex(int r) {
        if (r < 0 || r >= NUM_REGS) {
            throw std::out_of_range("register index out of range");
        }
    }

public:
    static constexpr int numRegs() { return NUM_REGS; }

    int getPC() const { return pc; }
    int getStallUntil() const { return stall_until; }
    bool isDone() const { return done; }
    int getReg(int r) const { requireRegIndex(r); return regs[r]; }
    uint32_t getActiveMask() const { return active_mask; }

    void setPC(int p) { pc = p; }
    void setStallUntil(int s) { stall_until = s; }
    void setDone(bool d) { done = d; }
    void setReg(int r, int val) { requireRegIndex(r); regs[r] = val; }
    void setActiveMask(uint32_t m) { active_mask = m; }

    void pushMask(uint32_t m)  { mask_stack.push(m); }
    uint32_t popMask() {
        if (mask_stack.empty()) {
            throw std::runtime_error("mask stack underflow");
        }
        uint32_t m = mask_stack.top();
        mask_stack.pop();
        return m;
    }
    bool maskStackEmpty() const { return mask_stack.empty(); }
};

struct SimState {
    int cycle = 0;
    int rr_ptr = 0;
    std::vector<Warp> warps;
    SimStats stats;
};

int scheduler(SimState& s);