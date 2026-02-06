#include <iostream>
#include "scheduler.cpp"

int cycle = 0;

int main(){
    //loop structure
    //fina a ready warp
    //issue its current instruction (to execute)

    int w = scheduler(cycle);
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

    return 0;
}