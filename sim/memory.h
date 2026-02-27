#pragma once
#include <vector>

extern std::vector<int> mem;
void init_memory(int size);
int mem_read(int addr);
void mem_write(int addr, int val);