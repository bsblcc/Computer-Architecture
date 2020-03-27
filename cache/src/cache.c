#include "cache.h"



#define GET_INSTR_CACHE_TAG
#define GET_INSTR_CACHE_TAG
#define GET_INSTR_CACHE_OFFSET


void init_cache()
{
	data_cache.block_size = DATA_CACHE_BLOCK_SIZE;
	data_cache.size = DATA_CACHE_SIZE;
	data_cache.associate_ways = DATA_CACHE_ASSOCIATE_WAYS;
	data_cache.associate_sets / (data_cache.block_size * data_cache.associate_ways);
	data_cache.cache = malloc(data_cache.associate_sets * data_cache.associate_ways * sizeof(Cache_Line));
	memset(data_cache.cache, 0, data_cache.associate_sets * data_cache.associate_ways * sizeof(Cache_Line));
	for (int i = 0; i < data_cache.associate_sets; i++)
	{
		for (int j = 0; j < data_cache.associate_ways; j++)
		{
			data_cache.cache[i][j].data = malloc(DATA_CACHE_BLOCK_SIZE);
		}
	}
	
	
	instr_cache.block_size = INSTR_CACHE_BLOCK_SIZE;
	instr_cache.size = INSTR_CACHE_SIZE;
	instr_cache.associate_ways = INSTR_CACHE_ASSOCIATE_WAYS;
	instr_cache.associate_sets / (instr_cache.block_size * instr_cache.associate_ways);
	instr_cache.cache = malloc(instr_cache.associate_sets * instr_cache.associate_ways * sizeof(Cache_Line));
	memset(instr_cache.cache, 0, instr_cache.associate_sets * instr_cache.associate_ways * sizeof(Cache_Line));
	for (int i = 0; i < instr_cache.associate_sets; i++)
	{
		for (int j = 0; j < instr_cache.associate_ways; j++)
		{
			instr_cache.cache[i][j].data = malloc(INSTR_CACHE_BLOCK_SIZE);
		}
	}
	
}





// ASSUME THAT BLOCK SIZE IS ALWAYS GREATER THAN 32B.
uint32_t data_cache_read_32(uint32_t addr)
{
	return cache_fetch(&data_cache, addr);
}

uint32_t instr_cache_read_32(uint32_t addr)
{
	return cache_fetch(&instr_cache, addr);
}



// fecth the data in cache, retrieve if not present
uint32_t cache_fetch(Associate_Cache *cache_p, uint32_t addr);

// retrieve a whole cache line from memory
int cache_retrieve(Associate_Cache *cache_p, uint32_t addr, uint8_t *data);

// search the cache for that address, return -1 if not present in cache
int cache_search(Associate_Cache *cache_p, int set_idx, int tag, Cache_Line **result)
{
	for (int i = 0; i < cache_p->associate_ways; i++)
	{
		if ((cache_p->cache[set_idx][i].tag == tag) && (cache_p->cache[set_idx][i].valid == 1))
		{// match, present
			*result = &cache_p->cache[set_idx][i];
			return 0;
		}
	}
	return -1;
}

int cache_replace(Associate_Cache *cache_p, int set_idx, int tag, uint32_t data)
{
	
}


int cache_retrieve(Associate_Cache *cache_p, uint32_t addr, uint8_t *data)
{
	int block_size = cache_p->block_size;
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
uint32_t cache_fetch(Associate_Cache *cache_p, uint32_t addr)
{
	int block_size = cache_p->block_size;
	int n_ways = cache_p->associate_ways;
	int size = cache_p->size;
	int n_sets = cache_p->size / (cache_p->block_size * cache_p->associate_ways);
	
	int tag = addr / (n_sets * block_size);
	int set_idx = addr / (block_size * n_ways);
	int offset = addr % block_size;
	
	uint8_t *data;
	int ret;
	// first, see if the data is right in the cache
	Cache_Line *result;
	if ((ret = cache_search(cache_p, set_idx, tag, &result)) < 0)
	{// not present
		
		
		// select a cache line to evict
		cache_replace(cache_p, set_idx, tag, &result);
		
		// 
		if (result->dirty == 1)
		{
			cache_flush(cache_p, addr, result->data);
		}
		// copy from memory to target cache line.
		cache_retrieve(cache_p, addr, result->data);
		result->valid = 1;
	}
	else 
	{
		// ASSUME THAT CACHE LINE SIZE CAN BE DIVIED BY 32.
		data = *((uint32_t *)(result->data + offset));
	}
}   