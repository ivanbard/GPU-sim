#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include "instructions.h"
#include "memory.h"

std::vector<Instruction> program = {}; // populated by load_program()

const char* opToStr(Op op){
    switch(op){
        case ALU: return "ALU";
        case LD: return "LD";
        case ST: return "ST";
        case EXIT:     return "EXIT";
        case BEQ:      return "BEQ";
        case ELSE_OP:  return "ELSE";
        case ENDIF_OP: return "ENDIF";
        default:       return "UNKNOWN"; //for any errors
    }
}

int L = 5; // LD latency in cycles
int ST_L = 1; // ST latency in cycles

void exec_inst(SimState& s, int warp_idx){
    int pc_val = s.warps[warp_idx].getPC();

    if (pc_val < 0 || pc_val >= (int)program.size()){
        s.warps[warp_idx].setDone(true);
        return;
    }

    const Instruction& inst = program[pc_val];
    Op op = inst.op;
    std::cout << "[cycle " << s.cycle << "] warp " << warp_idx << " PC=" << pc_val << " op=" << opToStr(op) << " stall_until=" << s.warps[warp_idx].getStallUntil() << "\n";

    if (op == ALU){
        int result = s.warps[warp_idx].getReg(inst.src1) + s.warps[warp_idx].getReg(inst.src2);
        s.warps[warp_idx].setReg(inst.dst, result);
        s.warps[warp_idx].setPC(pc_val + 1);
    } else if (op == LD){
        int addr = s.warps[warp_idx].getReg(inst.src1);
        s.warps[warp_idx].setReg(inst.dst, mem_read(addr)); // placeholder value until memory model exists
        s.warps[warp_idx].setStallUntil(s.cycle + L);
        s.warps[warp_idx].setPC(pc_val + 1);
    } else if (op == ST){
        int val  = s.warps[warp_idx].getReg(inst.src1);
        int addr = s.warps[warp_idx].getReg(inst.src2);
        mem_write(addr, val);
        s.warps[warp_idx].setStallUntil(s.cycle + ST_L);
        s.warps[warp_idx].setPC(pc_val + 1);
    } else if (op == EXIT){
        s.warps[warp_idx].setDone(true);
        s.stats.warps_completed++;
    } else if (op == BEQ){
        // Simplified divergence model: lower 16 threads of the active mask take the
        // branch, upper 16 do not. Represents a 50/50 thread split within the warp.
        // (Full model would compute taken_mask per-thread from register values.)
        uint32_t taken_mask  = s.warps[warp_idx].getActiveMask() & 0x0000FFFF;
        uint32_t not_taken   = s.warps[warp_idx].getActiveMask() & ~taken_mask;
        s.warps[warp_idx].pushMask(not_taken);        // save else-threads for ELSE
        s.warps[warp_idx].setActiveMask(taken_mask);  // only taken threads active in if-body
        s.warps[warp_idx].setPC(pc_val + 1);
    } else if (op == ELSE_OP){
        uint32_t else_mask  = s.warps[warp_idx].popMask();                       // not-taken threads
        uint32_t full_mask  = s.warps[warp_idx].getActiveMask() | else_mask;     // full pre-branch mask
        s.warps[warp_idx].pushMask(full_mask);        // save for ENDIF to restore
        s.warps[warp_idx].setActiveMask(else_mask);   // activate else-threads
        s.warps[warp_idx].setPC(pc_val + 1);
    } else if (op == ENDIF_OP){
        uint32_t full_mask = s.warps[warp_idx].popMask();
        s.warps[warp_idx].setActiveMask(full_mask);   // all threads reunite
        s.warps[warp_idx].setPC(pc_val + 1);
    }
}

// parse r0 = 0, r3 = 3, etc
static int parseReg(const std::string& token){
    return std::stoi(token.substr(1));
}

// load up the instructions from the kernel file
void load_program(const std::string& filepath){
    std::ifstream inputFile(filepath);
    if (!inputFile.is_open()){
        std::cerr << "Error: could not open kernel file: " << filepath << "\n";
        exit(1);
    }
    std::string line;
    while (std::getline(inputFile, line)) {
        if (line.empty()) continue;
        std::istringstream ss(line);
        std::string mnemonic;
        ss >> mnemonic;

        Instruction inst;
        std::string a, b, c;

        if (mnemonic == "ALU" || mnemonic == "ADD"){
            inst.op = ALU;
            if (ss >> a >> b >> c){
                inst.dst  = parseReg(a);
                inst.src1 = parseReg(b);
                inst.src2 = parseReg(c);
            }
        } else if (mnemonic == "LD"){
            inst.op = LD;
            if (ss >> a) inst.dst = parseReg(a);
        } else if (mnemonic == "ST"){
            inst.op = ST;
            if (ss >> a) inst.src1 = parseReg(a);
        } else if (mnemonic == "EXIT"){
            inst.op = EXIT;
        } else if (mnemonic == "BEQ"){
            inst.op = BEQ;
            // operands are two registers to compare (src1, src2)
            if (ss >> a >> b){
                inst.src1 = parseReg(a);
                inst.src2 = parseReg(b);
            }
        } else if (mnemonic == "ELSE"){
            inst.op = ELSE_OP;
        } else if (mnemonic == "ENDIF"){
            inst.op = ENDIF_OP;
        } else {
            std::cerr << "Warning: unknown mnemonic '" << mnemonic << "' -- skipping\n";
            continue;
        }
        program.push_back(inst);
    }
}