#include <iostream>
#include "memory.h"

std::vector<int> mem = {}; 

void init_memory(int size){
    mem.resize(size);
    for(int i = 0; i < size; i++){
        mem[i] = i;
    }
}

int mem_read(int addr){
    return mem[addr];
}

void mem_write(int addr, int val){
    mem[addr] = val;
}