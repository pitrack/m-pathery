make: pathery/pathery.cpp pathery/pathery.h mcts/mcts.h
	c++ --std=c++11 -fopenmp -DUSE_OPENMP -O3 pathery/board.cpp pathery/tile.cpp pathery/pathery.cpp -o solve

test: connect_four.cpp connect_four.h mcts.h
	c++ --std=c++11 -fopenmp -DUSE_OPENMP -O3 connect_four.cpp -o connect_four

test_cpu: connect_four.cpp connect_four.h mcts.h
	c++ --std=c++0x -fopenmp -DUSE_OPENMP -O3 connect_four.cpp -o connect_four_cpu

clean: 
	rm connect_four connect_four_cpu 
