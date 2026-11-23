#include<iostream>
#include<cstdint>
#pragma once


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