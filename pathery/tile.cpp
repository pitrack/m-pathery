#include "tile.h"

Tile::Tile(TileType t) : t(t) {}
Tile::Tile() : t(Uninit){}


TileType Tile::getTileType(){
  return t;
}


void Tile::changeTileType(TileType new_t){
  t = new_t;
}
