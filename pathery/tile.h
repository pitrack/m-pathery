#pragma once

enum TileType { Uninit, Empty, Start, End, FixedWall, FreeWall, Target, Blocks};

class Tile {
 public:
  TileType getTileType();
  void changeTileType(TileType new_t);
  Tile(TileType t);
  Tile();
  
 private:
  TileType t;  
};
