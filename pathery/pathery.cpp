#include <iostream>
#include <cstdlib>
using namespace std;

#include "tile.h"
#include "board.h"

#include "pathery.h"

TileType TEST_GRID[2][8] = {{Start, Empty, Empty, Empty, Empty, FixedWall, Empty, End},
			  {Start, Empty, Empty, Empty, Empty, Empty, Empty, End}};

TileType TEST_GRID3[7][13] = 
  {{Start, Empty, Empty, Empty, Empty, Blocks, FixedWall, FixedWall, Empty, Empty, Empty, Empty, End},
   {Empty, Empty, Empty, Empty, FixedWall, Empty, Empty, Empty, Empty, Empty, Empty, Empty, End},
   {FreeWall, Empty, FixedWall, FixedWall, Empty, Empty, Empty, Empty, Empty, Empty, Empty, Empty, End},
   {Empty, FixedWall, Empty, Empty, Empty, Empty, Empty, Empty, Empty, Empty, Empty, Empty, End},
   {Empty, Empty, Empty, Empty, Empty, Empty, Empty, Empty, Empty, Empty, Empty, Empty, End},
   {Empty, FixedWall, Empty, Empty, Empty, Empty, Empty, Empty, Empty, Empty, FixedWall, Empty, End},
   {Empty, Empty, Empty, Empty, FixedWall, Empty, Empty, Empty, Empty, Empty, Empty, Empty, End}
  };

TileType GRIDS[3][7][13] = 
  {
    {{Start, Empty, Empty, Empty, Empty, Empty, FixedWall, FixedWall, Empty, Empty, Empty, Empty, End},
     {Start, Empty, Empty, Empty, Empty, Empty, Empty, Empty, Empty, Empty, Empty, Empty, End},
     {Start, Empty, FixedWall, Empty, Empty, Empty, Empty, Empty, Empty, Empty, Empty, Empty, End},
     {Start, Empty, Empty, Empty, Empty, Empty, Empty, Empty, Empty, Empty, Empty, Empty, End},
     {Start, Empty, Empty, Empty, Empty, Empty, Empty, Empty, Empty, Empty, Empty, Empty, End},
     {Start, FixedWall, Empty, Empty, Empty, Empty, Empty, Empty, Empty, Empty, FixedWall, Empty, End},
     {Start, Empty, Empty, Empty, FixedWall, Empty, Empty, Empty, Empty, Empty, Empty, Empty, End}
    },
    {{Start, Empty, Empty, Empty, Empty, Empty, Empty, Empty, Empty, Empty, Empty, Empty, End},
     {Start, Empty, Empty, Empty, Empty, Empty, FixedWall, Empty, Empty, Empty, Empty, Empty, End},
     {Start, Empty, FixedWall, Empty, FixedWall, Empty, Empty, Empty, Empty, Empty, Empty, Empty, End},
     {Start, Empty, FixedWall, Empty, FixedWall, Empty, Empty, Empty, Empty, Empty, Empty, FixedWall, End},
     {Start, Empty, Empty, Empty, FixedWall, Empty, Empty, FixedWall, Empty, Empty, Empty, Empty, End},
     {Start, Empty, Empty, Empty, Empty, Empty, Empty, Empty, Empty, Empty, FixedWall, Empty, End},
     {Start, Empty, Empty, Empty, Empty, Empty, Empty, Empty, Empty, Empty, Empty, Empty, End}
    },
    {{Start, Empty, Empty, Empty, Empty, Empty, Empty, Empty, Empty, Empty, Empty, FixedWall, End},
     {Start, FixedWall, Empty, Empty, FixedWall, Empty, Empty, Empty, Empty, Empty, Empty, Empty, End},
     {Start, Empty, Empty, Empty, Empty, Empty, Empty, Empty, Empty, Empty, Empty, Empty, End},
     {Start, Empty, FixedWall, Empty, Empty, Empty, Empty, Empty, Empty, Empty, Empty, Empty, End},
     {Start, FixedWall, Empty, Empty, Empty, Empty, Empty, FixedWall, Empty, Empty, FixedWall, Empty, End},
     {Start, Empty, Empty, Empty, Empty, Empty, Empty, Empty, FixedWall, Empty, Empty, Empty, End},
     {Start, Empty, Empty, Empty, Empty, Empty, Empty, Empty, Empty, Empty, Empty, Empty, End}
    }
  };


void main_program(int roots, int ptrs, int grid, int iterations)
{
  
  Board board(13, 7, GRIDS[grid]);
  
  MCTS::ComputeOptions player_options;
  player_options.max_iterations = iterations;
  player_options.verbose = true;
  player_options.number_of_roots = roots;
  player_options.number_of_pointers = ptrs;
  
  
  PatheryState state(board, 7);
  //cout << board.run() << endl;
  //cout << "Best score" << state.get_result() << endl;
  while (state.has_moves())  {
    PatheryState::Move move = MCTS::compute_move(state, player_options);
    state.do_move(move);
    std::cout << move.first << "|" << move.second << endl;
    state.print();
  }

  cout << "Final score: " << (state.get_result() + state.current_score) << endl;
  
}


int main(int argc, char* argv[]){
  try {
    if(argc < 5){
      std::cerr << "Usage" << std::endl;
    }
    main_program(atoi(argv[1]), atoi(argv[2]), atoi(argv[3]), atoi(argv[4]));
  }
  catch (std::runtime_error& error) {
    std::cerr << "ERROR: " << error.what() << std::endl;
    return 1;
  }
}
