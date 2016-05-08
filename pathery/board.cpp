#include "board.h"
#include "tile.h"

#include <algorithm>
#include <iostream>
#include <queue>
#include <vector>
#include <map>


//using namespace tile;

Board::Board(int width, int height) : width(width), height(height) {
  grid = new Tile*[width];
  for (int w = 0; w < width; w++) {
    grid[w] = new Tile[height];
    for (int h = 0; h < height; h++) {
      grid[w][h] = Tile(Uninit);
    }
  }
}

Board::Board(const int width, const int height, const TileType base_grid[7][13]) : width(width), height(height) {
  std::cout << "Using preloaded map" << std::endl;
  grid = new Tile*[width];
  for (int w = 0; w < width; w++) {
    grid[w] = new Tile[height];
    for (int h = 0; h < height; h++) {
      grid[w][h] = base_grid[h][w];
    }
  }  
}


Board::Board(int width, int height, Tile** base_grid) : width(width), height(height) {
  grid = new Tile*[width];
  for (int w = 0; w < width; w++) {
    grid[w] = new Tile[height];
    for (int h = 0; h < height; h++) {
      grid[w][h] = base_grid[w][h];
    }
  }  
}


Board Board::copy() const {
  Board board(width, height, grid);
  return board;
}

int Board::getHeight() const { return height; }
int Board::getWidth() const { return width; }

//If the path can walk over this tile, then it should be returned
std::vector<std::pair<int, int>> Board::getNeighbors(int x, int y) const {
  std::vector<std::pair<int, int>> neighbors;
  for (int i = x-1; i <= x+1; i++){
    for (int j = y-1; j <= y+1; j++){
      if (i >= 0 && i < width &&
	  j >= 0 && j < height &&
	  ((i == x) != (j == y)))
	{ 
           TileType gridTile = grid[i][j].getTileType();
            if (gridTile != TileType::FixedWall && 
                gridTile != TileType::FreeWall) {
	        neighbors.push_back(std::make_pair(i, j));
            }
	} 
    } 
  } 
  return neighbors;
} 

bool Board::isEmpty (int x, int y) const {
  return grid[x][y].getTileType() == TileType::Empty;
}

bool Board::hasNeighbors(int x, int y) const {
  for (int i = x-1; i <= x+1; i++){
    for (int j = y-1; j <= y+1; j++){
      if (i >= 0 && i < width &&
	  j >= 0 && j < height &&
	  !(i == x && j == y))
	{ 
	  //std::cout << i << j << "has type" << grid[i][j].getTileType() << std::endl;
	  if (grid[i][j].getTileType() != TileType::Empty) {
	    return true;
	  }
	}
    }
  }
  return false;
}

Tile Board::getTileAt(int x, int y) const {
  return grid[x][y];
}

void Board::putTileAt(int x, int y, TileType t){
  if (x >= 0 && y >= 0){
    grid[x][y].changeTileType(t);
  }
}

void Board::putWallAt(int x, int y){
  if (x >= 0 && y >= 0){
    this->putTileAt(x, y, FreeWall);
    // auto neighbors = getNeighbors(x,y);
    // for(auto ngh : neighbors) {
    //     if (isEmpty(ngh.first, ngh.second)) {
    //         putTileAt(ngh.first, ngh.second, FreeWall);
    //         if(run() == 0){
    //             putTileAt(ngh.first, ngh.second, Blocks);
    //         }
    //         else{
    //             undo(ngh.first, ngh.second);
    //         }
    //     }
    // }
  }
}

void Board::undo(int x, int y) {
  if (x >= 0 && y >= 0){
    this->putTileAt(x, y, Empty);
  }
}


// runs kbfs
int Board::run() const {
  typedef std::pair<int, int> Move;
  int min = 0; //Realistically won't be greater than 1000
  
  std::vector<Move> sources;
  //Initialize source and targets
  for (int i = 0; i < width; i++){
    for(int j = 0; j < height; j++){
      if (grid[i][j].getTileType() == TileType::Start) {
	sources.push_back(std::make_pair(i, j));
      }
    }
  }
  
  //Do the search
  std::map<Move, int> visited;
  for (auto source : sources)
    {
      int score;
      bool done = false;
      std::queue<std::pair<Move, int>> bfs_queue;
      bfs_queue.push(make_pair(source, 0));
      visited[source] = 0;
      while(!bfs_queue.empty() && !done) {
	std::pair<Move, int> current = bfs_queue.front();
	bfs_queue.pop();
	Move curr_loc = current.first;
	std::vector<Move> neighbors = getNeighbors(curr_loc.first, curr_loc.second);
	for (Move ngh : neighbors) {
	  if(visited.find(ngh) == visited.end() || visited[ngh] > current.second + 1) {
	    bfs_queue.push(make_pair(ngh, current.second + 1));
	    visited[ngh] = current.second + 1;
	    if(grid[ngh.first][ngh.second].getTileType() == TileType::End){
	      score = current.second + 1;
	      if (min == 0 || score < min) {
		min = score;
		done = true;
		break;
	      }
	    }
	  }
	}      
      }
      }
  return min;
}


void Board::print() const {
  for(int i = 0; i < height; i++){
    for (int j = 0; j < width; j++){
      std::cout << grid[j][i].getTileType() << " ";
    }
    std::cout << std::endl;
  }
}

