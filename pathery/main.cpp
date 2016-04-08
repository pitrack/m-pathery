#include "tile.h"
#include "board.h"

#include <iostream>

using namespace std;

TileType TEST_GRID[2][8] = {{Start, Empty, Empty, Empty, Empty, FixedWall, Empty, End},
			  {Start, Empty, Empty, Empty, Empty, Empty, Empty, End}};


int main(){
  cout << "Hello" << endl;
  Board b(8, 2, TEST_GRID);
  b.putWallAt(3, 1);
  b.print();
  return 0;
}
