# Cache Simulator

A CPU cache simulator written in C++17. It models how a memory address is
mapped to a cache line, and measures hits and misses for different access
patterns, associativities, and replacement policies.

## Status

- [x] Milestone 1: Direct-mapped L1 cache
- [ ] Milestone 2: N-way set associativity, LRU and FIFO replacement (in progress)
- [ ] Milestone 3: Real memory traces (Valgrind) and L1/L2 hierarchy
- [ ] Milestone 4: Prefetcher and benchmark report

## Cache configuration

The Cache class is configured by total size, block size, number of ways,
and replacement policy. The number of sets is calculated:

    num_sets = total_size / (block_size * ways)

Configuration used in all results below:

| Parameter   | Value                          |
|-------------|--------------------------------|
| Total size  | 32 KB                          |
| Block size  | 64 bytes                       |
| Ways        | 1, 2, 4, 8                     |
| Sets        | 512, 256, 128, 64              |
| Policy      | LRU (FIFO in the hand test)    |
| Address     | 64 bit                         |

A cache with 1 way is a direct-mapped cache.

## How it works

Each address is split into three fields:

    [ tag | set index | block offset ]

- Block offset: selects a byte inside the block (6 bits for 64-byte blocks).
- Set index: selects the set (log2 of the number of sets).
- Tag: identifies which block currently occupies a line.

An access is a hit if a valid line in the selected set has a matching tag.
Otherwise it is a miss, and a line is replaced:

1. If the set has an invalid line, use it.
2. Otherwise evict the line with the smallest last_used timestamp.

The two policies differ in one place only:

- LRU updates last_used on every hit, so the oldest timestamp is the
  least recently used line.
- FIFO sets last_used only when a block is loaded, so the oldest
  timestamp is the line that has been in the cache longest.

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

Random runs use a fixed seed (42), so results are repeatable.

The strided pattern cycles through K addresses that all map to set 0,
repeated for 1000 rounds. The stride is num_sets * block_size.

## Results

All runs use a 32 KB cache with 64-byte blocks and LRU.

### Sequential and random, 1,000,000 accesses

The results are identical for 1, 2, 4, and 8 ways, except for random 1 MB,
which varies by about 0.01 percentage points.

| Pattern    | Setting     | Hit ratio (%) | Explanation                          |
|------------|-------------|---------------|--------------------------------------|
| Sequential | step 4      | 93.75         | 1 miss, then 15 hits per block       |
| Sequential | step 64     | 0.00          | Every access is a new block          |
| Sequential | step 128    | 0.00          | Every access is a new block          |
| Random     | range 16 KB | 99.97         | 256 cold misses, data fits           |
| Random     | range 32 KB | 99.95         | 512 cold misses, data fills cache    |
| Random     | range 1 MB  | about 3.1     | Capacity misses, cache is 1/32 of range |

More ways do not help these patterns. They have no conflict misses, and
the 1 MB case is limited by capacity, not by associativity.

### Strided pattern, hit ratio (%)

K is the number of distinct blocks that map to the same set.

| K | 1 way | 2 ways | 4 ways | 8 ways |
|---|-------|--------|--------|--------|
| 2 | 0     | 99.9   | 99.9   | 99.9   |
| 3 | 0     | 0      | 99.9   | 99.9   |
| 4 | 0     | 0      | 99.9   | 99.9   |
| 5 | 0     | 0      | 0      | 99.9   |
| 9 | 0     | 0      | 0      | 0      |

When K is at most the number of ways, only the first round misses. When K
is larger, LRU always evicts the block that is needed next, so every
access misses.

### LRU versus FIFO hand test

Cache: 2 ways, 256 sets. Addresses A=0x0, B=0x4000, C=0x8000 all map to
set 0. Sequence: A B A C B.

| Policy | A    | B    | A   | C    | B    |
|--------|------|------|-----|------|------|
| LRU    | MISS | MISS | HIT | MISS | MISS |
| FIFO   | MISS | MISS | HIT | MISS | HIT  |

On the access to C, LRU evicts B (A was used more recently), while FIFO
evicts A (A was loaded first). This shows that only LRU reacts to hits.

## Build and run

Requirements: g++ with C++17 support, and make (Linux or WSL2).

    make          build the simulator
    make run      build and run
    make clean    remove build outputs

Run all commands from the repository root.

The program checks the address split with assert() on three hand-checked
addresses, runs the LRU and FIFO hand test, and then runs all experiments.

## Project layout

    cache-sim-l1l2/
    |-- Makefile
    |-- README.md
    |-- .gitignore
    `-- src/
        |-- main.cpp        experiments and output
        |-- address.hpp     split address into tag, index, offset
        |-- cache.hpp       CacheLine, Policy, Cache class
        |-- cache.cpp       Cache implementation
        `-- patterns.hpp    sequential, random, and strided generators

## Planned work

- LRU versus FIFO experiments on more access patterns
- Command-line options for size, block, ways, and policy
- Valgrind trace reader and matrix multiplication workloads
- L2 cache and latency model
- Next-line and stride prefetchers
- Benchmark tables in the README

## License

None for now.