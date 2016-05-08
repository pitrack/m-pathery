#include "board.h"
#include "tile.h"

#include <iostream>

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


std::vector<std::pair<int, int>> Board::getNeighbors(int x, int y) const {
  std::vector<std::pair<int, int>> neighbors;
  for (int i = x-1; i <= x+1; i++){
    for (int j = y-1; j <= y+1; j++){
      if (i >= 0 && i < width &&
	  j >= 0 && j < height &&
	  ((i == x) != (j == y)))
	{ 
	  if (grid[i][j].getTileType() != TileType::FixedWall && 
	      grid[i][j].getTileType() != TileType::FreeWall) {
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
  }
}

void Board::undo(int x, int y) {
  if (x >= 0 && y >= 0){
    this->putTileAt(x, y, Empty);
  }
}


// runs kbfs
int Board::run(){
}


void Board::print() const {
  for(int i = 0; i < height; i++){
    for (int j = 0; j < width; j++){
      std::cout << grid[j][i].getTileType() << " ";
    }
    std::cout << std::endl;
  }
}

