#include<iostream>
#include "cache.hpp"
#include "address.hpp"

// function bodies for Cache class


Cache::Cache(uint64_t total_size, uint64_t block_size, uint64_t ways, Policy policy) : total_size_(total_size), block_size_(block_size), ways_(ways), policy_(policy) {
      num_sets_ = total_size / (block_size*ways);
      sets_.assign(num_sets_, std::vector<CacheLine>(ways_));
      misses_ = 0;
      hits_ = 0;
      clock_ = 0;
      verbose_ = false;
}

void Cache::access(uint64_t address){
    clock_++;
    AddressParts parts = split(address, block_size_, num_sets_);
    std::vector<CacheLine>& set = sets_[parts.set_index];

    // Search for a hit
    for(CacheLine& line : set){
        if(line.valid && line.tag == parts.tag){
            hits_++;
            if(policy_ == Policy::LRU){
                line.last_used = clock_;
            }
            if(verbose_){
                std::cout << "0x" << std::hex << address << std::dec
                          << " set " << parts.set_index << " HIT\n";
            }
            return;   // outside the verbose check
        }
    }

    // Miss: choose a victim
    misses_++;
    CacheLine* victim = nullptr;
    for(CacheLine& line : set){
        if(!line.valid){
            victim = &line;
            break;
        }
    }
    if(victim == nullptr){
        victim = &set[0];
        for(CacheLine& line : set){
            if(line.last_used < victim->last_used){
                victim = &line;
            }
        }
    }

    victim->valid = true;
    victim->tag = parts.tag;
    victim->last_used = clock_;

    if(verbose_){
        std::cout << "0x" << std::hex << address << std::dec
                  << " set " << parts.set_index << " MISS\n";
    }
}


void Cache::reset() {
    for (std::vector<CacheLine>& set : sets_) {
        for (CacheLine& line : set) {
            line = CacheLine();
        }
    }

    hits_ = 0;
    misses_ = 0;
    clock_ = 0;
}

double Cache::hit_ratio() const{
    double total = hits_ + misses_;
    if(total == 0){
      return 0.0;
    }
    return 100 * hits_/total;
}

uint64_t Cache::get_hits() const{
    return hits_;
}

uint64_t Cache::get_misses() const{
    return misses_;
}

void Cache::set_verbose(bool v){
      verbose_ = v; 
}
