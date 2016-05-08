#include <iostream>
using namespace std;

#include "tile.h"
#include "board.h"

#include "pathery.h"

TileType TEST_GRID[2][8] = {{Start, Empty, Empty, Empty, Empty, FixedWall, Empty, End},
			  {Start, Empty, Empty, Empty, Empty, Empty, Empty, End}};

TileType TEST_GRID2[7][13] = 
  {{Start, Empty, Empty, Empty, Empty, Empty, FixedWall, FixedWall, Empty, Empty, Empty, Empty, End},
   {Start, Empty, Empty, Empty, Empty, Empty, Empty, Empty, Empty, Empty, Empty, Empty, End},
   {Start, Empty, FixedWall, Empty, Empty, Empty, Empty, Empty, Empty, Empty, Empty, Empty, End},
   {Start, Empty, Empty, Empty, Empty, Empty, Empty, Empty, Empty, Empty, Empty, Empty, End},
   {Start, Empty, Empty, Empty, Empty, Empty, Empty, Empty, Empty, Empty, Empty, Empty, End},
   {Start, FixedWall, Empty, Empty, Empty, Empty, Empty, Empty, Empty, Empty, FixedWall, Empty, End},
   {Start, Empty, Empty, Empty, FixedWall, Empty, Empty, Empty, Empty, Empty, Empty, Empty, End}
  };

TileType TEST_GRID3[7][13] = 
  {{Start, Empty, Empty, Empty, Empty, Empty, FixedWall, FixedWall, Empty, Empty, Empty, Empty, Empty},
   {Empty, Empty, Empty, Empty, Empty, Empty, Empty, Empty, Empty, Empty, Empty, Empty, Empty},
   {FreeWall, Empty, FixedWall, Empty, Empty, Empty, Empty, Empty, Empty, Empty, Empty, Empty, Empty},
   {End, FixedWall, Empty, Empty, Empty, Empty, Empty, Empty, Empty, Empty, Empty, Empty, Empty},
   {Empty, Empty, Empty, Empty, Empty, Empty, Empty, Empty, Empty, Empty, Empty, Empty, Empty},
   {Empty, FixedWall, Empty, Empty, Empty, Empty, Empty, Empty, Empty, Empty, FixedWall, Empty, Empty},
   {Empty, Empty, Empty, Empty, FixedWall, Empty, Empty, Empty, Empty, Empty, Empty, Empty, Empty}
  };



void main_program()
{
  Board board(13, 7, TEST_GRID2);
  
  MCTS::ComputeOptions player_options;
  player_options.max_iterations = 1000;
  player_options.verbose = true;
  
  
  PatheryState state(board, 7);
  //cout << "Best score" << state.get_result() << endl;
  while (state.has_moves())  {
    PatheryState::Move move = MCTS::compute_move(state, player_options);
    state.do_move(move);
    std::cout << move.first << "|" << move.second << endl;
    state.print();
  }
  
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
