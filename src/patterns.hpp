#include "cache.hpp"
#include "address.hpp"


#pragma once



    const uint64_t block_size = 64;   // 6 offset bits
    const uint64_t num_sets = 512;    // 9 index bits
    bool verbose = true;



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