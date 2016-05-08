#include <algorithm>
#include <vector>
#include <chrono>

#include <iostream>
#include <random>
using namespace std;

#include "../mcts/mcts.h"
#include "board.h"
#include "tile.h"

typedef std::chrono::high_resolution_clock Clock;

class PatheryState
{
 public:
  typedef pair<int, int> Move;
  static constexpr Move no_move = make_pair(-1, -1);
  
  PatheryState(Board board_, int num_tiles_)
   : player_to_move(1), //only one player
    board(board_),
    num_tiles(num_tiles_)
    {
      current_score = board.run();
    }

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
      std::vector<Move> moves = get_moves();
      std::uniform_int_distribution<int> move_rng(0, moves.size() - 1);
      
      int m = move_rng(*engine);
      do_move(moves[m]);
      
    }
  
  bool has_moves() const
  {
    return num_tiles > 0;
  }
  
  std::vector<Move> get_moves() const
    {
      /* std::cout << "For board:" << std::endl; */
      /* board.print(); */
      std::vector<Move> moves;
      if (num_tiles == 0) {
	return moves;
      }
      for (int y = 0; y < board.getHeight(); y++) {
	for (int x = 0; x < board.getWidth(); x++) {
	  //cout << "Checking" << x << "|" << y << endl;;
	  /* if (board.isEmpty(x, y)) cout << " is empty" << endl; */
	  /* else cout << endl; */
	  if (board.isEmpty(x, y) && board.hasNeighbors(x, y)) {
	    /* cout << "Pushing back" << x << "|" << y << endl; */
	    //cout << "Added" << endl;
	    // Can't put things here
	    moves.push_back(make_pair(x, y));
	  }
	}      
      }
      /* board.print(); */
      /* cout << "List of moves" << endl; */
      /* for (int i = 0; i < moves.size(); i++){ */
      /* 	cout << "(" << moves[i].first << "," << moves[i].second << ")" << endl; */
      /* } */
      return moves;
    }

  //  don't need to get winner, just find the score
  // This might be parallelizable too, but might not be worth it
  double get_result() const
  {    
    return double(board.run() - current_score);
  }

  void print() const
  {
    board.print();
  }

  int player_to_move;
  int current_score;
 private:  
  Board board;
  int num_tiles;
};
