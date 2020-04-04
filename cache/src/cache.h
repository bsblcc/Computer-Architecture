#ifndef CACHE_H
#define CACHE_H

#include "shell.h"
#ifndef DATA_CACHE_BLOCK_SIZE
#define DATA_CACHE_BLOCK_SIZE 64        // 32B
#endif

#ifndef DATA_CACHE_ASSOCIATE_WAYS
#define DATA_CACHE_ASSOCIATE_WAYS 16    // 8
#endif

#ifndef DATA_CACHE_SIZE
#define DATA_CACHE_SIZE (64 << 10)  // 64KB
#endif


#ifndef INSTR_CACHE_BLOCK_SIZE
#define INSTR_CACHE_BLOCK_SIZE 32        // 32B
#endif

#ifndef INSTR_CACHE_ASSOCIATE_WAYS
#define INSTR_CACHE_ASSOCIATE_WAYS 4    // 4
#endif

#ifndef INSTR_CACHE_SIZE
#define INSTR_CACHE_SIZE (8 << 10)  // 8KB
#endif


#ifndef REPLACE_POLICY
#define REPLACE POLICY 0
#endif



struct cache_line_t
{
    uint32_t tag;
    uint8_t valid;
    uint8_t dirty;
    void *data;
};
typedef struct cache_line_t Cache_Line;

struct associate_cache_t
{
    int block_size; /*int n_offset_bits;*/
    int size;
    int associate_ways; /*int n_ways_bits;*/
    int associate_sets; /*int n_sets_bits;*/
    Cache_Line **cache;
    
};




typedef struct associate_cache_t Associate_Cache;







// interface of cache
void init_cache();
void destroy_cache();

int instr_cache_read_32(uint32_t addr, uint32_t *value);
int data_cache_write_32(uint32_t addr, uint32_t value);
int data_cache_read_32(uint32_t addr, uint32_t *value);

#endif