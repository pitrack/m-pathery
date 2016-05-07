#pragma once
#include "tile.h"


class Board {
 public:
  Board(int width, int height);
  Board(const int width, const int height, const TileType base_grid[2][8]);
  Board(int width, int height, Tile** base_grid);

  Board copy() const;

  int getHeight() const;
  int getWidth() const;

  Tile getTileAt(int x, int y) const;
  bool isEmpty(int x, int y) const;
  bool hasNeighbors(int x, int y) const ;
  void putTileAt(int x, int y, TileType t); //This should be treated as private
  void putWallAt(int x, int y);

  int run();

  void print() const;
  // other functions like clear board, remove, or copy might be desired.


 private:
  int width;
  int height;
  Tile** grid;

};
