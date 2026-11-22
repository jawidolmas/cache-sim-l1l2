#include<iostream>
#include<cstdint>
#include<cassert>
#include<array>

struct CacheLine{             
    bool  valid = false;
    uint64_t tag = 0;
    uint64_t last_used = 0;  // timestamp for LRU   // used for phase 3
};

struct AddressParts{
    uint64_t tag;
    uint64_t set_index;
    uint64_t offset;
};


AddressParts split(uint64_t address, uint64_t block_size, uint64_t num_sets){
    int offset_bits = __builtin_ctzll(block_size);
    int index_bits = __builtin_ctzll(num_sets);

    AddressParts p;     // >> shift(operator) to right by N.  // & Mask Operator 
    p.offset = address & (block_size -1);   
    p.set_index = (address >> offset_bits) & (num_sets -1);
    p.tag = address >> (offset_bits + index_bits);
    return p; 
}

std::array<CacheLine, 512> lines;
int hits = 0;
int misses = 0;

void cache_access(uint64_t address, uint64_t block_size, uint64_t num_sets){
    AddressParts parts = split(address, block_size, num_sets);
    CacheLine &line = lines[parts.set_index];

    if(line.valid && line.tag == parts.tag){
      hits++;
      std::cout << address << " HIT " << std::endl;
    }else{
      misses++;
      std::cout << address << " MISS" << std::endl;
      line.valid = true;
      line.tag = parts.tag;
    }
}


int main()
{
    const uint64_t block_size = 64;   // 6 offset bits
    const uint64_t num_sets = 512;    // 9 index bits

    // Part 1: split checks
    AddressParts a = split(0x1234, block_size, num_sets);
    assert(a.tag == 0x0 && a.set_index == 72 && a.offset == 52);

    AddressParts b = split(0x8000, block_size, num_sets);
    assert(b.tag == 0x1 && b.set_index == 0 && b.offset == 0);

    AddressParts c = split(0xDEADBEEF, block_size, num_sets);
    assert(c.tag == 0x1BD5B && c.set_index == 251 && c.offset == 47);

    std::cout << "All address splits correct.\n\n";

    // Part 2: Test A (expected: MISS, MISS, MISS)
    std::cout << "--- A ---\n";
    uint64_t testA[] = {0x0, 0x8000, 0x0};
    for (uint64_t x : testA) {
        cache_access(x, block_size, num_sets);
    }
    std::cout << "hits = " << hits << ", misses = " << misses << "\n\n";

    // Reset before the next test
    lines.fill(CacheLine());
    hits = 0;
    misses = 0;

    // Part 3: Test B (expected: MISS, HIT, MISS, HIT)
    std::cout << "--- B ---\n";
    uint64_t testB[] = {0x0, 0x0, 0x8000, 0x8000};
    for (uint64_t x : testB) {
        cache_access(x, block_size, num_sets);
    }
    std::cout << "hits = " << hits << ", misses = " << misses << "\n\n";

    // Reset again
    lines.fill(CacheLine());
    hits = 0;
    misses = 0;

    // Part 4: Test C (expected: MISS, HIT, HIT)
    std::cout << "--- C ---\n";
    uint64_t testC[] = {0x0, 0x20, 0x3F};
    for (uint64_t x : testC) {
        cache_access(x, block_size, num_sets);
    }
    std::cout << "hits = " << hits << ", misses = " << misses << "\n";

    return 0;
}


/*
  Direct-mapped cache simulator (32 KB, 64-byte blocks, 512 sets).

  For each memory address, split() divides the bits into:
    [ tag | set index | block offset ]

  The set index selects one cache line. The access is a hit if that
  line is valid and its stored tag matches the address tag. Otherwise
  it is a miss, and the line is replaced with the new block.

  Tests:
    A: two addresses in the same set -> conflict misses
    B: repeated addresses            -> hits after the first load
    C: addresses in one block        -> spatial locality
*/
