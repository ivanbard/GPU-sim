#include <iostream>
#include <cctype>
#include <fstream>
#include <sstream>
#include <string>
#include <stdexcept>
#include "instructions.h"
#include "memory.h"

std::vector<Instruction> program = {}; // populated by load_program()

const char* opToStr(Op op){
    switch(op){
        case ALU: return "ALU";
        case LD: return "LD";
        case ST: return "ST";
        case EXIT: return "EXIT";
        case BEQ: return "BEQ";
        case ELSE_OP: return "ELSE";
        case ENDIF_OP: return "ENDIF";
        default: return "UNKNOWN"; //for any errors
    }
}

int L = 5; // LD latency in cycles
int ST_L = 1; // ST latency in cycles

static void requireKernel(bool condition, const std::string& message){
    if (!condition){
        throw std::runtime_error(message);
    }
}

static std::string stripComment(const std::string& line){
    size_t comment_pos = line.find('#');
    if (comment_pos == std::string::npos){
        return line;
    }
    return line.substr(0, comment_pos);
}

static bool isBlank(const std::string& line){
    for (char ch : line){
        if (!std::isspace(static_cast<unsigned char>(ch))){
            return false;
        }
    }
    return true;
}

static void requireNoExtraOperands(std::istringstream& ss, int line_no){
    std::string extra;
    requireKernel(!(ss >> extra),
                  "line " + std::to_string(line_no) + ": unexpected extra operand '" + extra + "'");
}

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
        s.warps[warp_idx].setReg(inst.dst, mem_read(addr));
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
        // Synthetic divergence marker: this does not compare registers yet.
        // It simply splits the currently active threads into a 50/50 branch.
        uint32_t taken_mask = s.warps[warp_idx].getActiveMask() & 0x0000FFFF;
        uint32_t not_taken = s.warps[warp_idx].getActiveMask() & ~taken_mask;
        s.warps[warp_idx].pushMask(not_taken);
        s.warps[warp_idx].setActiveMask(taken_mask);
        s.warps[warp_idx].setPC(pc_val + 1);
    } else if (op == ELSE_OP){
        uint32_t else_mask = s.warps[warp_idx].popMask();
        uint32_t full_mask = s.warps[warp_idx].getActiveMask() | else_mask;
        s.warps[warp_idx].pushMask(full_mask);
        s.warps[warp_idx].setActiveMask(else_mask);
        s.warps[warp_idx].setPC(pc_val + 1);
    } else if (op == ENDIF_OP){
        uint32_t full_mask = s.warps[warp_idx].popMask();
        s.warps[warp_idx].setActiveMask(full_mask);
        s.warps[warp_idx].setPC(pc_val + 1);
    }
}

static int parseReg(const std::string& token){
    requireKernel(token.size() >= 2 && (token[0] == 'R' || token[0] == 'r'),
                  "invalid register token: '" + token + "'");
    size_t parsed_chars = 0;
    int reg = std::stoi(token.substr(1), &parsed_chars);
    requireKernel(parsed_chars == token.size() - 1,
                  "invalid register token: '" + token + "'");
    requireKernel(reg >= 0 && reg < Warp::numRegs(),
                  "register out of range: '" + token + "'");
    return reg;
}

void load_program(const std::string& filepath){
    std::ifstream inputFile(filepath);
    if (!inputFile.is_open()){
        std::cerr << "Error: could not open kernel file: " << filepath << "\n";
        exit(1);
    }

    program.clear();
    std::string line;
    int line_no = 0;
    while (std::getline(inputFile, line)) {
        line_no++;
        line = stripComment(line);
        if (isBlank(line)) continue;

        std::istringstream ss(line);
        std::string mnemonic;
        ss >> mnemonic;

        Instruction inst;
        std::string a, b, c;

        try {
            if (mnemonic == "ALU" || mnemonic == "ADD"){
                inst.op = ALU;
                requireKernel(static_cast<bool>(ss >> a >> b >> c),
                              "line " + std::to_string(line_no) + ": ADD requires dst src1 src2");
                inst.dst  = parseReg(a);
                inst.src1 = parseReg(b);
                inst.src2 = parseReg(c);
                requireNoExtraOperands(ss, line_no);
            } else if (mnemonic == "LD"){
                inst.op = LD;
                requireKernel(static_cast<bool>(ss >> a >> b),
                              "line " + std::to_string(line_no) + ": LD requires dst addr_reg");
                inst.dst = parseReg(a);
                inst.src1 = parseReg(b);
                requireNoExtraOperands(ss, line_no);
            } else if (mnemonic == "ST"){
                inst.op = ST;
                requireKernel(static_cast<bool>(ss >> a >> b),
                              "line " + std::to_string(line_no) + ": ST requires value_reg addr_reg");
                inst.src1 = parseReg(a);
                inst.src2 = parseReg(b);
                requireNoExtraOperands(ss, line_no);
            } else if (mnemonic == "EXIT"){
                inst.op = EXIT;
                requireNoExtraOperands(ss, line_no);
            } else if (mnemonic == "BEQ"){
                inst.op = BEQ;
                requireKernel(static_cast<bool>(ss >> a >> b),
                              "line " + std::to_string(line_no) + ": BEQ requires two registers");
                inst.src1 = parseReg(a);
                inst.src2 = parseReg(b);
                requireNoExtraOperands(ss, line_no);
            } else if (mnemonic == "DIVERGE"){
                inst.op = BEQ;
                requireNoExtraOperands(ss, line_no);
            } else if (mnemonic == "ELSE"){
                inst.op = ELSE_OP;
                requireNoExtraOperands(ss, line_no);
            } else if (mnemonic == "ENDIF"){
                inst.op = ENDIF_OP;
                requireNoExtraOperands(ss, line_no);
            } else {
                requireKernel(false,
                              "line " + std::to_string(line_no) + ": unknown mnemonic '" + mnemonic + "'");
            }
            program.push_back(inst);
        } catch (const std::exception& e) {
            std::cerr << "Kernel parse error in " << filepath << ": " << e.what() << "\n";
            exit(1);
        }
    }
}
