#include <algorithm>
#include <vector>
#include <queue>

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
      for (int y = 0; y < board.getHeight(); y++) {
	for (int x = 0; x < board.getWidth(); x++) {
	  //cout << "Checking" << x << "|" << y << endl;;
	  /* if (board.isEmpty(x, y)) cout << " is empty" << endl; */
	  /* else cout << endl; */
	  if (board.isEmpty(x, y) && board.hasNeighbors(x, y)) {
	    /* cout << "Pushing back" << x << "|" << y << endl; */
	    //cout << "Added" << endl;
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
    //get the score here. Result might depend on existing max score
    int min = 0; //Realistically won't be greater than 1000


    std::vector<std::pair<int, int>> sources;

    //Initialize source and targets
    for (int i = 0; i < board.getWidth(); i++){
      for(int j = 0; j < board.getHeight(); j++){
	if (board.getTileAt(i,j).getTileType() == TileType::Start) {
	  sources.push_back(std::make_pair(i, j));
	}
      }
    }
    
    for (auto source : sources) {
      int score;
      bool done = false;
      std::set<Move> visited;
      std::queue<std::pair<Move, int>> bfs_queue;
      bfs_queue.push(make_pair(source, 0));
      visited.insert(source);
      while(!bfs_queue.empty() && !done) {
	std::pair<Move, int> current = bfs_queue.front();
	bfs_queue.pop();
	Move curr_loc = current.first;
	std::vector<Move> neighbors = board.getNeighbors(curr_loc.first, curr_loc.second);
	for (Move ngh : neighbors) {
	  if(board.getTileAt(ngh.first, ngh.second).getTileType() == TileType::End){
	    score = current.second + 1;
	    if (min == 0 || score < min) {
	      min = score;
	      done = true;
	      break;
	    }
	  }
	  if(visited.find(ngh) == visited.end()) {
	    bfs_queue.push(make_pair(ngh, current.second+1));
	    visited.insert(ngh);
	  }
	}      
      }
    }
    

    return min;
    /* if (board.getTileAt(8,3).getTileType() != TileType::Empty) return 1; */
    /* return 0; */

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
