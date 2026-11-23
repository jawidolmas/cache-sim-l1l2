# Cache Simulator

A CPU cache simulator written in C++17. It models how a memory address is
mapped to a cache line, and measures hits and misses for different access
patterns and cache configurations.

## Status

- [x] Milestone 1: Direct-mapped L1 cache
- [ ] Milestone 2: N-way set associativity, LRU and FIFO replacement (in progress)
- [ ] Milestone 3: Real memory traces (Valgrind) and L1/L2 hierarchy
- [ ] Milestone 4: Prefetcher and benchmark report

## Cache configuration

The Cache class is configured by total size, block size, number of ways,
and replacement policy. The number of sets is calculated:

    num_sets = total_size / (block_size * ways)

Default configuration used so far:

| Parameter   | Value         |
|-------------|---------------|
| Total size  | 32 KB         |
| Block size  | 64 bytes      |
| Ways        | 1 (direct)    |
| Sets        | 512           |
| Address     | 64 bit        |

## How it works

Each address is split into three fields:

    [ tag | set index | block offset ]
       49       9            6        (bits, for 512 sets and 64-byte blocks)

- Block offset: selects a byte inside the block.
- Set index: selects the set.
- Tag: identifies which block currently occupies a line.

An access is a hit if a valid line in the selected set has a matching tag.
Otherwise it is a miss, and a line is replaced.

The split uses bit shifts and masks:

    offset = address & (block_size - 1)
    index  = (address >> offset_bits) & (num_sets - 1)
    tag    = address >> (offset_bits + index_bits)

## Access patterns

| Pattern    | Parameters        | Purpose                                  |
|------------|-------------------|------------------------------------------|
| Sequential | step, count       | Shows spatial locality                   |
| Random     | range, count      | Shows the effect of working set size     |
| Strided    | stride, K, rounds | Forces conflict misses in a single set   |

Random runs use a fixed seed, so results are repeatable.

## Results

Milestone 1, direct-mapped, 1,000,000 accesses:

| Pattern    | Setting      | Hit ratio |
|------------|--------------|-----------|
| Sequential | step 4       |           |
| Sequential | step 64      |           |
| Sequential | step 128     |           |
| Random     | range 16 KB  |           |
| Random     | range 32 KB  |           |
| Random     | range 1 MB   |           |

## Build and run

Requirements: g++ with C++17 support (Linux or WSL2).

    g++ -std=c++17 -Wall -Wextra src/main.cpp src/cache.cpp -o sim
    ./sim

A Makefile will replace this command in Milestone 2.

## Tests

The program checks the address split with assert() on three hand-checked
addresses before running the experiments.

| Test | Input                    | Expected result        | Shows              |
|------|--------------------------|------------------------|--------------------|
| A    | 0x0, 0x8000, 0x0         | MISS, MISS, MISS       | Conflict misses    |
| B    | 0x0, 0x0, 0x8000, 0x8000 | MISS, HIT, MISS, HIT   | Reuse after load   |
| C    | 0x0, 0x20, 0x3F          | MISS, HIT, HIT         | Spatial locality   |

## Project layout

    cache-sim-l1l2/
    |-- README.md
    |-- .gitignore
    `-- src/
        |-- main.cpp        experiments and output
        |-- address.hpp     split address into tag, index, offset
        |-- cache.hpp       CacheLine, Policy, Cache class
        |-- cache.cpp       Cache implementation
        `-- patterns.hpp    address generators

## Planned work

- N-way lookup with LRU and FIFO replacement
- Strided pattern and associativity benchmark
- Command-line options for size, block, ways, and policy
- Makefile
- Valgrind trace reader
- L2 cache and latency model
- Next-line and stride prefetchers
- Benchmark tables in the README

## License

None for now.