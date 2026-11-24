#pragma once
#include <iostream>
#include <cstdint>
#include <random>
#include "cache.hpp"
#include "address.hpp"

inline void sequential_experiment(uint64_t step, int count, Cache& cache){
    cache.reset();
    for(int i = 0;  i < count; i++){
        cache.access(i * step);
    } 
    std::cout << "HITS = " << cache.get_hits() << " MISSES = " << cache.get_misses() <<  " HIT RATIO = " << cache.hit_ratio() << std::endl;
}

inline void random_experiment(uint64_t range, int count, Cache& cache){
    cache.reset();

    std::mt19937_64 gen(42);                                  // fixed seed
    std::uniform_int_distribution<uint64_t> dist(0, range - 1);   // rand()

    for(int i = 0; i<count; i++){
      uint64_t address  = dist(gen);
      cache.access(address);
    }
    std::cout << "HITS = " << cache.get_hits() << " MISSES = " << cache.get_misses() <<  " HIT RATIO = " << cache.hit_ratio() << std::endl;
}

inline void strided_pattern(uint64_t base, uint64_t stride, int k, int rounds, Cache& cache){
    for(int r = 0; r<rounds; r++){
        for(int i = 0; i<k; i++){
          cache.access(base+ i * stride);
        }
    }
}




