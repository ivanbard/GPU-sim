#include <iostream>
#include "scheduler.h"

int main(){
    SimState s;
    s.warps.resize(4);
    s.cycle = 0;

    //loop structure
    //fina a ready warp
    //issue its current instruction (to execute)
    for (s.cycle = 0; s.cycle < 10; s.cycle++){
        std::cout << "Current cycle: " << s.cycle << std::endl;
        int w = scheduler(s);
        if (w==-1){
            std::cout << "cycle: IDLE" << std::endl;
        } else {
            std::cout << "cycle: issued warp " << w << std::endl;
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