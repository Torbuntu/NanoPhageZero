#include "Level.hpp"
#include <Tilemap.hpp>
#include <maps.h>

namespace Level{
    void LevelManager::setMap(const uint8_t* map){
        tilemap.set(map[0], map[1], map+2);
        for(int i=0; i<sizeof(tiles)/(POK_TILE_W*POK_TILE_H); i++){
            tilemap.setTile(i, POK_TILE_W, POK_TILE_H, tiles+i*POK_TILE_W*POK_TILE_H);
        }
    }
    
    void LevelManager::setMap(){
        switch(level){
            case 0:
                LevelManager::setMap(lobby);
                break;
            case 1:
                LevelManager::setMap(lvl1);
                break;
        }
    }
    
    void LevelManager::addLvl(){
        level++;
    }
    
    void LevelManager::setLvl(int lvl){
        level = lvl;
    }
    
    int LevelManager::getLvl(){
        return level;
    }
    
    void LevelManager::init(){
        tilemap.fillOutOfBounds = 255;
        level = 0;
    }
    
    void LevelManager::update(){
        
    }
    
    void LevelManager::render(int x, int y){
        tilemap.draw(x, y);
    }
    
}