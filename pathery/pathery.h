#include <algorithm>
#include <iostream>
#include <random>
using namespace std;

#include "../mcts/mcts.h"
#include "board.h"
#include "tile.h"

class PatheryState
{
 public:
  typedef pair<int, int> Move;
  static constexpr Move no_move = make_pair(-1, -1);
  
  PatheryState(Board board_, int num_tiles_)
   : player_to_move(1), //only one player
    board(board_),
    num_tiles(num_tiles_)
    {}

  PatheryState copy() const{
    Board board_copy = board.copy();
    return PatheryState(board_copy, num_tiles);
  }

  void do_move(Move move)
  {
    board.putWallAt(move.first, move.second);
    num_tiles--;
  }
  
  template<typename RandomEngine>
    void do_random_move(RandomEngine* engine)
    {     
      std::uniform_int_distribution<int> xs(0, board.getWidth() - 1);
      std::uniform_int_distribution<int> ys(0, board.getHeight() - 1);
      
      while (true) {
	auto x = xs(*engine);
	auto y = ys(*engine);
	if (board.isEmpty(x,y)){
	  do_move(make_pair(x, y));
	  return;
	}
      }
      
    }
  
  bool has_moves() const
  {
    return num_tiles > 0;
  }
  
  std::vector<Move> get_moves() const
    {
      std::vector<Move> moves;
      for (int y = 0; y < board.getHeight(); y++) {
	for (int x = 0; x < board.getWidth(); x++) {
	  if (board.isEmpty(x, y) && board.hasNeighbors(x, y)) {
	    moves.push_back(make_pair(x, y));
	  }
	}      
      }
      return moves;
    }

  //  don't need to get winner, just find the score
  double get_result(int current_player_to_move) const
  {
    //get the score here. Result might depend on existing max score
    int score = 0;
    for(int i = 0; i < board.getWidth(); i++){
      if (board.isEmpty(i, 0)) {
	score++;
      }
    }
    return score;
  }

  void print() const
  {
    board.print();
  }

  int player_to_move;
 private:  
  Board board;
  int num_tiles;
};
