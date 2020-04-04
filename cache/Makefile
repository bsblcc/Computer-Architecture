SRC = $(wildcard src/*.c)
SRC := $(filter-out src/test.c, $(SRC))
INPUT ?= $(wildcard inputs/*/*.x)

.PHONY: all verify clean

all: sim

sim: $(SRC) src/cache.h
	gcc -g -D USE_INSTR_CACHE -O2 $^ -o $@ -D USE_DATA_CACHE 

basesim: $(SRC)
	gcc -g -O2 $^ -o $@ 
	
base: $(SRC)
	gcc -g -O2 $^ -o $@

run: sim
	@./run $(INPUT)

clean:
	rm sim

