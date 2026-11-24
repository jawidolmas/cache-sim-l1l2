#include <iostream>
#include <cstdint>
#include <cassert>
#include "address.hpp"
#include "cache.hpp"
#include "patterns.hpp"

int main()
{
    // Part 1: split checks (512 sets, 64-byte blocks)
    const uint64_t block_size = 64;
    const uint64_t num_sets = 512;

    AddressParts a = split(0x1234, block_size, num_sets);
    assert(a.tag == 0x0 && a.set_index == 72 && a.offset == 52);

    AddressParts b = split(0x8000, block_size, num_sets);
    assert(b.tag == 0x1 && b.set_index == 0 && b.offset == 0);

    AddressParts c = split(0xDEADBEEF, block_size, num_sets);
    assert(c.tag == 0x1BD5B && c.set_index == 251 && c.offset == 47);

    std::cout << "All address splits correct.\n\n";

    // Part 2: LRU vs FIFO hand test, 2 ways, 256 sets
    // A, B, C all map to set 0 (stride = 256 * 64 = 0x4000)
    const uint64_t A = 0x0, B = 0x4000, C = 0x8000;
    uint64_t seq[] = {A, B, A, C, B};

    std::cout << "=== LRU: expect MISS MISS HIT MISS MISS ===\n";
    Cache lru(32768, 64, 2, Policy::LRU);
    lru.set_verbose(true);
    for (uint64_t x : seq) lru.access(x);

    std::cout << "\n=== FIFO: expect MISS MISS HIT MISS HIT ===\n";
    Cache fifo(32768, 64, 2, Policy::FIFO);
    fifo.set_verbose(true);
    for (uint64_t x : seq) fifo.access(x);
    std::cout << "\n";

    // Part 3: sequential and random, for each number of ways
    const int count = 1000000;
    const uint64_t ways_list[] = {1, 2, 4, 8};

    for (uint64_t ways : ways_list) {
        std::cout << "##### " << ways << " way(s), LRU #####\n";

        Cache cache(32768, 64, ways, Policy::LRU);   // silent by default

        std::cout << "seq step 4:    ";
        sequential_experiment(4, count, cache);
        std::cout << "seq step 64:   ";
        sequential_experiment(64, count, cache);
        std::cout << "seq step 128:  ";
        sequential_experiment(128, count, cache);

        std::cout << "rand 16 KB:    ";
        random_experiment(16 * 1024, count, cache);
        std::cout << "rand 32 KB:    ";
        random_experiment(32 * 1024, count, cache);
        std::cout << "rand 1 MB:     ";
        random_experiment(1024 * 1024, count, cache);
        std::cout << "\n";
    }

    // Part 4: strided pattern, all addresses in one set
    std::cout << "##### Strided, LRU #####\n";
    const int ks[] = {2, 3, 4, 5, 9};

    for (uint64_t ways : ways_list) {
        for (int k : ks) {
            Cache cache(32768, 64, ways, Policy::LRU);
            uint64_t stride = 32768 / ways;   // = num_sets * block_size
            strided_pattern(0, stride, k, 1000, cache);
            std::cout << "ways " << ways << ", K " << k
                      << ": hit ratio " << cache.hit_ratio() << "%\n";
        }
    }

    return 0;
}