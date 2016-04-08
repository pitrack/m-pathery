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

Board::Board(const int width, const int height, const TileType base_grid[2][8]) : width(width), height(height) {
  std::cout << "Using preloaded map" << std::endl;
  grid = new Tile*[width];
  for (int w = 0; w < width; w++) {
    grid[w] = new Tile[height];
    for (int h = 0; h < height; h++) {
      grid[w][h] = base_grid[h][w];
    }
  }  
}

int Board::getHeight(){ return height; }
int Board::getWidth(){ return width; }


Tile Board::getTileAt(int x, int y){
  return grid[x][y];
}

void Board::putTileAt(int x, int y, TileType t){
  grid[x][y].changeTileType(t);
}

void Board::putWallAt(int x, int y){
  this->putTileAt(x, y, FreeWall);
}


// runs kbfs
int Board::run(){
  

}


void Board::print(){
  std::cout << "Printing" << std::endl;
  for(int i = 0; i < height; i++){
    for (int j = 0; j < width; j++){
      std::cout << grid[j][i].getTileType() << " ";
    }
    std::cout << std::endl;
  }
}

