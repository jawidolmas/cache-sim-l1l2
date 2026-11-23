#include<iostream>
#include<cstdint>
#include<cassert>
#include<array>
#include<random>
#include "address.hpp"
#include "cache.hpp"
#include "patterns.hpp"
  










int main()
{
    verbose = false;   // no per-access printing for big runs

    // Part 1: split checks
    AddressParts a = split(0x1234, block_size, num_sets);
    assert(a.tag == 0x0 && a.set_index == 72 && a.offset == 52);

    AddressParts b = split(0x8000, block_size, num_sets);
    assert(b.tag == 0x1 && b.set_index == 0 && b.offset == 0);

    AddressParts c = split(0xDEADBEEF, block_size, num_sets);
    assert(c.tag == 0x1BD5B && c.set_index == 251 && c.offset == 47);

    std::cout << "All address splits correct.\n\n";

    const int count = 1000000;

    // Part 2: sequential experiments
    std::cout << "=== Sequential ===\n";
    std::cout << "step 4:   ";
    sequential_experiment(4, count);
    std::cout << "step 64:  ";
    sequential_experiment(64, count);
    std::cout << "step 128: ";
    sequential_experiment(128, count);

    // Part 3: random experiments
    std::cout << "\n=== Random ===\n";
    std::cout << "range 16 KB: ";
    random_experiment(16 * 1024, count);
    std::cout << "range 32 KB: ";
    random_experiment(32 * 1024, count);
    std::cout << "range 1 MB:  ";
    random_experiment(1024 * 1024, count);

    return 0;
}

/*
  Direct-mapped cache simulator (32 KB, 64-byte blocks, 512 sets).

  For each memory address, split() divides the bits into:
    [ tag | set index | block offset ]

  The set index selects one cache line. The access is a hit if that
  line is valid and its stored tag matches the address tag. Otherwise
  it is a miss, and the line is replaced with the new block.

  Experiments (1,000,000 accesses each):
    Sequential: addresses advance by a fixed step (4, 64, 128 bytes)
                -> shows spatial locality
    Random:     addresses drawn uniformly from a range (16 KB, 32 KB, 1 MB)
                -> shows how the working set size affects the hit rate

  Set verbose = true to print every access (HIT or MISS).
  Results are repeatable because the random generator uses a fixed seed.
*/

