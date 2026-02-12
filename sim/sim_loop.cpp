#include <iostream>
#include "scheduler.h"
#include "instructions.cpp"

int main(){
    SimState s;
    s.warps.resize(4);
    s.cycle = 0;

    //loop structure
    //fina a ready warp
    //issue its current instruction (to execute)
    for (s.cycle = 0; s.cycle < 10; s.cycle++){
        int w = scheduler(s);
        if (w==-1){
            std::cout << "[cycle "<< s.cycle << "] IDLE" << std::endl;
        } else {
            exec_inst(s, w);
        }
        //update its state
        /*
        pc++
        stall_until = cycle + L, then pc++
        pc++
        done = true  
        */

        // then increment cycle

        //end when all warps are done
    }
    

    return 0;
}