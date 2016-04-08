#pragma once
#include "../pathery/board.h"
#include "state.h"

using namespace std;

class MCTS {
 public:
  MCTS(Board b);
  void update();
  State get_sol();
  void run_game();
  
 private:
  vector<State> tried_states;
  unordered_map<State, int> results; // everything in the above vector should be mapped to something here
}
