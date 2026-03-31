#include <iostream>
#include <stdexcept>
#include "memory.h"

std::vector<int> mem = {};

void init_memory(int size){
    mem.resize(size);
    for(int i = 0; i < size; i++){
        mem[i] = i;
    }
}

int mem_read(int addr){
    if (addr < 0 || addr >= (int)mem.size()){
        throw std::out_of_range("memory read address out of range");
    }
    return mem[addr];
}

void mem_write(int addr, int val){
    if (addr < 0 || addr >= (int)mem.size()){
        throw std::out_of_range("memory write address out of range");
    }
    mem[addr] = val;
}
