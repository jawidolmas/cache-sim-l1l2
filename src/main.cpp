#include<iostream>
#include<cstdint>
#include<cassert>
#include<array>
#include<random>


    const uint64_t block_size = 64;   // 6 offset bits
    const uint64_t num_sets = 512;    // 9 index bits
    bool verbose = true;

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
      if(verbose) std::cout << address << " HIT " << std::endl;
    }else{
      misses++;
      if(verbose) std::cout << address << " MISS" << std::endl;
      line.valid = true;
      line.tag = parts.tag;
    }
}



double hit_ratio(){
    double total = hits + misses;
    if(total == 0){
      return 0.0;
    }
    return 100 * hits/total;
}

void reset_cache(){
  for(CacheLine& l : lines){
    l = CacheLine();
  }
  hits = 0;
  misses = 0;
}

void sequential_experiment(uint64_t step, int count){
    reset_cache();
    for(int i = 0;  i < count; i++){
        cache_access(i * step, block_size, num_sets);
    } 
    std::cout << "HITS = " << hits << " MISSES = " << misses <<  " HIT RATIO = " << hit_ratio() << std::endl;
}

void random_experiment(uint64_t range, int count){
    reset_cache();

    std::mt19937_64 gen(42);                                  // fixed seed
    std::uniform_int_distribution<uint64_t> dist(0, range - 1);   // rand()

    for(int i = 0; i<count; i++){
      uint64_t address  = dist(gen);
      cache_access(address, block_size, num_sets);
    }
    std::cout << "HITS = " << hits << " MISSES = " << misses <<  " HIT RATIO = " << hit_ratio() << std::endl;
}


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

