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

TEST RESULTS 

All address splits correct.

=== LRU: expect MISS MISS HIT MISS MISS ===
0x0 set 0 MISS
0x4000 set 0 MISS
0x0 set 0 HIT
0x8000 set 0 MISS
0x4000 set 0 MISS

=== FIFO: expect MISS MISS HIT MISS HIT ===
0x0 set 0 MISS
0x4000 set 0 MISS
0x0 set 0 HIT
0x8000 set 0 MISS
0x4000 set 0 HIT

##### 1 way(s), LRU #####
seq step 4:    HITS = 937500 MISSES = 62500 HIT RATIO = 93.75
seq step 64:   HITS = 0 MISSES = 1000000 HIT RATIO = 0
seq step 128:  HITS = 0 MISSES = 1000000 HIT RATIO = 0
rand 16 KB:    HITS = 999744 MISSES = 256 HIT RATIO = 99.9744
rand 32 KB:    HITS = 999488 MISSES = 512 HIT RATIO = 99.9488
rand 1 MB:     HITS = 31039 MISSES = 968961 HIT RATIO = 3.1039

##### 2 way(s), LRU #####
seq step 4:    HITS = 937500 MISSES = 62500 HIT RATIO = 93.75
seq step 64:   HITS = 0 MISSES = 1000000 HIT RATIO = 0
seq step 128:  HITS = 0 MISSES = 1000000 HIT RATIO = 0
rand 16 KB:    HITS = 999744 MISSES = 256 HIT RATIO = 99.9744
rand 32 KB:    HITS = 999488 MISSES = 512 HIT RATIO = 99.9488
rand 1 MB:     HITS = 31192 MISSES = 968808 HIT RATIO = 3.1192

##### 4 way(s), LRU #####
seq step 4:    HITS = 937500 MISSES = 62500 HIT RATIO = 93.75
seq step 64:   HITS = 0 MISSES = 1000000 HIT RATIO = 0
seq step 128:  HITS = 0 MISSES = 1000000 HIT RATIO = 0
rand 16 KB:    HITS = 999744 MISSES = 256 HIT RATIO = 99.9744
rand 32 KB:    HITS = 999488 MISSES = 512 HIT RATIO = 99.9488
rand 1 MB:     HITS = 31106 MISSES = 968894 HIT RATIO = 3.1106

##### 8 way(s), LRU #####
seq step 4:    HITS = 937500 MISSES = 62500 HIT RATIO = 93.75
seq step 64:   HITS = 0 MISSES = 1000000 HIT RATIO = 0
seq step 128:  HITS = 0 MISSES = 1000000 HIT RATIO = 0
rand 16 KB:    HITS = 999744 MISSES = 256 HIT RATIO = 99.9744
rand 32 KB:    HITS = 999488 MISSES = 512 HIT RATIO = 99.9488
rand 1 MB:     HITS = 31062 MISSES = 968938 HIT RATIO = 3.1062

##### Strided, LRU #####
ways 1, K 2: hit ratio 0%
ways 1, K 3: hit ratio 0%
ways 1, K 4: hit ratio 0%
ways 1, K 5: hit ratio 0%
ways 1, K 9: hit ratio 0%
ways 2, K 2: hit ratio 99.9%
ways 2, K 3: hit ratio 0%
ways 2, K 4: hit ratio 0%
ways 2, K 5: hit ratio 0%
ways 2, K 9: hit ratio 0%
ways 4, K 2: hit ratio 99.9%
ways 4, K 3: hit ratio 99.9%
ways 4, K 4: hit ratio 99.9%
ways 4, K 5: hit ratio 0%
ways 4, K 9: hit ratio 0%
ways 8, K 2: hit ratio 99.9%
ways 8, K 3: hit ratio 99.9%
ways 8, K 4: hit ratio 99.9%
ways 8, K 5: hit ratio 99.9%
ways 8, K 9: hit ratio 0%

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