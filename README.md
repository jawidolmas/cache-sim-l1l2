# Cache Simulator

A CPU cache simulator written in C++17. It models how a memory address is
mapped to a cache line, and measures hits and misses.

## Status

- [x] Milestone 1: Direct-mapped L1 cache (in progress)
- [ ] Milestone 2: N-way set associativity, LRU and FIFO replacement
- [ ] Milestone 3: Real memory traces (Valgrind) and L1/L2 hierarchy
- [ ] Milestone 4: Prefetcher and benchmark report

## Cache configuration

| Parameter   | Value         |
|-------------|---------------|
| Total size  | 32 KB         |
| Block size  | 64 bytes      |
| Sets        | 512           |
| Ways        | 1 (direct)    |
| Address     | 64 bit        |

## How it works

Each address is split into three fields:

    [ tag | set index | block offset ]
       49       9            6        (bits)

- Block offset: selects a byte inside the 64-byte block.
- Set index: selects the cache line.
- Tag: identifies which block currently occupies the line.

An access is a hit if the selected line is valid and its tag matches the
address tag. Otherwise it is a miss, and the line is replaced.

The split uses bit shifts and masks:

    offset = address & (block_size - 1)
    index  = (address >> offset_bits) & (num_sets - 1)
    tag    = address >> (offset_bits + index_bits)

## Build and run

Requirements: g++ with C++17 support (Linux or WSL2).

    g++ -std=c++17 -Wall -Wextra src/main.cpp -o sim
    ./sim

## Tests

The program verifies the address split with assert() on three hand-checked
addresses, then runs three access patterns:

| Test | Input                  | Expected result          | Shows                 |
|------|------------------------|--------------------------|-----------------------|
| A    | 0x0, 0x8000, 0x0       | MISS, MISS, MISS         | Conflict misses       |
| B    | 0x0, 0x0, 0x8000, 0x8000 | MISS, HIT, MISS, HIT   | Reuse after load      |
| C    | 0x0, 0x20, 0x3F        | MISS, HIT, HIT           | Spatial locality      |

## Project layout

    cache-sim/
    |-- README.md
    `-- src/
        `-- main.cpp

## Planned work

- Sequential and random address generators
- Command-line options for ways and policy
- Valgrind trace reader
- L2 cache and latency model
- Next-line and stride prefetchers
- Benchmark tables in the README

## License

NONE-FOR-NOW