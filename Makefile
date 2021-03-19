CC = g++
CXXFLAGS = -O3 -mcx16 -march=native -std=c++17 -Wall -Wextra -Wno-sign-compare -Wno-unused-variable -fcilkplus -DCILK -fpic

all: nearestNeighbor_parallel

reduce: nearestNeighbor_parallel.cpp get_time.h
	$(CC) $(CXXFLAGS) reduce.cc -o reduce

clean:
	rm reduce

.PHONY: clean
