SRC = $(wildcard src/*.c)
INPUT ?= $(wildcard inputs/*/*.x)

.PHONY: all verify clean

all: sim

sim: $(SRC)
	gcc -g -O2 $^ -o $@

basesim: $(SRC)
	gcc -g -O2 $^ -o $@

run: sim
	@./run $(INPUT)

clean:
	rm -rf *.o *~ sim

