#include <iostream>
using namespace std;

#include "tile.h"
#include "board.h"

#include "pathery.h"

TileType TEST_GRID[2][8] = {{Start, Empty, Empty, Empty, Empty, FixedWall, Empty, End},
			  {Start, Empty, Empty, Empty, Empty, Empty, Empty, End}};

void main_program()
{
  Board board(8, 2, TEST_GRID);
  
  MCTS::ComputeOptions player_options;
  player_options.max_iterations = 1000;
  player_options.verbose = true;
  
  
  PatheryState state(board, 4);
  while (state.has_moves())  {
    PatheryState::Move move = MCTS::compute_move(state, player_options);
    state.do_move(move);
    std::cout << move.first << "|" << move.second << endl;
  }
  state.print();
}


int main(){
  try {
    main_program();
  }
  catch (std::runtime_error& error) {
    std::cerr << "ERROR: " << error.what() << std::endl;
    return 1;
  }
}
