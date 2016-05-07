make: connect_four.cpp connect_four.h mcts.h
	c++ --std=c++0x -fopenmp -DUSE_OPENMP -O3 connect_four.cpp -o connect_four_cpu

cpu: connect_four.cpp connect_four.h mcts.h
	c++ --std=c++11 -fopenmp -DUSE_OPENMP -O3 connect_four.cpp -o connect-four