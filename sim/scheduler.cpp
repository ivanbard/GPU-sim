#include <iostream>
#include <vector>

int num_warps = 4; //try with just 4 warps currently for developing
int rr_ptr = 0; //round robin pointer
bool issued_this_cycle = false;

//warp container
class Warp{
    private:
        int pc = 0;
        int stall_until = 0;
        bool done = false;
        //int id;
    
        public:
            //getters
            int getPC() const {return pc; };
            int getStallUntil() const {return stall_until;};
            bool isDone() const {return done;};

            //setters
            void setPC(int p) {pc=p;};
            void setStallUntil(int s) {stall_until = s;};
            void setDone(bool d) {done = d;};

};

// vector of 32 warps, each with containing the items from the class
// this is all the warps in a specific SM
std::vector<Warp> warps(num_warps);

// check what warp meets "ready" condition, if none then SM is idle this cycle
int scheduler(int curr_cycle){
    for (int i = 0; i < num_warps; i++){
        int idx = (rr_ptr + i) % num_warps;
        if (!warps[idx].isDone() && warps[idx].getStallUntil() <= curr_cycle){
            rr_ptr = (idx + 1) % num_warps; // so that it starts AFTER current warp if this one is successful
            std::cout << "Round-Robin Pointer: " << rr_ptr << std::endl;
            return idx; 
        }
    }
    return -1; //if no warp ready, idle, continue running

}