#include<iostream>
#include <cstdint>
#include <vector>
#pragma once

enum class Policy   {LRU, FIFO};


struct CacheLine{             
    bool  valid = false;
    uint64_t tag = 0;
    uint64_t last_used = 0;  // timestamp for LRU   // used for phase 3
};



class Cache{
  private:
    uint64_t total_size_;
    uint64_t block_size_;
    uint16_t ways_;
    uint64_t num_sets_;
    std::vector<std::vector<CacheLine>> sets_;
    uint64_t hits_;
    uint64_t misses_;
    uint64_t clock_;
    bool verbose_;
    Policy policy_;
  public:
    Cache(uint64_t total_size, uint64_t block_size, uint64_t ways, Policy policy);
    void access(uint64_t address);
    void reset();
    double hit_ratio() const;
    uint64_t get_hits() const;
    uint64_t get_misses() const;
    void set_verbose(bool v);



};

