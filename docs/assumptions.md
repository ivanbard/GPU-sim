- Warp size of 32
- 1 SM
- 1 instruction issued per cycle
- instructions: ALU, LD, ST, EXIT
- Latencies
  - ALU = 1
  - LD = L cycles
  - ST = 1 (or L)
- memory: no cache, and no coalescing yet

- will be measuring:
  - total cycles
  - instructions issued
  - stall cycles (waiting for memory)

## Kernel planning
The kernel will be represented in a small-sized, tiny Instruction Register
- will make it easier to develop in later phases

## Data structure planning
### Instruction
- op (ALU/LD/ST/EXIT)
- dst/src ids later 

### Warp
- pc (program counter into instruction list)
- stall_until (cycle number when it becomes ready again)
- done (flag)

### Scheduler
- will hold the warps
- during each cycle: pick a warp with a stall_until <= cycle, and is not already done

### Memory model
- latency(op) returns fixed L and LD (and maybe ST?)

