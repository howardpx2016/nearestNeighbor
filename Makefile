CC = g++
CXXFLAGS = -O3 -mcx16 -march=native -std=c++17 -Wall -Wextra -fcilkplus -DCILK -fpic

all: nearestNeighbor_parallel

reduce: nearestNeighbor_parallel.cpp get_time.h
	$(CC) $(CXXFLAGS) reduce.cc -o reduce

clean:
	rm reduce

.PHONY: clean
