#include "cache.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


Associate_Cache data_cache;
Associate_Cache instr_cache;

void cache_write(Associate_Cache *cache_p, uint32_t addr, uint32_t value);

// fecth the data in cache, retrieve if not present
uint32_t cache_read(Associate_Cache *cache_p, uint32_t addr);

// retrieve a whole cache line from memory
int cache_retrieve(Associate_Cache *cache_p, uint32_t addr, uint8_t *data);

// search the cache for that address, return -1 if not present in cache
int cache_search(Associate_Cache *cache_p, int set_idx, uint32_t tag, Cache_Line **result);

int cache_replace(Associate_Cache *cache_p, int set_idx, Cache_Line **result);
int cache_flush(Associate_Cache *cache_p, uint32_t addr, uint8_t *data);
void cache_write(Associate_Cache *cache_p, uint32_t addr, uint32_t value);



void init_cache();
void destroy_cache();

// how many bits to represent a value
int calc_bits(uint32_t x)
{
	int ret = 0;
	int acc = 1;
	while (acc < x)
	{
		
		acc = acc << 1;
		ret++;
	}
	
	return ret;
}



void init_cache()
{
	data_cache.block_size = DATA_CACHE_BLOCK_SIZE;
	data_cache.size = DATA_CACHE_SIZE;
	data_cache.associate_ways = DATA_CACHE_ASSOCIATE_WAYS;
	data_cache.associate_sets = data_cache.size / (data_cache.block_size * data_cache.associate_ways);
	data_cache.cache = malloc(data_cache.associate_sets * sizeof(Cache_Line *));
	//memset(data_cache.cache, 0, data_cache.associate_sets * data_cache.associate_ways * sizeof(Cache_Line));
	for (int i = 0; i < data_cache.associate_sets; i++)
	{
		data_cache.cache[i] = malloc(data_cache.associate_ways * sizeof(Cache_Line));
		for (int j = 0; j < data_cache.associate_ways; j++)
		{
			data_cache.cache[i][j].data = malloc(DATA_CACHE_BLOCK_SIZE);
			data_cache.cache[i][j].dirty = data_cache.cache[i][j].valid = 0;
		}
	}
	
	
	instr_cache.block_size = INSTR_CACHE_BLOCK_SIZE;
	instr_cache.size = INSTR_CACHE_SIZE;
	instr_cache.associate_ways = INSTR_CACHE_ASSOCIATE_WAYS;
	instr_cache.associate_sets = instr_cache.size / (instr_cache.block_size * instr_cache.associate_ways);
	instr_cache.cache = malloc(instr_cache.associate_sets * sizeof(Cache_Line *));
	for (int i = 0; i < instr_cache.associate_sets; i++)
	{
		instr_cache.cache[i] = malloc(instr_cache.associate_ways * sizeof(Cache_Line));
		for (int j = 0; j < instr_cache.associate_ways; j++)
		{
			instr_cache.cache[i][j].data = malloc(INSTR_CACHE_BLOCK_SIZE);
			instr_cache.cache[i][j].dirty = instr_cache.cache[i][j].valid = 0;
		}
	}
	
}

void destroy_cache()
{
	for (int i = 0; i < data_cache.associate_sets; i++)
	{
		for (int j = 0; j < data_cache.associate_ways; j++)
		{
			free(data_cache.cache[i][j].data);
		}
		free(data_cache.cache[i]);
	}
	for (int i = 0; i < instr_cache.associate_sets; i++)
	{
		for (int j = 0; j < instr_cache.associate_ways; j++)
		{
			free(instr_cache.cache[i][j].data);
		}
		free(instr_cache.cache[i]);
	}
	free(data_cache.cache);
	free(instr_cache.cache);
}



// ASSUME THAT BLOCK SIZE IS ALWAYS GREATER THAN 32B.
uint32_t data_cache_read_32(uint32_t addr)
{
	return cache_read(&data_cache, addr);
}

uint32_t instr_cache_read_32(uint32_t addr)
{
	return cache_read(&instr_cache, addr);
}

void data_cache_write_32(uint32_t addr, uint32_t value)
{
	cache_write(&data_cache, addr, value);
}

int cache_search(Associate_Cache *cache_p, int set_idx, uint32_t tag, Cache_Line **result)
{
	for (int i = 0; i < cache_p->associate_ways; i++)
	{
		//fprintf(stderr, "check cache line at %p, tag: %x valid: %x\n", &(cache_p->cache[set_idx][i]), cache_p->cache[set_idx][i].tag, cache_p->cache[set_idx][i].valid);
			
		if ((cache_p->cache[set_idx][i].tag == tag) && (cache_p->cache[set_idx][i].valid == 1))
		{// match, present
			*result = &(cache_p->cache[set_idx][i]);
			return 0;
		}
	}
	return -1;
}

int cache_replace(Associate_Cache *cache_p, int set_idx, Cache_Line **result)
{
	#if (REPLACE_POLICY == 0)
	/* dummiest policy, check first for a free cache line,
	 * if there's not any, just select the first line.
	 */
		for (int i = 0; i < cache_p->associate_ways; i++)
		{
			if (cache_p->cache[set_idx][i].valid == 0)
			{
				*result = &(cache_p->cache[set_idx][i]);
				return 0;
			}
		}
		*result = cache_p->cache[set_idx];
		return 0;
	#endif
}


int cache_retrieve(Associate_Cache *cache_p, uint32_t addr, uint8_t *data)
{
	int block_size = cache_p->block_size;
	addr = addr & (~(block_size - 1));
	for (int i = 0; i < block_size; i += 4)
	{
		*((uint32_t *)(data + i)) = mem_read_32(addr + i);
	}
}
int cache_flush(Associate_Cache *cache_p, uint32_t addr, uint8_t *data)
{
	int block_size = cache_p->block_size;
	for (int i = 0; i < block_size; i += 4)
	{
		mem_write_32(addr + i, *((uint32_t *)(data + i)));
	}
}
uint32_t cache_read(Associate_Cache *cache_p, uint32_t addr)
{
	// tag: [31:11]
	// set index: [10:5]
	// offset: [4:0]
	
	int n_offset_bits = calc_bits(cache_p->block_size);
	int n_set_bits = calc_bits(cache_p->associate_sets);
	int n_tag_bits = 32 - n_offset_bits - n_set_bits;
	int offset = addr & (cache_p->block_size - 1);
	int set_idx = (addr >> n_offset_bits) & (cache_p->associate_sets - 1);
	int tag = (addr >> (n_offset_bits + n_set_bits));
	;
	
	//fprintf(stderr, "cache read at %x, tag: %x, set: %x, offset: %x\n", addr, tag, set_idx, offset);
	
	uint32_t ret;
	// first, see if the data is right in the cache
	Cache_Line *result;
	if (cache_search(cache_p, set_idx, tag, &result) < 0)
	{// not present
		
		//fprintf(stderr, "not in cache\n");
		// select a cache line, free or to be evicted.
		cache_replace(cache_p, set_idx, &result);
		
		// 
		if (result->dirty == 1)
		{
			cache_flush(cache_p, addr, result->data);
			result->dirty = 0;
		}
		// copy from memory to target cache line.
		cache_retrieve(cache_p, addr, result->data);
		result->valid = 1;
		result->tag = tag;
		ret = *((uint32_t *) (result->data + offset));
	}
	else 
	{
		//fprintf(stderr, "in cache line %p\n", result);
		// ASSUME THAT CACHE LINE SIZE CAN BE DIVIED BY 32.
		ret = *((uint32_t *)(result->data + offset));
	}
	//fprintf(stderr, "got %x\n", ret);
	return ret;
}   

void cache_write(Associate_Cache *cache_p, uint32_t addr, uint32_t value)
{

	int n_offset_bits = calc_bits(cache_p->block_size);
	int n_set_bits = calc_bits(cache_p->associate_sets);
	int n_tag_bits = 32 - n_offset_bits - n_set_bits;
	int offset = addr & (cache_p->block_size - 1);
	int set_idx = (addr >> n_offset_bits) & (cache_p->associate_sets - 1);
	int tag = (addr >> (n_offset_bits + n_set_bits));
	
	//fprintf(stderr, "cache write at %x, value: %x, tag: %x, set: %x, offset: %x, value: %x\n", addr, value, tag, set_idx, offset, value);

	
	uint32_t ret;
	// first, see if the data is right in the cache
	Cache_Line *result;
	if (cache_search(cache_p, set_idx, tag, &result) < 0)
	{// not present
		
		
		// select a cache line, free or to be evicted.
		cache_replace(cache_p, set_idx, &result);
		
		// 
		if (result->dirty == 1)
		{
			cache_flush(cache_p, addr, result->data);
			//result->dirty = 0;
		}
		// copy from memory to target cache line.
		cache_retrieve(cache_p, addr, result->data);
		result->valid = 1;
		result->dirty = 1;
		result->tag = tag;
		*((uint32_t *) (result->data + offset)) = value;
		//fprintf(stderr, "not in cache, use cache line at %p\n", result);
		//fprintf(stderr, "value: %x\n", *((uint32_t *) (result->data)));
	}
	else 
	{
		//fprintf(stderr, "in cache line %p\n", result);
		// ASSUME THAT CACHE LINE SIZE CAN BE DIVIED BY 32.
		*((uint32_t *)(result->data + offset)) = value;
		result->dirty = 1;
	}
	
}